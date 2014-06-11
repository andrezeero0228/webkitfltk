/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "JSTestEventConstructor.h"

#include "JSDictionary.h"
#include "ScriptExecutionContext.h"
#include "TestEventConstructor.h"
#include "URL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table for constructor */

static const struct CompactHashIndex JSTestEventConstructorConstructorTableIndex[1] = {
    { -1, -1 },
};


static const HashTableValue JSTestEventConstructorConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestEventConstructorConstructorTable = { 0, 0, false, JSTestEventConstructorConstructorTableValues, 0, JSTestEventConstructorConstructorTableIndex };
EncodedJSValue JSC_HOST_CALL JSTestEventConstructorConstructor::constructJSTestEventConstructor(ExecState* exec)
{
    JSTestEventConstructorConstructor* jsConstructor = jsCast<JSTestEventConstructorConstructor*>(exec->callee());

    ScriptExecutionContext* executionContext = jsConstructor->scriptExecutionContext();
    if (!executionContext)
        return throwVMError(exec, createReferenceError(exec, "Constructor associated execution context is unavailable"));

    AtomicString eventType = exec->argument(0).toString(exec)->value(exec);
    if (UNLIKELY(exec->hadException()))
        return JSValue::encode(jsUndefined());

    TestEventConstructorInit eventInit;

    JSValue initializerValue = exec->argument(1);
    if (!initializerValue.isUndefinedOrNull()) {
        // Given the above test, this will always yield an object.
        JSObject* initializerObject = initializerValue.toObject(exec);

        // Create the dictionary wrapper from the initializer object.
        JSDictionary dictionary(exec, initializerObject);

        // Attempt to fill in the EventInit.
        if (!fillTestEventConstructorInit(eventInit, dictionary))
            return JSValue::encode(jsUndefined());
    }

    RefPtr<TestEventConstructor> event = TestEventConstructor::create(eventType, eventInit);
    return JSValue::encode(toJS(exec, jsConstructor->globalObject(), event.get()));
}

bool fillTestEventConstructorInit(TestEventConstructorInit& eventInit, JSDictionary& dictionary)
{
    if (!dictionary.tryGetProperty("attr2", eventInit.attr2))
        return false;
    return true;
}

const ClassInfo JSTestEventConstructorConstructor::s_info = { "TestEventConstructorConstructor", &Base::s_info, &JSTestEventConstructorConstructorTable, 0, CREATE_METHOD_TABLE(JSTestEventConstructorConstructor) };

JSTestEventConstructorConstructor::JSTestEventConstructorConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSTestEventConstructorConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSTestEventConstructorPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(1), ReadOnly | DontDelete | DontEnum);
}

bool JSTestEventConstructorConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSTestEventConstructorConstructor, JSDOMWrapper>(exec, JSTestEventConstructorConstructorTable, jsCast<JSTestEventConstructorConstructor*>(object), propertyName, slot);
}

ConstructType JSTestEventConstructorConstructor::getConstructData(JSCell*, ConstructData& constructData)
{
    constructData.native.function = constructJSTestEventConstructor;
    return ConstructTypeHost;
}

/* Hash table for prototype */

static const HashTableValue JSTestEventConstructorPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestEventConstructorConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "attr1", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestEventConstructorAttr1), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "attr2", DontDelete | ReadOnly | CustomAccessor, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestEventConstructorAttr2), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

const ClassInfo JSTestEventConstructorPrototype::s_info = { "TestEventConstructorPrototype", &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSTestEventConstructorPrototype) };

JSObject* JSTestEventConstructorPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTestEventConstructor>(vm, globalObject);
}

void JSTestEventConstructorPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestEventConstructorPrototypeTableValues, *this);
}

const ClassInfo JSTestEventConstructor::s_info = { "TestEventConstructor", &Base::s_info, 0, 0 , CREATE_METHOD_TABLE(JSTestEventConstructor) };

JSTestEventConstructor::JSTestEventConstructor(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<TestEventConstructor> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

JSObject* JSTestEventConstructor::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTestEventConstructorPrototype::create(vm, globalObject, JSTestEventConstructorPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSTestEventConstructor::destroy(JSC::JSCell* cell)
{
    JSTestEventConstructor* thisObject = static_cast<JSTestEventConstructor*>(cell);
    thisObject->JSTestEventConstructor::~JSTestEventConstructor();
}

JSTestEventConstructor::~JSTestEventConstructor()
{
    releaseImplIfNotNull();
}

EncodedJSValue jsTestEventConstructorAttr1(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTestEventConstructor* castedThis = jsDynamicCast<JSTestEventConstructor*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTestEventConstructorPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TestEventConstructor", "attr1");
        return throwGetterTypeError(*exec, "TestEventConstructor", "attr1");
    }
    TestEventConstructor& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.attr1());
    return JSValue::encode(result);
}


EncodedJSValue jsTestEventConstructorAttr2(ExecState* exec, JSObject* slotBase, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(exec);
    UNUSED_PARAM(slotBase);
    UNUSED_PARAM(thisValue);
    JSTestEventConstructor* castedThis = jsDynamicCast<JSTestEventConstructor*>(JSValue::decode(thisValue));
    if (UNLIKELY(!castedThis)) {
        if (jsDynamicCast<JSTestEventConstructorPrototype*>(slotBase))
            return reportDeprecatedGetterError(*exec, "TestEventConstructor", "attr2");
        return throwGetterTypeError(*exec, "TestEventConstructor", "attr2");
    }
    TestEventConstructor& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.attr2());
    return JSValue::encode(result);
}


EncodedJSValue jsTestEventConstructorConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSTestEventConstructorPrototype* domObject = jsDynamicCast<JSTestEventConstructorPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSTestEventConstructor::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSTestEventConstructor::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestEventConstructorConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

bool JSTestEventConstructorOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSTestEventConstructorOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSTestEventConstructor* jsTestEventConstructor = jsCast<JSTestEventConstructor*>(handle.slot()->asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestEventConstructor->impl(), jsTestEventConstructor);
    jsTestEventConstructor->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestEventConstructor@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore20TestEventConstructorE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, TestEventConstructor* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSTestEventConstructor>(globalObject, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7TestEventConstructor@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore20TestEventConstructorE[2];
#if COMPILER(CLANG)
    // If this fails TestEventConstructor does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(TestEventConstructor), TestEventConstructor_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // TestEventConstructor has subclasses. If TestEventConstructor has subclasses that get passed
    // to toJS() we currently require TestEventConstructor you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createNewWrapper<JSTestEventConstructor>(globalObject, impl);
}

TestEventConstructor* toTestEventConstructor(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestEventConstructor*>(value))
        return &wrapper->impl();
    return nullptr;
}

}
