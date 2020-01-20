#pragma once

#include "HelperMacros.h"
#include "Memory.h"

#include <cstring>

namespace Noble
{
	namespace Logger
	{
		/**
		 * Helps format log strings
		 */
		class StringFormatter
		{
			// 128 byte buffer to write to
			SafeBuffer<128> m_Buffer;
			U8 m_Index;

		public:

			StringFormatter()
				: m_Index(0), m_Buffer() {}

			template <class T>
			StringFormatter& operator<< (T data)
			{
				// StringFormatter does not write non-POD data
				return *this;
			}

			template<>
			StringFormatter& operator<< (int data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%i", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (unsigned int data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%u", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (long data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%i", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (unsigned long data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%u", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (long long data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%lli", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (unsigned long long data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%llu", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (float data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%f", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (double data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%f", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (long double data)
			{
				I32 res = sprintf_s(&m_Buffer[m_Index], 128 - m_Index, "%f", data);
				if (res > 0)
				{
					m_Index += res;
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (char data)
			{
				m_Buffer[m_Index++] = data;

				return *this;
			}

			template<>
			StringFormatter& operator<< (const char* data)
			{
				for (U32 i = 0; i < std::strlen(data); i++)
				{
					if (data[i] != '\0')
					{
						m_Buffer[m_Index++] = data[i];
					}
				}

				return *this;
			}

			template<>
			StringFormatter& operator<< (const wchar_t* data)
			{
				for (U32 i = 0; i < std::wcslen(data); i++)
				{
					if (data[i] != '\0')
					{
						m_Buffer[m_Index++] = char(data[i]);
					}
				}

				return *this;
			}

			/**
			 * This function implicitly adds the null terminator to the string
			 */
			const char* GetBuffer()
			{
				m_Buffer[m_Index] = '\0';
				return m_Buffer;
			}
		};

		/**
		 * Different levels of information to be logged
		 */
		enum LogLevel
		{
			LOG_LEVEL_DEBUG, // most verbose, used for spitting out debugging or testing info
			LOG_LEVEL_INFO, // informative log info
			LOG_LEVEL_WARNING, // issues that the program can generally gracefully recover from
			LOG_LEVEL_ERROR, // issues that may alter the running state of the program or cause cascading issues
			LOG_LEVEL_CRITICAL, // issues that will have a considerable effect on the running state of the program
			LOG_LEVEL_FATAL // issues that cause program termination
		};

		/**
		 * Allocates the space for the log data
		 */
		void InitLog();

		/**
		 * Logs the given string at the given level
		 */
		void Log(LogLevel level, StringFormatter& str);

		/**
		 * Saves the log to a file
		 */
		void PrintLog(const char* filename = "LogFile.txt");

		/**
		 * Frees space used for logging
		 */
		void CloseLog();
	}
}

// Logging macros

#ifdef NOBLE_DEBUG
#define NE_LOG_DEBUG(STR)	 ::Noble::Logger::Log(::Noble::Logger::LOG_LEVEL_DEBUG,    ::Noble::Logger::StringFormatter() << STR)
#else
#define NE_LOG_DEBUG(STR)
#endif
#define NE_LOG_INFO(STR)	 ::Noble::Logger::Log(::Noble::Logger::LOG_LEVEL_INFO,	   ::Noble::Logger::StringFormatter() << STR)
#define NE_LOG_WARNING(STR)  ::Noble::Logger::Log(::Noble::Logger::LOG_LEVEL_WARNING,  ::Noble::Logger::StringFormatter() << STR)
#define NE_LOG_ERROR(STR)	 ::Noble::Logger::Log(::Noble::Logger::LOG_LEVEL_ERROR,	   ::Noble::Logger::StringFormatter() << STR)
#define NE_LOG_CRITICAL(STR) ::Noble::Logger::Log(::Noble::Logger::LOG_LEVEL_CRITICAL, ::Noble::Logger::StringFormatter() << STR)
#define NE_LOG_FATAL(STR)	 ::Noble::Logger::Log(::Noble::Logger::LOG_LEVEL_FATAL,    ::Noble::Logger::StringFormatter() << STR)