#pragma once

#include "Array.h"
#include "Types.h"
#include "Memory.h"

#include <type_traits>

namespace Noble
{
	// String character typedef, in case I want to jump up to 16-bit chars eventually
	typedef char s_char;
	static const size_t CHAR_SIZE = sizeof(s_char);

	/**
	 * Primary string class for the engine, uses any Container allocator
	 */
	template <class Allocator>
	class NStringBase
	{
	public:

		typedef s_char CharType;

		NStringBase()
		{
			m_Array.Add(s_char(0));
		}

		/**
		 * Creates a copy of the string from the given NString
		 */
		NStringBase(const NStringBase& other)
			: m_Array(other.m_Array)
		{}

		/**
		 * Moves the requested string into this instance
		 */
		NStringBase(NStringBase&& other)
			: m_Array(std::move(other.m_Array))
		{}

		/**
		 * Initializes the string with a raw string
		 */
		NStringBase(const s_char* init)
		{
			AppendString(init, std::strlen(init));
		}

	public:

		/**
		 * Makes sure the string is prepared to hold the requested number of characters
		 */
		void Reserve(const Size amt)
		{
			if (amt > GetLength())
			{
				m_Array.Resize(amt);
			}
		}

		/**
		 * Copy assignment
		 */
		NStringBase& operator=(const NStringBase other)
		{
			if (this == &other)
			{
				return *this;
			}

			m_Array = other.m_Array;
			return *this;
		}

		/**
		 * Move assignment
		 */
		NStringBase& operator=(NStringBase&& other)
		{
			m_Array = std::move(other.m_Array);
			return *this;
		}

		/**
		 * Appends the raw string to this instance
		 */
		NStringBase& operator+=(const s_char* val)
		{
			AppendString(val, std::strlen(val));

			return *this;
		}

		/**
		 * Appends the character to the end of the string
		 */
		NStringBase& operator+=(const s_char val)
		{
			AppendChar(val);

			return *this;
		}

		/**
		 * Appends the NString to this instance
		 */
		NStringBase& operator+=(const NStringBase& other)
		{
			AppendString(other.GetCharArray(), other.GetLength());

			return *this;
		}

		/**
		 * Returns a new NString with the given raw string added to it
		 */
		NStringBase operator+(const s_char* val)
		{
			NStringBase tmp(*this);
			return tmp += val;
		}

		/**
		 * Returns a new NString with the given NString added to it
		 */
		NStringBase operator+(const NStringBase& other)
		{
			NStringBase tmp(*this);
			return tmp += other;
		}
		
		/**
		 * Removes the given number of characters from the beginning of the string
		 */
		NStringBase& TrimStart(const Size count)
		{
			if (count >= GetLength())
			{
				// If trying to trim the length of the string or more, just empty the string
				Reset();
				return *this;
			}
			m_Array.RemoveMultiple(0, count);
			return *this;
		}

		/**
		 * Removes the given number of characters from the end of the string
		 */
		NStringBase& TrimEnd(const Size count)
		{
			if (count >= GetLength())
			{
				// If trying to trim the length of the string or more, just empty the string
				Reset();
				return *this;
			}
			// Add 1 to the count to account for null terminator in the starting index
			// but do not add 1 to the count to remove
			// this ensures that removemultiple just shifts the null terminator down
			m_Array.RemoveMultiple(m_Array.GetCount() - (count + 1), count);

			return *this;
		}

		/**
		 * Allows access to string characters via [] operator
		 */
		s_char& operator[](Size index)
		{
			return m_Array[index];
		}

		/**
		 * Allows access to string characters via [] operator
		 */
		const s_char& operator[](Size index) const
		{
			return m_Array[index];
		}

	public:

		/**
		 * Returns true if this string starts with the given query
		 */
		bool StartsWith(const s_char* query) const
		{
			CHECK(query);
			const Size len = std::strlen(query);

			return StartsWith(query, len);
		}

		/**
		 * Returns true if this string starts with the given query
		 */
		bool StartsWith(const NStringBase& query) const
		{
			return StartsWith(query.GetCharArray(), query.GetLength());
		}

