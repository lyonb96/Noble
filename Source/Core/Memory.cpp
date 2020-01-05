#include "Memory.h"

#include "Logger.h"
#include "HelperMacros.h"

#include <memory>

namespace Noble
{
	/**
	 * Aligns the given pointer to the requested alignment by shifting it up
	 * Returns the same address if it's already aligned
	 */
	void* AlignUp(void* in, Size align, Size offset = 0)
	{
		CHECK((align & (align - 1)) == 0); // verify that align is a power of 2

		union
		{
			void* as_void;
			std::uintptr_t as_uint;
		};

		as_void = in;
		as_uint += offset;
		as_uint = (as_uint + (align - 1)) & (~(align - 1));

		return as_void;
	}

	// -----------------------------------------------------

	BlockAllocator::BlockAllocator()
		: BlockAllocator(DefaultBlockSize)
	{}

	BlockAllocator::BlockAllocator(Size blockSize)
	{
		// Store the block size and init members
		m_BlockSize = blockSize;
		m_Head = nullptr;
		m_Tail = nullptr;
		m_FreeAllocs = nullptr;
		m_FreeAllocTail = nullptr;

		// Allocate the head
		AllocateNewBlock();
	}

	void BlockAllocator::AllocateNewBlock()
	{
		void* data = Memory::Malloc(m_BlockSize, 16);

#ifdef NOBLE_DEBUG
		if (!data)
		{
			NE_LOG_ERROR("Malloc failed! That's not supposed to happen...");
			return;
		}
#endif

		if (!m_Head)
		{
			// First block
			m_Head = (Block*)data;
			m_Head->CurrentPointer = (U8*)data + BlockHeaderSize;
			m_Head->NextBlock = nullptr;

			// tail and head are the same (there's only 1 block, ya dip)
			m_Tail = m_Head;
		}
		else
		{
			// New tail block
			Block* newBlock = (Block*)data;
			newBlock->CurrentPointer = (U8*)data + BlockHeaderSize;
			newBlock->NextBlock = nullptr;

			m_Tail->NextBlock = newBlock;
			m_Tail = newBlock;
		}
	}

	void* BlockAllocator::Allocate(Size allocSize, Size align, Size offset)
	{
		CHECK(m_Head != nullptr && allocSize > 0 && align > 0);
		
		if ((allocSize + BlockHeaderSize + AllocHeaderSize) > m_BlockSize)
		{
			NE_LOG_WARNING("Requested allocation is too large for this Block Allocator, returning nullptr");
			return nullptr;
		}

		// check for fitting allocations in the freed list
		Alloc* freeAlloc = m_FreeAllocs;
		Alloc* lastAlloc = nullptr;
		while (freeAlloc)
		{
			// If the alloc is at least large enough, but not more than twice the necessary size
			if (freeAlloc->AllocSize >= allocSize && freeAlloc->AllocSize <= allocSize * 2)
			{
				// remove this from the freelist
				if (lastAlloc)
				{
					lastAlloc->Next = freeAlloc->Next;
				}
				else
				{
					// first alloc in the list was used, so just change the head
					m_FreeAllocs = freeAlloc->Next;
				}
				return freeAlloc->Data;
			}
			// advance to the next item
			lastAlloc = freeAlloc;
			freeAlloc = freeAlloc->Next;
		}

		U8* addr = (U8*) AlignUp(m_Tail->CurrentPointer, align, AllocHeaderSize + offset);
		CHECK(((uintptr_t)addr) % align == 0);
		// addr now points to the data, and (addr - AllocHeaderSize) is the alloc header

		U8* blockEnd = ((U8*)m_Tail) + m_BlockSize;
		
		if (addr + allocSize >= blockEnd)
		{
			// too large
			// allocate a new block and try again
			AllocateNewBlock();
			return Allocate(allocSize, align);
		}
		else
		{
			// it will fit
			Alloc* allocHeader = /*&((Alloc*)addr)[-1]*/(Alloc*)(addr - AllocHeaderSize);
			allocHeader->AllocSize = allocSize;
			allocHeader->Data = addr;
			allocHeader->Next = nullptr; // next member is used for the free alloc list

			// advance the current pointer
			m_Tail->CurrentPointer = addr + allocSize;

			return addr;
		}
	}

	void BlockAllocator::Free(void* ptr)
	{
		CHECK(ptr != nullptr);

		Alloc* alloc = (Alloc*)(((U8*)ptr) - AllocHeaderSize);

		if (!m_FreeAllocs)
		{
			m_FreeAllocs = m_FreeAllocTail = alloc;
		}
		else
		{
			m_FreeAllocTail->Next = alloc;
			m_FreeAllocTail = alloc;
		}
	}

	Size BlockAllocator::GetAllocatedSize() const
	{
		U8 totalBlocks = 0;

		Block* next = m_Head;
		while (next)
		{
			++totalBlocks;
			next = next->NextBlock;
		}

		return totalBlocks * m_BlockSize;
	}

	void BlockAllocator::FreeBlock(Block* block)
	{
		CHECK(block != nullptr);

		if (block->NextBlock)
		{
			FreeBlock(block->NextBlock);
			block->NextBlock = nullptr;
		}

		// Check the free list and remove any entries in this block
		uintptr_t lowerBound = (uintptr_t)block;
		uintptr_t upperBound = lowerBound + m_BlockSize;
		Alloc* free = m_FreeAllocs;
		Alloc* last = nullptr;
		while (free)
		{
			uintptr_t freeAddr = (uintptr_t)free;
			if (lowerBound <= freeAddr && upperBound > freeAddr)
			{
				// free alloc is in this block
				// remove from freelist
				if (last)
				{
					last->Next = free->Next;
				}
				else
				{
					m_FreeAllocs = free->Next;
				}
				last = free;
				free = free->Next;
			}
		}

		Memory::Free(block);
	}

	void BlockAllocator::FreeExcessBlocks()
	{
		CHECK(m_Head != nullptr);

		if (m_Head->NextBlock)
		{
			FreeBlock(m_Head->NextBlock);
			m_Head->NextBlock = nullptr;
		}
	}

	BlockAllocator::~BlockAllocator()
	{
		FreeBlock(m_Head);
	}
}