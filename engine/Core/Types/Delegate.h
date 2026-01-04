/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "Traits.h"

constexpr inline int MR_DEFAULT_DELEGATE_SIZE = 512;
constexpr inline int MR_GIGANTIC_DELEGATE_SIZE = 4096;

template<typename RetVal, int MaxBindings = MR_DEFAULT_DELEGATE_SIZE, typename... Args>
class Delegate
{
    using StubType = RetVal(*)(void*, Args...);

    struct Binding
    {
        void* m_Object = nullptr;
        StubType m_Stub = nullptr;
    };

    template<RetVal(*StaticFunc)(Args...)>
    static RetVal StaticStub(void* obj, Args... args)
    {
        return (StaticFunc)(args...);
    }

    template<class C, RetVal(C::*MemberFunc)(Args...)>
    static RetVal MemberStub(void* obj, Args... args)
    {
        C* realObject = static_cast<C*>(obj);
        return (realObject->*MemberFunc)(args...);
    }

public:
    constexpr Delegate() 
        : m_Count(0) 
    {
    
    }

    template<RetVal(*StaticFunc)(Args...)>
    void Bind()
    {
        if (m_Count >= MaxBindings) return;

        m_Bindings[m_Count].m_Object = nullptr;
        m_Bindings[m_Count].m_Stub = &StaticStub<StaticFunc>;
        m_Count++;
    }

    template<class C, RetVal(C::* MemberFunc)(Args...)>
    void Bind(C* object)
    {
        if (m_Count >= MaxBindings) return;

        m_Bindings[m_Count].m_Object = object;
        m_Bindings[m_Count].m_Stub = &MemberStub<C, MemberFunc>;
        m_Count++;
    }

    RetVal Broadcast(Args... args)
    {
        if constexpr (same_v<RetVal, void>)
        {
            for (int i = 0; i < m_Count; ++i)
            {
                m_Bindings[i].m_Stub(m_Bindings[i].m_Object, args...);
            }

            return;
        }
        else
        {
            RetVal result = RetVal();
            for (int i = 0; i < m_Count; ++i)
            {
                result = m_Bindings[i].m_Stub(m_Bindings[i].m_Object, args...);
            }

            return result;
        }
    }

    constexpr void Clear()
    {
        m_Count = 0;
    }

    constexpr int GetSize() const
    {
        return m_Count;
    }

private:
    Binding m_Bindings[MaxBindings]{};
    int m_Count = 0;
};


#define CREATE_DELEGATE(Name, ...) \
    typedef Delegate<void, MR_DEFAULT_DELEGATE_SIZE, __VA_ARGS__> Del##Name

#define CREATE_DELEGATE_RETVAL(RetVal, Name, ...) \
    typedef Delegate<RetVal, MR_DEFAULT_DELEGATE_SIZE, __VA_ARGS__> DelRet##Name

#define CREATE_GIGANTIC_DELEGATE(Name, ...) \
    typedef Delegate<void, MR_GIGANTIC_DELEGATE_SIZE, __VA_ARGS__> DelGig##Name