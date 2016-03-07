/*
 * Copyright (C) 2008, 2009, 2012, 2013, 2014 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Cameron Zwarich <cwzwarich@uwaterloo.ca>
 * Copyright (C) 2012 Igalia, S.L.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BytecodeGenerator_h
#define BytecodeGenerator_h

#include "CodeBlock.h"
#include <wtf/HashTraits.h>
#include "Instruction.h"
#include "Label.h"
#include "LabelScope.h"
#include "Interpreter.h"
#include "ParserError.h"
#include "RegisterID.h"
#include "SymbolTable.h"
#include "Debugger.h"
#include "Nodes.h"
#include "StaticPropertyAnalyzer.h"
#include "UnlinkedCodeBlock.h"

#include <functional>

#include <wtf/PassRefPtr.h>
#include <wtf/SegmentedVector.h>
#include <wtf/Vector.h>


namespace JSC {

    class Identifier;

    enum ExpectedFunction {
        NoExpectedFunction,
        ExpectObjectConstructor,
        ExpectArrayConstructor
    };

    class CallArguments {
    public:
        CallArguments(BytecodeGenerator&, ArgumentsNode*, unsigned additionalArguments = 0);

        RegisterID* thisRegister() { return m_argv[0].get(); }
        RegisterID* argumentRegister(unsigned i) { return m_argv[i + 1].get(); }
        unsigned stackOffset() { return -m_argv[0]->index() + JSStack::CallFrameHeaderSize; }
        unsigned argumentCountIncludingThis() { return m_argv.size() - m_padding; }
        RegisterID* profileHookRegister() { return m_profileHookRegister.get(); }
        ArgumentsNode* argumentsNode() { return m_argumentsNode; }

    private:
        RefPtr<RegisterID> m_profileHookRegister;
        ArgumentsNode* m_argumentsNode;
        Vector<RefPtr<RegisterID>, 8, UnsafeVectorOverflow> m_argv;
        unsigned m_padding;
    };

    struct FinallyContext {
        StatementNode* finallyBlock;
        unsigned scopeContextStackSize;
        unsigned switchContextStackSize;
        unsigned forInContextStackSize;
        unsigned tryContextStackSize;
        unsigned labelScopesSize;
        int finallyDepth;
        int dynamicScopeDepth;
    };

    struct ControlFlowContext {
        bool isFinallyBlock;
        FinallyContext finallyContext;
    };

    class ForInContext {
    public:
        ForInContext(RegisterID* localRegister)
            : m_localRegister(localRegister)
            , m_isValid(true)
        {
        }

        virtual ~ForInContext()
        {
        }

        bool isValid() const { return m_isValid; }
        void invalidate() { m_isValid = false; }

        enum ForInContextType {
            StructureForInContextType,
            IndexedForInContextType
        };
        virtual ForInContextType type() const = 0;

        RegisterID* local() const { return m_localRegister.get(); }

    private:
        RefPtr<RegisterID> m_localRegister;
        bool m_isValid;
    };

    class StructureForInContext : public ForInContext {
    public:
        StructureForInContext(RegisterID* localRegister, RegisterID* indexRegister, RegisterID* propertyRegister, RegisterID* enumeratorRegister)
            : ForInContext(localRegister)
            , m_indexRegister(indexRegister)
            , m_propertyRegister(propertyRegister)
            , m_enumeratorRegister(enumeratorRegister)
        {
        }

        virtual ForInContextType type() const
        {
            return StructureForInContextType;
        }

        RegisterID* index() const { return m_indexRegister.get(); }
        RegisterID* property() const { return m_propertyRegister.get(); }
        RegisterID* enumerator() const { return m_enumeratorRegister.get(); }

    private:
        RefPtr<RegisterID> m_indexRegister;
        RefPtr<RegisterID> m_propertyRegister;
        RefPtr<RegisterID> m_enumeratorRegister;
    };

    class IndexedForInContext : public ForInContext {
    public:
        IndexedForInContext(RegisterID* localRegister, RegisterID* indexRegister)
            : ForInContext(localRegister)
            , m_indexRegister(indexRegister)
        {
        }

        virtual ForInContextType type() const
        {
            return IndexedForInContextType;
        }

        RegisterID* index() const { return m_indexRegister.get(); }

    private:
        RefPtr<RegisterID> m_indexRegister;
    };

    struct TryData {
        RefPtr<Label> target;
        unsigned targetScopeDepth;
    };

    struct TryContext {
        RefPtr<Label> start;
        TryData* tryData;
    };

    enum CaptureMode {
        NotCaptured,
        IsCaptured
    };

    class Local {
    public:
        Local()
            : m_local(0)
            , m_attributes(0)
            , m_kind(NormalLocal)
        {
        }

        enum LocalKind { NormalLocal, SpecialLocal };

        Local(RegisterID* local, unsigned attributes, LocalKind kind)
            : m_local(local)
            , m_attributes(attributes)
            , m_kind(kind)
        {
        }

        operator bool() const { return m_local; }

        RegisterID* get() const { return m_local; }

        bool isReadOnly() const { return m_attributes & ReadOnly; }
        bool isSpecial() const { return m_kind != NormalLocal; }

    private:
        RegisterID* m_local;
        unsigned m_attributes;
        LocalKind m_kind;
    };

    struct ResolveScopeInfo {
        ResolveScopeInfo()
            : m_localIndex(0)
            , m_resolveScopeKind(NonLocalScope)
        {
        }

        ResolveScopeInfo(int index)
            : m_localIndex(index)
            , m_resolveScopeKind(LocalScope)
        {
        }

        bool isLocal() const { return m_resolveScopeKind == LocalScope; }
        int localIndex() const { return m_localIndex; }

    private:
        int m_localIndex;
        enum { LocalScope, NonLocalScope } m_resolveScopeKind;
    };

    struct TryRange {
        RefPtr<Label> start;
        RefPtr<Label> end;
        TryData* tryData;
    };

    enum ProfileTypeBytecodeFlag {
        ProfileTypeBytecodePutToScope,
        ProfileTypeBytecodeGetFromScope,
        ProfileTypeBytecodePutToLocalScope,
        ProfileTypeBytecodeGetFromLocalScope,
        ProfileTypeBytecodeHasGlobalID,
        ProfileTypeBytecodeDoesNotHaveGlobalID,
        ProfileTypeBytecodeFunctionArgument,
        ProfileTypeBytecodeFunctionReturnStatement
    };

    class BytecodeGenerator {
        WTF_MAKE_FAST_ALLOCATED;
        WTF_MAKE_NONCOPYABLE(BytecodeGenerator);
    public:
        typedef DeclarationStacks::VarStack VarStack;
        typedef DeclarationStacks::FunctionStack FunctionStack;

        BytecodeGenerator(VM&, ProgramNode*, UnlinkedProgramCodeBlock*, DebuggerMode, ProfilerMode);
        BytecodeGenerator(VM&, FunctionNode*, UnlinkedFunctionCodeBlock*, DebuggerMode, ProfilerMode);
        BytecodeGenerator(VM&, EvalNode*, UnlinkedEvalCodeBlock*, DebuggerMode, ProfilerMode);

        ~BytecodeGenerator();
        
        VM* vm() const { return m_vm; }
        ParserArena& parserArena() const { return m_scopeNode->parserArena(); }
        const CommonIdentifiers& propertyNames() const { return *m_vm->propertyNames; }

        bool isConstructor() { return m_codeBlock->isConstructor(); }

        ParserError generate();

        bool isArgumentNumber(const Identifier&, int);

        void setIsNumericCompareFunction(bool isNumericCompareFunction);

        bool willResolveToArgumentsRegister(const Identifier&);

        bool hasSafeLocalArgumentsRegister() { return m_localArgumentsRegister; }
        RegisterID* uncheckedLocalArgumentsRegister();

        bool isCaptured(int operand);
        CaptureMode captureMode(int operand) { return isCaptured(operand) ? IsCaptured : NotCaptured; }
        
        Local local(const Identifier&);
        Local constLocal(const Identifier&);

        // Returns the register storing "this"
        RegisterID* thisRegister() { return &m_thisRegister; }
        
        RegisterID* scopeRegister() { return m_scopeRegister; }

        // Returns the next available temporary register. Registers returned by
        // newTemporary require a modified form of reference counting: any
        // register with a refcount of 0 is considered "available", meaning that
        // the next instruction may overwrite it.
        RegisterID* newTemporary();

        // The same as newTemporary(), but this function returns "suggestion" if
        // "suggestion" is a temporary. This function is helpful in situations
        // where you've put "suggestion" in a RefPtr, but you'd like to allow
        // the next instruction to overwrite it anyway.
        RegisterID* newTemporaryOr(RegisterID* suggestion) { return suggestion->isTemporary() ? suggestion : newTemporary(); }

        // Functions for handling of dst register

        RegisterID* ignoredResult() { return &m_ignoredResultRegister; }

        // Returns a place to write intermediate values of an operation
        // which reuses dst if it is safe to do so.
        RegisterID* tempDestination(RegisterID* dst)
        {
            return (dst && dst != ignoredResult() && dst->isTemporary()) ? dst : newTemporary();
        }

        // Returns the place to write the final output of an operation.
        RegisterID* finalDestination(RegisterID* originalDst, RegisterID* tempDst = 0)
        {
            if (originalDst && originalDst != ignoredResult())
                return originalDst;
            ASSERT(tempDst != ignoredResult());
            if (tempDst && tempDst->isTemporary())
                return tempDst;
            return newTemporary();
        }

        RegisterID* destinationForAssignResult(RegisterID* dst)
        {
            if (dst && dst != ignoredResult() && m_codeBlock->needsFullScopeChain())
                return dst->isTemporary() ? dst : newTemporary();
            return 0;
        }

        // Moves src to dst if dst is not null and is different from src, otherwise just returns src.
        RegisterID* moveToDestinationIfNeeded(RegisterID* dst, RegisterID* src)
        {
            return dst == ignoredResult() ? 0 : (dst && dst != src) ? emitMove(dst, src) : src;
        }

        LabelScopePtr newLabelScope(LabelScope::Type, const Identifier* = 0);
        PassRefPtr<Label> newLabel();

        void emitNode(RegisterID* dst, StatementNode* n)
        {
            // Node::emitCode assumes that dst, if provided, is either a local or a referenced temporary.
            ASSERT(!dst || dst == ignoredResult() || !dst->isTemporary() || dst->refCount());
            // Should never store directly into a captured variable.
            ASSERT(!dst || dst == ignoredResult() || !isCaptured(dst->index()));
            if (!m_vm->isSafeToRecurse()) {
                emitThrowExpressionTooDeepException();
                return;
            }
            n->emitBytecode(*this, dst);
        }

        void emitNode(StatementNode* n)
        {
            emitNode(0, n);
        }

        RegisterID* emitNode(RegisterID* dst, ExpressionNode* n)
        {
            // Node::emitCode assumes that dst, if provided, is either a local or a referenced temporary.
            ASSERT(!dst || dst == ignoredResult() || !dst->isTemporary() || dst->refCount());
            // Should never store directly into a captured variable.
            ASSERT(!dst || dst == ignoredResult() || !isCaptured(dst->index()));
            if (!m_vm->isSafeToRecurse())
                return emitThrowExpressionTooDeepException();
            return n->emitBytecode(*this, dst);
        }

        RegisterID* emitNode(ExpressionNode* n)
        {
            return emitNode(0, n);
        }

        void emitNodeInConditionContext(ExpressionNode* n, Label* trueTarget, Label* falseTarget, FallThroughMode fallThroughMode)
        {
            if (!m_vm->isSafeToRecurse()) {
                emitThrowExpressionTooDeepException();
                return;
            }

            n->emitBytecodeInConditionContext(*this, trueTarget, falseTarget, fallThroughMode);
        }

        void emitExpressionInfo(const JSTextPosition& divot, const JSTextPosition& divotStart, const JSTextPosition& divotEnd)
        {            
            ASSERT(divot.offset >= divotStart.offset);
            ASSERT(divotEnd.offset >= divot.offset);

            int sourceOffset = m_scopeNode->source().startOffset();
            unsigned firstLine = m_scopeNode->source().firstLine();

            int divotOffset = divot.offset - sourceOffset;
            int startOffset = divot.offset - divotStart.offset;
            int endOffset = divotEnd.offset - divot.offset;

            unsigned line = divot.line;
            ASSERT(line >= firstLine);
            line -= firstLine;

            int lineStart = divot.lineStartOffset;
            if (lineStart > sourceOffset)
                lineStart -= sourceOffset;
            else
                lineStart = 0;

            if (divotOffset < lineStart)
                return;

            unsigned column = divotOffset - lineStart;

            unsigned instructionOffset = instructions().size();
            if (!m_isBuiltinFunction)
                m_codeBlock->addExpressionInfo(instructionOffset, divotOffset, startOffset, endOffset, line, column);
        }


        ALWAYS_INLINE bool leftHandSideNeedsCopy(bool rightHasAssignments, bool rightIsPure)
        {
            return (m_codeType != FunctionCode || m_codeBlock->needsFullScopeChain() || rightHasAssignments) && !rightIsPure;
        }

        ALWAYS_INLINE PassRefPtr<RegisterID> emitNodeForLeftHandSide(ExpressionNode* n, bool rightHasAssignments, bool rightIsPure)
        {
            if (leftHandSideNeedsCopy(rightHasAssignments, rightIsPure)) {
                PassRefPtr<RegisterID> dst = newTemporary();
                emitNode(dst.get(), n);
                return dst;
            }

            return emitNode(n);
        }

        void emitTypeProfilerExpressionInfo(const JSTextPosition& startDivot, const JSTextPosition& endDivot);
        void emitProfileType(RegisterID* registerToProfile, ProfileTypeBytecodeFlag, const Identifier*);

        void emitProfileControlFlow(int);

        RegisterID* emitLoad(RegisterID* dst, bool);
        RegisterID* emitLoad(RegisterID* dst, const Identifier&);
        RegisterID* emitLoad(RegisterID* dst, JSValue);
        RegisterID* emitLoadGlobalObject(RegisterID* dst);

        RegisterID* emitUnaryOp(OpcodeID, RegisterID* dst, RegisterID* src);
        RegisterID* emitBinaryOp(OpcodeID, RegisterID* dst, RegisterID* src1, RegisterID* src2, OperandTypes);
        RegisterID* emitEqualityOp(OpcodeID, RegisterID* dst, RegisterID* src1, RegisterID* src2);
        RegisterID* emitUnaryNoDstOp(OpcodeID, RegisterID* src);

        RegisterID* emitCreateThis(RegisterID* dst);
        RegisterID* emitNewObject(RegisterID* dst);
        RegisterID* emitNewArray(RegisterID* dst, ElementNode*, unsigned length); // stops at first elision

        RegisterID* emitNewFunction(RegisterID* dst, FunctionBodyNode*);
        RegisterID* emitLazyNewFunction(RegisterID* dst, FunctionBodyNode* body);
        RegisterID* emitNewFunctionInternal(RegisterID* dst, unsigned index, bool shouldNullCheck);
        RegisterID* emitNewFunctionExpression(RegisterID* dst, FuncExprNode* func);
        RegisterID* emitNewRegExp(RegisterID* dst, RegExp*);

        RegisterID* emitMove(RegisterID* dst, RegisterID* src);

        RegisterID* emitToNumber(RegisterID* dst, RegisterID* src) { return emitUnaryOp(op_to_number, dst, src); }
        RegisterID* emitInc(RegisterID* srcDst);
        RegisterID* emitDec(RegisterID* srcDst);

        void emitCheckHasInstance(RegisterID* dst, RegisterID* value, RegisterID* base, Label* target);
        RegisterID* emitInstanceOf(RegisterID* dst, RegisterID* value, RegisterID* basePrototype);
        RegisterID* emitTypeOf(RegisterID* dst, RegisterID* src) { return emitUnaryOp(op_typeof, dst, src); }
        RegisterID* emitIn(RegisterID* dst, RegisterID* property, RegisterID* base) { return emitBinaryOp(op_in, dst, property, base, OperandTypes()); }

        RegisterID* emitInitGlobalConst(const Identifier&, RegisterID* value);

        RegisterID* emitGetById(RegisterID* dst, RegisterID* base, const Identifier& property);
        RegisterID* emitGetArgumentsLength(RegisterID* dst, RegisterID* base);
        RegisterID* emitPutById(RegisterID* base, const Identifier& property, RegisterID* value);
        RegisterID* emitDirectPutById(RegisterID* base, const Identifier& property, RegisterID* value);
        RegisterID* emitDeleteById(RegisterID* dst, RegisterID* base, const Identifier&);
        RegisterID* emitGetByVal(RegisterID* dst, RegisterID* base, RegisterID* property);
        RegisterID* emitGetArgumentByVal(RegisterID* dst, RegisterID* base, RegisterID* property);
        RegisterID* emitPutByVal(RegisterID* base, RegisterID* property, RegisterID* value);
        RegisterID* emitDirectPutByVal(RegisterID* base, RegisterID* property, RegisterID* value);
        RegisterID* emitDeleteByVal(RegisterID* dst, RegisterID* base, RegisterID* property);
        RegisterID* emitPutByIndex(RegisterID* base, unsigned index, RegisterID* value);
        void emitPutGetterSetter(RegisterID* base, const Identifier& property, RegisterID* getter, RegisterID* setter);
        
        ExpectedFunction expectedFunctionForIdentifier(const Identifier&);
        RegisterID* emitCall(RegisterID* dst, RegisterID* func, ExpectedFunction, CallArguments&, const JSTextPosition& divot, const JSTextPosition& divotStart, const JSTextPosition& divotEnd);
        RegisterID* emitCallEval(RegisterID* dst, RegisterID* func, CallArguments&, const JSTextPosition& divot, const JSTextPosition& divotStart, const JSTextPosition& divotEnd);
        RegisterID* emitCallVarargs(RegisterID* dst, RegisterID* func, RegisterID* thisRegister, RegisterID* arguments, RegisterID* firstFreeRegister, int32_t firstVarArgOffset, RegisterID* profileHookRegister, const JSTextPosition& divot, const JSTextPosition& divotStart, const JSTextPosition& divotEnd);

        void emitEnumeration(ThrowableExpressionData* enumerationNode, ExpressionNode* subjectNode, const std::function<void(BytecodeGenerator&, RegisterID*)>& callBack);
        
        RegisterID* emitReturn(RegisterID* src);
        RegisterID* emitEnd(RegisterID* src) { return emitUnaryNoDstOp(op_end, src); }

        RegisterID* emitConstruct(RegisterID* dst, RegisterID* func, ExpectedFunction, CallArguments&, const JSTextPosition& divot, const JSTextPosition& divotStart, const JSTextPosition& divotEnd);
        RegisterID* emitStrcat(RegisterID* dst, RegisterID* src, int count);
        void emitToPrimitive(RegisterID* dst, RegisterID* src);

        ResolveType resolveType();
        RegisterID* emitResolveConstantLocal(RegisterID* dst, const Identifier&, ResolveScopeInfo&);
        RegisterID* emitResolveScope(RegisterID* dst, const Identifier&, ResolveScopeInfo&);
        RegisterID* emitGetFromScope(RegisterID* dst, RegisterID* scope, const Identifier&, ResolveMode, const ResolveScopeInfo&);
        RegisterID* emitPutToScope(RegisterID* scope, const Identifier&, RegisterID* value, ResolveMode, const ResolveScopeInfo&);

        PassRefPtr<Label> emitLabel(Label*);
        void emitLoopHint();
        PassRefPtr<Label> emitJump(Label* target);
        PassRefPtr<Label> emitJumpIfTrue(RegisterID* cond, Label* target);
        PassRefPtr<Label> emitJumpIfFalse(RegisterID* cond, Label* target);
        PassRefPtr<Label> emitJumpIfNotFunctionCall(RegisterID* cond, Label* target);
        PassRefPtr<Label> emitJumpIfNotFunctionApply(RegisterID* cond, Label* target);
        void emitPopScopes(RegisterID* srcDst, int targetScopeDepth);

        RegisterID* emitHasIndexedProperty(RegisterID* dst, RegisterID* base, RegisterID* propertyName);
        RegisterID* emitHasStructureProperty(RegisterID* dst, RegisterID* base, RegisterID* propertyName, RegisterID* enumerator);
        RegisterID* emitHasGenericProperty(RegisterID* dst, RegisterID* base, RegisterID* propertyName);
        RegisterID* emitGetEnumerableLength(RegisterID* dst, RegisterID* base);
        RegisterID* emitGetStructurePropertyEnumerator(RegisterID* dst, RegisterID* base, RegisterID* length);
        RegisterID* emitGetGenericPropertyEnumerator(RegisterID* dst, RegisterID* base, RegisterID* length, RegisterID* structureEnumerator);
        RegisterID* emitNextEnumeratorPropertyName(RegisterID* dst, RegisterID* enumerator, RegisterID* index);
        RegisterID* emitToIndexString(RegisterID* dst, RegisterID* index);

        void emitReadOnlyExceptionIfNeeded();

        // Start a try block. 'start' must have been emitted.
        TryData* pushTry(Label* start);
        // End a try block. 'end' must have been emitted.
        RegisterID* popTryAndEmitCatch(TryData*, RegisterID* targetRegister, Label* end);

        void emitThrow(RegisterID* exc)
        { 
            m_usesExceptions = true;
            emitUnaryNoDstOp(op_throw, exc);
        }

        void emitThrowReferenceError(const String& message);

        void emitPushFunctionNameScope(RegisterID* dst, const Identifier& property, RegisterID* value, unsigned attributes);
        void emitPushCatchScope(RegisterID* dst, const Identifier& property, RegisterID* value, unsigned attributes);

        void emitGetScope();
        RegisterID* emitPushWithScope(RegisterID* dst, RegisterID* scope);
        void emitPopScope(RegisterID* srcDst);

        void emitDebugHook(DebugHookID, unsigned line, unsigned charOffset, unsigned lineStart);

        int scopeDepth() { return m_localScopeDepth + m_finallyDepth; }
        bool hasFinaliser() { return m_finallyDepth != 0; }

        void pushFinallyContext(StatementNode* finallyBlock);
        void popFinallyContext();

        void pushIndexedForInScope(RegisterID* local, RegisterID* index);
        void popIndexedForInScope(RegisterID* local);
        void pushStructureForInScope(RegisterID* local, RegisterID* index, RegisterID* property, RegisterID* enumerator);
        void popStructureForInScope(RegisterID* local);
        void invalidateForInContextForLocal(RegisterID* local);

        LabelScopePtr breakTarget(const Identifier&);
        LabelScopePtr continueTarget(const Identifier&);

        void beginSwitch(RegisterID*, SwitchInfo::SwitchType);
        void endSwitch(uint32_t clauseCount, RefPtr<Label>*, ExpressionNode**, Label* defaultLabel, int32_t min, int32_t range);

        CodeType codeType() const { return m_codeType; }

        bool shouldEmitProfileHooks() { return m_shouldEmitProfileHooks; }
        bool shouldEmitDebugHooks() { return m_shouldEmitDebugHooks; }
        
        bool isStrictMode() const { return m_codeBlock->isStrictMode(); }
        
        bool isBuiltinFunction() const { return m_isBuiltinFunction; }

        OpcodeID lastOpcodeID() const { return m_lastOpcodeID; }

    private:
        void emitOpcode(OpcodeID);
        UnlinkedArrayAllocationProfile newArrayAllocationProfile();
        UnlinkedObjectAllocationProfile newObjectAllocationProfile();
        UnlinkedArrayProfile newArrayProfile();
        UnlinkedValueProfile emitProfiledOpcode(OpcodeID);
        int kill(RegisterID* dst)
        {
            int index = dst->index();
            m_staticPropertyAnalyzer.kill(index);
            return index;
        }

        void retrieveLastBinaryOp(int& dstIndex, int& src1Index, int& src2Index);
        void retrieveLastUnaryOp(int& dstIndex, int& srcIndex);
        ALWAYS_INLINE void rewindBinaryOp();
        ALWAYS_INLINE void rewindUnaryOp();

        void allocateAndEmitScope();
        void emitComplexPopScopes(RegisterID*, ControlFlowContext* topScope, ControlFlowContext* bottomScope);

        typedef HashMap<double, JSValue> NumberMap;
        typedef HashMap<StringImpl*, JSString*, IdentifierRepHash> IdentifierStringMap;
        
        // Helper for emitCall() and emitConstruct(). This works because the set of
        // expected functions have identical behavior for both call and construct
        // (i.e. "Object()" is identical to "new Object()").
        ExpectedFunction emitExpectedFunctionSnippet(RegisterID* dst, RegisterID* func, ExpectedFunction, CallArguments&, Label* done);
        
        RegisterID* emitCall(OpcodeID, RegisterID* dst, RegisterID* func, ExpectedFunction, CallArguments&, const JSTextPosition& divot, const JSTextPosition& divotStart, const JSTextPosition& divotEnd);

        RegisterID* newRegister();

        // Adds a var slot and maps it to the name ident in symbolTable().
        enum WatchMode { IsWatchable, NotWatchable };
        RegisterID* addVar(const Identifier& ident, ConstantMode constantMode, WatchMode watchMode)
        {
            RegisterID* local;
            addVar(ident, constantMode, watchMode, local);
            return local;
        }

        // Ditto. Returns true if a new RegisterID was added, false if a pre-existing RegisterID was re-used.
        bool addVar(const Identifier&, ConstantMode, WatchMode, RegisterID*&);
        
        // Adds an anonymous var slot. To give this slot a name, add it to symbolTable().
        RegisterID* addVar()
        {
            ++m_codeBlock->m_numVars;
            RegisterID* result = newRegister();
            ASSERT(VirtualRegister(result->index()).toLocal() == m_codeBlock->m_numVars - 1);
            result->ref(); // We should never free this slot.
            return result;
        }

        // Returns the index of the added var.
        void addParameter(const Identifier&, int parameterIndex);
        RegisterID* resolveCallee(FunctionNode*);
        void addCallee(FunctionNode*, RegisterID*);

        void preserveLastVar();

        RegisterID& registerFor(int index)
        {
            if (operandIsLocal(index))
                return m_calleeRegisters[VirtualRegister(index).toLocal()];

            if (index == JSStack::Callee)
                return m_calleeRegister;

            ASSERT(m_parameters.size());
            return m_parameters[VirtualRegister(index).toArgument()];
        }

        bool hasConstant(const Identifier&) const;
        unsigned addConstant(const Identifier&);
        RegisterID* addConstantValue(JSValue);
        RegisterID* addConstantEmptyValue();
        unsigned addRegExp(RegExp*);

        unsigned addConstantBuffer(unsigned length);
        
        UnlinkedFunctionExecutable* makeFunction(FunctionBodyNode* body)
        {
            return UnlinkedFunctionExecutable::create(m_vm, m_scopeNode->source(), body, isBuiltinFunction() ? UnlinkedBuiltinFunction : UnlinkedNormalFunction);
        }

        RegisterID* emitInitLazyRegister(RegisterID*);
        
        RegisterID* emitConstructVarargs(RegisterID* dst, RegisterID* func, RegisterID* thisRegister, RegisterID* arguments, RegisterID* firstFreeRegister, int32_t firstVarArgOffset, RegisterID* profileHookRegister, const JSTextPosition& divot, const JSTextPosition& divotStart, const JSTextPosition& divotEnd);
        RegisterID* emitCallVarargs(OpcodeID, RegisterID* dst, RegisterID* func, RegisterID* thisRegister, RegisterID* arguments, RegisterID* firstFreeRegister, int32_t firstVarArgOffset, RegisterID* profileHookRegister, const JSTextPosition& divot, const JSTextPosition& divotStart, const JSTextPosition& divotEnd);
        RegisterID* initializeCapturedVariable(RegisterID* dst, const Identifier&, RegisterID*);

        // We'll may want a non-return mode in future, but currently
        // this is only used during emitReturn(). emitReturn() occurs
        // with the novel state of having popped off all the local scope
        // nodes, but not actually modify any internal stack depth tracking.
        enum OwnScopeLookupRules { OwnScopeForReturn };
        RegisterID* emitGetOwnScope(RegisterID* dst, const Identifier&, OwnScopeLookupRules);

    public:
        JSString* addStringConstant(const Identifier&);

        Vector<UnlinkedInstruction, 0, UnsafeVectorOverflow>& instructions() { return m_instructions; }

        SymbolTable& symbolTable() { return *m_symbolTable; }

        bool shouldOptimizeLocals()
        {
            if (m_codeType != FunctionCode)
                return false;

            if (m_localScopeDepth)
                return false;

            return true;
        }

        bool canOptimizeNonLocals()
        {
            if (m_localScopeDepth)
                return false;

            if (m_codeType == EvalCode)
                return false;

            if (m_codeType == FunctionCode && m_codeBlock->usesEval())
                return false;

            return true;
        }

        bool shouldTearOffArgumentsEagerly()
        {
            return m_codeType == FunctionCode && isStrictMode() && m_scopeNode->modifiesParameter();
        }

        bool shouldCreateArgumentsEagerly()
        {
            if (m_codeType != FunctionCode)
                return false;
            return m_lexicalEnvironmentRegister && m_codeBlock->usesArguments();
        }

        RegisterID* emitThrowExpressionTooDeepException();

        void createArgumentsIfNecessary();
        RegisterID* createLazyRegisterIfNecessary(RegisterID*);
        
        bool hasWatchableVariable(int operand) const
        {
            VirtualRegister reg(operand);
            if (!reg.isLocal())
                return false;
            if (static_cast<size_t>(reg.toLocal()) >= m_watchableVariables.size())
                return false;
            const Identifier& ident = m_watchableVariables[reg.toLocal()];
            if (ident.isNull())
                return false;
            ASSERT(hasConstant(ident)); // Should have already been added.
            return true;
        }
        
        const Identifier& watchableVariableIdentifier(int operand) const
        {
            ASSERT(hasWatchableVariable(operand));
            VirtualRegister reg(operand);
            return m_watchableVariables[reg.toLocal()];
        }

    private:
        Vector<UnlinkedInstruction, 0, UnsafeVectorOverflow> m_instructions;

        bool m_shouldEmitDebugHooks;
        bool m_shouldEmitProfileHooks;

        SymbolTable* m_symbolTable { nullptr };

        ScopeNode* const m_scopeNode;
        Strong<UnlinkedCodeBlock> m_codeBlock;

        // Some of these objects keep pointers to one another. They are arranged
        // to ensure a sane destruction order that avoids references to freed memory.
        HashSet<RefPtr<StringImpl>, IdentifierRepHash> m_functions;
        RegisterID m_ignoredResultRegister;
        RegisterID m_thisRegister;
        RegisterID m_calleeRegister;
        RegisterID* m_scopeRegister { nullptr };
        RegisterID* m_lexicalEnvironmentRegister { nullptr };
        RegisterID* m_emptyValueRegister { nullptr };
        RegisterID* m_globalObjectRegister { nullptr };
        RegisterID* m_localArgumentsRegister { nullptr };

        Vector<Identifier, 16> m_watchableVariables;
        SegmentedVector<RegisterID, 32> m_constantPoolRegisters;
        SegmentedVector<RegisterID, 32> m_calleeRegisters;
        SegmentedVector<RegisterID, 32> m_parameters;
        SegmentedVector<Label, 32> m_labels;
        LabelScopeStore m_labelScopes;
        int m_finallyDepth { 0 };
        int m_localScopeDepth { 0 };
        const CodeType m_codeType;

        Vector<ControlFlowContext, 0, UnsafeVectorOverflow> m_scopeContextStack;
        Vector<SwitchInfo> m_switchContextStack;
        Vector<std::unique_ptr<ForInContext>> m_forInContextStack;
        Vector<TryContext> m_tryContextStack;
        Vector<std::pair<RefPtr<RegisterID>, const DeconstructionPatternNode*>> m_deconstructedParameters;
        
        Vector<TryRange> m_tryRanges;
        SegmentedVector<TryData, 8> m_tryData;

        int m_nextConstantOffset { 0 };

        int m_firstLazyFunction { 0 };
        int m_lastLazyFunction { 0 };
        HashMap<unsigned int, FunctionBodyNode*, WTF::IntHash<unsigned int>, WTF::UnsignedWithZeroKeyHashTraits<unsigned int>> m_lazyFunctions;
        typedef HashMap<FunctionBodyNode*, unsigned> FunctionOffsetMap;
        FunctionOffsetMap m_functionOffsets;
        
        // Constant pool
        IdentifierMap m_identifierMap;
        JSValueMap m_jsValueMap;
        IdentifierStringMap m_stringMap;

        StaticPropertyAnalyzer m_staticPropertyAnalyzer { &m_instructions };

        VM* m_vm;

        OpcodeID m_lastOpcodeID = op_end;
#ifndef NDEBUG
        size_t m_lastOpcodePosition { 0 };
#endif

        bool m_usesExceptions { false };
        bool m_expressionTooDeep { false };
        bool m_isBuiltinFunction { false };
    };

}

#endif // BytecodeGenerator_h
