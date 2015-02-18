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

#ifndef JSTestCustomNamedGetter_h
#define JSTestCustomNamedGetter_h

#include "JSDOMWrapper.h"
#include "TestCustomNamedGetter.h"

namespace WebCore {

class WEBCORE_EXPORT JSTestCustomNamedGetter : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSTestCustomNamedGetter* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, Ref<TestCustomNamedGetter>&& impl)
    {
        JSTestCustomNamedGetter* ptr = new (NotNull, JSC::allocateCell<JSTestCustomNamedGetter>(globalObject->vm().heap)) JSTestCustomNamedGetter(structure, globalObject, WTF::move(impl));
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static JSC::JSObject* getPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static TestCustomNamedGetter* toWrapped(JSC::JSValue);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static bool getOwnPropertySlotByIndex(JSC::JSObject*, JSC::ExecState*, unsigned propertyName, JSC::PropertySlot&);
    static void destroy(JSC::JSCell*);
    ~JSTestCustomNamedGetter();

    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    TestCustomNamedGetter& impl() const { return *m_impl; }
    void releaseImpl() { m_impl->deref(); m_impl = 0; }

    void releaseImplIfNotNull()
    {
        if (m_impl) {
            m_impl->deref();
            m_impl = 0;
        }
    }

private:
    TestCustomNamedGetter* m_impl;
protected:
    JSTestCustomNamedGetter(JSC::Structure*, JSDOMGlobalObject*, Ref<TestCustomNamedGetter>&&);

    void finishCreation(JSC::VM& vm)
    {
        Base::finishCreation(vm);
        ASSERT(inherits(info()));
    }

    static const unsigned StructureFlags = JSC::HasImpureGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
private:
    static bool canGetItemsForName(JSC::ExecState*, TestCustomNamedGetter*, JSC::PropertyName);
    static JSC::EncodedJSValue nameGetter(JSC::ExecState*, JSC::JSObject*, JSC::EncodedJSValue, JSC::PropertyName);
};

class JSTestCustomNamedGetterOwner : public JSC::WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld&, TestCustomNamedGetter*)
{
    DEPRECATED_DEFINE_STATIC_LOCAL(JSTestCustomNamedGetterOwner, jsTestCustomNamedGetterOwner, ());
    return &jsTestCustomNamedGetterOwner;
}

WEBCORE_EXPORT JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, TestCustomNamedGetter*);
inline JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, TestCustomNamedGetter& impl) { return toJS(exec, globalObject, &impl); }


} // namespace WebCore

#endif
