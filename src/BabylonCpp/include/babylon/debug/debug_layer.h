#ifndef BABYLON_DEBUG_DEBUG_LAYER_H
#define BABYLON_DEBUG_DEBUG_LAYER_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class Scene;

class BABYLON_SHARED_EXPORT DebugLayer {

public:
  DebugLayer(Scene* scene);
  ~DebugLayer();

  void hide();

public:
  Scene* _scene;

}; // end of class DebugLayer

} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_DEBUG_LAYER_H
