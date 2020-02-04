#pragma once

#include <utility>

#include "Types.h"
#include "Memory.h"

namespace Noble
{

	/**
	 * Implements a singly-linked list
	 */
	template <typename Element, typename Allocator>
	class LinkedListBase
	{
	private:

		/**
		 * Entry to the LinkedList
		 */
		struct ListEntry
		{
			Element Elem;
			ListEntry* Next;
		};

	public:

		/**
		 * Empty-initializes the LinkedList
		 */
		LinkedListBase()
		{
			m_Head = m_Tail = m_UnusedList = nullptr;
			m_MaxEntries = m_CurrentEntries = 0;
		}

		/**
		 * Initializes the list with space for a requested number of elements
		 * Note that depending on the allocator, the request may be ignored.
		 */
		LinkedListBase(const Size& init)
			: LinkedListBase()
		{
			CHECK(init > 0);

			Resize(init);
		}

		/**
		 * Copies the given element to the tail of the LinkedList
		 */
		void PushTail(const Element& elem)
		{
			m_Tail = Insert(m_Tail, nullptr, elem);
			if (!m_Head)
			{
				m_Head = m_Tail;
			}
		}

		/**
		 * Moves the given element to the tail of the LinkedList
		 */
		void PushTail(Element&& elem)
		{
			m_Tail = Insert(m_Tail, nullptr, elem);
			if (!m_Head)
			{
				m_Head = m_Tail;
			}
		}

		/**
		 * Copies the given element to the head of the LinkedList
		 */
		void PushHead(const Element& elem)
		{
			m_Head = Insert(nullptr, m_Head, elem);
			if (!m_Tail)
			{
				m_Tail = m_Head;
			}
		}

		/**
		 * Moves the given element to the head of the LinkedList
		 */
		void PushHead(Element&& elem)
		{
			m_Head = Insert(nullptr, m_Head, elem);
			if (!m_Tail)
			{
				m_Tail = m_Head;
			}
		}

		/**
		 * Copies the given element to the requested index of the LinkedList
		 */
		void Push(Size index, const Element& elem)
		{
			CHECK(m_CurrentEntries > 0);

			if (index == 0)
			{
				PushHead(elem);
				return;
			}
			else if (index == m_CurrentEntries)
			{
				PushTail(elem);
				return;
			}

			ListEntry* start = m_Head, *last = nullptr;
			for (Size i = 0; i < index; ++i)
			{
				if (!start)
				{
					return;
				}
				last = start;
				start = start->Next;
			}

			Insert(last, start, elem);
		}

		/**
		 * Moves the given element to the requested index of the LinkedList
		 */
		void Push(Size index, Element&& elem)
		{
			CHECK(m_CurrentEntries > 0);

			if (index == 0)
			{
				PushHead(elem);
				return;
			}
			else if (index == m_CurrentEntries)
			{
				PushTail(elem);
				return;
			}

			ListEntry* start = m_Head, last = nullptr;
			for (Size i = 0; i < index; ++i)
			{
				if (!start)
				{
					return;
				}
				last = start;
				start = start->Next;
			}

			Insert(last, start, elem);
		}

		/**
		 * Resizes the LinkedList to hold the requested number of entries
		 */
		void Resize(const Size& newMax)
		{
			CHECK(newMax >= m_CurrentEntries);

			Size oldMax = m_MaxEntries;
			m_MaxEntries = m_Allocator.Resize(newMax);

			if (m_MaxEntries > oldMax)
			{
				// Initialize the new empty entries
				InitializeEmpty(oldMax);
			}
		}

		/**
		 * Resizes the LinkedList to hold only as many entries as it currently has
		 * Or only 1 if there are no entries
		 */
		void Shrink()
		{
			Resize(glm::max(1, m_CurrentEntries));
		}

		/**
		 * Returns the element at the head of the list
		 */
		Element& GetHead()
		{
			CHECK(m_Head);
			return m_Head->Elem;
		}

		/**
		 * Returns the element at the head of the list
		 */
		const Element& GetHead() const
		{
			CHECK(m_Head);
			return m_Head->Elem;
		}

		/**
		 * Returns the element at the tail of the list
		 */
		Element& GetTail()
		{
			CHECK(m_Tail);
			return m_Tail->Elem;
		}

		/**
		 * Returns the element at the tail of the list
		 */
		const Element& GetTail() const
		{
			CHECK(m_Tail);
			return m_Tail->Elem;
		}

		/**
		 * Returns the element at the given index
		 */
		Element& Get(Size index)
		{
			CHECK(index >= 0 && index < m_CurrentEntries);

			ListEntry* entry = m_Head;
			for (Size i = 0; i < index; ++i)
			{
				CHECK(entry);
				entry = entry->Next;
			}

			return entry->Elem;
		}

