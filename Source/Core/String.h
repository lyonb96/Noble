#pragma once

#include <stdexcept>

#include "Types.h"
#include "Memory.h"

namespace Noble
{
	// String character typedef, in case I want to jump up to 16-bit chars eventually
	typedef char s_char;
	static const size_t CHAR_SIZE = sizeof(s_char);

	FORCEINLINE Size StringLength(const s_char* str)
	{
		if (str)
		{
			char c = str[0];
			Size out = 0;
			for (out = 0; c != 0; c = str[out++]);

			return out;
		}

		return 0;
	}

	FORCEINLINE Size StringCopy(s_char* dest, const s_char* src, Size max)
	{
		if (src && dest)
		{
			Size i = 0;
			while (i < max && src[i] != 0)
			{
				dest[i] = src[i++];
			}

			return i;
		}

		return 0;
	}

	/**
	 * Fixed-length strings. These can be manipulated, but can never exceed their fixed size.
	 */
	template <Size N>
	class FixedString
	{
	public:

		// Constructors

		explicit FixedString(const char c)
		{
			AddChar(c);
		}

		// Initializes the string with the given null-terminated c-string
		explicit FixedString(const char* cstr)
		{
			AddCString(cstr);
		}

		// Copies another FixedString (of the same length) to this one
		FixedString(const FixedString& other)
		{
			m_End = other.m_End;
			std::memcpy(m_RawStr, other.m_RawStr, N);
		}

		// Default constructor - empty string
		FixedString()
		{
			m_RawStr[0] = '\0';
		}


		// Operators

		// Add a char to the end of the string, if possible
		FixedString& operator+(const char c)
		{
			AddChar(c);

			return *this;
		}

		// Add a c-string to the end of this string, if possible
		FixedString& operator+(const char* cstr)
		{
			AddCString(cstr);

			return *this;
		}

		// Add a char to the end of the string, if possible
		FixedString& operator+=(const char c)
		{
			AddChar(c);

			return *this;
		}

		// Add a c-string to the end of this string, if possible
		FixedString& operator+=(const char* cstr)
		{
			AddCString(cstr);

			return *this;
		}

		// Array accessor
		char& operator[](Size index)
		{
			if (index > 0 && index < N)
			{
				return m_RawStr[index];
			}
		}

		// Equals comparison operator
		bool operator==(const FixedString& other)
		{
			for (Size i = 0; i < N; ++i)
			{
				if (m_RawStr[i] == '\0' && other.m_RawStr[i] == '\0')
				{
					break;
				}

				if (m_RawStr[i] != other.m_RawStr[i])
				{
					return false;
				}
			}

			return true;
		}

		bool operator==(const char* other)
		{
			for (Size i = 0; i < N; ++i)
			{
				if (m_RawStr[i] == '\0' && other[i] == '\0')
				{
					break;
				}

				if (m_RawStr[i] != other[i])
				{
					return false;
				}
			}

			return true;
		}

		FixedString<N>& operator=(const char* in)
		{
			ClearString();
			AddCString(in);

			return *this;
		}


		// Useful manipulators

		// Removes the last character from the string
		void Pop()
		{
			if (m_End > 0)
			{
				m_RawStr[--m_End] = '\0';
			}
		}

		// Returns this string represented as a raw c-string
		const char* GetRawString()// const
		{
			return m_RawStr;
		}

		operator const char*() const
		{
			return m_RawStr;
		}

		const Size& GetSize() const
		{
			return m_End;
		}

		void ClearString()
		{
			Size i = 0;
			for (i = 0; i < N; i++)
			{
				m_RawStr[i] = 0;
			}
			m_RawStr[0] = '\0';
			m_End = 0;
		}

	private:

		// Helpful functions

		// This function will try to add a char to the end of the string.
		// It will return false if it fails.
		bool AddChar(const char c)
		{
			if (m_End == N - 1)
			{
				return false;
			}

			m_RawStr[m_End] = c;
			m_RawStr[++m_End] = '\0';

			return true;
		}

		// This function will add as much of a c-string as possible to the string.
		void AddCString(const char* cstr)
		{
			Size i = 0;
			while (cstr[i] != '\0' && AddChar(cstr[i]))
			{
				++i;
			}
		}

	private:

		// Raw c-string
		char m_RawStr[N];
		// The index of the null terminator
		Size m_End = 0;
	};

	/**
	 * Variable length String that uses a Container allocator for memory allocs
	 */
	template <class Allocator>
	class BaseString
	{
	public:

		/**
		 * Creates an empty string
		 */
		BaseString()
			: m_RawString(nullptr),
			m_StrLength(0),
			m_StrLimit(0)
		{}

		/**
		 * Allocates the given number of characters ahead of time
		 */
		explicit BaseString(const U32 reserve)
		{
			m_StrLength = 0;
			if (reserve > 0)
			{
				m_RawString = (s_char*) m_Alloc.Resize(CHAR_SIZE, reserve);
				m_StrLimit = reserve;
			}
			else
			{
				m_RawString = nullptr;
				m_StrLimit = 0;
			}
		}

		/**
		 * Fills the string with the given raw string (and exactly enough memory to hold it)
		 */
		BaseString(const s_char* rawIn)
		{
			m_RawString = nullptr;
			m_StrLength = 0;
			m_StrLimit = 0;

			AppendRawString(rawIn, StringLength(rawIn));
		}

