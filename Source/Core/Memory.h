#pragma once

#include "Types.h"
#include <type_traits>

// Macro to shorten the POD type evaluation
#define POD_CHECK(TYPE) ::Noble::MemHelper::BoolToType<MemHelper::IsPOD<TYPE>::Value>()

namespace Noble
{

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
	 * Wrappers to system calls to support mem tracking
	 */
	struct Memory
	{
		/**
		 * Main memory allocation function
		 */
		FORCEINLINE static void* Malloc(Size size, Size align)
		{
#ifdef NOBLE_DEBUG
			++AllocCount;
#endif
			return _aligned_malloc(size, align);
		}

		/**
		 * Main memory free function, should work with Memory::Malloc
		 */
		FORCEINLINE static void Free(void* data)
		{
#ifdef NOBLE_DEBUG
			--AllocCount;
#endif
			_aligned_free(data);
		}

		/**
		 * Main memory realloc function, should be compatible with Memory::Malloc
		 */
		FORCEINLINE static void* Realloc(void* ptr, Size size, Size align)
		{
#ifdef NOBLE_DEBUG
			if (!ptr)
			{
				++AllocCount;
			}
#endif
			return _aligned_realloc(ptr, size, align);
		}

		FORCEINLINE static void* Memset(void* dst, I32 val, Size size)
		{
			return memset(dst, val, size);
		}

		FORCEINLINE static void* Memcpy(void* dst, const void* src, Size size)
		{
			return memcpy(dst, src, size);
		}

		FORCEINLINE static const U32 GetAllocCount()
		{
			return AllocCount;
		}

	private:

		// ++ for alloc, -- for free
		inline static U32 AllocCount = 0;
	};

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
		operator char* ()
		{
			return m_Buffer;
		}

