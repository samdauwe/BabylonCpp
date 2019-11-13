#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PERLIN_NOISE_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_PERLIN_NOISE_PERLIN_NOISE_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class PerlinNoiseProceduralTexture;
using PerlinNoiseProceduralTexturePtr
  = std::shared_ptr<PerlinNoiseProceduralTexture>;

class BABYLON_SHARED_EXPORT PerlinNoiseProceduralTexture
    : public ProceduralTexture {

public:
  template <typename... Ts>
  static PerlinNoiseProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<PerlinNoiseProceduralTexture>(
      new PerlinNoiseProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~PerlinNoiseProceduralTexture(); // = default

  void updateShaderUniforms();
  void render(bool useCameraPostProcess = false);
  void resize(const Size& size, bool generateMipMaps = false);

  /**
   * @brief Serializes this perlin noise procedural texture.
   * @returns a serialized perlin noise procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a Perlin Noise Procedural Texture from parsed perlin noise
   * procedural texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing perlin noise procedural
   * texture information
   * @returns a parsed Perlin Noise Procedural Texture
   */
  static std::unique_ptr<PerlinNoiseProceduralTexture>
  Parse(const json& parsedTexture, Scene* scene, const std::string& rootUrl);

  PerlinNoiseProceduralTexture(const std::string& name, const Size& size,
                               Scene* scene, Texture* fallbackTexture = nullptr,
                               bool generateMipMaps = false);

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
