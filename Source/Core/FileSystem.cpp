#include "FileSystem.h"

namespace Noble
{
	namespace fs = std::filesystem;

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

	bool File::OpenFile(const char* path, bool bin, FileMode mode)
	{
		// Close any existing open file
		if (m_FileHandle)
		{
			CloseFile();
		}

		// Open the new requested file
		errno_t err = fopen_s(&m_FileHandle, path, GetModeString(mode, bin));

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
		return fs;
	}

	Size File::Read(U8* buffer, I32 maxRead) const
	{
		if (m_FileHandle == nullptr || buffer == nullptr)
		{
			return 0;
		}

		for (Size i = 0; i < maxRead; ++i)
		{
			buffer[i] = getc(m_FileHandle);
			if (feof(m_FileHandle))
			{
				return i;
			}
		}
		return maxRead;
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