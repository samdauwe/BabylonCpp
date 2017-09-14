#ifndef BABYLON_RENDERING_OUTLINE_RENDERER_H
#define BABYLON_RENDERING_OUTLINE_RENDERER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT OutlineRenderer {

public:
  OutlineRenderer(Scene* scene);
  ~OutlineRenderer();

  void render(SubMesh* subMesh, _InstancesBatch* batch,
              bool useOverlay = false);
  bool isReady(SubMesh* subMesh, bool useInstances);

public:
  float zOffset;

private:
  Scene* _scene;
  Effect* _effect;
  string_t _cachedDefines;

}; // end of class OutlineRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_OUTLINE_RENDERER_H
