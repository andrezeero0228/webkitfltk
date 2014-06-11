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
#include "JSTestMediaQueryListListener.h"

#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSMediaQueryListListener.h"
#include "TestMediaQueryListListener.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table for constructor */

static const struct CompactHashIndex JSTestMediaQueryListListenerConstructorTableIndex[1] = {
    { -1, -1 },
};


static const HashTableValue JSTestMediaQueryListListenerConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestMediaQueryListListenerConstructorTable = { 0, 0, false, JSTestMediaQueryListListenerConstructorTableValues, 0, JSTestMediaQueryListListenerConstructorTableIndex };
const ClassInfo JSTestMediaQueryListListenerConstructor::s_info = { "TestMediaQueryListListenerConstructor", &Base::s_info, &JSTestMediaQueryListListenerConstructorTable, 0, CREATE_METHOD_TABLE(JSTestMediaQueryListListenerConstructor) };

JSTestMediaQueryListListenerConstructor::JSTestMediaQueryListListenerConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSTestMediaQueryListListenerConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSTestMediaQueryListListenerPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSTestMediaQueryListListenerConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSTestMediaQueryListListenerConstructor, JSDOMWrapper>(exec, JSTestMediaQueryListListenerConstructorTable, jsCast<JSTestMediaQueryListListenerConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSTestMediaQueryListListenerPrototypeTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestMediaQueryListListenerConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
    { "method", JSC::Function, NoIntrinsic, (intptr_t)static_cast<NativeFunction>(jsTestMediaQueryListListenerPrototypeFunctionMethod), (intptr_t) (1) },
};

const ClassInfo JSTestMediaQueryListListenerPrototype::s_info = { "TestMediaQueryListListenerPrototype", &Base::s_info, 0, 0, CREATE_METHOD_TABLE(JSTestMediaQueryListListenerPrototype) };

JSObject* JSTestMediaQueryListListenerPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTestMediaQueryListListener>(vm, globalObject);
}

void JSTestMediaQueryListListenerPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestMediaQueryListListenerPrototypeTableValues, *this);
}

const ClassInfo JSTestMediaQueryListListener::s_info = { "TestMediaQueryListListener", &Base::s_info, 0, 0 , CREATE_METHOD_TABLE(JSTestMediaQueryListListener) };

JSTestMediaQueryListListener::JSTestMediaQueryListListener(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<TestMediaQueryListListener> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

JSObject* JSTestMediaQueryListListener::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTestMediaQueryListListenerPrototype::create(vm, globalObject, JSTestMediaQueryListListenerPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSTestMediaQueryListListener::destroy(JSC::JSCell* cell)
{
    JSTestMediaQueryListListener* thisObject = static_cast<JSTestMediaQueryListListener*>(cell);
    thisObject->JSTestMediaQueryListListener::~JSTestMediaQueryListListener();
}

JSTestMediaQueryListListener::~JSTestMediaQueryListListener()
{
    releaseImplIfNotNull();
}

EncodedJSValue jsTestMediaQueryListListenerConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue, PropertyName)
{
    JSTestMediaQueryListListenerPrototype* domObject = jsDynamicCast<JSTestMediaQueryListListenerPrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSTestMediaQueryListListener::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSTestMediaQueryListListener::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestMediaQueryListListenerConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

EncodedJSValue JSC_HOST_CALL jsTestMediaQueryListListenerPrototypeFunctionMethod(ExecState* exec)
{
    JSValue thisValue = exec->thisValue();
    JSTestMediaQueryListListener* castedThis = jsDynamicCast<JSTestMediaQueryListListener*>(thisValue);
    if (UNLIKELY(!castedThis))
        return throwThisTypeError(*exec, "TestMediaQueryListListener", "method");
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSTestMediaQueryListListener::info());
    TestMediaQueryListListener& impl = castedThis->impl();
    if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
    if (!exec->argument(0).isFunction())
        return throwArgumentMustBeFunctionError(*exec, 0, "listener", "TestMediaQueryListListener", "method");
    RefPtr<MediaQueryListListener> listener = JSMediaQueryListListener::create(asObject(exec->uncheckedArgument(0)), castedThis->globalObject());
    impl.method(listener);
    return JSValue::encode(jsUndefined());
}

bool JSTestMediaQueryListListenerOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSTestMediaQueryListListenerOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSTestMediaQueryListListener* jsTestMediaQueryListListener = jsCast<JSTestMediaQueryListListener*>(handle.slot()->asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestMediaQueryListListener->impl(), jsTestMediaQueryListListener);
    jsTestMediaQueryListListener->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestMediaQueryListListener@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore26TestMediaQueryListListenerE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject* globalObject, TestMediaQueryListListener* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSTestMediaQueryListListener>(globalObject, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7TestMediaQueryListListener@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore26TestMediaQueryListListenerE[2];
#if COMPILER(CLANG)
    // If this fails TestMediaQueryListListener does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(TestMediaQueryListListener), TestMediaQueryListListener_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // TestMediaQueryListListener has subclasses. If TestMediaQueryListListener has subclasses that get passed
    // to toJS() we currently require TestMediaQueryListListener you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createNewWrapper<JSTestMediaQueryListListener>(globalObject, impl);
}

TestMediaQueryListListener* toTestMediaQueryListListener(JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestMediaQueryListListener*>(value))
        return &wrapper->impl();
    return nullptr;
}

}
