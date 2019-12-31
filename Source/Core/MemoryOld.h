#pragma once

// For std::is_pod<T>
#include <type_traits>

#include "HelperMacros.h"
#include "Types.h"

namespace Noble
{
	//void* Align(void* in, Size align, Size offset);

	namespace MemHelper
	{

		template <class T>
		struct IsPOD
		{
			static const bool Value = std::is_pod<T>::value;
		};

		template<bool val>
		struct BoolToType
		{};

		typedef BoolToType<true> PODType;
		typedef BoolToType<false> NonPODType;
	}

	/**
	 * A very simple buffer class that wraps a char[] array and provides index checking
	 * If an out of index read/write occurs, it maps to the nearest index. Definitely
	 * slower than a straight buffer, but also much safer.
	 */
	template <Size N>
	struct SafeBuffer
	{
	private:

		// Raw buffer to manage
		char m_Buffer[N];

	public:

		/**
		 * Safely accesses the given index, or rounds to the nearest index if out of bounds
		 */
		char& operator[](const int i)
		{
			if (i < 0)
			{
				return m_Buffer[0];
			}
			else if (i >= N)
			{
				return m_Buffer[N - 1];
			}
			else
			{
				return m_Buffer[i];
			}
		}

		/**
		 * Implicit char* cast
		 */
		operator char*()
		{
			return m_Buffer;
		}

		/**
		 * Implicit const char* cast
		 */
		operator const char*()
		{
			return m_Buffer;
		}
	};

	/**
	 * Builds and tracks an "area" of memory that is a fixed size
	 */
	class MemoryArea
	{
	protected:

		// Start of the buffer
		void* m_MemPtr;
		// End of the buffer
		void* m_EndPtr;
		// Size of the buffer
		Size m_Size;

	public:

		/**
		 * Simple constructor, just zeroes everything out
		 */
		MemoryArea();

		/**
		 * Allocates a buffer of the given size
		 */
		explicit MemoryArea(const Size& areaSize);

		// No copying
		MemoryArea(const MemoryArea&) = delete;

		/**
		 * Returns a pointer to the start of the buffer
		 */
		void* GetMemory() const { return m_MemPtr; };

		/**
		 * Returns a pointer to the end of the buffer
		 */
		void* GetMemoryEnd() const { return m_EndPtr; }

		/**
		 * Returns the size in bytes of the buffer
		 */
		const Size GetSize() const { return m_Size; };

		/**
		 * Returns true if the given address is in the buffer
		 */
		bool CheckContains(void* addr) const;

		/**
		 * Frees the buffer
		 */
		void FreeMemory();

		/**
		 * Clears the buffer's contents to 0
		 */
		void Clear();
	};

	/**
	 * Memory Arena, manages a MemoryArea with a set allocation strategy
	 * Also optionally tracks allocations to report memory leaks
	 */
	template <class Allocator, class Tracker>
	class MemoryArena
	{
	protected:

		MemoryArea m_Area;
		Allocator m_Alloc;
		Tracker m_Tracker;

	public:

		// No copying
		MemoryArena(const MemoryArena&) = delete;

		// Default constructor doesn't need to do anything
		MemoryArena()
		{
		}

		/**
		 * Builds the memory arena with the given MemoryArea
		 * If this memory area is part of another MemoryArena, you're gonna have a bad time.
		 */
		explicit MemoryArena(const MemoryArea& mem)
		{
			SetMemoryArea(mem);
		}

		/**
		 * Sets the memory area for this Arena, and prepares the allocator
		 */
		void SetMemoryArea(const MemoryArea& mem)
		{
			m_Area = mem;
			m_Alloc.Prepare(mem);
		}

		/**
		 * Attempt to allocate a block of memory with the given alignment
		 * Returns nullptr if the allocation fails
		 */
		void* Allocate(Size size, Size align, const SourceInfo& info, Size offset = 0)
		{
			void* ptr = m_Alloc.Allocate(size, align, offset);
			if (!ptr)
			{
				return nullptr;
			}

			m_Tracker.OnAllocate(ptr, size, info);

			return ptr;
		}

		/**
		 * Checks if the memory is contained in the MemoryArea, and frees it if so
		 */
		void Free(void* in)
		{
			if (m_Area.CheckContains(in) && m_Alloc.CanFree())
			{
				m_Tracker.OnFree(in);
				m_Alloc.Free(in);
			}
		}