		/**
		 * Implicit const char* cast
		 */
		operator const char* ()
		{
			return m_Buffer;
		}
	};

	/**
	 * Not to be used; just here to show structure of a Container Allocator
	 */
	class ContainerAllocatorInterface
	{
		/**
		 * Copy constructor, should allocate the same amount of memory and copy elements
		 */
		ContainerAllocatorInterface(const ContainerAllocatorInterface& other) {}

		/**
		 * Move constructor, should leave "other" in a deterministic state
		 */
		ContainerAllocatorInterface(ContainerAllocatorInterface&& other) noexcept {}

		/**
		 * Calculates how much the allocator can/should grow by, and returns recommended
		 * new max count
		 */
		const Size CalculateGrowSize(const Size& elementSize, const Size& requestedCount = 0) { return 0; }

		/**
         * Resizes the array to fit @newMax elements of size @elementSize
		 * Returns the element count
		 */
		Size Resize(Size elementSize, Size newMax) { return 0; }

		/**
		 * Returns the total amount of memory allocated by this allocator
		 */
		Size GetAllocationSize() const { return 0; }

		/**
		 * Returns a pointer to the allocated data
		 */
		void* GetData() { return nullptr; }

		/**
		 * Returns a pointer to the allocated data (const version of above)
		 */
		const void* GetData() const { return nullptr; }

		/**
		 * Returns true if this allocator has made any heap allocations
		 */
		bool HasAllocated() const { return false; }
	};

	/**
	 * Most basic implementation of a container allocator; uses Malloc/Realloc/Free
	 */
	template <typename ElementType>
	class DefaultContainerAllocator
	{
	public:

		/**
		 * Empty initializes the allocator
		 */
		DefaultContainerAllocator()
			: m_Data(nullptr), m_AllocSize(0), m_ElemCount(0)
		{
		}

		/**
		 * Deep copies the allocator - fully reallocates the storage
		 * This is slow and should be avoided if at all possible
		 */
		DefaultContainerAllocator(const DefaultContainerAllocator& other)
		{
			if (other.m_ElemCount > 0)
			{
				m_ElemCount = 0;
				m_AllocSize = 0;
				m_Data = nullptr;
				// Resize to make room for the copy
				Resize(other.m_ElemCount);
				// Copy all of the elements
				Memory::Memcpy(m_Data, other.m_Data, m_AllocSize);
			}
			else
			{
				m_ElemCount = other.m_ElemCount;
				m_AllocSize = other.m_AllocSize;
				m_Data = nullptr;
			}
		}

		/**
		 * Moves the data from "other" to this one, leaving "other" in a clean state
		 */
		DefaultContainerAllocator(DefaultContainerAllocator&& other) noexcept
		{
			m_ElemCount = other.m_ElemCount;
			m_AllocSize = other.m_AllocSize;
			m_Data = other.m_Data;

			other.m_ElemCount = 0;
			other.m_AllocSize = 0;
			other.m_Data = nullptr;
		}

		/**
		 * Copy assignment
		 */
		DefaultContainerAllocator& operator=(const DefaultContainerAllocator& other)
		{
			if (this == &other)
			{
				return *this;
			}

			if (m_Data)
			{
				Memory::Free(m_Data);
			}

			if (other.m_ElemCount > 0)
			{
				m_ElemCount = 0;
				m_AllocSize = 0;
				m_Data = nullptr;
				// Resize to make room for the copy
				Resize(other.m_ElemCount);
				// Copy all of the elements
				Memory::Memcpy(m_Data, other.m_Data, m_AllocSize);
			}
			else
			{
				m_ElemCount = other.m_ElemCount;
				m_AllocSize = other.m_AllocSize;
				m_Data = nullptr;
			}

			return *this;
		}

		/**
		 * Move assignment
		 */
		DefaultContainerAllocator& operator=(DefaultContainerAllocator&& other) noexcept
		{
			if (m_Data)
			{
				Memory::Free(m_Data);
			}

			m_ElemCount = other.m_ElemCount;
			m_AllocSize = other.m_AllocSize;
			m_Data = other.m_Data;

			other.m_ElemCount = 0;
			other.m_AllocSize = 0;
			other.m_Data = nullptr;
		}

		/**
		 * Calculates a suitable new max size
		 */
		const Size CalculateGrowSize(const Size& requestedCount = 0)
		{
			if (requestedCount > m_ElemCount)
			{
				return requestedCount;
			}
			else
			{
				return glm::max((m_ElemCount * 3) / 2, m_ElemCount + 4);
			}
		}

		/**
		 * Resizes the array to fit @newMax elements
		 * Returns the element count
		 */
		Size Resize(Size newMax)
		{
			CHECK(newMax > m_ElemCount);

			Size newAllocSize = sizeof(ElementType) * newMax;
			void* newBuffer = Memory::Realloc(m_Data, newAllocSize, alignof(ElementType));

			m_Data = newBuffer;
			m_AllocSize = newAllocSize;
			m_ElemCount = newMax;

			return m_ElemCount;
		}

		/**
		 * Resets the allocator to an empty state, freeing any memory it holds
		 */
		void Reset()
		{
			if (m_Data)
			{
				Memory::Free(m_Data);
			}
			m_Data = nullptr;
			m_AllocSize = 0;
			m_ElemCount = 0;
		}

		/**
		 * Returns the total amount of memory allocated by this allocator
		 */
		Size GetAllocationSize() const
		{
			return m_AllocSize;
		}

		/**
		 * Returns a pointer to the allocated data
		 */
		void* GetData()
		{
			return m_Data;
		}

		/**
		 * Returns a pointer to the allocated data (const version of above)
		 */
		const void* GetData() const
		{
			return m_Data;
		}

		/**
		 * Returns true if this allocator has made any heap allocations
		 */
		bool HasAllocated() const
		{
			return m_Data != nullptr;
		}

		/**
		 * Destructor frees memory if necessary
		 */
		virtual ~DefaultContainerAllocator()
		{
			if (m_Data)
			{
				Memory::Free(m_Data);
			}
		}

	private:

		// Pointer to raw data
		void* m_Data;
		// Total allocated memory in bytes
		Size m_AllocSize;
		// Total number of "elements"
		Size m_ElemCount;

	};

	/**
	 * A container allocator that only holds a fixed number of elements and cannot be resized
	 */
	template <typename ElementType, Size N>
	class FixedContainerAllocator
	{
	public:

		FixedContainerAllocator()
		{
			Memory::Memset(GetData(), 0, sizeof(m_Data));
		}

		/**
		 * Copies the elements from the given allocator to this one
		 */
		FixedContainerAllocator(const FixedContainerAllocator& other)
		{
			// Copy elements
			Memory::Memcpy(GetData(), other.GetData(), sizeof(m_Data));
		}

		/**
		 * Moves the elements from the given allocator to this one, invalidating the original
		 */
		FixedContainerAllocator(FixedContainerAllocator&& other)
		{
			// Copy the data
			Memory::Memcpy(GetData(), other.GetData(), sizeof(m_Data));
			// Then clear the other to 0
			Memory::Memset(other.GetData(), 0, sizeof(other.m_Data));
		}

		/**
		 * Copies the elements from the given allocator to this one
		 * Also frees any memory that is currently in this allocator!
		 */
		FixedContainerAllocator& operator=(const FixedContainerAllocator& other)
		{
			if (this == &other)
			{
				return *this;
			}

			// Copy elements
			Memory::Memcpy(GetData(), other.GetData(), sizeof(m_Data));

			return *this;
		}

		/**
		 * Moves the elements from the given allocator to this one, invalidating the original
		 * Also frees any memory that is currently in this allocator!
		 */
		FixedContainerAllocator& operator=(FixedContainerAllocator&& other)
		{
			// Copy the data
			Memory::Memcpy(GetData(), other.GetData(), sizeof(m_Data));
			// Then clear the other to 0
			Memory::Memset(other.GetData(), 0, sizeof(other.m_Data));

			return *this;
		}

		/**
		 * Fixed Allocator - just returns the fixed size
		 */
		const Size CalculateGrowSize(const Size& requestedCount = 0) { return N; }

		/**
		 * Fixed Allocator - just returns the fixed count since the data is not heap allocated
		 */
		Size Resize(Size newMax)
		{
			return N;
		}

		/**
		 * Fixed allocator - doesn't need to do anything
		 */
		void Reset() {}

		/**
		 * Returns the total amount of memory allocated by this allocator
		 */
		Size GetAllocationSize() const
		{
			return sizeof(m_Data);
		}


		/**
		 * Returns a pointer to the allocated data
		 */
		void* GetData()
		{
			return &m_Data[0];
		}

		/**
		 * Returns a pointer to the allocated data (const version of above)
		 */
		const void* GetData() const
		{
			return &m_Data[0];
		}

		/**
		 * Returns true if this allocator has allocated elements
		 */
		bool HasAllocated() const
		{
			return true;
		}

		/**
		 * Destructor - does nothing since memory is not heap alloc
		 */
		virtual ~FixedContainerAllocator()
		{
		}

	private:

		STATIC_CHECK(sizeof(ElementType) % alignof(ElementType) == 0, "Alignment should be a multiple of size");

		// Raw Data
		// Rather than storing as array of ElementType, store as array of Byte
		// This avoids default construction happening in the allocator, which
		// should remain as agnostic to the function of the elements as possible
		Byte m_Data[N * sizeof(ElementType)];
	};

	/**
	 * Based on Epic's InlineAllocator. Allocates @Count @Elements on the stack.
	 * If requirements exceed this point, the second allocator is used.
	 */
	template <class Element, U32 Count, class Allocator>
	class InlineAllocator
	{
		
	};

	/**
	 * Not to be used; just shows the proper structure of an Allocator
	 */
	class AllocatorInterface
	{
		/**
		 * Returns an @allocSize block of memory aligned to @align
		 */
		void* Allocate(Size allocSize, Size align) { return nullptr; }

		/**
		 * Frees memory allocated with this allocator
		 */
		void Free(void* ptr) {}

		/**
		 * Returns how much memory (in bytes) this allocator is using
		 */
		Size GetAllocatedSize() const { return 0; }

		/**
		 * Returns true if this allocator has made any heap allocs
		 */
		bool HasAllocated() const { return false; }
	};

	/**
	 * This allocator allocates blocks of memory in a given size.
	 * Once the block has been doled out, it allocates another block
	 * and begins handing out that block. If an allocation is larger
	 * than the block size, the alloc just fails and returns nullptr
	 */
	class BlockAllocator
	{
	public:
		// Default block size is 16KB
		static const Size DefaultBlockSize = (1 << 14);

	public:

		/**
		 * Creates a BlockAllocator with the default block size
		 */
		BlockAllocator();

		/**
		 * Creates a BlockAllocator with a custom block size
		 */
		BlockAllocator(Size blockSize);

		/**
		 * Attempts to allocate a piece of memory of size "allocSize" aligned to "align" bytes
		 * Note that if allocSize is greater than the block size (minus some book-keeping data)
		 * this will return nullptr.
		 */
		void* Allocate(Size allocSize, Size align, Size offset = 0);

		/**
		 * Attempts to free the memory from the allocator
		 */
		void Free(void* ptr);

		/**
		 * Returns the total size of all allocated blocks
		 */
		Size GetAllocatedSize() const;

		/**
		 * Returns true if this allocator has allocated any blocks
		 */
		bool HasAllocated() const { return m_Head != nullptr; }

		/**
		 * This function frees all except the first block
		 * Useful for "resetting" the allocator
		 */
		void FreeExcessBlocks();

		/**
		 * Frees all of the blocks
		 */
		~BlockAllocator();

	private:

		// Struct for tracking block info
		struct Block
		{
			// Pointer to the next block
			Block* NextBlock;
			// Current pointer
			U8* CurrentPointer;
		};

		// Struct for tracking individual allocations
		struct Alloc
		{
			// Size of the allocation
			Size AllocSize;
			// Pointer to alloc data
			void* Data;
			// Pointer to next alloc in list
			Alloc* Next;
		};

	private:

		/**
		 * Allocates a new block
		 */
		void AllocateNewBlock();

		/**
		 * Handles recursively freeing blocks to ensure no memory is left unfreed
		 * Frees the given block and all blocks that were allocated after it
		 */
		void FreeBlock(Block* block);

	private:

		static const Size BlockHeaderSize = sizeof(Block);
		static const Size AllocHeaderSize = sizeof(Alloc);

		// Block size
		Size m_BlockSize;
		// First block
		Block* m_Head;
		// Newest block
		Block* m_Tail;
		// Free alloc list
		Alloc* m_FreeAllocs;
		Alloc* m_FreeAllocTail;
	};

	/**
	 * No tracking policy
	 */
	class NoTrackingPolicy
	{
	public:

		// Does nothing
		FORCEINLINE void OnAllocate(void* ptr, Size size, const SourceInfo& info) const {}

		// Does nothing
		FORCEINLINE void OnFree(void* ptr) const {}

		// Does something!
		// Just kidding.
		FORCEINLINE const bool HasAllocs() const { return false; }
	};

	/**
	 * Simple memory tracking, keeps a count of unfreed allocations
	 */
	class SimpleTrackingPolicy
	{
	private:

		U32 m_Count;

	public:

		/**
		 * Increases the allocation count
		 */
		FORCEINLINE void OnAllocate(void* ptr, Size size, const SourceInfo& sourceInfo) { m_Count++; }

		/**
		 * Decreases the allocation count
		 */
		FORCEINLINE void OnFree(void* in) { m_Count--; }

		/**
		 * Returns the number of unfreed allocations
		 */
		FORCEINLINE const U32 GetAllocCount() const { return m_Count; }

		/**
		 * Returns true if there are any unfreed allocations
		 */
		FORCEINLINE const bool HasAllocs() const { return m_Count > 0; }
	};

	/**
	 * Memory arenas wrap an allocator and some tracking/debugging tools
	 * and provide easy macros for new/delete.
	 * Unlike previous implementations, the Allocator instance is what handles
	 * actual OS calls to allocate memory.
	 */
	template <class Allocator, class Tracker>
	class MemoryArena
	{
	public:

		/**
		 * Templated constructor to allow passing values to allocator's constructor
		 */
		template<typename... Args>
		MemoryArena(Args... arg)
			: m_Alloc(arg...)
		{}

		/**
		 * Returns some memory of the requested size and alignment, handled by the Arena's allocator
		 */
		void* Allocate(Size size, Size align, const SourceInfo& info, Size offset = 0)
		{
			void* ptr = m_Alloc.Allocate(size, align, offset);
			CHECK(ptr);

			m_Track.OnAllocate(ptr, size, info);

			return ptr;
		}

		/**
		 * Frees the given pointer
		 */
		void Free(void* ptr)
		{
			m_Alloc.Free(ptr);
			m_Track.OnFree(ptr);
		}

		/**
		 * Returns the allocator used in this Arena
		 */
		Allocator& GetAllocator()
		{
			return m_Alloc;
		}

		/**
		 * Returns the memory tracker used for this arena
		 */
		Tracker& GetTracker()
		{
			return m_Track;
		}

	private:

		Allocator m_Alloc;
		Tracker m_Track;
	};

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
	 * Delete function, just provides a clean place for calls to NE_DELETE to redirect
	 * Also determines if destructor needs to be called
	 */
	template <class Arena, class Type>
	void Delete(Arena& arena, Type* ptr)
	{
		Delete(arena, ptr, POD_CHECK(Type));
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
		DeleteArray(ptr, arena, POD_CHECK(Type));
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
		Size count = *(as_Size - 1);
		// Need to set pointer to the last item of the array
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

// Deletes a buffer that was allocated from NE_BUFFER_ALLOC
#define NE_BUFFER_FREE(ARENA, PTR) ::Noble::FreeBuffer(ARENA, PTR);

#undef POD_CHECK