		/**
		 * Returns true if this string starts with the given query
		 */
		bool StartsWith(const s_char* query, Size len) const
		{
			if (len > GetLength())
			{
				// String is not long enough to even contain the query, let alone start with it
				return false;
			}

			for (Size i = 0; i < len; ++i)
			{
				if (m_Array[i] != query[i])
				{
					return false;
				}
			}

			return true;
		}

		/**
		 * Returns true if this string ends with the given query
		 */
		bool EndsWith(const s_char* query) const
		{
			CHECK(query);
			const Size len = std::strlen(query);

			return EndsWith(query, len);
		}

		/**
		 * Returns true if this string ends with the given query
		 */
		bool EndsWith(const NStringBase& query) const
		{
			return EndsWith(query.GetCharArray(), query.GetLength());
		}

		/**
		 * Returns true if this string ends with the given query
		 */
		bool EndsWith(const s_char* query, Size len) const
		{
			if (len > GetLength())
			{
				// String is not long enough to even contain the query, let alone end with it
				return false;
			}

			for (Size i = 0; i < len; ++i)
			{
				if (m_Array[GetLength() - i] != query[len - i])
				{
					return false;
				}
			}

			return true;
		}

		/**
		 * Returns the length of the string
		 */
		const Size GetLength() const
		{
			// Minus 1 is there to exclude the null terminator
			return m_Array.GetCount() - 1;
		}

		/**
		 * Returns the current max number of characters the string can hold (without reallocating)
		 */
		const Size GetMaxLength() const
		{
			// Minus 1 is there to leave room for null terminator
			return m_Array.GetMax() - 1;
		}

		/**
		 * Returns a raw string array
		 */
		const s_char* GetCharArray() const
		{
			return m_Array.GetData();
		}

		/**
		 * Implicit conversion to raw array of characters
		 */
		operator const s_char* () const
		{
			return m_Array.GetData();
		}

		/**
		 * Resets the string to an empty state
		 */
		void Reset()
		{
			m_Array.Reset();
			m_Array.Add('\0');
		}

	private:

		/**
		 * Internally used as a single central place for appending multiple chars to the string
		 */
		void AppendString(const s_char* str, Size count)
		{
			CHECK(str && count > 0);

			// Remove the top element, which will always be the null terminator
			m_Array.RemoveAt(GetLength());
			// Add the string to it, including null terminator
			m_Array.AddMultiple(str, count + 1);

			// Special case for fixed allocator: Reset the final character to null terminator manually
			// This is in case AddMultiple runs over the final character and clears it, and it won't
			// break non-fixed allocation methods
			m_Array[GetLength()] = '\0';
		}

		void AppendChar(const s_char val)
		{
			CHECK(val != '\0');

			// Change final character to val
			m_Array[GetLength()] = val;
			// Re-add null term
			m_Array.Add('\0');
			// Special case required for fixed allocator: reset final char to null terminator manually
			// the call above to "add" may fail if the allocator is full, and if that's the case, it
			// won't write the null terminator
			m_Array[GetLength()] = '\0';
		}

	private:

		// Array that handles all of the adding/removing
		ArrayBase<s_char, Allocator> m_Array;
	};

	/**
	 * Standard growable string implementation
	 */
	typedef NStringBase<DefaultContainerAllocator<s_char>> NString;

	/**
	 * Fixed-length version of NString, cannot grow past the specified size
	 * Keep in mind that the null terminator is included so Fixed<32> can
	 * hold 31 characters + 1 null terminator.
	 */
	template <Size N>
	using NStringFixed = NStringBase<FixedContainerAllocator<s_char, N>>;

	/**
	 * FNV1a C++11 Compile Time Hash Function by UnderscoreDiscovery on Github
	 * https://gist.github.com/underscorediscovery/81308642d0325fd386237cfa3b44785c
	 *
	 * FNV-1a is decently collision-averse, and likely will work fine moving forward
	 */
	constexpr U32 val32 =   0x811C9DC5;
	constexpr U32 prime32 = 0x01000193;
	constexpr const U32 HashString(const char* in, const U32 value = val32)
	{
		return (in[0] == '\0') ? value : HashString(&in[1], static_cast<U32>((value ^ U32(in[0])) * static_cast<U64>(prime32)));
	}