		/**
		 * Allocates fresh space and copies the other string's contents into this one
		 */
		BaseString(const BaseString& other)
		{
			m_StrLimit = other.m_StrLimit;
			m_StrLength = 0;
			m_RawString = nullptr;
			if (m_StrLimit > 0)
			{
				// Allocate space to copy the string
				m_RawString = (s_char*) m_Alloc.Resize(CHAR_SIZE, m_StrLimit);
				// Copy the other string's content into this one
				StringCopy(m_RawString, other.m_RawString, m_StrLimit);
				// Store the string length
				m_StrLength = other.m_StrLength;
			}
		}

		/**
		 * Adds the contents of the given BaseString to the end of this one
		 */
		BaseString& operator+(const BaseString& other)
		{
			AppendRawString(other.m_RawString, other.m_StrLength);

			return *this;
		}

		/**
		 * Adds the contents of the given raw string to the end of this one
		 */
		BaseString& operator+(const s_char* other)
		{
			AppendRawString(other, StringLength(other));

			return *this;
		}


		/**
		 * Adds the contents of the given BaseString to the end of this one
		 */
		BaseString& operator+=(const BaseString& other)
		{
			AppendRawString(other.m_RawString, other.m_StrLength);

			return *this;
		}


		/**
		 * Adds the contents of the given raw string to the end of this one
		 */
		BaseString& operator+=(const s_char* other)
		{
			AppendRawString(other, StringLength(other));

			return *this;
		}

		const s_char* GetRawString() const
		{
			return m_RawString;
		}

		const Size GetLength() const
		{
			return m_StrLength;
		}

	private:

		void AppendRawString(const s_char* str, Size length)
		{
			if (length > 0)
			{
				Size totalStrSize = m_StrLength + length;

				if (totalStrSize > m_StrLimit)
				{
					// Allocate space for the new string with some extra space (defaults to 4 for now)
					// In the future I'd like to just round up to the next power of 2
					Size newLimit = totalStrSize + 4;
					m_RawString = (s_char*) m_Alloc.Resize(CHAR_SIZE, newLimit);
					m_StrLimit = newLimit;
				}

				I32 srcIndex = 0, destIndex = (m_StrLength == 0 ? 0 : m_StrLength - 1);
				while (srcIndex < length)
				{
					m_RawString[destIndex++] = str[srcIndex++];
				}
				// Add the additional length onto this string, minus 1 to exclude the null terminator from the original string (if applicable)
				m_StrLength += (m_StrLength == 0 ? srcIndex : srcIndex - 1);
			}
		}

	private:

		// Container Allocator
		Allocator m_Alloc;
		// Raw string array
		s_char* m_RawString;
		// Length of the string including null terminator
		Size m_StrLength;
		// Characters allocated
		Size m_StrLimit;
	};

	typedef BaseString<BasicContainerAllocator> String;

	/**
	 * Time to go for compile-time string hashing
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

	constexpr const U32 operator "" _hash(char const* in, Size)
	{
		return HashString(in);
	}

	template<U32 N>
	struct HashStruct
	{
		static constexpr const U32 value = N;
	};

	/**
	 * This will hopefully be a compile-time string class
	 * Works fine as both a constexpr string class and a
	 * holder for a regular const char* at runtime
	 * Constexpr can make effective use of compile-time hashing
	 *
	 * Based on Scott Schurr's str_const, presented at C++ Now 2012
	 *
	 * For my own sanity... the P stands for permanent. I know I'll forget that at some point.
	 */
	class PString
	{
	public:

		template <Size N>
		constexpr PString(const char(&in)[N])
			: m_Data(in), m_Size(N), m_Hash(HashString(in))
		{}

		constexpr char operator[](Size index) const
		{
			return index < m_Size ? m_Data[index] : throw std::out_of_range("Index out of range in PString");
		}

		constexpr const Size GetSize() const { return m_Size; }

		constexpr operator const char*() const { return m_Data; }

		constexpr operator const U32() const { return m_Hash; }

		constexpr const U32 GetHash() const { return m_Hash; }

	private:

		const char* m_Data;
		const Size m_Size;
		const U32 m_Hash;
	};

	// Below are some nifty string manipulating functions

	inline bool StrStartsWith(const char* in, const char* phrase)
	{
		I32 ind = 0;
		while (in[ind] != '\0' && phrase[ind] != '\0')
		{
			if (in[ind] != phrase[ind])
			{
				return false;
			}
			++ind;
		}

		return true;
	}

	inline bool StrMatches(const char* a, const char* b)
	{
		I32 ind = 0;
		while (a[ind] == b[ind])
		{
			if (a[ind] == '\0')
			{
				return true;
			}
			++ind;
		}

		return false;
	}

	/**
	 * Returns the index of the first instance of the given character in the string, or -1 if none
	 */
	inline I32 IndexOf(char val, const char* str)
	{
		I32 ind = 0;
		while (str[ind] != '\0')
		{
			if (str[ind] == val)
			{
				return ind;
			}
			++ind;
		}

		return -1;
	}

	/**
	 * Copies "source" into "dest" and optionally appends a null terminator
	 */
	inline void CopyInto(const char* source, char* dest, bool appendNull = true)
	{
		I32 ind = 0;
		while (source[ind] != '\0')
		{
			dest[ind] = source[ind];
			++ind;
		}

		if (appendNull)
		{
			dest[ind] = '\0';
		}
	}
}

// Uses some template magic to run the hash at compile time, only works on constants
#define SID(x) ::Noble::HashStruct<::Noble::HashString(x)>::value

// Runs the hash at runtime, be careful with this one!
#define HASH(x) ::Noble::HashString(x)