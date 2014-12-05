/*
*  Copyright (C) 1999-2002 Harri Porten (porten@kde.org)
*  Copyright (C) 2001 Peter Kelly (pmk@post.com)
*  Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2013 Apple Inc. All rights reserved.
*  Copyright (C) 2007 Cameron Zwarich (cwzwarich@uwaterloo.ca)
*  Copyright (C) 2007 Maks Orlovich
*  Copyright (C) 2007 Eric Seidel <eric@webkit.org>
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Library General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Library General Public License for more details.
*
*  You should have received a copy of the GNU Library General Public License
*  along with this library; see the file COPYING.LIB.  If not, write to
*  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
*  Boston, MA 02110-1301, USA.
*
*/

#include "config.h"
#include "Nodes.h"
#include "NodeConstructors.h"

#include "BytecodeGenerator.h"
#include "CallFrame.h"
#include "Debugger.h"
#include "JIT.h"
#include "JSFunction.h"
#include "JSGlobalObject.h"
#include "JSNameScope.h"
#include "LabelScope.h"
#include "Lexer.h"
#include "JSCInlines.h"
#include "Parser.h"
#include "PropertyNameArray.h"
#include "RegExpObject.h"
#include "SamplingTool.h"
#include <wtf/Assertions.h>
#include <wtf/RefCountedLeakCounter.h>
#include <wtf/Threading.h>

using namespace WTF;

