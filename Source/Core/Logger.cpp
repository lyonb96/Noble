#include "Logger.h"

#include "FileSystem.h"
#include "String.h"

#ifdef NOBLE_DEBUG
#include "WindowsMinimal.h" // OutputDebugString
#endif

// default 256KB log buffer
#ifndef LOG_BUFFER_SIZE
#define LOG_BUFFER_SIZE 256*1024
#endif

namespace Noble
{
	Logger::Logger()
	{
		// Allocate log space
		m_LogBuffer = static_cast<char*>(Memory::Malloc(LOG_BUFFER_SIZE, NOBLE_DEFAULT_ALIGN));
		m_LogBufferPos = 0;
		m_LogWrites = 0;
	}

	Logger::~Logger()
	{
		if (m_LogBuffer)
		{
			Memory::Free(m_LogBuffer);
			m_LogBufferPos = 0;
		}
	}

	char* Logger::GetCurrentPos() const
	{
		return m_LogBuffer + m_LogBufferPos;
	}

	Size Logger::GetRemainingSpace() const
	{
		return LOG_BUFFER_SIZE - m_LogBufferPos;
	}

	bool Logger::IsLogBufferFull() const
	{
		return m_LogBufferPos >= ((LOG_BUFFER_SIZE * 9) / 10);
	}

	void Logger::WriteString(const char* str, bool newline)
	{
		for (U32 i = 0; i < std::strlen(str); i++)
		{
			// don't write null terminators
			if (str[i] != '\0')
			{
				m_LogBuffer[m_LogBufferPos++] = str[i];
			}
		}

#ifdef NOBLE_DEBUG
		OutputDebugString(str);
#endif

		if (newline)
		{
			m_LogBuffer[m_LogBufferPos++] = '\n';
#ifdef NOBLE_DEBUG
			OutputDebugString("\n");
#endif
		}
	}

	void Logger::WriteLevel(LogLevel level)
	{
		const char* str;
		switch (level)
		{
		case LogLevel::LV_DEBUG:
			str = "[Noble] DEBUG: ";
			break;

		case LogLevel::LV_INFO:
			str = "[Noble] INFO: ";
			break;

		case LogLevel::LV_WARNING:
			str = "[Noble] WARNING: ";
			break;

		case LogLevel::LV_ERROR:
			str = "[Noble] ERROR: ";
			break;

		case LogLevel::LV_CRITICAL:
			str = "[Noble] CRITICAL: ";
			break;

		case LogLevel::LV_FATAL:
			str = "[Noble] FATAL: ";
			break;
		default:
			str = "";
			break;
		}

		WriteString(str);
	}

	void Logger::ClearLog()
	{
		// Fill out the log name and print to the file
		char logname[24];
		sprintf_s(&logname[0], 24, "LogFile%i.txt", ++m_LogWrites);
		PrintLogInternal(logname);

		// Clear the buffer
		memset(m_LogBuffer, 0, LOG_BUFFER_SIZE);
		m_LogBufferPos = 0;
	}

	void Logger::PrintLogInternal(const char* file)
	{
		File logFile;
		logFile.OpenFile(file, false, FileMode::READWRITE);

		logFile.Write(m_LogBuffer, m_LogBufferPos);
	}
}