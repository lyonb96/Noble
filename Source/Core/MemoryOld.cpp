#include "Memory.h"

#include <memory>
#include <cmath>

#include "Logger.h"

namespace Noble
{
	void* Align(void* in, Size align, Size offset)
	{
#ifdef _DEBUG
		if (!std::_Is_pow_2(align))
		{
			NE_LOG_ERROR("Given alignment value [" << align << "] is not a power of two! Returning original address");
			return in;
		}
#endif

		union
		{
			void* as_void;
			std::uintptr_t as_uint;
		};

		as_void = in;
		as_uint += offset;

		as_uint = (as_uint + (align - 1)) & (~align);

		return as_void;
	}

	MemoryArea::MemoryArea()
	{
		m_Size = 0;
		m_MemPtr = 0;
		m_EndPtr = 0;
	}

	MemoryArea::MemoryArea(const Size& areaSize)
	{
		m_Size = areaSize;
		m_MemPtr = malloc(areaSize);

		union
		{
			void* asVoid;
			char* asChar;
			Size asSize;
		};

		asVoid = m_MemPtr;
		asChar += areaSize;
		m_EndPtr = asVoid;
	}

	void MemoryArea::FreeMemory()
	{
		if (m_MemPtr)
		{
			free(m_MemPtr);
			m_MemPtr = 0;
		}
		m_Size = 0;
	}

	void MemoryArea::Clear()
	{
		memset(m_MemPtr, 0, m_Size);
	}

	bool MemoryArea::CheckContains(void* addr) const
	{
		return (addr >= m_MemPtr && addr < m_EndPtr);
	}

	void StackAllocator::Prepare(const MemoryArea& mem)
	{
		m_Start = m_Head = mem.GetMemory();
		m_End = mem.GetMemoryEnd();
		m_Size = mem.GetSize();
	}

	void* StackAllocator::Allocate(Size size, Size align, Size offset)
	{
		union
		{
			void* as_ptr;
			char* as_char;
		};
		as_ptr = Align(m_Head, align, offset);

		if (as_char + size > m_End)
		{
			return nullptr;
		}

		//as_char += size;

		union
		{
			void* head_as_ptr;
			char* head_as_char;
		};
		head_as_ptr = m_Head;
		head_as_char += size;
		m_Head = head_as_ptr;

		return as_ptr;
	}

	void StackAllocator::FreeAll()
	{
		m_Head = m_Start;
	}
}