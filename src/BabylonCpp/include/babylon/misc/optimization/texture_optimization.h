#ifndef BABYLON_MISC_OPTIMIZATION_TEXTURE_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_TEXTURE_OPTIMIZATION_H

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT TextureOptimization : public SceneOptimization {

public:
  TextureOptimization(int priority = 0, int maximumSize = 1024);
  ~TextureOptimization() override;

  bool apply(Scene* scene) override;

public:
  int maximumSize;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_TEXTURE_OPTIMIZATION_H
