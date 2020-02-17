#pragma once

// Used for argument packing in Fat function pointers
#include <tuple>
#include "Types.h"

namespace Noble
{
	template <typename>
	class Function;

	// A brain-dead simple function pointer wrapper
	template <typename ReturnType, typename... FunctionArgs>
	class Function<ReturnType(FunctionArgs...)>
	{
	public:

		typedef ReturnType(*Pointer)(FunctionArgs...);

		Function() = delete;

		// Non-explicit means we can build straight from a compatible function pointer
		Function(Pointer func)
			: m_InternalPointer(func) {}

		// Override the () operator to turn it into a functor, which just pushes the args to the function
		ReturnType operator() (FunctionArgs... argv)
		{
			return m_InternalPointer(argv...);
		}

	private:

		Pointer m_InternalPointer;
	};


	template<typename Obj, typename>
	class MemberFunction;

	/**
	 * TODO: Add assignment operator and copy constructor
	 */
	template<typename Obj, typename ReturnType, typename... FunctionArgs>
	class MemberFunction<Obj, ReturnType(FunctionArgs...)>
	{
	public:

		typedef ReturnType(Obj::*Pointer)(FunctionArgs...);

		MemberFunction() = delete;

		MemberFunction(Obj* obj, Pointer func)
			: m_ObjHandle(obj), m_InternalPointer(func)
		{}

		ReturnType operator() (FunctionArgs... argv)
		{
			return (m_ObjHandle->*m_InternalPointer)(argv...);
		}

	private:

		Obj* m_ObjHandle;
		Pointer m_InternalPointer;
	};

	/**
	 * Fat function pointers actually carry the parameters for the function call
	 */

	template <typename>
	class FatFunction;

	template <typename ReturnType, typename... FunctionArgs>
	class FatFunction<ReturnType(FunctionArgs...)>
	{
	public:

		typedef ReturnType(*Pointer)(FunctionArgs...);

		FatFunction() = delete;

		FatFunction(Pointer func, FunctionArgs... argv)
			: m_InternalPointer(func)
		{
			m_Args = std::tuple<FunctionArgs...>(argv...);
		}

		ReturnType operator() ()
		{
			return CallFunc(m_Args, std::index_sequence_for<FunctionArgs...>());
		}

	private:

		// Calling functions using tuples for arguments is easier with a helper
		template <Size... Is>
		ReturnType CallFunc(const std::tuple<FunctionArgs...>& t, std::index_sequence<Is...>)
		{
			return m_InternalPointer(std::get<Is>(t)...);
		}

	private:

		Pointer m_InternalPointer;
		std::tuple<FunctionArgs...> m_Args;
	};

	/**
	 * Fat member function pointers too
	 */

	template <typename Obj, typename>
	class FatMemberFunction;

	template <typename Obj, typename ReturnType, typename... FunctionArgs>
	class FatMemberFunction<Obj, ReturnType(FunctionArgs...)>
	{

	public:

		typedef ReturnType(Obj::*Pointer)(FunctionArgs...);

		FatMemberFunction() = delete;

		FatMemberFunction(Obj* obj, Pointer func, FunctionArgs... argv)
			: m_ObjHandle(obj), m_InternalPointer(func)
		{
			m_Args = std::tuple<FunctionArgs...>(argv...);
		}

		ReturnType operator() ()
		{
			return CallFunc(m_Args, std::index_sequence_for<FunctionArgs...>());
		}

	private:

		// Calling functions using tuples for arguments is easier with a helper
		template <Size... Is>
		ReturnType CallFunc(const std::tuple<FunctionArgs...>& t, std::index_sequence<Is...>)
		{
			return (m_ObjHandle->*m_InternalPointer)(std::get<Is>(t)...);
		}

	private:

		Obj* m_ObjHandle;
		Pointer m_InternalPointer;
		std::tuple<FunctionArgs...> m_Args;
	};
}