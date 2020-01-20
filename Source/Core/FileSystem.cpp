#include "FileSystem.h"

#include "Logger.h"

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
}