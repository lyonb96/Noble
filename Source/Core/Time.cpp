#include "Time.h"

#include <utility>

#include "WindowsMinimal.h"
#include "Logger.h"

namespace Noble
{
	Timestamp& Timestamp::operator+(const Timestamp& other)
	{
		Timestamp tmp(*this);
		return tmp += other;
	}

	Timestamp& Timestamp::operator-(const Timestamp& other)
	{
		Timestamp tmp(*this);
		return tmp -= other;
	}

	Timestamp& Timestamp::operator+=(const Timestamp& other)
	{
		Stamp += other.Stamp;
		return *this;
	}

	Timestamp& Timestamp::operator-=(const Timestamp& other)
	{
		Stamp -= other.Stamp;
		return *this;
	}

	// Declare statics
	U64 Time::PerfFreq = 0;
	U64 Time::LoopMicroseconds = 0;
	F32 Time::LoopSeconds = 0.0F;
	U64 Time::FrameCount = 0;

	void Time::Initialize()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		PerfFreq = li.QuadPart / 1000000;
	}

	Timestamp Time::GetNowTimestamp()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);

		Timestamp t(0);
		t.Stamp = li.QuadPart;
		return std::move(t);
	}

	F32 Time::GetDeltaTime()
	{
		return LoopSeconds;
	}

	U64 Time::GetDeltaTimeMicro()
	{
		return LoopMicroseconds;
	}

	F32 Time::GetDuration(const Timestamp& time)
	{
		return GetDurationMicro(time) / 1000000.0F;
	}

	U64 Time::GetDurationMicro(const Timestamp& time)
	{
		return (time.Stamp / PerfFreq);
	}
	
	U64 Time::GetFrameCount()
	{
		return FrameCount;
	}

	U64 Time::GetFrameRate()
	{
		return 1000000 / LoopMicroseconds;
	}

	void Time::SetLoopTime(Timestamp duration)
	{
		LoopMicroseconds = GetDurationMicro(duration);
		LoopSeconds = float(LoopMicroseconds) / 1000000.0F;
		++FrameCount;
	}


	BenchmarkHelper::BenchmarkHelper(const char* id)
		: m_Identifier(id)
	{
		m_StartTime = Time::GetNowTimestamp();
	}

	BenchmarkHelper::~BenchmarkHelper()
	{
		Timestamp duration = Time::GetNowTimestamp() - m_StartTime;
		NE_LOG_DEBUG("Benchmark Helper: %s - %uu", m_Identifier, Time::GetDurationMicro(duration));
	}
}