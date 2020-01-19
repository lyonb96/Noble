#include "Logger.h"

#include "FileSystem.h"
#include "String.h"

#ifdef _DEBUG
#include "WindowsMinimal.h" // OutputDebugString
#endif

// default 256KB log buffer
#ifndef LOG_BUFFER_SIZE
#define LOG_BUFFER_SIZE 256*1024
#endif

namespace Noble
{
	namespace Logger
	{
		namespace
		{
			// log buffer
			char* g_LogBuffer;
			// how much of the log buffer is used / position to write at
			U32 g_LogBufferPos;
			// How many times the log has been written off
			U8 g_LogWrites;
		}

		void InitLog()
		{
			g_LogBuffer = new char[LOG_BUFFER_SIZE];
			g_LogBufferPos = 0;
			g_LogWrites = 0;
		}

		/**
		 * Dumps the current log to a file and clears the buffer
		 */
		void CleanLog()
		{
			// make the log name
			char logname[16];
			sprintf_s(&logname[0], 16, "LogFile%i.txt", ++g_LogWrites);
			// print the log
			PrintLog(logname);

			// clear the buffer
			memset(g_LogBuffer, 0, LOG_BUFFER_SIZE);
			g_LogBufferPos = 0;
		}

		// Writes a string to the log file
		void WriteString(const char* str, bool addNewline = false)
		{
			for (U32 i = 0; i < std::strlen(str); i++)
			{
				// don't write null terminators
				if (str[i] != '\0')
				{
					g_LogBuffer[g_LogBufferPos++] = str[i];
				}
			}

#ifdef _DEBUG
			OutputDebugString(str);
#endif

			if (addNewline)
			{
				g_LogBuffer[g_LogBufferPos++] = '\n';
#ifdef _DEBUG
				OutputDebugString("\n");
#endif
			}
		}

		// Builds the string for the log level then writes it to the file
		void WriteLogLevel(LogLevel level)
		{
			const char* str;
			switch (level)
			{
				case LOG_LEVEL_DEBUG:
					str = "[Noble] DEBUG: ";
					break;

				case LOG_LEVEL_INFO:
					str = "[Noble] INFO: ";
					break;

				case LOG_LEVEL_WARNING:
					str = "[Noble] WARNING: ";
					break;

				case LOG_LEVEL_ERROR:
					str = "[Noble] ERROR: ";
					break;

				case LOG_LEVEL_CRITICAL:
					str = "[Noble] CRITICAL: ";
					break;

				case LOG_LEVEL_FATAL:
					str = "[Noble] FATAL: ";
					break;
				default:
					str = "";
					break;
			}
			WriteString(str);
		}

		void Log(LogLevel level, StringFormatter& str)
		{
			// First write the log level to the buffer
			WriteLogLevel(level);
			WriteString(str.GetBuffer(), true); // append new line

			// check if log file is getting full (>90% used)
			if (g_LogBufferPos >= ((LOG_BUFFER_SIZE * 9) / 10))
			{
				// clear the log to make room
				CleanLog();
			}
		}

		void PrintLog(const char* filename)
		{
			File logFile;
			logFile.OpenFile(filename, false, FileMode::READWRITE);
			
			logFile.Write(g_LogBuffer, g_LogBufferPos);
		}

		void CloseLog()
		{
			delete[] g_LogBuffer;
		}
	}
}