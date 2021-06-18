#ifndef BABYLON_ISTENCIL_STATE_H
#define BABYLON_ISTENCIL_STATE_H

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IStencilState {
  bool enabled;

  unsigned int mask;

  unsigned int func;
  unsigned int funcRef;
  unsigned int funcMask;

  unsigned int opStencilDepthPass;
  unsigned int opStencilFail;
  unsigned int opDepthFail;

  virtual void reset() = 0;
}; // end of struct IStencilState

} // end of namespace BABYLON

#endif // end of BABYLON_ISTENCIL_STATE_H
