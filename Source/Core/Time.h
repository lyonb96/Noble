#pragma once

#include "Types.h"

namespace Noble
{
	/**
	 * Called once at the beginning of the app to prepare timing information
	 */
	inline void InitTiming();

	/**
	 * This class is a lightweight clock / stopwatch, it provides microsecond accurate time measurements.
	 */
	class Clock
	{
	public:

		/**
		 * Default Constructor initializes static variables as required and prepares default values
		 */
		Clock();

		/**
		 * Places a mark at the current time, which can be used to reference a specific moment
		 */
		void Mark();

		/**
		 * Returns the number of microseconds since the clock started
		 */
		U64 GetMicrosecondsSinceStart();

		/**
		 * Returns the number of microseconds since the last call to Mark()
		 * If Mark() has not been called yet, this is the same as GetMicrosecondsSinceStart()
		 */
		U64 GetMicrosecondsSinceMark();

		/**
		 * Same as GetMicrosecondsSinceStart, but returns seconds in float format
		 */
		F32 GetSecondsSinceStart();

		/**
		 * Same as GetMicrosecondsSinceMark, but returns seconds in float format
		 */
		F32 GetSecondsSinceMark();

		/**
		 * Only called once per frame, updates the clock's "now" marker
		 */
		static void UpdateNow();

	private:
		
		/**
		 * Converts arbitrary deltas to microseconds
		 */
		U64 Microseconds(U64 rawIn) const;

	private:

		/**
		 * Frequency will be the same across all clocks
		 */
		static U64 m_Freq;

		/**
		 * Each clock will have access to a variable that is updated at the start of each frame
		 */
		static U64 m_Now;

		/**
		 * Log of when the clock started
		 */
		U64 m_StartTime;
		/**
		 * Log of last call to Mark()
		 */
		U64 m_LastMarker;
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
		BenchmarkHelper(const BenchmarkHelper&&) = delete;

		/**
		 * Determines how long the BenchmarkHelper was alive and logs this under DEBUG
		 */
		~BenchmarkHelper();

	private:

		// A stringized representation of whatever the user passes into BENCHMARK
		const char* m_Identifier;
		// The time the BenchmarkHelper instance was created
		U64 m_StartTime;
	};
}

// Benchmarking macros
#define BENCHMARK(ID) ::Noble::BenchmarkHelper bh##ID(#ID)