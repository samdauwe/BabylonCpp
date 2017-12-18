#ifndef BABYLON_BEHAVIORS_BEHAVIOR_H
#define BABYLON_BEHAVIORS_BEHAVIOR_H

#include <babylon/babylon_global.h>

namespace BABYLON {

template <class TNode>
struct BABYLON_SHARED_EXPORT Behavior {

  string_t name;

  virtual void init()              = 0;
  virtual void attach(TNode* node) = 0;
  virtual void detach()            = 0;

}; // end of class Behavior<TNode>

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_BEHAVIOR_H
