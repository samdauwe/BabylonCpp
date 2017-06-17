#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PERLIN_NOISE_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PERLIN_NOISE_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT PerlinNoiseProceduralTexture
  : public ProceduralTexture {

public:
  PerlinNoiseProceduralTexture(const std::string& name, const Size& size,
                               Scene* scene, Texture* fallbackTexture = nullptr,
                               bool generateMipMaps = false);
  ~PerlinNoiseProceduralTexture();

  void updateShaderUniforms();
  void render(bool useCameraPostProcess = false);
  void resize(const Size& size, bool generateMipMaps = false);

public:
  float time;
  float speed;
  float translationSpeed;

private:
  float _currentTranslation;

}; // end of class PerlinNoiseProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PERLIN_NOISE_PROCEDURAL_TEXTURE_H
