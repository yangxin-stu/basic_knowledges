#pragma once
#if defined(_WIN32) || defined(_WIN64) || defined(USE_PLATFORM_WINDOWS)
#include "core_msvc.h"
#elif defined(__clang__) || defined(__GNUC__)
#include "core_clang.h"
#endif