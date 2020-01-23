#include "HelperMacros.h"

#include "Logger.h"

namespace Noble
{
	FORCENOINLINE CODE_SEGMENT("Debug") void OnCheckFailure(const char* expStr, const SourceInfo& si)
	{
		NE_LOG_ERROR("Expression check failed: [%s] at %s line %i", expStr, si.File, si.Line);
		DEBUG_BREAK();
	}
}