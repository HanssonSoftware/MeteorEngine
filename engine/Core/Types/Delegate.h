/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

//* What the hell is this?
template<typename ReturnType, typename... Args>
class Delegate
{
private:
	using ThunkType = ReturnType(*)(void*, Args...);

	void* m_Instance = nullptr;
	ThunkType m_Thunk = nullptr;

	template<class T, ReturnType(T::*Func)(Args...)>
	static ReturnType MethodThunk(void* instance, Args... args)
	{
		return (static_cast<T*>(instance)->*Func)(args...);
	}

	template<class T, ReturnType(T::* Func)(Args...) const>
	static ReturnType ConstMethodThunk(void* instance, Args... args)
	{
		return (static_cast<const T*>(instance)->*Func)(args...);
	}

	template<ReturnType(*Func)(Args...)>
	static ReturnType StaticThunk(void*, Args... args)
	{
		return Func(args...);
	}

public:
	Delegate() noexcept = default;

	template<class T, ReturnType(T::* Func)(Args...)>
	void Bind(T* instance) noexcept
	{
		m_Instance = instance;
		m_Thunk = &MethodThunk<T, Func>;
	}

	template<class T, ReturnType(T::* Func)(Args...) const>
	void Bind(const T* instance) noexcept
	{
		m_Instance = const_cast<void*>(static_cast<const void*>(instance));
		m_Thunk = &ConstMethodThunk<T, Func>;
	}

	template<ReturnType(*Func)(Args...)>
	void Bind() noexcept
	{
		m_Instance = nullptr;
		m_Thunk = &StaticThunk<Func>;
	}

	void Unbind() noexcept
	{
		m_Instance = nullptr;
		m_Thunk = nullptr;
	}

	inline bool IsBound() const noexcept
	{
		return m_Thunk != nullptr;
	}

	inline ReturnType Execute(Args... args) const
	{
		return m_Thunk(m_Instance, args...);
	}

	inline ReturnType operator()(Args... args) const
	{
		return Execute(args...);
	}
};