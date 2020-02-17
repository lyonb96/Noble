#pragma once

#include <utility>

#include "Types.h"
#include "Memory.h"

namespace Noble
{

	/**
	 * Iterators that work on containers that use indices for element access
	 * Requires that the Container implements operator [], GetCount()/GetMax(),
	 * and CheckIndex().
	 * Based on Epic's TIndexedContainerIterator from UE4
	 */
	template <typename Container, typename ElementType>
	class IndexedContainerIterator
	{
	public:

		explicit IndexedContainerIterator(Container& arr, Size start = 0)
			: m_Container(arr), m_Index(start)
		{}

		/**
		 * Prefix increment operator
		 */
		IndexedContainerIterator& operator++()
		{
			++m_Index;
			return *this;
		}

		/**
		 * Postfix increment operator
		 */
		IndexedContainerIterator operator++(int)
		{
			IndexedContainerIterator tmp(*this);
			++m_Index;
			return tmp;
		}

		/**
		 * Prefix decrement operator
		 */
		IndexedContainerIterator& operator--()
		{
			--m_Index;
			return *this;
		}

		/**
		 * Postfix decrement operator
		 */
		IndexedContainerIterator operator--(int)
		{
			IndexedContainerIterator tmp(*this);
			--m_Index;
			return tmp;
		}

		/**
		 * Arithmetic operators
		 */

		IndexedContainerIterator& operator+=(Size offset)
		{
			m_Index += offset;
			return *this;
		}

		IndexedContainerIterator operator+(Size offset) const
		{
			IndexedContainerIterator tmp(*this);
			return tmp += offset;
		}

		IndexedContainerIterator& operator-=(Size offset)
		{
			m_Index -= offset;
			return *this;
		}

		IndexedContainerIterator operator-(Size offset) const
		{
			IndexedContainerIterator tmp(*this);
			return tmp -= offset;
		}

		/**
		 * Dereference operator accesses the element in the container at the current index
		 */
		ElementType& operator* () const
		{
			return m_Container[m_Index];
		}

		/**
		 * Dereference operator accesses the element in the container at the current index
		 */
		ElementType* operator-> () const
		{
			return &m_Container[m_Index];
		}

		/**
		 * Allows conversion to bool for quick checking if the iterator is valid
		 */
		FORCEINLINE operator bool() const
		{
			return m_Container.CheckIndex(m_Index);
		}

		/**
		 * Returns the current index of the iterator
		 */
		Size GetIndex() const
		{
			return m_Index;
		}

		/**
		 * Sets the index of the iterator to 0
		 */
		void Reset()
		{
			m_Index = 0;
		}

		/**
		 * Sets the index of the iterator to the end of the container
		 */
		void SetToEnd()
		{
			m_Index = m_Container.GetCount();
		}

		/**
		 * Removes the current element from the container
		 */
		void RemoveCurrent()
		{
			m_Container.RemoveAt(m_Index);
			--m_Index;
		}

		/**
		 * Comparison operators
		 */

		FORCEINLINE friend bool operator==(const IndexedContainerIterator& lhs, const IndexedContainerIterator& rhs)
		{
			return (&lhs.m_Container == &rhs.m_Container) && (lhs.GetIndex() == rhs.GetIndex());
		}

		FORCEINLINE friend bool operator!=(const IndexedContainerIterator& lhs, const IndexedContainerIterator& rhs)
		{
			return (lhs.GetIndex() != rhs.GetIndex()) || (&lhs.m_Container != &rhs.m_Container);
		}

	protected:

		Container& m_Container;
		Size m_Index;

	};

	/**
	 * An array implementation inspired by Unreal Engine's TArray
	 *
	 * Capable of allocating more memory on the fly to hold more elements
	 * Elements are required to have a copy constructor, and users should
	 * not hold pointers to elements in the array, as they may be moved.
	 */
	template <typename ElementType, typename Allocator>
	class ArrayBase
	{
		template <typename OtherElemType, typename OtherAlloc>
		friend class ArrayBase;

	public:

		typedef IndexedContainerIterator<ArrayBase, ElementType> Iterator;
		typedef IndexedContainerIterator<const ArrayBase, const ElementType> ConstIterator;

