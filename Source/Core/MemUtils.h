#pragma once

#include "Types.h"

#include <cstdint>

namespace Noble
{
	inline char* AlignAddress(char* in, Size align)
	{
		union
		{
			char* in_as_char;
			std::uintptr_t in_as_ptr;
		};

		in_as_char = in;

		return (char*) (in_as_ptr % align);
	}
}