	/**
	 * Returns a hash of a string literal, can be computed at compile time if possible
	 */
	constexpr const U32 operator "" _hash(const char* in, Size)
	{
		return HashString(in);
	}

	/**
	 * Returns the length of a string literal
	 */
	constexpr const Size operator "" _len(char const* in, Size len)
	{
		return len;
	}

	/**
	 * Little bit of voodoo template magic that may or may not be necessary
	 */
	template<U32 N>
	struct HashStruct
	{
		static constexpr const U32 value = N;
	};

	/**
	 * An identifier that includes a hash of the string it represents
	 * Comparisons of NIdentifiers are based on the hash instead of a string comparison
	 *
	 * Originally this could be implicitly created from a string literal, but that proved
	 * to be a challenge for ensuring compile-time computation of its members. Now, generally,
	 * an NIdentifier must be created via the ID() macro.
	 */
	class NIdentifier
	{
	public:

		/**
		 * Default constructor
		 */
		NIdentifier()
			: m_Data(nullptr), m_Size(0), m_Hash(0)
		{}

		/**
		 * Constexpr constructor to allow compile-time construction of the type
		 */
		constexpr NIdentifier(const char* const str, const Size len, const U32 hash)
			: m_Data(str), m_Size(len), m_Hash(hash)
		{
		}

		/**
		 * Copy constructor
		 */
		NIdentifier(const NIdentifier& other)
			: m_Data(other.m_Data), m_Size(other.m_Size), m_Hash(other.m_Hash)
		{}

		/**
		 * Move constructor
		 */
		NIdentifier(NIdentifier&& other) noexcept
			: m_Data(other.m_Data), m_Size(other.m_Size), m_Hash(other.m_Hash)
		{
			other.m_Data = nullptr;
			other.m_Size = 0;
			other.m_Hash = 0;
		}

		/**
		 * Copy assignment
		 */
		NIdentifier& operator=(const NIdentifier& other)
		{
			if (this == &other)
			{
				return *this;
			}

			m_Data = other.m_Data;
			m_Size = other.m_Size;
			m_Hash = other.m_Hash;

			return *this;
		}

		/**
		 * Move assignment
		 */
		NIdentifier& operator=(NIdentifier&& other) noexcept
		{
			m_Data = other.m_Data;
			m_Size = other.m_Size;
			m_Hash = other.m_Hash;

			other.m_Data = nullptr;
			other.m_Size = 0;
			other.m_Hash = 0;

			return *this;
		}

		/**
		 * Returns the raw string that this Identifier represents
		 */
		const char* GetString() const { return m_Data; }

		/**
		 * Returns the length of the string
		 */
		const Size GetSize() const { return m_Size; }

		/**
		 * Returns the hash of the string
		 */
		const U32 GetHash() const { return m_Hash; }

		/**
		 * Comparison operator that uses hashes for speed
		 */
		friend bool operator==(const NIdentifier& lhs, const NIdentifier& rhs) { return lhs.GetHash() == rhs.GetHash(); }

		/**
		 * Comparison operator that uses hashes for speed
		 */
		friend bool operator!=(const NIdentifier& lhs, const NIdentifier& rhs) { return lhs.GetHash() != rhs.GetHash(); }

	private:

		// Original string
		const char* m_Data;
		// Length of the original string
		Size m_Size;
		// Hash of the string
		U32 m_Hash;
	};
}

/**
 * The below macros provide a simplified approach to hashing, particularly with 
 * regards to guaranteeing compile-time computation when possible. The use of
 * a lambda allows me to make certain the variable is marked constexpr regardless
 * of the state of the variable into which it is being read - this means that even
 * a non-const or non-constexpr variable or function argument can have a compile-time
 * hash as long as the argument to the hash is a literal or can be deduced at compile time.
 */

// Returns a hash (of type U32) of the given string
#define HASH(x) ([&] { return ::Noble::HashString(x); }())

// Creates an NIdentifier from the given string literal
#define ID(x) ([] { constexpr ::Noble::Size len = x##_len; constexpr ::Noble::U32 hash = x##_hash; return ::Noble::NIdentifier(x, len, hash); }())