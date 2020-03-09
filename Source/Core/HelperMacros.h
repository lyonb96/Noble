#pragma once

#ifdef _M_X64
#define NOBLE_BUILD_64BIT
#else
#define NOBLE_BUILD_32BIT
#endif

#ifdef _DEBUG
#define NOBLE_DEBUG
#endif

#ifdef _MSC_VER // Windows macros

#define NOBLE_WINDOWS

#define FORCEINLINE __forceinline
#define FORCENOINLINE __declspec(noinline)
#define CODE_SEGMENT(NAME) __declspec(code_seg(NAME))
#define DEBUG_BREAK() __debugbreak()
#else // Linux macros

#define NOBLE_LINUX

#define FORCEINLINE
#define FORCENOINLINE
#define CODE_SEGMENT(NAME)
#define DEBUG_BREAK()
#endif

#ifndef NOBLE_DEFAULT_ALIGN
#define NOBLE_DEFAULT_ALIGN 16
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if ((x)) { (x)->Release(); (x) = nullptr; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if ((x)) { delete (x); }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) if ((x)) { delete[] (x); }
#endif

#define NO_COPY(CLASS) \
CLASS(const CLASS&) = delete;\
CLASS& operator= (const CLASS&) = delete;

#define NO_MOVE(CLASS) \
CLASS(CLASS&&) = delete;\
CLASS& operator=(CLASS&&) = delete;

#define NO_COPY_NO_MOVE(CLASS)\
NO_COPY(CLASS)\
NO_MOVE(CLASS)

// -------------------
// Sanity checks
// -------------------

namespace Noble
{
	/**
	 * Source info wrapper
	 */
	struct SourceInfo
	{
		const char* File;
		const int Line;

		SourceInfo(const char* file, const int line)
			: File(file), Line(line)
		{}
	};

	/**
	 * Handles check failures by breaking execution and logging the failure
	 */
	FORCENOINLINE CODE_SEGMENT("Debug") void OnCheckFailure(const char* expStr, const SourceInfo& si);

	/**
	 * Checks the expression and breaks if it fails
	 */
	FORCEINLINE void CheckImpl(bool expression, const char* exprStr, const SourceInfo& si)
	{
		if (!expression)
		{
			OnCheckFailure(exprStr, si);
		}
	}
}

#define SOURCE_INFO ::Noble::SourceInfo(__FILE__, __LINE__)

#define STATIC_CHECK(EXPR, STR) static_assert(EXPR, STR)

#ifdef NOBLE_DEBUG
#define CHECK(EXPR) ::Noble::CheckImpl(EXPR, #EXPR, SOURCE_INFO)
#else
#define CHECK(EXPR)
#endif