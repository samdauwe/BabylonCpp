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

  /**
   * @brief Serializes this perlin noise procedural texture.
   * @returns a serialized perlin noise procedural texture object
   */
  Json::object serialize() const;

  /**
   * @brief Creates a Perlin Noise Procedural Texture from parsed perlin noise
   * procedural texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing perlin noise procedural
   * texture information
   * @returns a parsed Perlin Noise Procedural Texture
   */
  static unique_ptr_t<PerlinNoiseProceduralTexture>
  Parse(const Json::value& parsedTexture, Scene* scene,
        const string_t& rootUrl);

public:
  float time;
  float timeScale;
  float translationSpeed;

private:
  float _currentTranslation;

}; // end of class PerlinNoiseProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PERLIN_NOISE_PROCEDURAL_TEXTURE_H
