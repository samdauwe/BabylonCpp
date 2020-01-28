#ifndef BABYLONCPP_LOG_TRACE_H
#define BABYLONCPP_LOG_TRACE_H

#include <iostream>

// Tracing utilities for emscripten where we often have to resort to trace for debugging
// You need to define BABYLON_LOG_TRACE_ON manually in a cpp file in order to activate it.

// TRACE_BEFORE_AFTER : run any (even complicated) expression, and log before / after
#ifdef BABYLON_LOG_TRACE_ON

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )

#define TRACE_WHERE(msg) \
  std::cout << "LOG_TRACE TRACE_WHERE: " << msg << " in " << __FUNCTION__ <<  "() in " << __FILE__ << ":" << __LINE__ << std::endl;

#define TRACE_WHERE_VAR(var) \
  std::cout << "LOG_TRACE TRACE_WHERE_VAR: " << #var << "=" << var << "  in " << __FUNCTION__ <<  "() in " << __FILE__ << ":" << __LINE__ << std::endl;


#define TRACE_BEFORE_AFTER(...)                   \
    std::cout << "LOG_TRACE TRACE_BEFORE_AFTER: " << "Before " << #__VA_ARGS__ << "\n";  \
    __VA_ARGS__; \
    std::cout << "LOG_TRACE TRACE_BEFORE_AFTER: "<< "After " << #__VA_ARGS__ << "\n";

#define TRACE_VARIABLE(variable) \
{ \
  std::cout << "LOG_TRACE: "<< #variable << "= -->" << variable << "<--" << "address(" << #variable << ") = " << (size_t)(void *)(&variable) << "\n"; \
}

#define TRACE_VARIABLE_POINTER(variable) \
{ \
  std::cout << "LOG_TRACE: "<< "address(" << #variable << ") = " << (size_t)(void *)(&variable) << "\n"; \
}

#define TRACE_CONTAINER_SIZE(variable) \
{ \
  std::cout << "LOG_TRACE: "<< "size(" << #variable << ")=" << variable.size() << " address(" << #variable << ") = " << (size_t)(void *)(&variable) << "\n"; \
}

#else //BABYLON_LOG_TRACE_ON

#define TRACE_WHERE(msg)
#define TRACE_WHERE_VAR(var)
#define TRACE_BEFORE_AFTER(...)  __VA_ARGS__
#define TRACE_VARIABLE_POINTER(variable)
#define TRACE_VARIABLE(variable)
#define TRACE_CONTAINER_SIZE(variable)

#endif // BABYLON_LOG_TRACE_ON

#endif // BABYLONCPP_LOG_TRACE_H