		/**
		 * Default constructor - this does not prepare the array for any elements
		 */
		ArrayBase()
		{
			m_ArrayCount = 0;
			m_ArrayMax = 0;
		}

		/**
		 * Takes a pointer to an array of ElementType and the number of elements
		 * Performs a simple memory copy into the array
		 */
		ArrayBase(ElementType* ptr, Size count)
		{
			AddMultiple(ptr, count);
		}

		/**
		 * Creates an exact copy of the given array
		 */
		ArrayBase(const ArrayBase& other)
			: m_Allocator(other.m_Allocator)
		{
			m_ArrayMax = other.m_ArrayMax;
			m_ArrayCount = other.m_ArrayCount;
		}

		/**
		 * Moves the data from the other array to this one
		 * Leaves the other array in an empty state
		 */
		ArrayBase(ArrayBase&& other) noexcept
			: m_Allocator(std::move(other.m_Allocator))
		{
			m_ArrayMax = other.m_ArrayMax;
			m_ArrayCount = other.m_ArrayCount;

			other.m_ArrayMax = 0;
			other.m_ArrayCount = 0;
		}

		/**
		 * Copy assignment
		 */
		ArrayBase& operator=(const ArrayBase& other)
		{
			if (this == &other)
			{
				return *this;
			}

			m_Allocator = other.m_Allocator;
			m_ArrayMax = other.m_ArrayMax;
			m_ArrayCount = other.m_ArrayCount;

			return *this;
		}

		/**
		 * Move assignment
		 */
		ArrayBase& operator=(ArrayBase&& other)
		{
			m_Allocator = std::move(other.m_Allocator);
			m_ArrayMax = other.m_ArrayMax;
			m_ArrayCount = other.m_ArrayCount;

			other.m_ArrayCount = 0;
			other.m_ArrayMax = 0;
		}

	public:

		/**
		 * Moves the requested element to the end of array and returns its index
		 */
		Size Add(ElementType&& elem)
		{
			Size index = MakeRoom();

			Emplace(index, std::move(elem));

			return index;
		}

		/**
		 * Copies the requested element to the end of array and returns its index
		 */
		Size Add(const ElementType& elem)
		{
			Size index = MakeRoom();

			Emplace(index, elem);

			return index;
		}

		/**
		 * Moves @count elements from the array @ptr
		 * Returns the index of the last added element
		 */
		Size AddMultiple(ElementType* ptr, Size count)
		{
			// Prepare the array
			Size index = MakeRoom(count);

			for (U32 i = 0; i < count; ++i)
			{
				Emplace(index, std::move(*(ptr + i)));
				++index;
			}

			return index;
		}

		/**
		 * Copies @count elements from the array @ptr
		 * Returns the index of the last added element
		 */
		Size AddMultiple(const ElementType* ptr, Size count)
		{
			// Prepare the array
			Size index = MakeRoom(count);

			for (U32 i = 0; i < count; ++i)
			{
				Emplace(index, *(ptr + i));
				++index;
			}

			return index;
		}

		/**
		 * Copies the given element to the requested index, assuming the index is valid
		 * Shifts up all proceeding array elements
		 */
		Size Insert(const ElementType& elem, Size index)
		{
			/* VIBE */ CHECK((index >= 0) && (index < m_ArrayCount));

			Size currentEnd = MakeRoom();
			ShiftUp(index);

			Emplace(index, elem);

			return index;
		}

		/**
		 * Moves the given element to the requested index, assuming the index is valid
		 * Shifts up all proceeding array elements
		 */
		Size Insert(ElementType&& elem, Size index)
		{
			// vibe check
			CHECK((index >= 0) && (index < m_ArrayCount));

			MakeRoom();
			ShiftUp(index);

			Emplace(index, elem);

			return index;
		}

		/**
		 * Removes the given element from the array, if it is part of the array
		 * Returns true if the element was removed, false if it was not part of the array
		 */
		bool Remove(ElementType& elem)
		{
			Size index = Find(elem);
			if (index != SizeMaxValue)
			{
				return RemoveAt(index);
			}
			return false;
		}

