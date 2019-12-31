#include "FileIO.h"

#include <json/json.hpp>

#include "Logger.h"

namespace Noble
{
	bool CheckExists(const char* path)
	{
		std::ifstream f(path);
		return f.good();
	}

	void CreateFile(const char* path)
	{
		std::ofstream f;
		f.open(path, std::ios::out | std::fstream::trunc);
	}

	File::File(const char* path, bool truncate, bool create)
		: m_FilePath(path)
	{
		if (create && !CheckExists(path))
		{
			CreateFile(path);
		}
		m_FileStream.open(path, std::ios::in | std::ios::out | std::ios::binary | (truncate ? std::fstream::trunc : 0));
		if (!m_FileStream.is_open())
		{
			NE_LOG_ERROR("Failed to open file " << path);
			return;
		}
		m_FileStream.seekg(0, m_FileStream.end);
		m_FileSize = m_FileStream.tellg();
		m_FileStream.seekg(0, m_FileStream.beg);
	}

	File::File(const File& other) :
		m_FilePath(other.m_FilePath),
		m_FileSize(other.m_FileSize)
	{
		m_FileStream.open(m_FilePath, std::ios::in | std::ios::out | std::ios::binary);
		if (!m_FileStream.is_open())
		{
			NE_LOG_ERROR("Failed to open copied file " << m_FilePath);
			return;
		}
		m_FileStream.seekg(0, m_FileStream.end);
		m_FileSize = m_FileStream.tellg();
		m_FileStream.seekg(0, m_FileStream.beg);
	}

	const Size File::GetFileSize() const
	{
		return m_FileSize;
	}

	void File::ReadIntoBuffer(char* buffer)
	{
		m_FileStream.read(buffer, m_FileSize);
	}

	bool File::ReadLineIntoBuffer(char* buffer, Size len)
	{
		m_FileStream.getline(buffer, len);
		return m_FileStream.eof();
	}

	void File::ReadIntoJson(json& in)
	{
		m_FileStream >> in;
	}

	File::operator bool() const
	{
		return m_FileStream.is_open();
	}

	File::~File()
	{
		m_FileStream.close();
	}
}