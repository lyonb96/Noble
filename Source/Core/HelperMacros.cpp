#include "HelperMacros.h"

#include "Logger.h"

namespace Noble
{
	FORCENOINLINE CODE_SEGMENT("Debug") void OnCheckFailure(const char* expStr, const SourceInfo& si)
	{
		NE_LOG_ERROR("Expression check failed: [" << expStr << "] at " << si.m_File << " line " << si.m_Line);
		DEBUG_BREAK();
	}
}