		/**
		 * Removes the element at the given index, assuming the index is valid
		 * Returns true if an element was removed, false if the index was invalid
		 */
		bool RemoveAt(Size index)
		{
			if (index >= 0 && index < m_ArrayCount)
			{
				ShiftDown(index);
				--m_ArrayCount;
				return true;
			}
			
			return false;
		}

		/**
		 * Removes the requested number of elements, starting with the given index
		 */
		void RemoveMultiple(const Size startIndex, const Size count)
		{
			CHECK(CheckIndex(startIndex) && count > 0 && startIndex + count <= GetCount());
			
			Size itemsToMove = GetCount() - (startIndex + count);
			for (Size i = 0; i < itemsToMove; ++i)
			{
				MoveElement(startIndex + count + i, startIndex + i);
			}

			m_ArrayCount -= count;
		}

		/**
		 * Checks to see if the element is in this Array and returns its index if so
		 * If it is not in the array, it returns Size's max value
		 */
		Size Find(ElementType&& elem) const
		{
			for (Size i = 0; i < m_ArrayCount; ++i)
			{
				if (GetData()[i] == elem)
				{
					return i;
				}
			}

			return SizeMaxValue;
		}

		/**
		 * Checks to see if the element is in this Array and returns its index if so
		 * If it is not in the array, it returns Size's max value
		 */
		Size Find(const ElementType& elem) const
		{
			for (Size i = 0; i < m_ArrayCount; ++i)
			{
				if (GetData()[i] == elem)
				{
					return i;
				}
			}

			return SizeMaxValue;
		}

		/**
		 * Provides access to Array elements via [] operator
		 */
		ElementType& operator[](Size index)
		{
			CHECK(index >= 0 && index < m_ArrayCount);

			return GetData()[index];
		}

		/**
		 * Provides access to Array elements via [] operator
		 */
		const ElementType& operator[](Size index) const
		{
			CHECK(index >= 0 && index < m_ArrayCount);

			return GetData()[index];
		}

		/**
		 * Resizes the Array to hold the new requested amount
		 * Only performs an action if newCount >= current count
		 */
		void Resize(Size newCount)
		{
			CHECK(newCount >= m_ArrayCount);

			m_ArrayMax = m_Allocator.Resize(newCount);
			ConstructDefaults(m_ArrayCount);
		}

		/**
		 * Resizes the array to hold exactly as many elements as it currently has
		 * or only 1 if the array is empty
		 */
		void Shrink()
		{
			if ((m_ArrayCount < m_ArrayMax) || m_ArrayCount == 0)
				Resize(glm::max(Size(1), m_ArrayCount));
		}

		/**
		 * Resets the array to an empty state with no stored elements
		 */
		void Reset()
		{
			m_Allocator.Reset();
			m_ArrayCount = 0;
			m_ArrayMax = 0;
		}

		/**
		 * Returns a pointer to the array data
		 */
		ElementType* GetData()
		{
			return static_cast<ElementType*>(m_Allocator.GetData());
		}

		/**
		 * Returns a pointer to the array data
		 */
		const ElementType* GetData() const
		{
			return static_cast<const ElementType*>(m_Allocator.GetData());
		}

		/**
		 * Iterator that points to the first element in the Array
		 */
		Iterator Start()
		{
			return Iterator(*this, 0);
		}

		/**
		 * Iterator that points to the first element in the Array
		 */
		ConstIterator Start() const
		{
			return ConstIterator(*this, 0);
		}

		/**
		 * Iterator that points to the end of the Array
		 */
		Iterator End()
		{
			return Iterator(*this, GetCount());
		}

		/**
		 * Iterator that points to the end of the Array
		 */
		ConstIterator End() const
		{
			return ConstIterator(*this, GetCount());
		}

		// RANGED FOR LOOP SUPPORT

		/**
		 * Iterator that points to the first element in the Array
		 */
		Iterator begin()
		{
			return Start();
		}

		/**
		 * Iterator that points to the first element in the Array
		 */
		ConstIterator begin() const
		{
			return Start();
		}

		/**
		 * Iterator that points to the end of the Array
		 */
		Iterator end()
		{
			return End();
		}

		/**
		 * Iterator that points to the end of the Array
		 */
		ConstIterator end() const
		{
			return End();
		}