		/**
		 * Frees all allocations in this area and sets all of its data to 0s.
		 */
		void FreeAll()
		{
			m_Area.Clear();
			m_Alloc.FreeAll();
			m_Tracker.OnFreeAll();
		}

		/**
		 * On destruction, free the memory in the MemoryArea
		 */
		~MemoryArena()
		{
			m_Area.FreeMemory();
		}
	};

	/**
	 * Simple stack allocator. Allocations cannot be freed independently.
	 */
	class StackAllocator
	{
	protected:

		// Start of the buffer
		void* m_Start;
		// End of the buffer
		void* m_End;
		// Current head of allocations
		void* m_Head;
		// Size of the buffer
		Size m_Size;

	public:

		/**
		 * Prepares the allocator. Just stores start, end, and size info
		 */
		void Prepare(const MemoryArea& mem);

		/**
		 * Attempts to allocate some memory, returns nullptr if there's not enough room
		 */
		void* Allocate(Size size, Size align, Size offset);

		/**
		 * Does nothing, this Stack Allocator does not support independent free operations
		 */
		inline void Free(void* in) {}; // no freeing

		/**
		 * Sets the head pointer back to the start of the buffer
		 */
		void FreeAll();

		/**
		 * Returns false - this allocator does not support Free() calls
		 */
		inline bool CanFree() const { return false; }
	};

	/**
	 * Ring allocator - allocates up to the end of the buffer, then wraps back to the head
	 * Uses a doubly linked list to track blocks of free memory to allow out-of-order frees
	 */
	class RingAllocator
	{
	public:


	};

	/**
	 * An allocator that will only return blocks of exactly the given size. If a request is made
	 * for an allocation that exceeds this size for any reason (including padded bytes, alignment,
	 * etc.) then it will return nullptr.
	 */
	template <Size SIZE>
	class FixedAllocator
	{

	};

	/**
	 * An allocator that simply runs malloc and free for all requests. Hence the name.
	 */
	class DumbAllocator
	{

	};

	/**
	 * Disabled memory tracking - good for release builds
	 */
	class NoTracking
	{
	public:

		/**
		 * Does nothing
		 */
		inline void OnAllocate(void* ptr, Size size, const SourceInfo& sourceInfo) {}

		/**
		 * Still does nothing
		 */
		inline void OnFree(void* ptr) {}

		/**
		 * Yep, still nothing
		 */
		inline void OnFreeAll() {}
	};

	/**
	 * Simple memory tracking, keeps a count of unfreed allocations
	 */
	class SimpleTracking
	{
	private:

		U32 m_Count;

	public:

		/**
		 * Increases the allocation count
		 */
		void OnAllocate(void* ptr, Size size, const SourceInfo& sourceInfo) { m_Count++; }

		/**
		 * Decreases the allocation count
		 */
		void OnFree(void* in) { m_Count--; }

		/**
		 * Sets the allocation count to 0
		 */
		void OnFreeAll() { m_Count = 0; }

		/**
		 * Returns the number of unfreed allocations
		 */
		const U32 GetAllocCount() const { return m_Count; }

		/**
		 * Returns true if there are any unfreed allocations
		 */
		const bool HasAllocs() const { return m_Count > 0; }
	};

	/**
	 * Tracking that logs file and line of unfreed allocations
	 * This one is slow and heavy-weight, but should help greatly in tracking down leaks
	 */
	class InDepthTracking
	{

	};

	// -----------------------------------------------
	// Utility functions for alloc and free
	// -----------------------------------------------

	/**
	 * Allocates space for an array and calls placement new on its members
	 */
	template <class Type, class Arena>
	Type* NewArray(Arena& arena, Size count, const SourceInfo& sourceInfo)
	{
		Size sizeOfObj = sizeof(Type);
		Size totalSize = (sizeOfObj * count) + sizeof(Size);

		union
		{
			void* as_ptr;
			char* as_char;
			Size* as_Size;
			Type* as_type;
		};

		as_ptr = arena.Allocate(totalSize, alignof(Type), sourceInfo, sizeof(Size));

		if (as_ptr)
		{

			*as_Size = count;
			++as_Size;

			for (Size i = 0; i < count; ++i)
			{
				as_type = new (as_ptr) Type;
				++as_type;
			}

			return as_type - count;
		}
		else
		{
			return nullptr;
		}
	}

