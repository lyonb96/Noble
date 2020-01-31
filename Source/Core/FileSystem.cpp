#include "FileSystem.h"

#include "Logger.h"
#include "WindowsMinimal.h"

namespace Noble
{
	Directory::Directory()
	{}

	Directory::Directory(const fs::path& path)
	{
		SetPath(path);
	}

	void Directory::SetPath(const fs::path& path)
	{
		if (fs::exists(path) && fs::is_directory(path))
		{
			m_Path = path;
		}
	}

	void Directory::GotoParent()
	{
		m_Path = m_Path.parent_path();
	}

	void Directory::GotoSubdir(const fs::path& dir)
	{
		fs::path newPath = m_Path / dir;
		if (fs::exists(newPath) && fs::is_directory(newPath))
		{
			m_Path = newPath;
		}
	}

	void Directory::GotoDirectory(const fs::path& path)
	{
		if (fs::exists(path) && fs::is_directory(path))
		{
			m_Path = path;
		}
	}

	MappedFile Directory::MapFile(const fs::path& filename) const
	{
		MappedFile file;
		fs::path filepath = m_Path / filename;

		if (fs::exists(filepath) && fs::is_regular_file(filepath))
		{
			file.Open(filepath);
		}

		return std::move(file);
	}

	Array<fs::path> Directory::Iterate()
	{
		// First count the number of relevant children
		Size count = 0;
		for (const auto& entry : fs::directory_iterator(m_Path))
		{
			if (fs::is_directory(entry) || fs::is_regular_file(entry))
			{
				count++;
			}
		}

		// Now fill the array
		Array<fs::path> arr;
		if (count > 0)
		{
			arr.Resize(count);
			for (const auto& entry : fs::directory_iterator(m_Path))
			{
				if (fs::is_directory(entry) || fs::is_regular_file(entry))
				{
					fs::path path = entry.path();
					arr.Add(path);
				}
			}
		}

		return std::move(arr);
	}

	Array<fs::path> Directory::IterateRecursive()
	{
		// First count the number of relevant children
		Size count = 0;
		for (const auto& entry : fs::recursive_directory_iterator(m_Path))
		{
			if (fs::is_directory(entry) || fs::is_regular_file(entry))
			{
				count++;
			}
		}

		// Now fill the array
		Array<fs::path> arr;
		if (count > 0)
		{
			arr.Resize(count);
			for (const auto& entry : fs::recursive_directory_iterator(m_Path))
			{
				if (fs::is_directory(entry) || fs::is_regular_file(entry))
				{
					fs::path path = entry.path();
					arr.Add(path);
				}
			}
		}

		return std::move(arr);
	}


	bool CheckExists(const char* path)
	{
		fs::path p = path;
		return fs::exists(p);
	}

	const Size CheckFileSize(const char* path)
	{
		fs::path p = path;
		return fs::file_size(p);
	}

	// -----  File v3.0  -----

	File::File()
	{
		m_Handle = NULL;
		m_FileSize = 0;
		m_Mode = FileMode::FILE_READ;
	}

	File::File(const fs::path& path, FileMode mode, bool create)
		: File()
	{
		Open(path, mode, create);
	}

	File::File(File&& other) noexcept
	{
		m_Handle = other.m_Handle;
		m_FileSize = other.m_FileSize;
		m_Mode = other.m_Mode;

		other.m_Handle = NULL;
		other.m_FileSize = 0;
		other.m_Mode = FileMode::FILE_READ;
	}

	File& File::operator=(File&& other) noexcept
	{
		m_Handle = other.m_Handle;
		m_FileSize = other.m_FileSize;
		m_Mode = other.m_Mode;

		other.m_Handle = NULL;
		other.m_FileSize = 0;
		other.m_Mode = FileMode::FILE_READ;

		return *this;
	}

	File::~File()
	{
		Close();
	}