namespace JSC {


// ------------------------------ StatementNode --------------------------------

void StatementNode::setLoc(unsigned firstLine, unsigned lastLine, int startOffset, int lineStartOffset)
{
    m_lastLine = lastLine;
    m_position = JSTextPosition(firstLine, startOffset, lineStartOffset);
    ASSERT(m_position.offset >= m_position.lineStartOffset);
}

// ------------------------------ SourceElements --------------------------------

void SourceElements::append(StatementNode* statement)
{
    if (statement->isEmptyStatement())
        return;
    m_statements.append(statement);
}

StatementNode* SourceElements::singleStatement() const
{
    size_t size = m_statements.size();
    return size == 1 ? m_statements[0] : 0;
}

// ------------------------------ ScopeNode -----------------------------

ScopeNode::ScopeNode(ParserArena& parserArena, const JSTokenLocation& startLocation, const JSTokenLocation& endLocation, bool inStrictContext)
    : StatementNode(endLocation)
    , ParserArenaRoot(parserArena)
    , m_startLineNumber(startLocation.line)
    , m_startStartOffset(startLocation.startOffset)
    , m_startLineStartOffset(startLocation.lineStartOffset)
    , m_features(inStrictContext ? StrictModeFeature : NoFeatures)
    , m_numConstants(0)
    , m_statements(0)
{
}

ScopeNode::ScopeNode(ParserArena& parserArena, const JSTokenLocation& startLocation, const JSTokenLocation& endLocation, const SourceCode& source, SourceElements* children, VarStack* varStack, FunctionStack* funcStack, IdentifierSet& capturedVariables, CodeFeatures features, int numConstants)
    : StatementNode(endLocation)
    , ParserArenaRoot(parserArena)
    , m_startLineNumber(startLocation.line)
    , m_startStartOffset(startLocation.startOffset)
    , m_startLineStartOffset(startLocation.lineStartOffset)
    , m_features(features)
    , m_source(source)
    , m_numConstants(numConstants)
    , m_statements(children)
{
    if (varStack)
        m_varStack.swap(*varStack);
    if (funcStack)
        m_functionStack.swap(*funcStack);
    m_capturedVariables.swap(capturedVariables);
}

StatementNode* ScopeNode::singleStatement() const
{
    return m_statements ? m_statements->singleStatement() : 0;
}

// ------------------------------ ProgramNode -----------------------------

ProgramNode::ProgramNode(ParserArena& parserArena, const JSTokenLocation& startLocation, const JSTokenLocation& endLocation, unsigned startColumn, unsigned endColumn, SourceElements* children, VarStack* varStack, FunctionStack* funcStack, IdentifierSet& capturedVariables, const SourceCode& source, CodeFeatures features, int numConstants)
    : ScopeNode(parserArena, startLocation, endLocation, source, children, varStack, funcStack, capturedVariables, features, numConstants)
    , m_startColumn(startColumn)
    , m_endColumn(endColumn)
{
}

void ProgramNode::setClosedVariables(Vector<RefPtr<StringImpl>>&& closedVariables)
{
    m_closedVariables = WTF::move(closedVariables);
}

// ------------------------------ EvalNode -----------------------------

EvalNode::EvalNode(ParserArena& parserArena, const JSTokenLocation& startLocation, const JSTokenLocation& endLocation, unsigned, unsigned endColumn, SourceElements* children, VarStack* varStack, FunctionStack* funcStack, IdentifierSet& capturedVariables, const SourceCode& source, CodeFeatures features, int numConstants)
    : ScopeNode(parserArena, startLocation, endLocation, source, children, varStack, funcStack, capturedVariables, features, numConstants)
    , m_endColumn(endColumn)
{
}

// ------------------------------ FunctionBodyNode -----------------------------

PassRefPtr<FunctionParameters> FunctionParameters::create(ParameterNode* firstParameter)
{
    unsigned parameterCount = 0;
    for (ParameterNode* parameter = firstParameter; parameter; parameter = parameter->nextParam())
        ++parameterCount;

    size_t objectSize = sizeof(FunctionParameters) - sizeof(void*) + sizeof(DeconstructionPatternNode*) * parameterCount;
    void* slot = fastMalloc(objectSize);
    return adoptRef(new (slot) FunctionParameters(firstParameter, parameterCount));
}

FunctionParameters::FunctionParameters(ParameterNode* firstParameter, unsigned size)
    : m_size(size)
{
    unsigned i = 0;
    for (ParameterNode* parameter = firstParameter; parameter; parameter = parameter->nextParam()) {
        auto pattern = parameter->pattern();
        pattern->ref();
        patterns()[i++] = pattern;
    }
}

FunctionParameters::~FunctionParameters()
{
    for (unsigned i = 0; i < m_size; ++i)
        patterns()[i]->deref();
}

FunctionBodyNode::FunctionBodyNode(ParserArena&, const JSTokenLocation& startLocation, const JSTokenLocation& endLocation, unsigned startColumn, unsigned endColumn, bool isInStrictContext)
    : StatementNode(endLocation)
    , m_startColumn(startColumn)
    , m_endColumn(endColumn)
    , m_startStartOffset(startLocation.startOffset)
    , m_isInStrictContext(isInStrictContext)
{
}

void FunctionBodyNode::finishParsing(const SourceCode& source, ParameterNode* firstParameter, const Identifier& ident, enum FunctionMode functionMode)
{
    m_source = source;
    m_parameters = FunctionParameters::create(firstParameter);
    m_ident = ident;
    m_functionMode = functionMode;
}

void FunctionBodyNode::setEndPosition(JSTextPosition position)
{
    m_lastLine = position.line;
    m_endColumn = position.offset - position.lineStartOffset;
}

// ------------------------------ FunctionNode -----------------------------

FunctionNode::FunctionNode(ParserArena& parserArena, const JSTokenLocation& startLocation, const JSTokenLocation& endLocation, unsigned startColumn, unsigned endColumn, SourceElements* children, VarStack* varStack, FunctionStack* funcStack, IdentifierSet& capturedVariables, const SourceCode& sourceCode, CodeFeatures features, int numConstants)
    : ScopeNode(parserArena, startLocation, endLocation, sourceCode, children, varStack, funcStack, capturedVariables, features, numConstants)
    , m_startColumn(startColumn)
    , m_endColumn(endColumn)
{
}

void FunctionNode::finishParsing(PassRefPtr<FunctionParameters> parameters, const Identifier& ident, enum FunctionMode functionMode)
{
    ASSERT(!source().isNull());
    m_parameters = parameters;
    m_ident = ident;
    m_functionMode = functionMode;
}

} // namespace JSC
