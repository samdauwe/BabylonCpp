#ifndef BABYLON_SAMPLES_EXTENSIONS_HEX_PLANET_GENERATION_PROCEDURAL_HEX_PLANET_GENERATION_SCENE_H
#define BABYLON_SAMPLES_EXTENSIONS_HEX_PLANET_GENERATION_PROCEDURAL_HEX_PLANET_GENERATION_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace Extensions {
class SimplePlanet;
}

namespace Samples {

class ProceduralHexPlanetGenerationScene : public IRenderableScene {

public:
  ProceduralHexPlanetGenerationScene(ICanvas* iCanvas);
  ~ProceduralHexPlanetGenerationScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  std::unique_ptr<Extensions::SimplePlanet> _hexPlanet;

}; // end of class ProceduralHexPlanetGenerationScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
// BABYLON_SAMPLES_EXTENSIONS_HEX_PLANET_GENERATION_PROCEDURAL_HEX_PLANET_GENERATION_SCENE_H
