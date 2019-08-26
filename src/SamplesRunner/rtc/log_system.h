#pragma once

#ifndef STDIOLOGSYSTEM_INCLUDED
#define STDIOLOGSYSTEM_INCLUDED

#include <RuntimeCompiler/ICompilerLogger.h>

#include <string>
#include <stdio.h>

namespace BABYLON {
namespace rtc {

// StdioLogSystem for compiler

const size_t LOGSYSTEM_MAX_BUFFER = 4096;

class LogSystem : public ICompilerLogger
{
public:	
	virtual void LogError(const char * format, ...);
	virtual void LogWarning(const char * format, ...);
    virtual void LogInfo(const char * format, ...);

protected:
  enum class MsgType {
    Error, Warning, Info
  };

	void LogInternal(MsgType msgType, const char * format, va_list args);
	char m_buff[LOGSYSTEM_MAX_BUFFER];
};

} // namespace BABYLON
} // namespace rtc

#endif //STDIOLOGSYSTEM_INCLUDED