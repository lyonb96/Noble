#include "Time.h"

#include "WindowsMinimal.h"
#include "Logger.h"

namespace Noble
{
	U64 Clock::m_Freq = 0;
	U64 Clock::m_Now = 0;

	// Result of QueryPerformanceFrequency, only needs to be set once for the whole app lifetime
	U64 g_PFreq = 0;

	inline void InitTiming()
	{
		if (g_PFreq == 0)
		{
			LARGE_INTEGER li;
			QueryPerformanceFrequency(&li);
			g_PFreq = li.QuadPart / 1000000;
		}
	}

	/**
	 * Helper function, wraps QPC call and just returns the Quad Part of the LARGE_INTEGER
	 */
	inline U64 GetTimeNow()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}

	Clock::Clock()
	{
		LARGE_INTEGER li;

		if (m_Freq == 0)
		{
			// Grab frequency if it hasn't been setup yet
			QueryPerformanceFrequency(&li);
			m_Freq = li.QuadPart / 1000000;
		}

		// Log start time
		QueryPerformanceCounter(&li);
		m_StartTime = li.QuadPart;
		m_LastMarker = m_StartTime;
	}

	void Clock::UpdateNow()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);

		m_Now = li.QuadPart;
	}

	U64 Clock::Microseconds(U64 rawIn) const
	{
		return rawIn / m_Freq;
	}

	void Clock::Mark()
	{
		m_LastMarker = m_Now;
	}

	U64 Clock::GetMicrosecondsSinceStart()
	{
		return Microseconds(m_Now - m_StartTime);
	}

	U64 Clock::GetMicrosecondsSinceMark()
	{
		return Microseconds(m_Now - m_LastMarker);
	}

	F32 Clock::GetSecondsSinceStart()
	{
		return (float) GetMicrosecondsSinceStart() / 1000000.0F;
	}

	F32 Clock::GetSecondsSinceMark()
	{
		return (float) GetMicrosecondsSinceMark() / 1000000.0F;
	}


	BenchmarkHelper::BenchmarkHelper(const char* id)
		: m_Identifier(id)
	{
		m_StartTime = GetTimeNow();
	}

	BenchmarkHelper::~BenchmarkHelper()
	{
		U64 duration = GetTimeNow() - m_StartTime;
		NE_LOG_DEBUG("Benchmark Helper: %s - %uu", m_Identifier);
	}
}