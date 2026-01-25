#pragma once

// Platform header: centralizes Windows SDK includes and compatibility fixes
// Include this instead of <windows.h> directly

// SAL compatibility for DirectX 9 SDK
#include "../../sal_compat.h"

// Windows configuration
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Windows headers
#include <windows.h>