	void File::Open(const fs::path& path, FileMode mode, bool create)
	{
		// Skip if a file is already open
		if (IsValid())
		{
			NE_LOG_WARNING("Attempted to open a file without closing the original - file %s will not be opened.", path.string().c_str());
			return;
		}

		// Set up access params
		m_Mode = mode;
		DWORD dwAccess = 0;
		DWORD dwShare = 0;
		DWORD dwCreation = 0;

		switch (mode)
		{
			case FileMode::FILE_READ:
				dwAccess = GENERIC_READ;
				dwShare = FILE_SHARE_READ;
				dwCreation = OPEN_EXISTING;
				break;
			case FileMode::FILE_WRITE_REPLACE:
				dwAccess = GENERIC_WRITE;
				dwShare = 0;
				dwCreation = TRUNCATE_EXISTING;
				break;
			case FileMode::FILE_WRITE_APPEND:
				dwAccess = GENERIC_WRITE;
				dwShare = 0;
				dwCreation = create ? OPEN_ALWAYS : OPEN_EXISTING;
				break;
		}

		// Open the file and check for error
		m_Handle = CreateFileW(path.c_str(), dwAccess, dwShare, NULL, dwCreation, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_Handle == INVALID_HANDLE_VALUE)
		{
			NE_LOG_WARNING("Failed to open file %s - Error Msg: %u", path.string().c_str(), GetLastError());
			m_Handle = NULL;
			return;
		}

		// Grab file size
		LARGE_INTEGER result;
		if (!GetFileSizeEx(m_Handle, &result))
		{
			NE_LOG_WARNING("Failed to find file size for file %s", path.string().c_str());
			m_FileSize = 0;
			return;
		}
		m_FileSize = static_cast<Size>(result.QuadPart);
	}

	void File::Close()
	{
		if (m_Handle)
		{
			CloseHandle(m_Handle);
		}
	}

	Size File::Read(void* buffer, Size maxRead)
	{
		// Check for file validity
		if (!IsValid())
		{
			NE_LOG_WARNING("Attempted to read from invalid File object");
			return 0;
		}

		// Read from the file and store bytes read
		DWORD bytesRead = 0;
		BOOL result = ReadFile(m_Handle, buffer, maxRead, &bytesRead, NULL);

		// Check for error and unexpected state
		if (result == FALSE)
		{
			NE_LOG_WARNING("File read failed - Error Msg: %u", GetLastError());
			return 0;
		}

		if (bytesRead != maxRead)
		{
			NE_LOG_INFO("Read %u bytes from file, which is different from requested %u bytes", bytesRead, maxRead);
		}

		return bytesRead;
	}

	Size File::Read(BitStream& buffer, Size maxRead)
	{
		// Update max read value
		maxRead = maxRead == 0 ? GetFileSize() : maxRead;

		// Check if buffer needs resizing
		if ((buffer.GetMaxBytes() - buffer.GetStoredBytes()) < maxRead)
		{
			buffer.Resize(maxRead);
		}

		// Perform the read directly into the buffer
		Size bytesRead = Read(buffer.GetData(), maxRead);
		
		// Update the buffer's "stored bytes" value
		buffer.UpdateStoredBytes(bytesRead);

		return bytesRead;
	}

	Size File::Write(const void* data, Size maxWrite)
	{
		// Check for file validity
		if (!IsValid())
		{
			NE_LOG_WARNING("Attempted to write to invalid File object");
			return 0;
		}

		// Write to file and store bytes written
		DWORD bytesWritten = 0;
		BOOL result = WriteFile(m_Handle, data, maxWrite, &bytesWritten, NULL);

		// Check for error and unexpected state
		if (result == FALSE)
		{
			NE_LOG_WARNING("File write failed - Error Msg: %u", GetLastError());
			return 0;
		}

		if (bytesWritten != maxWrite)
		{
			NE_LOG_INFO("Wrote %u bytes to file, which is different from requested %u bytes", bytesWritten, maxWrite);
		}

		return bytesWritten;
	}

	// ----- Mapped File -----

	MappedFile::MappedFile()
		: m_FilePath(), m_FileSize(0), m_MappedSize(0)
	{
		m_MappedFile = nullptr;
		m_File = nullptr;
		m_MappedView = nullptr;
	}

	MappedFile::MappedFile(const fs::path& path, Size mappedSize, CacheHint hint)
		: MappedFile()
	{
		Open(path, mappedSize, hint);
	}

	MappedFile::MappedFile(MappedFile&& other) noexcept
	{
		m_FilePath = std::move(other.m_FilePath);
		m_FileSize = other.m_FileSize;
		m_MappedSize = other.m_MappedSize;
		m_MappedFile = other.m_MappedFile;
		m_MappedView = other.m_MappedView;
		m_File = other.m_File;

		other.m_FileSize = 0;
		other.m_MappedSize = 0;
		other.m_MappedFile = nullptr;
		other.m_MappedView = nullptr;
		other.m_File = nullptr;
	}

