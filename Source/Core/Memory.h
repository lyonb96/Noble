#pragma once

// Memory 2.0
// This time it won't just be a clone of Molecule's
// It'll be a clone of Unreal's!
// Just kidding...

#include "Types.h"
#include "Logger.h"

namespace Noble
{

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
			return _aligned_malloc(size, align);
		}

		/**
		 * Main memory free function, should work with Memory::Malloc
		 */
		FORCEINLINE static void Free(void* data)
		{
			_aligned_free(data);
		}

		/**
		 * Main memory realloc function, should be compatible with Memory::Malloc
		 */
		FORCEINLINE static void* Realloc(void* ptr, Size size, Size align)
		{
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
		void* GetData() const { return nullptr; }

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
			: m_Data(0), m_AllocSize(0), m_ElemCount(0)
		{
		}

		/**
		 * Resizes the array to fit @newMax elements of size @elementSize
		 */
		FORCEINLINE void* Resize(const Size& elementSize, Size newMax)
		{
			CHECK(elementSize > 0 && newMax > m_ElemCount);

			Size newAllocSize = elementSize * newMax;
			void* newBuffer;
			if (m_Data)
			{
				newBuffer = Memory::Realloc(m_Data, newAllocSize, NOBLE_DEFAULT_ALIGN);
#ifdef NOBLE_DEBUG
				if (newBuffer != m_Data)
				{
					NE_LOG_DEBUG("Realloc moved memory in container");
				}
#endif
			}
			else
			{
				newBuffer = Memory::Malloc(newAllocSize, NOBLE_DEFAULT_ALIGN);
			}

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
		FORCEINLINE void* GetData() const
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
	//class BlockAllocatorOld
	//{
	//public:
	//	// Default block size is 16KB
	//	static const Size DefaultBlockSize = (1 << 14);

	//public:

	//	/**
	//	 * Creates a BlockAllocator with the default block size
	//	 */
	//	BlockAllocatorOld();

	//	/**
	//	 * Creates a BlockAllocator with a custom block size
	//	 */
	//	BlockAllocatorOld(Size blockSize);

	//	/**
	//	 * Attempts to allocate a piece of memory of size "allocSize" aligned to "align" bytes
	//	 * Note that if allocSize is greater than the block size (minus some book-keeping data)
	//	 * this will return nullptr.
	//	 */
	//	void* Allocate(Size allocSize, Size align);

	//	/**
	//	 * Attempts to free the memory from the allocator
	//	 */
	//	void Free(void* ptr);

	//	/**
	//	 * Returns the total size of all allocated blocks
	//	 */
	//	Size GetAllocatedSize() const;

	//	/**
	//	 * Returns true if this allocator has allocated any blocks
	//	 */
	//	bool HasAllocated() const { return m_Head != nullptr; }

	//	/**
	//	 * This function frees all except the first block
	//	 * Useful for "resetting" the allocator
	//	 */
	//	void FreeExcessBlocks();

	//	/**
	//	 * Frees all of the blocks
	//	 */
	//	~BlockAllocatorOld();

	//private:

	//	// Struct for tracking alloc data
	//	struct Alloc
	//	{
	//		// Size of the allocation
	//		Size AllocSize;
	//		// Pointer to the next allocation
	//		Alloc* NextAlloc;
	//		// Pointer to last allocation
	//		Alloc* LastAlloc;
	//		// Pointer to the actual allocated memory
	//		U8* Data;
	//	};
	//	
	//	// Struct for tracking block info
	//	struct Block
	//	{
	//		// First alloc made
	//		Alloc* HeadAlloc;
	//		// Most recent alloc made
	//		Alloc* TailAlloc;
	//		// Pointer to the next block
	//		Block* NextBlock;
	//		//Pointer to the end of this block
	//		void* EndOfBlock;
	//	};


	//private:

	//	/**
	//	 * Allocates a new block
	//	 */
	//	void AllocateNewBlock();

	//	/**
	//	 * Handles recursively freeing blocks to ensure no memory is left unfreed
	//	 * Frees the given block and all blocks that were allocated after it
	//	 */
	//	void FreeBlock(Block* block);

	//private:

	//	static const Size BlockHeaderSize = sizeof(Block);
	//	static const Size AllocHeaderSize = sizeof(Alloc);

	//	// Block size
	//	Size m_BlockSize;
	//	// First block
	//	Block* m_Head;
	//	// Newest block
	//	Block* m_Tail;
	//};

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
		void* Allocate(Size allocSize, Size align);

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
}