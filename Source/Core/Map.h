#pragma once

#include "Array.h"

namespace Noble
{

	template <class KeyType, class ValueType >
	struct KeyValuePair
	{
		KeyValuePair()
			: Key(), Value()
		{}

		KeyValuePair(const KeyType& key, const ValueType& value)
			: Key(key), Value(value)
		{}

		KeyType Key;
		ValueType Value;
	};

	/**
	 * A container that stores data that is retrieved using the key it was stored with
	 * It does not allow multiple objects of the same key to be stored
	 */
	template <class KeyType, class ValueType, class Allocator>
	class MapBase
	{
		template <class OtherKey, class OtherValue, class OtherAllocator>
		friend class MapBase;

	public:

		typedef KeyValuePair<KeyType, ValueType> Pair;
		typedef ArrayBase<Pair, Allocator> PairContainer;

		//typedef MapIterator<PairContainer, KeyType, ValueType> Iterator;
		//typedef MapIterator<const PairContainer, const KeyType, const ValueType> ConstIterator;
		
		/**
		 * Default constructor does not prepare the map for any entries
		 */
		MapBase()
		{}

		/**
		 * Copy constructor
		 */
		MapBase(const MapBase& other)
			: m_Array(other.m_Array)
		{}

		/**
		 * Move constructor
		 */
		MapBase(MapBase&& other)
			: m_Array(std::move(other.m_Array))
		{}

		/**
		 * Copy assignment
		 */
		MapBase& operator=(const MapBase& other)
		{
			m_Array = other.m_Array;
		}

		/**
		 * Move assignment
		 */
		MapBase& operator=(MapBase&& other)
		{
			m_Array = std::move(other.m_Array);
		}

	public:

		/**
		 * Adds the key/value pair to the map, or overwrites the existing value if the key is in use
		 * Returns true if the key was not in use, false if otherwise
		 */
		bool Insert(const KeyType& key, const ValueType& value)
		{
			Size entry = GetKeyIndex(key);
			if (entry == SizeMaxValue)
			{
				Pair kvp;
				kvp.Key = key;
				kvp.Value = value;
				m_Array.Add(kvp);

				return true;
			}
			else
			{
				m_Array[entry].Value = value;

				return false;
			}
		}

		/**
		 * Adds the key/value pair to the map, or overwrites the existing value if the key is in use
		 * Returns true if the key was not in use, false if otherwise
		 */
		bool Insert(const KeyType& key, ValueType&& value)
		{
			Size entry = GetKeyIndex(key);
			if (entry == SizeMaxValue)
			{
				m_Array.Add( {key, std::move(value)} );

				return true;
			}
			else
			{
				m_Array[entry].Value = std::move(value);

				return false;
			}
		}

		/**
		 * Returns the internal index of the key/value pair with the given key,
		 * or SizeMaxValue if the key is not in use
		 */
		Size GetKeyIndex(const KeyType& key)
		{
			for (auto iter = m_Array.Start(); iter != m_Array.End(); ++iter)
			{
				if (iter->Key == key)
				{
					return iter.GetIndex();
				}
			}

			return SizeMaxValue;
		}

		/**
		 * Returns the internal index of the key/value pair with the given value,
		 * or SizeMaxValue if the key is not in use
		 */
		Size GetValueIndex(const ValueType& value)
		{
			for (auto iter = m_Array.Start(); iter != m_Array.End(); ++iter)
			{
				if (iter->Value == value)
				{
					return iter.GetIndex();
				}
			}

			return SizeMaxValue;
		}

		/**
		 * Returns true if the key is in use in this map, false if otherwise
		 */
		bool ContainsKey(const KeyType& key)
		{
			for (auto iter = m_Array.Start(); iter != m_Array.End(); ++iter)
			{
				if (iter->Key == key)
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * Returns true if the value is in use in this map, false if otherwise
		 */
		bool ContainsValue(const ValueType& value)
		{
			for (auto iter = m_Array.Start(); iter != m_Array.End(); ++iter)
			{
				if (iter->Value == value)
				{
					return true;
				}
			}

			return false;
		}

		/**
		 * Allows access to values by using the [] operator with a key type
		 */
		ValueType& operator[](const KeyType& key)
		{
			return At(key);
		}

		/**
		 * Returns the value type associated with the given key
		 */
		ValueType& At(const KeyType& key)
		{
			Size entry = GetKeyIndex(key);
			CHECK(entry != SizeMaxValue);

			return m_Array[entry].Value;
		}

		/**
		 * Removes a key/value pair by the key
		 */
		void RemoveByKey(const KeyType& key)
		{
			Size entry = GetKeyIndex(key);
			if (entry != SizeMaxValue)
			{
				m_Array.RemoveAt(entry);
			}
		}

		/**
		 * Removes a key/value pair by the value
		 */
		void RemoveByValue(const ValueType& value)
		{
			Size entry = GetValueIndex(value);
			if (entry != SizeMaxValue)
			{
				m_Array.RemoveAt(entry);
			}
		}

	private:

		// Container of key/value pairs
		PairContainer m_Array;
		//Array<KeyValuePair<KeyType, ValueType>> m_Array;

	};

	template <typename KeyType, typename ValueType>
	using Map = MapBase<KeyType, ValueType, DefaultContainerAllocator>;
}