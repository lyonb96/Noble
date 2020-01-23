#pragma once

#include "HelperMacros.h"
#include "Memory.h"
#include "String.h"

#include <cstring>

namespace Noble
{
	enum class LogLevel
	{
		LV_DEBUG, // most verbose, used for spitting out debugging or testing info
		LV_INFO, // informative log info
		LV_WARNING, // issues that the program can generally gracefully recover from
		LV_ERROR, // issues that may alter the running state of the program or cause cascading issues
		LV_CRITICAL, // issues that will have a considerable effect on the running state of the program
		LV_FATAL // issues that cause program termination
	};

	/**
	 * Class that wraps logging functionality
	 */
	class Logger
	{
	public:

		// No copying or moving
		Logger(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger& operator=(Logger&&) = delete;

		/**
		 * Uses printf-style formatting to write to the log
		 */
		template <typename... Args>
		static void Log(LogLevel level, const char* fmt, Args... args)
		{
			Get().LogInternal(level, fmt, args...);
		}

		/**
		 * Prints the current log data to the requested file
		 */
		FORCEINLINE static void PrintLog(const char* file)
		{
			Get().PrintLogInternal(file);
		}

	private:

		/**
		 * Allocates the space needed for the logger
		 */
		Logger();

		/**
		 * Prints anything in the log and frees the space used by the logger
		 */
		~Logger();

	private:

		/**
		 * Writes a string to the log buffer
		 */
		void WriteString(const char* str, bool newline = false);
		
		/**
		 * Converts the LogLevel to a string and writes it to the log buffer
		 */
		void WriteLevel(LogLevel level);

		/**
		 * Returns the current write position in the buffer
		 */
		char* GetCurrentPos() const;

		/**
		 * Returns the remaining space in the buffer
		 */
		Size GetRemainingSpace() const;

		/**
		 * Returns true if the log buffer has exceeded 90% usage
		 */
		bool IsLogBufferFull() const;

		/**
		 * Internal - just does whatever the static one asks
		 */
		template <typename... Args>
		void LogInternal(LogLevel level, const char* fmt, Args... args)
		{
			CHECK(fmt); // ensure the format string is valid

			WriteLevel(level);
			I32 res = sprintf_s(GetCurrentPos(), GetRemainingSpace(), fmt, args...);
			WriteString(GetCurrentPos(), true);

			if (IsLogBufferFull())
			{
				ClearLog();
			}
		}

		/**
		 * Dumps the current log contents if the buffer is getting too full
		 */
		void ClearLog();

		/**
		 * Prints the log to a file with the given name
		 */
		void PrintLogInternal(const char* file);

	private:

		/**
		 * Singleton get
		 */
		FORCEINLINE static Logger& Get()
		{
			static Logger inst;
			return inst;
		}

		// The buffer used for storing log strings
		char* m_LogBuffer;
		// Current write position of the buffer
		Size m_LogBufferPos;
		// Number of times the log has been written out this run
		U8 m_LogWrites;
	};
}

// Logging macros

#ifdef NOBLE_DEBUG
#define NE_LOG_DEBUG(STR, ...)	 ::Noble::Logger::Log(::Noble::LogLevel::LV_DEBUG, STR, __VA_ARGS__)
#else
#define NE_LOG_DEBUG(STR, ...)
#endif
#define NE_LOG_INFO(STR, ...)	  ::Noble::Logger::Log(::Noble::LogLevel::LV_INFO, STR, __VA_ARGS__)
#define NE_LOG_WARNING(STR, ...)  ::Noble::Logger::Log(::Noble::LogLevel::LV_WARNING, STR, __VA_ARGS__)
#define NE_LOG_ERROR(STR, ...)	  ::Noble::Logger::Log(::Noble::LogLevel::LV_ERROR, STR, __VA_ARGS__)
#define NE_LOG_CRITICAL(STR, ...) ::Noble::Logger::Log(::Noble::LogLevel::LV_CRITICAL, STR, __VA_ARGS__)
#define NE_LOG_FATAL(STR, ...)	  ::Noble::Logger::Log(::Noble::LogLevel::LV_FATAL, STR, __VA_ARGS__)