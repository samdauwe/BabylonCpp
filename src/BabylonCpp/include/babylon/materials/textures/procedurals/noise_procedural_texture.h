#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_NOISE_PROCEDURAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_NOISE_PROCEDURAL_TEXTURE_H

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

using json = nlohmann::json;

namespace BABYLON {

class NoiseProceduralTexture;
using NoiseProceduralTexturePtr = std::shared_ptr<NoiseProceduralTexture>;

/**
 * @brief Class used to generate noise procedural textures.
 */
class BABYLON_SHARED_EXPORT NoiseProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static NoiseProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<NoiseProceduralTexture>(
      new NoiseProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~NoiseProceduralTexture() override; // = default

  /**
   * @brief Generate the current state of the procedural texture.
   */
  void render(bool useCameraPostProcess = false) override;

  /**
   * @brief Serializes this noise procedural texture
   * @returns a serialized noise procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a NoiseProceduralTexture from parsed noise procedural
   * texture data
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @returns a parsed NoiseProceduralTexture
   */
  static NoiseProceduralTexturePtr Parse(const json& serializationObject, Scene* scene);

protected:
  /**
   * @brief Creates a new NoiseProceduralTexture.
   * @param name defines the name fo the texture
   * @param size defines the size of the texture (default is 256)
   * @param scene defines the hosting scene
   * @param fallbackTexture defines the texture to use if the
   * NoiseProceduralTexture can't be created
   * @param generateMipMaps defines if mipmaps must be generated (true by
   * default)
   */
  NoiseProceduralTexture(const std::string& name, int size = 256, Scene* scene = nullptr,
                         Texture* fallbackTexture = nullptr, bool generateMipMaps = false);

  std::string _getDefines() const override;

private:
  void _updateShaderUniforms();

public:
  /**
   * Gets or sets a value between 0 and 1 indicating the overall brightness of
   * the texture (default is 0.2)
   */
  float brightness;

  /**
   * Defines the number of octaves to process
   */
  int octaves;

  /**
   * Defines the level of persistence (0.8 by default)
   */
  float persistence;

  /**
   * Gets or sets animation speed factor (default is 1)
   */
  float animationSpeedFactor;

private:
  float _time;

}; // end of class CustomProceduralTexture

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_TEXTURES_PROCEDURALS_NOISE_PROCEDURAL_TEXTURE_H
