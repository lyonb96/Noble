#pragma once

#include <fstream>
#include <json/json_fwd.hpp>

#include "Types.h"

#define NE_FILE_RETURN "\r\n"

namespace Noble
{
	using json = nlohmann::json;

	/**
	 * Wraps file functionality, it's also RAII so the file is opened in the constructor and closed in the destructor
	 */
	class File
	{
	protected:

		// Path to the file
		const char* m_FilePath;
		// File stream to handle reads and writes
		std::fstream m_FileStream;
		// Size of the file, obtained from seekg()
		Size m_FileSize;

	public:

		/**
		 * Opens the given file
		 * Optional bool parameters:
		 * truncate clears the file contents on open
		 * create tells the system to create the file if it does not exist
		 */
		explicit File(const char* path, bool truncate = false, bool create = false);

		/**
		 * Copy constructor, opens the file a second time
		 */
		File(const File& other);

		/**
		 * Returns the size in bytes of the file
		 */
		const Size GetFileSize() const;

		/**
		 * Reads the entire file into the buffer
		 * Make sure the given buffer is at least as large as File::GetFileSize()!
		 */
		void ReadIntoBuffer(char* buffer);

		/**
		 * Reads a single line into the given buffer. Probably won't work too well for binary files.
		 */
		bool ReadLineIntoBuffer(char* buffer, Size len = 255);

		/**
		 * Reads the entire file's contents into the given JSON object
		 */
		void ReadIntoJson(json& in);

		/**
		 * Writes the given object to the file in binary
		 */
		template <class T>
		void WriteBinaryToFile(T& obj)
		{
			char* as_char = reinterpret_cast<char*>(&obj);

			m_FileStream.write(as_char, sizeof(T));
		}

		/**
		 * Writes the given value to the file in binary
		 */
		template <class T>
		void WriteBinaryToFile(T&& rvr)
		{
			T obj = rvr;
			char* as_char = reinterpret_cast<char*>(&obj);

			m_FileStream.write(as_char, sizeof(T));
		}

		/**
		 * Writes a constant reference to the file in binary
		 */
		template <class T>
		void WriteBinaryToFile(const T& obj)
		{
			char* as_char = const_cast<char*>(reinterpret_cast<const char*>(&obj));

			m_FileStream.write(as_char, sizeof(T));
		}

		/**
		 * Overloaded insertion operator to make writing easier
		 */
		template <class T>
		File& operator <<(T in)
		{
			m_FileStream << in;
			return *this;
		}

		template <class T>
		File& operator >>(T& in)
		{

			Size readLen = sizeof(T);
			union
			{
				T* as_T;
				char* as_char;
			};
			as_T = &in;

			m_FileStream.read(as_char, readLen);
			return *this;
		}

		/**
		 * Allows simpler checks for valid file handles
		 */
		operator bool() const;

		/**
		 * Properly closes the file
		 */
		~File();
	};
}