	MappedFile& MappedFile::operator=(MappedFile&& other) noexcept
	{
		// Close any existing opened file
		if (IsValid())
		{
			Close();
		}

		m_FilePath = std::move(other.m_FilePath);
		m_FileSize = other.m_FileSize;
		m_MappedSize = other.m_MappedSize;
		m_MappedFile = other.m_MappedFile;
		m_MappedView = other.m_MappedView;
		m_File = other.m_File;

		other.m_FileSize = 0;
		other.m_MappedSize = 0;
		other.m_MappedFile = nullptr;
		other.m_MappedView = nullptr;
		other.m_File = nullptr;

		return *this;
	}

	MappedFile::~MappedFile()
	{
		if (IsValid())
		{
			Close();
		}
	}

	bool MappedFile::Open(const fs::path& path, Size mappedSize, CacheHint hint)
	{
		// Don't map another file if this is already mapped
		if (IsValid())
		{
			return false;
		}

		// Prepare member fields
		m_File = nullptr;
		m_FileSize = 0;
		m_MappedFile = nullptr;
		m_MappedView = nullptr;

		// Grab the proper cache hint constant
		DWORD whint = 0;
		switch (hint)
		{
			case Normal: whint = FILE_ATTRIBUTE_NORMAL;
			case SequentialScan: whint = FILE_FLAG_SEQUENTIAL_SCAN;
			case RandomAccess: whint = FILE_FLAG_RANDOM_ACCESS;
			default: break;
		}

		m_File = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, whint, NULL);

		if (!m_File)
		{
			return false;
		}

		// Grab file size
		LARGE_INTEGER result;
		if (!GetFileSizeEx(m_File, &result))
		{
			return false;
		}
		m_FileSize = static_cast<Size>(result.QuadPart);

		// Convert to mapped file
		m_MappedFile = ::CreateFileMapping(m_File, NULL, PAGE_READONLY, 0, 0, NULL);
		if (!m_MappedFile)
		{
			return false;
		}

		// Map the initial part
		Remap(0, mappedSize);

		if (!m_MappedView)
		{
			return false;
		}

		return true;
	}

	void MappedFile::Close()
	{
		if (m_MappedView)
		{
			::UnmapViewOfFile(m_MappedView);
			m_MappedView = nullptr;
		}

		if (m_MappedFile)
		{
			::CloseHandle(m_MappedFile);
			m_MappedFile = NULL;
		}

		if (m_File)
		{
			::CloseHandle(m_File);
			m_File = 0;
		}

		m_FileSize = 0;
	}

	bool MappedFile::Remap(Size offset, Size mappedSize)
	{
		if (!m_File)
		{
			return false;
		}

		// mappedSize of 0 means map the whole file
		if (mappedSize == 0)
		{
			mappedSize = m_FileSize;
		}

		// Clear the existing mapped view
		if (m_MappedView)
		{
			::UnmapViewOfFile(m_MappedView);
			m_MappedView = NULL;
		}

		// Check the passed-in constants for validity, adjust accordingly
		if (offset > m_FileSize)
		{
			NE_LOG_ERROR("Cannot map file with offset greater than file size!");
			return false;
		}
		if (offset + mappedSize > m_FileSize)
		{
			NE_LOG_WARNING("Requested map size [%u] and offset [%u] exceeds file size [%u]\
				. Actual mapped size will be less than requested.", mappedSize, offset, m_FileSize);
			mappedSize = (m_FileSize - offset);
		}

		DWORD offsetLow = DWORD(offset & 0xFFFFFFFF);
		DWORD offsetHigh = DWORD(offset >> 32);
		m_MappedSize = mappedSize;

		m_MappedView = ::MapViewOfFile(m_MappedFile, FILE_MAP_READ, offsetHigh, offsetLow, mappedSize);
		if (m_MappedView == NULL)
		{
			m_MappedSize = 0;
			m_MappedView = NULL;
			return false;
		}

		return true;
	}

	Size MappedFile::GetFileSize() const
	{
		return m_FileSize;
	}

	Size MappedFile::GetMappedSize() const
	{
		return m_MappedSize;
	}

	U32 MappedFile::GetPageSize()
	{
		SYSTEM_INFO sys;
		GetSystemInfo(&sys);
		return sys.dwAllocationGranularity;
	}

	const UByte* MappedFile::GetData() const
	{
		return static_cast<UByte*>(m_MappedFile);
	}

	UByte MappedFile::At(Size offset) const
	{
		CHECK(offset >= 0 && offset < GetMappedSize());
		return GetData()[offset];
	}

	UByte MappedFile::operator[](Size offset) const
	{
		return At(offset);
	}
}