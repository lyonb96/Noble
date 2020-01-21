#pragma once

#include <cstdio>
#include <filesystem>

#include "Array.h"
#include "Memory.h"
#include "Types.h"

namespace Noble
{
	namespace fs = std::filesystem;

	// Predefine for use in Directory
	class MappedFile;

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
		 * Returns a mapped file from the current Directory
		 * If the file does not exist, is not a regular file, or otherwise 
		 * fails to open, the MappedFile returned will not be valid
		 */
		MappedFile MapFile(const char* filename) const;

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

	/**
	 * Memory-mapped File
	 * Inspired by Stephan Brumme's MemoryMapped
	 * https://github.com/stbrumme/portable-memory-mapping
	 */
	class MappedFile
	{
	public:

		enum CacheHint
		{
			Normal,
			SequentialScan,
			RandomAccess
		};

		/**
		 * Empty init
		 */
		MappedFile();

		/**
		 * Initializes the MappedFile and attempts to open the requested File
		 */
		explicit MappedFile(const fs::path& path, Size mappedSize = 0, CacheHint hint = Normal);

		/**
		 * Moves the mapped file info from the other instance to this one, invalidating the original
		 */
		MappedFile(MappedFile&& other) noexcept;

		/**
		 * Moves the mapped file info from the other instance to this one, invalidating the original
		 */
		MappedFile& operator=(MappedFile&& other) noexcept;

		// No copying allowed
		MappedFile(const MappedFile& other) = delete;
		MappedFile& operator=(const MappedFile& other) = delete;

		/**
		 * Cleans up and closes the file
		 */
		~MappedFile();

	public:

		/**
		 * Attempts to open the requested file and map the requested number of bytes (default: ALL THE BYTES)
		 * Returns false on failure
		 */
		bool Open(const fs::path& path, Size mappedSize = 0, CacheHint hint = Normal);

		/**
		 * Cleans up and closes the file
		 */
		void Close();

		/**
		 * Remaps to a new offset in the same file
		 * Offset must be a multiple of the OS page size
		 */
		bool Remap(Size offset, Size mappedSize);

		/**
		 * Returns true if a file is successfully opened
		 */
		FORCEINLINE bool IsValid() const { return m_MappedView != NULL; };

		/**
		 * Returns the total file size in bytes
		 * Returns 0 if no file is mapped
		 */
		Size GetFileSize() const;

		/**
		 * Returns the number of bytes currently mapped
		 * Returns 0 if no file is mapped
		 */
		Size GetMappedSize() const;

		/**
		 * Returns the data for the mapped portion of the file
		 */
		const UByte* GetData() const;

		/**
		 * Returns the byte at the requested offset
		 */
		UByte At(Size offset) const;

		/**
		 * Returns the byte at the requested offset
		 */
		UByte operator[](Size offset) const;

	private:

		/**
		 * Returns OS page file size
		 */
		static U32 GetPageSize();

		// Path to the file
		fs::path m_FilePath;
		// File size
		Size m_FileSize;
		// Mapped size
		Size m_MappedSize;

		// Platform specifics
		typedef void* FileHandle;
		void* m_MappedFile;
		FileHandle m_File;
		void* m_MappedView;
	};
}