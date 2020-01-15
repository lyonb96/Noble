#pragma once

// Memory 2.0
// This time it won't just be a clone of Molecule's
// It'll be a clone of Unreal's!
// Just kidding...

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

	// Container allocator structure:
	// void Resize(size, alignment)
	// Size GetAllocatedSize()
	// void* GetData()

	// The gist of the new memory system is focused on container 
	// allocation, because most allocation for the game will take
	// place in containers that require special allocation procedures.
	// Therefore a template based API similar to the old system will
	// be used to introduce virtual-free allocation and free mechanics,
	// that slot directly into custom containers. I didn't originaly
	// want to do custom containers, but doing so allows much more
	// freedom when it comes to allocation procedures and underlying
	// data structures. There's no guarantee that std::vector will be
	// consistently implemented acrossa platforms, and trying to write
	// my own custom std::allocator to fit may work on some platforms
	// and break on others, so I'd rather own the pipeline all the way
	// through.

	// The second major part of the new memory management system is 
	// standard Allocators. These will work similarly to the old 
	// MemoryArenas, but will be a little more pared down and 
	// simplified. They will simply return some bytes to the user.

	// All allocator types should clean up after themselves when their 
	// destructor runs, to prevent memory leaks. This may not be 
	// necessary for "gloabl" type allocators, but we'll cross that 
	// bridge when we come to it.

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

		FORCEINLINE static void* Memcpy(void* dst, void* src, Size size)
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
         * Resizes the array to fit @newMax elements of size @elementSize
		 */
		void* Resize(Size elementSize, Size newMax) { return nullptr; }

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
	class BasicContainerAllocator
	{
	public:

		FORCEINLINE BasicContainerAllocator()
			: m_Data(nullptr), m_AllocSize(0), m_ElemCount(0)
		{
		}

		/**
		 * Resizes the array to fit @newMax elements of size @elementSize
		 */
		FORCEINLINE void* Resize(const Size& elementSize, Size newMax, Size align = NOBLE_DEFAULT_ALIGN)
		{
			CHECK(elementSize > 0 && newMax > m_ElemCount);

			Size newAllocSize = elementSize * newMax;
			void* newBuffer = Memory::Realloc(m_Data, newAllocSize, align);

			m_Data = newBuffer;
			m_AllocSize = newAllocSize;
			m_ElemCount = newMax;

			return m_Data;
		}

		/**
		 * Returns the total amount of memory allocated by this allocator
		 */
		FORCEINLINE Size GetAllocationSize() const
		{
			return m_AllocSize;
		}


		/**
		 * Returns a pointer to the allocated data
		 */
		FORCEINLINE void* GetData()
		{
			return m_Data;
		}

		/**
		 * Returns a pointer to the allocated data (const version of above)
		 */
		FORCEINLINE const void* GetData() const
		{
			return m_Data;
		}

		/**
		 * Returns true if this allocator has made any heap allocations
		 */
		FORCEINLINE bool HasAllocated() const
		{
			return m_Data != nullptr;
		}

		/**
		 * Destructor frees memory if necessary
		 */
		FORCEINLINE virtual ~BasicContainerAllocator()
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
	 * Delete function, just provides a clean place for calls to HE_DELETE to redirect
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

// Deletes a buffer that was allocated from HE_BUFFER_ALLOC
#define NE_BUFFER_FREE(ARENA, PTR) ::Noble::FreeBuffer(ARENA, PTR);

#undef POD_CHECK