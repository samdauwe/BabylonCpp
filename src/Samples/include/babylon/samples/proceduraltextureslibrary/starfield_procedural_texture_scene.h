#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_STARFIELD_PROCEDURAL_TEXTURE_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_STARFIELD_PROCEDURAL_TEXTURE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {
class StarfieldProceduralTexture;
using StarfieldProceduralTexturePtr
  = std::shared_ptr<StarfieldProceduralTexture>;
} // end of namespace ProceduralTexturesLibrary
namespace Samples {

/**
 * @brief StarfieldProceduralTextureScene Scene. Example demonstrating how to
 * use starfield procedural texture.
 */
class StarfieldProceduralTextureScene : public IRenderableScene {

public:
  StarfieldProceduralTextureScene(ICanvas* iCanvas);
  ~StarfieldProceduralTextureScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ProceduralTexturesLibrary::StarfieldProceduralTexturePtr _starfieldTexture;

}; // end of class StarfieldProceduralTextureScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_STARFIELD_PROCEDURAL_TEXTURE_SCENE_H
