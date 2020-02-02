#pragma once

/**
 * This header strips out the parts of Windows.h that are either
 * unnecessary or annoying to deal with.
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>