#ifndef BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZATION_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class Scene;

class BABYLON_SHARED_EXPORT SceneOptimization {

public:
  SceneOptimization(int priority = 0);
  virtual ~SceneOptimization();

  virtual bool apply(Scene* scene);

public:
  int priority;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZATION_H
