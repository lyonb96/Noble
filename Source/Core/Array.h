#pragma once

#include <utility>

#include "Types.h"
#include "Memory.h"

namespace Noble
{
	/**
	 * An array of fixed size, which cannot be added or removed from.
	 * Changing elements is done by assignment rather than add/remove
	 * Primarily used as a safety wrapper to prevent out of bounds read/write
	 */
	template<class T, Size N>
	class FixedArray
	{
	public:

		/**
		 * Allows bracket access to members of the array and clamps the given
		 * index to ensure safe read/write
		 */
		T& operator[](Size index)
		{
			index = glm::clamp(index, Size(0), N - 1);
			return m_Array[index];
		}

		const Size GetSize() const { return N; }

	private:

		T m_Array[N];
	};

	/**
	 * Wraps an existing, externally-created Array for safe read/write
	 */
	template <class T>
	class SafeArray
	{
	public:

		SafeArray()
			: m_Array(nullptr), m_Size(0)
		{}

		SafeArray(T* data, Size count)
			: m_Array(data), m_Size(count)
		{}

		/**
		 * Sets the internal pointer and the number of elements
		 */
		void Initialize(T* data, Size count)
		{
			CHECK(data != nullptr);
			CHECK(count > 0);

			m_Array = data;
			m_Size = count;
		}

		/**
		 * Allows bracket access to members of the array and clamps the given
		 * index to ensure safe read/write
		 */
		T& operator[](Size index)
		{
			CHECK(m_Array != nullptr && m_Size > 0);

			index = glm::clamp(index, Size(0), m_Size - 1);
			return m_Array[index];
		}

		const Size GetSize() const { return m_Size; }
		
	private:

		T* m_Array;
		Size m_Size;
	};

	/**
	 * An array implementation inspired by Unreal Engine's TArray
	 *
	 * Capable of allocating more memory on the fly to hold more elements
	 * Elements are required to have a copy constructor, and users should
	 * not hold pointers to elements in the array, as they may be moved.
	 */
	template <class ElementType, class Allocator>
	class ArrayBase
	{
	public:

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
		{
			m_ArrayMax = other.m_ArrayMax;
			m_ArrayCount = other.m_ArrayCount;

			Resize(m_ArrayMax);

			m_Allocator.CopyFrom(other.m_Allocator);
		}

	public:

		/**
		 * Moves the requested element to the end of array and returns its index
		 */
		Size Add(ElementType&& elem)
		{
			Size index = MakeRoom();

			Emplace(index, elem);

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
		 * Copies @count elements from the array @ptr
		 * Returns the index of the last added element
		 */
		Size AddMultiple(ElementType* ptr, Size count)
		{
			// Prepare the array
			Size index = MakeRoom(count);

			for (U32 i = 0; i < count; ++i)
			{
				ElementType& elem = *(ptr + i);
				Emplace(index, elem);
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
			// vibe check
			CHECK((index >= 0) && (index < m_ArrayCount));

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
		Size Find(ElementType elem) const
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
		 * Resizes the Array to hold the new requested amount
		 * Only performs an action if newCount >= current count
		 */
		void Resize(Size newCount)
		{
			CHECK(newCount >= m_ArrayCount);

			m_Allocator.Resize(ElementSize, newCount, ElementAlign);
			m_ArrayMax = newCount;
		}

		/**
		 * Resizes the array to hold exactly as many elements as it currently has
		 * or only 1 if the array is empty
		 */
		void Shrink()
		{
			Resize(glm::max(0, m_ArrayCount));
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

	private:

		/**
		 * Moves the element to the given index
		 */
		Size Emplace(Size index, ElementType&& elem)
		{
			*(GetData() + index) = elem;
			++m_ArrayCount;
			return index;
		}

		/**
		 * Copies the element to the given index
		 */
		Size Emplace(Size index, const ElementType& elem)
		{
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
		 * Grows the Array by either 50% or 4 elements or the requested minimum new amount (if not 0)
		 * whichever is greatest
		 */
		void Grow(Size minNew = 0)
		{
			Size old = m_ArrayCount;
			// 50% increase, but always grow at least 4
			minNew = (minNew == 0 ? old + 4 : minNew);
			Size grow = glm::max((old * 3) / 2, minNew);

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
			ElementType tmp = std::move(GetData()[fromIndex]);
			*(GetData() + fromIndex) = std::move(*(GetData() + toIndex));
			*(GetData() + toIndex) = std::move(tmp);
		}

	private:

		// Size in bytes of the element type
		const Size ElementSize = sizeof(ElementType);
		// Required alignment of the element type
		const Size ElementAlign = alignof(ElementType);

		// Allocator instance
		Allocator m_Allocator;
		// Current number of items in the array
		Size m_ArrayCount;
		// Max number of items the array can currently hold
		Size m_ArrayMax;

	};

	template <typename T>
	using Array = ArrayBase<T, DefaultContainerAllocator>;
}