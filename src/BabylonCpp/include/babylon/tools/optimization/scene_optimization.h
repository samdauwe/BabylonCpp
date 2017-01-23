#ifndef BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZATION_H
#define BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZATION_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SceneOptimization {

public:
  SceneOptimization(int priority = 0);
  virtual ~SceneOptimization();

  virtual bool apply(Scene* scene);

public:
  int priority;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZATION_H
