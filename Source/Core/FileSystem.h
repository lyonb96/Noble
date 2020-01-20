#pragma once

#include <cstdio>
#include <filesystem>

#include "Array.h"
#include "Memory.h"
#include "Types.h"

namespace Noble
{
	namespace fs = std::filesystem;

	/**
	 * Represents a directory and allows traversal to parent and child directories
	 * Can also return an open File contained in the directory
	 */
	class Directory
	{
	public:

		Directory();

		/**
		 * Initializes the Directory in the given path
		 */
		Directory(const char* path);

		/**
		 * Changes the Directory to the given path
		 */
		void SetPath(const char* path);

		/**
		 * Changes the Directory to its parent
		 */
		void GotoParent();

		/**
		 * Changes the Directory to the given child of the current Directory
		 */
		void GotoSubdir(const char* dir);

		/**
		 * Returns an Array of paths to files or folders in the directory
		 */
		Array<fs::path> GetChildren();

	private:

		// Path object for manipulation and iteration
		fs::path m_Path;

	};



	/**
	 * Returns true if the requested file exists, false if not
	 */
	bool CheckExists(const char* path);

	/**
	 * Returns the size of the given file, or 0 if the file is not found
	 */
	const Size CheckFileSize(const char* path);

	/**
	 * Describes different modes that files can be opened with
	 */
	enum class FileMode
	{
		// Opens the file for reading only. Does not create the file if it doesn't exist.
		READONLY,
		// Opens the file for read/write. Will create the file if it does not exist. Overwrites existing content
		READWRITE,
		// Opens the file for write-only. Will create the file if it does not exist. Appends to existing content
		WRITE_APPEND,
		// Opens the file for read/write. Will create the file if it does not exist. Appends to existing content
		READ_APPEND
	};

	/**
	 * Converts a FileMode to an fopen() mode string
	 */
	const char* GetModeString(FileMode mode, bool bin);

	/**
	 * Wraps functionality for a file.
	 *
	 * The goal of this class is to provide access to reading and writing without any heap allocations.
	 * The user provides memory they have allocated themselves to receive file data.
	 */
	class File
	{
	public:

		File();

		/**
		 * Creates a File instance and opens the requested file immediately
		 */
		File(const char* path, bool bin = true, FileMode mode = FileMode::READONLY);

		/**
		 * Opens the requested file with the requested mode (default Read-Only)
		 * Can also be specified to be a binary or text-based file (default is binary)
		 * It will also close any existing file this instance is working on
		 */
		bool OpenFile(const char* path, bool bin = true, FileMode mode = FileMode::READONLY);

		/**
		 * Closes any existing file this instance is working on
		 */
		void CloseFile();

		/**
		 * Destructor, makes sure to close the file
		 */
		~File();

	public:

		/**
		 * Returns total size of the file in bytes
		 */
		Size GetFileSize() const;

		/**
		 * Reads from the file into the given buffer
		 * Returns the number of bytes read
		 */
		Size Read(U8* buffer, I32 maxRead) const;

		/**
		 * Reads a single line into the buffer (or as much as will fit)
		 * Returns false if the read fails
		 */
		bool ReadLine(U8* buffer, Size maxRead) const;

		/**
		 * Writes the requested buffer to the file
		 */
		void Write(const char* buffer, Size bufferSize) const;

	private:

		// Current file mode
		FileMode m_Mode;
		// Current file handle
		FILE* m_FileHandle;

	};
}