#pragma once

#include "Array.h"
#include "Types.h"
#include "Memory.h"

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

	private:

		// Array that handles all of the adding/removing
		ArrayBase<s_char, Allocator> m_Array;
	};

	/**
	 * Standard growable string implementation
	 */
	typedef NStringBase<DefaultContainerAllocator> NString;

	/**
	 * Fixed-length version of NString, cannot grow past the specified size
	 * Keep in mind that the null terminator is included so Fixed<32> can
	 * hold 31 characters + 1 null terminator.
	 */
	template <Size N>
	using NStringFixed = NStringBase<FixedContainerAllocator<N>>;

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

	constexpr const U32 operator "" _hash(char const* in, Size)
	{
		return HashString(in);
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
	 * Compile-time String class used for immutable strings such as identifiers
	 * Also contains a compile-time hash that can accelerate comparisons
	 * This class is great for automatically hashing a string constant in a function parameter
	 * However, it falls flat when said function parameter needs to be stored at all
	 */
	class NImmutableIdentifier
	{
		friend class NIdentifier;

	public:

		/**
		 * Constructs the Identifier and computes the hash at compile time, if possible
		 */
		template <Size N>
		constexpr NImmutableIdentifier(const char(&in)[N])
			: m_Data(in), m_Size(N), m_Hash(HashString(in))
		{}

		/**
		 * Allows access to characters in the string, clamped at the length of the string
		 */
		constexpr char operator[](Size index) const
		{
			return index < m_Size ? m_Data[index] : m_Data[m_Size - 1];
		}

		/**
		 * Returns the length of the string
		 */
		constexpr const Size GetSize() const { return m_Size; }

		/**
		 * Returns the string represented by this Identifier
		 */
		constexpr const char* GetString() const { return m_Data; }

		/**
		 * Returns the hash of the Identifier
		 */
		constexpr const U32 GetHash() const { return m_Hash; }

		/**
		 * Comparison operators use only the hash for fast comparisons
		 */
		friend bool operator==(const NImmutableIdentifier& lhs, const NImmutableIdentifier& rhs) { return lhs.m_Hash == rhs.m_Hash; }
		friend bool operator!=(const NImmutableIdentifier& lhs, const NImmutableIdentifier& rhs) { return lhs.m_Hash != rhs.m_Hash; }

	private:

		// Original string
		const char* m_Data;
		// Length of the original string
		const Size m_Size;
		// Hashed string
		const U32 m_Hash;
	};

	/**
	 * Still immutable, but copy- and move-able version of NImmutableIdentifier
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
		 * Allow copy straight from an immutable instance
		 */
		NIdentifier(const NImmutableIdentifier& init)
			: m_Data(init.m_Data), m_Size(init.m_Size), m_Hash(init.m_Hash)
		{}

		/**
		 * Allow construction from a string literal
		 * Note that this runs the hash at runtime, so don't do this
		 */
		template <Size N>
		NIdentifier(const char(&in)[N])
			: m_Data(in), m_Size(N), m_Hash(HashString(in))
		{}

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

// Uses some template magic to run the hash at compile time, only works on constants
#define SID(x) ::Noble::HashStruct<::Noble::HashString(x)>::value

// Runs the hash at runtime, be careful with this one!
#define HASH(x) ::Noble::HashString(x)