		/**
		 * Returns the element at the given index
		 */
		const Element& Get(Size index) const
		{
			CHECK(index >= 0 && index < m_CurrentEntries);

			ListEntry* entry = m_Head;
			for (Size i = 0; i < index; ++i)
			{
				CHECK(entry);
				entry = entry->Next;
			}

			return entry->Elem;
		}

		/**
		 * Removes the element at the head of the list and returns it
		 */
		Element&& PopHead()
		{
			ListEntry* head = m_Head;
			m_Head = m_Head->Next;

			FreeEntry(head);

			--m_CurrentEntries;
			
			return std::move(head->Elem);
		}

		/**
		 * Removes the element at the tail of the list and returns it
		 */
		Element&& PopTail()
		{
			ListEntry* tail = m_Tail;

			ListEntry* preTail = m_Head;
			while (preTail->Next != m_Tail)
			{
				preTail = preTail->Next;
			}
			preTail->Next = nullptr;
			m_Tail = preTail;
			
			FreeEntry(tail);

			--m_CurrentEntries;

			return std::move(tail->Elem);
		}

		/**
		 * Removes the element at the given index and returns it
		 */
		Element&& Pop(Size index)
		{
			if (index == 0)
			{
				return PopHead();
			}
			else if (index == m_CurrentEntries - 1)
			{
				return PopTail();
			}

			ListEntry* entry = m_Head, prev = nullptr;
			for (Size i = 0; i < index; ++i)
			{
				CHECK(entry != nullptr);
				prev = entry;
				entry = entry->Next;
			}

			prev->Next = entry->Next;
			FreeEntry(entry);

			--m_CurrentEntries;

			return std::move(entry->Elem);
		}

	private:

		/**
		 * Makes room for another entry, growing the total entries if necessary
		 */
		void MakeRoom()
		{
			Size freeCount = m_MaxEntries - m_CurrentEntries;
			if (freeCount == 0)
			{
				Size grow = m_Allocator.CalculateGrowSize();
				Resize(grow);
			}
		}

		/**
		 * Empty-initializes all of the ListEntries after the given index
		 */
		void InitializeEmpty(Size index)
		{
			for (Size i = index; i < m_MaxEntries; ++i)
			{
				ListEntry* entry = GetEntryArray() + i;
				entry->Next = nullptr;
				FreeEntry(entry);
			}
		}

		/**
		 * Grabs and returns an empty entry from the unused list
		 */
		ListEntry* GetEmptyEntry()
		{
			CHECK(m_UnusedList);

			ListEntry* entry = m_UnusedList;
			m_UnusedList = m_UnusedList->Next;

			return entry;
		}

		/**
		 * Returns a pointer to the raw array of Entries
		 */
		ListEntry* GetEntryArray()
		{
			return static_cast<ListEntry*>(m_Allocator.GetData());
		}

		/**
		 * Returns a pointer to the raw array of Entries
		 */
		const ListEntry* GetEntryArray() const
		{
			return static_cast<const ListEntry*>(m_Allocator.GetData());
		}

		/**
		 * Copies the given element into a new ListEntry between the two
		 * Returns the new entry
		 */
		ListEntry* Insert(ListEntry* prev, ListEntry* next, const Element& elem)
		{
			MakeRoom();

			ListEntry* entry = GetEmptyEntry();
			if (prev)
			{
				prev->Next = entry;
			}
			entry->Next = next;
			entry->Elem = elem; // copy

			++m_CurrentEntries;

			return entry;
		}

		/**
		 * Moves the given element into a new ListEntry between the two
		 * Returns the new entry
		 */
		ListEntry* Insert(ListEntry* prev, ListEntry* next, Element&& elem)
		{
			MakeRoom();

			ListEntry* entry = GetEmptyEntry();
			if (prev)
			{
				prev->Next = entry;
			}
			entry->Next = next;
			entry->Elem = std::move(elem); // move

			++m_CurrentEntries;

			return entry;
		}

		/**
		 * Marks the entry as free and adds it to the list of unused entries
		 */
		void FreeEntry(ListEntry* entry)
		{
			if (m_UnusedList)
			{
				ListEntry* unused = m_UnusedList;
				while (unused->Next)
				{
					unused = unused->Next;
				}
				unused->Next = entry;
			}
			else
			{
				m_UnusedList = entry;
			}
		}

	private:

		// Size of an element
		static const Size ElementSize = sizeof(Element);
		// Size of an entry to the list
		static const Size EntrySize = sizeof(ListEntry);
		// Required alignment of the entries
		static const Size EntryAlign = alignof(ListEntry);

		// Current head of the list
		ListEntry* m_Head;
		// Current tail of the list
		ListEntry* m_Tail;
		// Current maximum number of entries
		Size m_MaxEntries;
		// Current number of entries
		Size m_CurrentEntries;
		// Linked list of unused entries
		ListEntry* m_UnusedList;

		// Allocator
		Allocator m_Allocator;
	};

	template <typename T>
	using LinkedList = LinkedListBase<T, DefaultContainerAllocator<T>>;
}