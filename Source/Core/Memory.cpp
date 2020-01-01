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
		CHECK(std::_Is_pow_2(align) && in != nullptr);

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
	/* Old BlockAllocator code
	BlockAllocatorOld::BlockAllocatorOld()
		: BlockAllocatorOld(DefaultBlockSize)
	{}

	BlockAllocatorOld::BlockAllocatorOld(Size blockSize)
		: m_BlockSize(blockSize)
	{
		m_Head = nullptr;
		m_Tail = nullptr;

		// Allocate a new block so the allocator is ready
		AllocateNewBlock();
	}

	void BlockAllocatorOld::AllocateNewBlock()
	{
		void* blockAddr = _aligned_malloc(m_BlockSize, 16);

		CHECK(blockAddr != nullptr);
		
		if (m_Head)
		{
			Block* next = (Block*)blockAddr;
			m_Tail->NextBlock = next;
			m_Tail = next;

			next->HeadAlloc = nullptr;
			next->TailAlloc = nullptr;
			next->NextBlock = nullptr;
			next->EndOfBlock = next + 1;
		}
		else
		{
			Block* newHead = (Block*)blockAddr;
			newHead->HeadAlloc = nullptr;
			newHead->TailAlloc = nullptr;
			newHead->NextBlock = nullptr;
			newHead->EndOfBlock = newHead + 1;

			m_Head = newHead;
			m_Tail = newHead;
		}
	}

	void* BlockAllocatorOld::Allocate(Size allocSize, Size align)
	{
		CHECK(m_Head != nullptr);

		if ((allocSize + BlockHeaderSize + AllocHeaderSize) > m_BlockSize)
		{
			NE_LOG_WARNING("Requested allocation is too large for this Block Allocator, returning nullptr");
			return nullptr;
		}

		void* baseAddr;

		// Check if this is the first alloc of this block
		if (!m_Tail->HeadAlloc)
		{
			baseAddr = AlignUp(m_Tail + 1, alignof(Alloc));
			// baseAddr now points to alloc header

			Alloc* alloc = (Alloc*)baseAddr;
			alloc->AllocSize = allocSize;
			alloc->NextAlloc = nullptr;
			alloc->LastAlloc = nullptr;
			m_Tail->HeadAlloc = alloc;
			m_Tail->TailAlloc = alloc;

			// Now set up the data
			void* data = AlignUp(alloc + 1, align);
			alloc->Data = (U8*)data;

			return data;
		}
		else
		{
			Alloc* lastAlloc = m_Tail->TailAlloc;
			baseAddr = AlignUp(lastAlloc->Data + lastAlloc->AllocSize, alignof(Alloc));
			
			// baseAddr now points to alloc header

			U8* check = (U8*) baseAddr;
			check += AllocHeaderSize;
			check = (U8*) AlignUp(check, align);
			check += allocSize;

			if (check >= (U8*) m_Tail->EndOfBlock)
			{
				// alloc is too large for this block, so...
				// Grab a new empty block, and then...
				AllocateNewBlock();
				// retry the alloc, which will automatically use the new block
				return Allocate(allocSize, align);
			}
			else
			{
				// setup alloc header
				Alloc* newAlloc = (Alloc*)baseAddr;
				newAlloc->AllocSize = allocSize;
				newAlloc->LastAlloc = lastAlloc;
				newAlloc->NextAlloc = nullptr;

				lastAlloc->NextAlloc = newAlloc;

				// now set up the data
				void* data = AlignUp(newAlloc + 1, align);
				newAlloc->Data = (U8*)data;

				return data;
			}
		}
	}

	void BlockAllocatorOld::Free(void* ptr)
	{
		CHECK(m_Head != nullptr && ptr != nullptr);

		// for now, we're gonna do this the slow way
		Alloc* alloc = m_Head->HeadAlloc;
		Block* currentBlock = m_Head;
		while (alloc)
		{
			if (alloc->Data == ptr)
			{
				if (alloc->LastAlloc)
				{
					alloc->LastAlloc->NextAlloc = alloc->NextAlloc;
				}
				if (alloc->NextAlloc)
				{
					alloc->NextAlloc->LastAlloc = alloc->LastAlloc;
				}

				// if i feel like it, I could start adding freed allocs
				// to another freelist for reuse
				// it might help cache coherency but would probably
				// slow down allocs. will have to test and see

				return;
			}
			else
			{
				if (alloc->NextAlloc)
				{
					alloc = alloc->NextAlloc;
				}
				else if (currentBlock->NextBlock)
				{
					currentBlock = currentBlock->NextBlock;
					alloc = currentBlock->HeadAlloc;
				}
			}
		}
	}

	void BlockAllocatorOld::FreeBlock(Block* block)
	{
		CHECK(block != nullptr);

		if (block->NextBlock)
		{
			FreeBlock(block->NextBlock);
		}

		_aligned_free(block);
	}

	void BlockAllocatorOld::FreeExcessBlocks()
	{
		CHECK(m_Head != nullptr);

		if (m_Head->NextBlock)
		{
			FreeBlock(m_Head->NextBlock);
		}
	}

	BlockAllocatorOld::~BlockAllocatorOld()
	{
		FreeBlock(m_Head);
	}

	Size BlockAllocatorOld::GetAllocatedSize() const
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
	*/

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

		// Allocate the head
		AllocateNewBlock();
	}

	void BlockAllocator::AllocateNewBlock()
	{
		void* data = _aligned_malloc(m_BlockSize, 16);

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

	void* BlockAllocator::Allocate(Size allocSize, Size align)
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

		U8* addr = (U8*) AlignUp(m_Tail->CurrentPointer, align, AllocHeaderSize);
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

		Alloc* alloc = &((Alloc*)ptr)[-1];

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
		}

		// Check the free list and remove any entries in this block
		uintptr_t lowerBound = (uintptr_t)block;
		uintptr_t upperBound = (uintptr_t)(block + 1);
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

		_aligned_free(block);
	}

	void BlockAllocator::FreeExcessBlocks()
	{
		CHECK(m_Head != nullptr);

		if (m_Head->NextBlock)
		{
			FreeBlock(m_Head->NextBlock);
		}
	}

	BlockAllocator::~BlockAllocator()
	{
		FreeBlock(m_Head);
	}
}