#pragma once

#include <type_traits>

namespace Noble
{
	/**
	 * Returns the proper printf-style format specifier for the given type
	 */
	template <typename Integer, 
		typename std::enable_if<(std::is_integral<Integer>::value && std::is_signed<Integer>::value), int>::type = 0>
	const char* GetPrintFormat()
	{
		// signed integer formats
		return "%i";
	}

	/**
	 * Returns the proper printf-style format specifier for the given type
	 */
	template <typename UnsignedInteger,
		typename std::enable_if<(std::is_integral<UnsignedInteger>::value && std::is_unsigned<UnsignedInteger>::value), int>::type = 0>
		const char* GetPrintFormat()
	{
		// unsigned integer formats
		return "%u";
	}

	/**
	 * Returns the proper printf-style format specifier for the given type
	 */
	template <typename Floating,
		typename std::enable_if<std::is_floating_point<Floating>::value, int>::type = 0>
		const char* GetPrintFormat()
	{
		// float format
		return "%f";
	}

	/**
	 * Returns the proper printf-style format specifier for the given type
	 */
	template <typename Pointer,
		typename std::enable_if<std::is_pointer<Pointer>::value, int>::type = 0>
		const char* GetPrintFormat()
	{
		// pointer format
		return "%p";
	}
}