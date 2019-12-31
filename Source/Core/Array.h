#pragma once

#include "Types.h"

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
			index = glm::clamp(index, 0, N - 1);
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

			index = glm::clamp(index, 0, m_Size - 1);
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
	 * Makes the assumption that the elements are trivially movable
	 * and that the user is not holding pointers to the elements.
	 */
	template <class ElementType, class Allocator>
	class Array
	{

		/**
		 * Default constructor - this does not prepare the array for any elements
		 */
		Array();

		/**
		 * Takes a pointer to an array of ElementType and the number of elements
		 * Performs a simply memory copy into the array
		 */
		Array(ElementType* ptr, Size count);

		/**
		 * Creates an exact copy of the given array
		 * Note that this will invalidate the original array when constructing this one
		 */
		Array(const Array& other);


	};
}