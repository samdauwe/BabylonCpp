#ifndef BABYLON_CORE_NONCOPYABLE_H
#define BABYLON_CORE_NONCOPYABLE_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT NonCopyable {

protected:
  NonCopyable()
  {
  }
  ~NonCopyable()
  {
  }

private:
  NonCopyable(const NonCopyable&) = delete;
  const NonCopyable& operator=(const NonCopyable&) = delete;

}; // end of class NonCopyable

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_NONCOPYABLE_H
