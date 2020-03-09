#pragma once

#include "Types.h"

#ifndef FIXED_STEP_RATE
#define FIXED_STEP_RATE (1.0F / 60.0F)
#endif

namespace Noble
{
	/**
	 * Wraps some functionality for measuring and storing time
	 */
	struct Timestamp
	{
		// Time can access the Stamp member
		friend class Time;

		Timestamp(U64 stamp)
			: Stamp(stamp)
		{}

		Timestamp()
			: Stamp(0)
		{}

		/**
		 * Operators for basic arithmetic with Timestamps
		 * Subtracting time stamps makes them more like Durations
		 */

		Timestamp& operator+(const Timestamp& other);

		Timestamp& operator-(const Timestamp& other);

		Timestamp& operator+=(const Timestamp& other);

		Timestamp& operator-=(const Timestamp& other);

	private:

		// Internal timestamp value, generally filled by call to QPC
		U64 Stamp;

	};

	/**
	 * Wraps time-related functions such as timestamping, date/time conversion etc
	 */
	class Time
	{
	public:

		// allow the Engine to access private members for core loop timestamping
		friend class Engine;

		/**
		 * Returns a timestamp at the current time
		 */
		static Timestamp GetNowTimestamp();

		/**
		 * Returns the delta time in seconds as a float
		 */
		static F32 GetDeltaTime();

		/**
		 * Returns the delta time in microseconds as a U64
		 */
		static U64 GetDeltaTimeMicro();

		/**
		 * Returns the fixed delta time in seconds as a float
		 */
		static F32 GetFixedDeltaTime();

		/**
		 * Converts a Timestamp into a duration in seconds
		 */
		static F32 GetDuration(const Timestamp& time);

		/**
		 * Converts a Timestamp into a duration in microseconds
		 */
		static U64 GetDurationMicro(const Timestamp& time);

		/**
		 * Returns the number of times the loop clock has been reset
		 */
		static U64 GetFrameCount();

		/**
		 * Returns the calculated framerate based on the loop clock
		 */
		static U64 GetFrameRate();

	private:

		/**
		 * Initializes default values and system variables
		 */
		static void Initialize();

		/**
		 * Resets the loop clock and stores new delta information
		 */
		static void SetLoopTime(Timestamp duration);

	private:

		Time() {}

		// no copy or move
		Time(const Time&) = delete;
		Time(Time&&) = delete;
		Time& operator=(const Time&) = delete;
		Time& operator=(Time&&) = delete;

		// Result of QueryPerformanceFrequency
		static U64 PerfFreq;
		// Time the last loop took in microseconds
		static U64 LoopMicroseconds;
		// Time the last loop took in seconds
		static F32 LoopSeconds;
		// Number of times the loop clock has been updated
		static U64 FrameCount;

	};

	/**
	 * Uses RAII to provide a simple function or code chunk timer.
	 * Best not to use directly; use the BENCHMARK_* functions
	 */
	class BenchmarkHelper
	{
	public:

		/**
		 * Marks the start time of the benchmark
		 */
		BenchmarkHelper(const char* id);

		BenchmarkHelper(const BenchmarkHelper&) = delete;
		BenchmarkHelper(BenchmarkHelper&&) = delete;
		BenchmarkHelper& operator=(const BenchmarkHelper&) = delete;
		BenchmarkHelper& operator=(BenchmarkHelper&&) = delete;

		/**
		 * Determines how long the BenchmarkHelper was alive and logs this under DEBUG
		 */
		~BenchmarkHelper();

	private:

		// A stringized representation of whatever the user passes into BENCHMARK
		const char* m_Identifier;
		// The time the BenchmarkHelper instance was created
		Timestamp m_StartTime;
	};
}

// Benchmarking macros
#ifdef NOBLE_DEBUG
#define BENCHMARK(ID) ::Noble::BenchmarkHelper bh##ID(#ID)
#else
#define BENCHMARK(ID)
#endif