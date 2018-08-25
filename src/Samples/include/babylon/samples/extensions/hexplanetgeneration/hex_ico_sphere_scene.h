#ifndef BABYLON_SAMPLES_EXTENSIONS_HEX_PLANET_GENERATION_HEX_ICO_SPHERE_SCENE_H
#define BABYLON_SAMPLES_EXTENSIONS_HEX_PLANET_GENERATION_HEX_ICO_SPHERE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class HexIcosphereScene : public IRenderableScene {

public:
  HexIcosphereScene(ICanvas* iCanvas);
  virtual ~HexIcosphereScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _createHexIcosphere(std::size_t degree, float scale, Scene* scene);

private:
  MeshPtr _hexIcosphere;

}; // end of class HexIcosphereScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
// BABYLON_SAMPLES_EXTENSIONS_HEX_PLANET_GENERATION_HEX_ICO_SPHERE_SCENE_H