	/**
	 * Delete function, just provides a clean place for calls to HE_DELETE to redirect
	 * Also determines if destructor needs to be called
	 */
	template <class Arena, class Type>
	void Delete(Arena& arena, Type* ptr)
	{
		Delete(arena, ptr, MemHelper::BoolToType<MemHelper::IsPOD<Type>::Value>());
	}

	/**
	 * POD delete function. POD types don't need their destructors to be called on delete
	 */
	template <class Arena, class Type>
	void Delete(Arena& arena, Type* ptr, MemHelper::PODType)
	{
		arena.Free(ptr);
	}

	/**
	 * Non-POD delete function, calls destructor before calling Free
	 */
	template <class Arena, class Type>
	void Delete(Arena& arena, Type* ptr, MemHelper::NonPODType)
	{
		ptr->~Type();
		arena.Free(ptr);
	}

	/**
	 * Specialty function for deleting arrays, first delegates it to POD or non-POD
	 */
	template <class Arena, class Type>
	void DeleteArray(Type* ptr, Arena& arena)
	{
		DeleteArray(ptr, arena, MemHelper::BoolToType<MemHelper::IsPOD<Type>::Value>());
	}

	/**
	 * POD types don't need their destructor called, so find the original address and free it
	 */
	template <class Arena, class Type>
	void DeleteArray(Type* ptr, Arena& arena, MemHelper::PODType)
	{
		union
		{
			Type* as_type;
			Size* as_Size;
			void* as_void;
		};

		as_type = ptr;
		--as_Size;

		arena.Free(as_void);
	}

	/**
	 * Non-POD types need their destructor called in reverse order
	 */
	template <class Arena, class Type>
	void DeleteArray(Type* ptr, Arena& arena, MemHelper::NonPODType)
	{
		union
		{
			Type* as_type;
			Size* as_Size;
			void* as_void;
		};

		as_type = ptr;
		Size count = *(--as_Size);
		// Need to set pointer to the last item of the array
		++as_Size;
		as_type += count;

		// Call destructors in reverse order
		for (Size i = 0; i < count; ++i)
		{
			(as_type--)->~Type();
		}

		// Is this part necessary?
		++as_type;

		--as_Size;

		arena.Free(as_void);
	}

	/**
	 * Allocates a buffer of the given size in the arena
	 */
	template <class Arena>
	void* AllocBuffer(Arena& arena, Size size, Size align, const SourceInfo& si)
	{
		return arena.Allocate(size, align, si);
	}

	/**
	 * Frees a buffer that was allocated from the given arena
	 */
	template <class Arena>
	void FreeBuffer(Arena& arena, void* buf)
	{
		arena.Free(buf);
	}
}

/**
 * Overloaded new operator to support MemoryArena allocs
 */
template <class Arena>
void* operator new(Noble::Size size, Noble::Size align, Arena& arena, const ::Noble::SourceInfo& sourceInfo)
{
	return arena.Allocate(size, align, sourceInfo);
}

// Have to have a matching delete operator in case the overloaded new fails
template <class Arena>
void operator delete(void* ptr, Noble::Size align, Arena& arena, const ::Noble::SourceInfo& sourceInfo)
{
	Noble::Delete(arena, ptr);
}

// Creates an object of the given type and runs its constructor if necessary
#define NE_NEW(ARENA, TYPE) new (alignof(TYPE), ARENA, SOURCE_INFO) TYPE

// Creates an array of the given type and runs each member's constructor if necessary
#define NE_NEW_ARRAY(ARENA, TYPE, COUNT) ::Noble::NewArray<TYPE>(ARENA, COUNT, SOURCE_INFO)

// Allocates a buffer of memory of the given size and alignment
#define NE_BUFFER_ALLOC(ARENA, SIZE, ALIGN) ::Noble::AllocBuffer(ARENA, SIZE, ALIGN, SOURCE_INFO)

// Deletes an object that was allocated from the arena and runs its destructor if necessary
#define NE_DELETE(ARENA, PTR) ::Noble::Delete(ARENA, PTR)

// Deletes an array of objects and calls each member's destructor if necessary
#define NE_DELETE_ARRAY(ARENA, PTR) ::Noble::DeleteArray(PTR, ARENA)

// Deletes a buffer that was allocated from HE_BUFFER_ALLOC
#define NE_BUFFER_FREE(ARENA, PTR) ::Noble::FreeBuffer(ARENA, PTR);