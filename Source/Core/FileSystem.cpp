#include "FileSystem.h"

#include "Logger.h"
#include "WindowsMinimal.h"

namespace Noble
{
	Directory::Directory()
	{}

	Directory::Directory(const char* path)
	{
		SetPath(path);
	}

	void Directory::SetPath(const char* path)
	{
		CHECK(path);

		if (fs::exists(path) && fs::is_directory(path))
		{
			m_Path = path;
		}
	}

	void Directory::GotoParent()
	{
		m_Path = m_Path.parent_path();
	}

	void Directory::GotoSubdir(const char* dir)
	{
		CHECK(dir);

		fs::path newPath = m_Path / dir;
		if (fs::is_directory(newPath))
		{
			m_Path = newPath;
		}
	}

	MappedFile Directory::MapFile(const char* filename) const
	{
		MappedFile file;
		fs::path filepath = m_Path / filename;

		if (fs::exists(filepath) && fs::is_regular_file(filepath))
		{
			file.Open(filepath);
		}

		return std::move(file);
	}

	Array<fs::path> Directory::GetChildren()
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

	const char* GetModeString(FileMode mode, bool bin)
	{
		switch (mode)
		{
			case FileMode::READONLY:
				return bin ? "rb" : "r";
			case FileMode::READWRITE:
				return bin ? "wb+" : "w+";
			case FileMode::WRITE_APPEND:
				return bin ? "ab" : "a";
			case FileMode::READ_APPEND:
				return bin ? "ab+" : "a+";
			default:
				return "";
		}
	}

	File::File()
	{
		m_Mode = FileMode::READONLY;
		m_FileHandle = nullptr;
	}

	File::File(const char* path, bool bin, FileMode mode)
		: File()
	{
		OpenFile(path, bin, mode);
	}

	bool File::OpenFile(const char* path, bool bin, FileMode mode)
	{
		// Close any existing open file
		if (m_FileHandle)
		{
			CloseFile();
		}

		// Open the new requested file
		errno_t err = fopen_s(&m_FileHandle, path, GetModeString(mode, bin));
		m_Mode = mode;

		// Check for failure
		if (!m_FileHandle)
		{
			return false;
		}

		return true;
	}

	void File::CloseFile()
	{
		if (m_FileHandle)
		{
			fclose(m_FileHandle);
			m_FileHandle = nullptr;
		}
	}

	File::~File()
	{
		CloseFile();
	}

	Size File::GetFileSize() const
	{
		if (m_FileHandle == nullptr)
		{
			return -1;
		}
		I32 pos = ftell(m_FileHandle);

		fseek(m_FileHandle, 0, SEEK_END);
		Size fs = ftell(m_FileHandle);
		fseek(m_FileHandle, pos, SEEK_SET);

		// The above method was causing some bizarre behavior
		// Essentially, a sample file would show as 2012 bytes
		// but would actually trigger FEOF around 1937 bytes
		// Windows also shows the file as 2012 bytes
		// The issue turns out to be due to line ending normalization
		// in functions like getc and fputc. When opened in text mode,
		// getc converts a CRLF to a single LF, and fputc converts an
		// LF to a CRLF. However, fseek() does not perform the same
		// conversion, so each line ending counts as 2 bytes according
		// to fseek() but only 1 byte according to getc(). This caused reads
		// to hit EOF sooner than anticipated, causing several issues. 
		// Solution is to just not bother with text files, all files will
		// be opened as binary files so text mode support is being stripped.

		return fs;
	}

	Size File::Read(U8* buffer, I32 maxRead) const
	{
		if (m_FileHandle == nullptr || buffer == nullptr)
		{
			return 0;
		}

		Size i;
		for (i = 0; i < maxRead; ++i)
		{
			buffer[i] = getc(m_FileHandle);
			if (feof(m_FileHandle))
			{
				return i;
			}
		}
		return i;
	}

	bool File::ReadLine(U8* buffer, Size maxRead) const
	{
		if (m_FileHandle == nullptr || buffer == nullptr)
		{
			return false;
		}

		U8 c = getc(m_FileHandle);
		Size i = 0;
		while (c != '\n' || i < maxRead)
		{
			buffer[i++] = c;
			c = getc(m_FileHandle);
		}

		return true;
	}

	void File::Write(const char* buffer, Size bufferSize) const
	{
		if (!m_FileHandle)
		{
			return;
		}

		for (Size i = 0; i < bufferSize; ++i)
		{
			fputc(buffer[i], m_FileHandle);
		}
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

		// Messy! Convert std::filesystem::path's wchar_t string to a c_str that's Windows friendly
		Size strlen = path.string().length();
		const wchar_t* str = path.c_str();
		char* cstr = new char[strlen + 1];
		Size converted = 0;
		wcstombs_s(&converted, cstr, size_t(strlen + 1), str, size_t(strlen));

		// Actually open the file
		m_File = CreateFileA(cstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, whint, NULL);
		delete[] cstr;

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
			NE_LOG_WARNING("Requested map size [" << mappedSize << "] and offset [" << offset << "] exceeds file size ["
				<< m_FileSize << "]. Actual mapped size will be less than requested.");
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