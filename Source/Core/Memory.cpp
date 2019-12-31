#include "Memory.h"

#include "Logger.h"

#include <memory>

namespace Noble
{
	/**
	 * Aligns the given pointer to the requested alignment by shifting it up
	 * Returns the same address if it's already aligned
	 */
	void* AlignUp(void* in, Size align)
	{
		CHECK(std::_Is_pow_2(align) && in != nullptr);

		union
		{
			void* as_void;
			std::uintptr_t as_uint;
		};

		as_void = in;
		as_uint = (as_uint + (align - 1)) & (~align);

		return as_void;
	}

	BlockAllocator::BlockAllocator()
		: BlockAllocator(DefaultBlockSize)
	{}

	BlockAllocator::BlockAllocator(Size blockSize)
		: m_BlockSize(blockSize)
	{
		m_Head = nullptr;
		m_Tail = nullptr;

		// Allocate a new block so the allocator is ready
		AllocateNewBlock();
	}

	void BlockAllocator::AllocateNewBlock()
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

	void* BlockAllocator::Allocate(Size allocSize, Size align)
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

	void BlockAllocator::Free(void* ptr)
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

	void BlockAllocator::FreeBlock(Block* block)
	{
		CHECK(block != nullptr);

		if (block->NextBlock)
		{
			FreeBlock(block->NextBlock);
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
}