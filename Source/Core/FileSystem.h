#pragma once

#include <filesystem>

#include "Array.h"
#include "BitStream.h"
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
		Directory(const fs::path& path);

		/**
		 * Changes the Directory to the given path
		 */
		void SetPath(const fs::path& path);

		/**
		 * Changes the Directory to its parent
		 */
		void GotoParent();

		/**
		 * Changes the Directory to the given child of the current Directory
		 */
		void GotoSubdir(const fs::path& dir);

		/**
		 * Changes the directory to the given path
		 */
		void GotoDirectory(const fs::path& path);

		/**
		 * Returns a mapped file from the current Directory
		 * If the file does not exist, is not a regular file, or otherwise 
		 * fails to open, the MappedFile returned will not be valid
		 */
		MappedFile MapFile(const fs::path& filename) const;

		/**
		 * Returns an Array of paths to files or folders in the directory
		 */
		Array<fs::path> Iterate();

		/**
		 * Returns an Array of paths to files or folders in the directory and its subdirectories
		 */
		Array<fs::path> IterateRecursive();

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
	 * Supported modes for non-mapped files
	 */
	enum class FileMode
	{
		// Opens the file for reading - requires that the file exist beforehand
		FILE_READ,
		// Opens the file for writing and overwrites existing content - requires that the file exist beforehand
		FILE_WRITE_REPLACE,
		// Opens the file for writing and appends new content to the end
		FILE_WRITE_APPEND
	};

	/**
	 * A wrapper providing buffered file I/O
	 */
	class File
	{
	public:

		/**
		 * Empty constructor
		 */
		File();

		/**
		 * Opens the file from the given path using the given mode
		 * Optional param to create the file if it does not exist (default false)
		 */
		File(const fs::path& path, FileMode mode, bool create = false);

		// No copy allowed
		File(const File& other) = delete;
		File& operator=(const File& other) = delete;

		/**
		 * Move constructor
		 */
		File(File&& other) noexcept;

		/**
		 * Move assignment
		 */
		File& operator=(File&& other) noexcept;

		/**
		 * Closes the open file, if there is one
		 */
		~File();

	public:

		/**
		 * Opens the file from the given path using the given mode
		 * Optional param to create the file if it does not exist (default false)
		 */
		void Open(const fs::path& path, FileMode mode, bool create = false);

		/**
		 * Closes the open file, if there is one
		 */
		void Close();

		/**
		 * Reads the requested number of bytes into the given buffer
		 * Returns the actual number of bytes read
		 */
		Size Read(void* buffer, Size maxRead);

		/**
		 * Reads the requested number of bytes into the given BitStream
		 * If no number of bytes is requested explicitly, it will attempt
		 * to read the entire file content into the BitStream.
		 */
		Size Read(BitStream& buffer, Size maxRead = 0);

		/**
		 * Writes the requested number of bytes from the given buffer
		 * Returns the actual number of bytes written
		 */
		Size Write(const void* data, Size maxWrite);

		/**
		 * Returns true if a file is open
		 */
		bool IsValid() const { return m_Handle != nullptr; }

		/**
		 * Returns the size of the open file, or 0 if no file is open
		 */
		Size GetFileSize() const { return m_FileSize; }

	private:

		// Current file mode
		FileMode m_Mode;
		// Handle to the currently open file
		void* m_Handle;
		// Size of the currently open file
		Size m_FileSize;
	};

	/**
	 * Memory-mapped File
	 * Inspired by Stephan Brumme's MemoryMapped
	 * https://github.com/stbrumme/portable-memory-mapping
	 */
	class MappedFile
	{
	public:

		/**
		 * Used to hint at the OS how we plan to use the file
		 * Can optimize certain reads. Default is Normal
		 */
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

		void* m_File;
		void* m_MappedFile;
		void* m_MappedView;
	};
}