#pragma once

#include "HelperMacros.h"
#include "Memory.h"
#include "Types.h"

namespace Noble
{
	/**
	 * This container allows writing and reading arbitrary values to a buffer
	 */
	template <typename Allocator>
	class BitStreamBase
	{
	public:

		/**
		 * Empty-initializes the BitStream
		 */
		BitStreamBase()
			: m_Allocator()
		{
			m_ReaderPos = 0;
			m_StoredBytes = 0;
			m_MaxBytes = 0;
		}

		/**
		 * Initializes the BitStream with the requested number of bytes pre-allocated
		 */
		BitStreamBase(const Size startBytes)
			: BitStreamBase()
		{
			Resize(startBytes);
		}

		/**
		 * Initializes the BitStream with the given raw data
		 * Note that the raw data is copied, and the BitStream's limit is set to the 
		 * length of the buffer with no excess room (i.e. any follow-up writes are
		 * guaranteed to incur a call to Resize())
		 */
		BitStreamBase(const Byte* rawData, const Size dataSize)
			: BitStreamBase()
		{
			Resize(dataSize);
			WriteBytes(rawData, dataSize);
		}

		/**
		 * Creates a copy of the given BitStream
		 */
		BitStreamBase(const BitStreamBase& other)
			: m_Allocator(other.m_Allocator)
		{
			m_ReaderPos = other.m_ReaderPos;
			m_StoredBytes = other.m_StoredBytes;
			m_MaxBytes = other.m_MaxBytes;
		}

		/**
		 * Moves the state of the other BitStream into this new instance
		 * Leaves the original BitStream in an empty-initialized state
		 */
		BitStreamBase(BitStreamBase&& other)
			: m_Allocator(std::move(other.m_Allocator))
		{
			m_ReaderPos = other.m_ReaderPos;
			m_StoredBytes = other.m_StoredBytes;
			m_MaxBytes = other.m_MaxBytes;

			other.m_ReaderPos = 0;
			other.m_StoredBytes = 0;
			other.m_MaxBytes = 0;
		}

		/**
		 * Copies the contents of the given BitStream into this one
		 */
		BitStreamBase& operator=(const BitStreamBase& other)
		{
			m_Allocator = other.m_Allocator;
			m_ReaderPos = other.m_ReaderPos;
			m_StoredBytes = other.m_StoredBytes;
			m_MaxBytes = other.m_MaxBytes;
		}

		/**
		 * Moves the state of the other BitStream into this one
		 * Leaves the original BitStream in an empty-initialized state
		 */
		BitStreamBase& operator=(BitStreamBase&& other)
		{
			m_Allocator = std::move(other.m_Allocator);

			m_ReaderPos = other.m_ReaderPos;
			m_StoredBytes = other.m_StoredBytes;
			m_MaxBytes = other.m_MaxBytes;

			other.m_ReaderPos = 0;
			other.m_StoredBytes = 0;
			other.m_MaxBytes = 0;
		}

		/**
		 * Resizes the BitStream to hold the requested number of bytes
		 * Must be greater than or equal to the current stored bytes
		 */
		void Resize(const Size newBytes)
		{
			CHECK(newBytes >= m_StoredBytes);

			Size newMax = m_Allocator.Resize(1, newBytes);

			m_MaxBytes = newMax;
		}

		/**
		 * Shrinks the allocated size to fit the number of bytes in the BitStream
		 * If the BitStream is empty, the call does nothing
		 */
		void Shrink()
		{
			if (m_StoredBytes > 0)
			{
				Resize(m_StoredBytes);
			}
		}

		/**
		 * Returns a byte array of the data stored in the BitStream
		 */
		Byte* GetData()
		{
			return static_cast<Byte*>(m_Allocator.GetData());
		}

		/**
		 * Returns a const byte array of the data stored in the BitStream
		 */
		const Byte* GetData() const
		{
			return static_cast<const Byte*>(m_Allocator.GetData());
		}

		/**
		 * Returns the number of bytes stored in the BitStream
		 */
		const Size GetStoredBytes() const
		{
			return m_StoredBytes;
		}

		/**
		 * Returns the max number of bytes the BitStream can store
		 * Note that this can and often will change depending on the allocator
		 */
		const Size GetMaxBytes() const
		{
			return m_MaxBytes;
		}

		/**
		 * Writes an arbitrary type to the BitStream
		 */
		template <typename T>
		void Write(T value)
		{
			// Make room to store a value of size T
			MakeRoom(sizeof(T));
			
			// Use a union to make a Byte[] alias of the type
			union
			{
				T* as_T;
				Byte* as_byte_array;
			};
			as_T = &value;

			// Write the data
			for (I32 i = 0; i < sizeof(T); ++i)
			{
				GetData()[m_StoredBytes++] = as_byte_array[i];
			}
		}

		/**
		 * Writes a raw buffer of Bytes
		 */
		void WriteBytes(const Byte* data, const Size count)
		{
			CHECK(data && count > 0);

			// Make room for the new bytes
			MakeRoom(count);

			// Write the data
			for (I32 i = 0; i < count; ++i)
			{
				GetData()[m_StoredBytes++] = data[i];
			}
		}

		/**
		 * Reads an arbitrary value from the Stream
		 */
		template <typename T>
		T Read()
		{
			CHECK(m_ReaderPos + sizeof(T) <= m_StoredBytes);

			Size readPos = m_ReaderPos;
			m_ReaderPos += sizeof(T);
			return *((T*)(GetData() + readPos));
		}
		
		/**
		 * Reads the requested number of bytes into the given buffer
		 * Returns the number of bytes actually read
		 */
		Size ReadBytes(Byte* data, const Size count)
		{
			// Don't read beyond the end of the buffer
			Size bytesToRead = glm::min(count, m_StoredBytes - m_ReaderPos);

			for (U32 i = 0; i < bytesToRead; ++i)
			{
				data[i] = GetData()[m_ReaderPos++];
			}

			return bytesToRead;
		}

		/**
		 * Resets the BitStream to an empty state
		 */
		void Reset()
		{
			m_Allocator.Reset();
			m_StoredBytes = 0;
			m_MaxBytes = 0;
			m_ReaderPos = 0;
		}

		/**
		 * Called from various external read functions to notify the BitStream
		 * that bytes were manually placed in its buffer. This probably is not
		 * the function you're looking for.
		 */
		void UpdateStoredBytes(const Size bytes)
		{
			m_StoredBytes = bytes;
		}

	private:

		/**
		 * Allocates space to store the requested number of bytes
		 */
		void MakeRoom(const Size bytes)
		{
			CHECK(bytes > 0);

			Size requiredMax = m_StoredBytes + bytes;
			if (requiredMax > m_MaxBytes)
			{
				Size calcMax = m_Allocator.CalculateGrowSize(1, requiredMax);
				Resize(calcMax);
			}
		}

	private:

		// Current number of bytes stored
		Size m_StoredBytes;
		// Max number of bytes that can be stored
		Size m_MaxBytes;
		// Current position of the reader
		Size m_ReaderPos;
		// Allocator instance
		Allocator m_Allocator;
	};

	typedef BitStreamBase<DefaultContainerAllocator> BitStream;
}