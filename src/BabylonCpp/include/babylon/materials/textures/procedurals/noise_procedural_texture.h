#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_NOISE_PROCEDURAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_NOISE_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/core/json.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

namespace BABYLON {

/**
 * @brief Class used to generate noise procedural textures.
 */
class BABYLON_SHARED_EXPORT NoiseProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static NoiseProceduralTexture* New(Ts&&... args)
  {
    auto texture = new NoiseProceduralTexture(::std::forward<Ts>(args)...);
    texture->addToScene(
      static_cast<std::unique_ptr<NoiseProceduralTexture>>(texture));

    return texture;
  }
  ~NoiseProceduralTexture() override;

  /**
   * @brief Generate the current state of the procedural texture.
   */
  void render(bool useCameraPostProcess = false) override;

  /**
   * @brief Serializes this noise procedural texture
   * @returns a serialized noise procedural texture object
   */
  Json::object serialize() const;

  /**
   * @brief Creates a NoiseProceduralTexture from parsed noise procedural
   * texture data
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing noise procedural texture
   * information
   * @returns a parsed NoiseProceduralTexture
   */
  static void Parse(const Json::value& serializationObject, Scene* scene,
                    const std::string& rootUrl);

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
  NoiseProceduralTexture(const std::string& name, int size = 256,
                         Scene* scene             = nullptr,
                         Texture* fallbackTexture = nullptr,
                         bool generateMipMaps     = false);

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