		/**
		 * Returns the current number of elements in the array
		 */
		const Size GetCount() const
		{
			return m_ArrayCount;
		}

		/**
		 * Returns the maximum number of elements the array can currently hold
		 */
		const Size GetMax() const
		{
			return m_ArrayMax;
		}

		/**
		 * Returns true if the index is valid, false if otherwise
		 */
		bool CheckIndex(const Size& index) const
		{
			return (index >= 0 && index < m_ArrayCount);
		}

	private:

		/**
		 * Builds a default object for all array elements after the given index
		 */
		void ConstructDefaults(Size index = 0)
		{
			for (index; index < m_ArrayMax; ++index)
			{
				new (GetData() + index) ElementType;
			}
		}

		/**
		 * Moves the element to the given index
		 */
		Size Emplace(Size index, ElementType&& elem)
		{
			if (index >= m_ArrayMax)
			{
				return SizeMaxValue;
			}
			*(GetData() + index) = elem;
			++m_ArrayCount;
			return index;
		}

		/**
		 * Copies the element to the given index
		 */
		Size Emplace(Size index, const ElementType& elem)
		{
			if (index >= m_ArrayMax)
			{
				return SizeMaxValue;
			}
			*(GetData() + index) = elem;
			++m_ArrayCount;
			return index;
		}

		/**
		 * Adds @count (default 1) to the array count, and may grow the array
		 * if necessary
		 *
		 * Returns the number of array elements before adding
		 */
		Size MakeRoom(Size count = 1)
		{
			Size old = m_ArrayCount;
			if ((m_ArrayCount + count) > m_ArrayMax)
			{
				Grow(m_ArrayCount + count);
			}

			return old;
		}

		/**
		 * Grows the Array's storage to hold more data. It can optionally use a
		 * requested minimum as guidance for the calculation, though it is up to
		 * the Allocator if this is actually used or not.
		 */
		void Grow(Size minNew = 0)
		{
			Size grow = m_Allocator.CalculateGrowSize(minNew);

			Resize(grow);
		}

		/**
		 * Shifts every element starting at the given index up by the requested amount (default 1)
		 * Function assumes that room has already been made to accomodate the action
		 * End result is that the passed-in index is now "available" for use
		 */
		void ShiftUp(Size index, Size amount = 1)
		{
			Size elementsToMove = m_ArrayCount - index;

			for (Size i = elementsToMove; i > 0; --i)
			{
				Size elementIndex = index + i;
				MoveElement(elementIndex, elementIndex + amount);
			}
		}

		/**
		 * Shifts every element after the given index down by 1
		 * End result is that the given index has been overwritten by the next element in the array,
		 * and that element overwritten by its next, etc
		 */
		void ShiftDown(Size index)
		{
			Size elementsToMove = m_ArrayCount - index;

			// i = 1 to skip the first copy because that's the one we want to overwrite
			for (Size i = 1; i < elementsToMove; ++i)
			{
				Size elementIndex = index + i;
				MoveElement(elementIndex, elementIndex - 1);
			}
		}

		/**
		 * Moves the element at the "from" index to the "to" index
		 * It will overwrite any existing element at the "to" index
		 */
		void MoveElement(Size fromIndex, Size toIndex)
		{
			// it ain't pretty, but it does the job
			*(GetData() + toIndex) = std::move(*(GetData() + fromIndex));
		}

	private:

		// Allocator instance
		Allocator m_Allocator;
		// Current number of items in the array
		Size m_ArrayCount;
		// Max number of items the array can currently hold
		Size m_ArrayMax;

	};

	// RANGED FOR LOOP SUPPORT

	template <typename Container, typename ElementType>
	IndexedContainerIterator<Container, ElementType> begin(Container& cont)
	{
		return cont.begin();
	}

	template <typename Container, typename ElementType>
	IndexedContainerIterator<Container, ElementType> end(Container& cont)
	{
		return cont.end();
	}

	/**
	 * Array that uses the default heap allocator
	 */
	template <typename T>
	using Array = ArrayBase<T, DefaultContainerAllocator<T>>;

	template <typename T, Size N>
	using FixedArray = ArrayBase<T, FixedContainerAllocator<T, N>>;
}