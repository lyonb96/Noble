#pragma once


namespace Noble
{
	/**
	 * Template comparison for type equality
	 *
	 * Based on UE4's TAreTypesEqual
	 */
	template<typename A, typename B>
	struct AreTypesEqual;

	template <typename, typename>
	struct AreTypesEqual
	{
		enum
		{
			Value = false
		};
	};

	template <typename A>
	struct AreTypesEqual<A, A>
	{
		enum
		{
			Value = true
		};
	};

#define ARE_TYPES_EQUAL(A, B) ::Noble::AreTypesEqual<A, B>::Value;
}