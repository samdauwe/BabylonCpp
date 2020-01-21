#ifndef BABYLON_CORE_TIMER_H
#define BABYLON_CORE_TIMER_H

#if defined (__unix__) || defined(__APPLE__)
// Unix timer functions
#include <babylon/core/profiling/timer_unix.h>
#elif _WIN32
// Windows timer functions
#include <babylon/core/profiling/timer_win32.h>
#endif

#endif // BABYLON_CORE_TIMER_H
