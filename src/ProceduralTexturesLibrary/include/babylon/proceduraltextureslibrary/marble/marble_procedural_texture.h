#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_MARBLE_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_MARBLE_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/maths/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

FWD_CLASS_SPTR(MarbleProceduralTexture)

class BABYLON_SHARED_EXPORT MarbleProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static MarbleProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<MarbleProceduralTexture>(
      new MarbleProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~MarbleProceduralTexture() override; // = default

  void updateShaderUniforms();

  /**
   * @brief Serializes this marble procedural texture.
   * @returns a serialized marble procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a Marble Procedural Texture from parsed marble procedural
   * texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing marble procedural texture
   * information
   * @returns a parsed Marble Procedural Texture
   */
  static std::unique_ptr<MarbleProceduralTexture> Parse(const json& parsedTexture, Scene* scene,
                                                        const std::string& rootUrl);

protected:
  MarbleProceduralTexture(const std::string& name, const RenderTargetTextureSize& size,
                          Scene* scene, Texture* fallbackTexture = nullptr,
                          bool generateMipMaps = false);

  float get_numberOfTilesHeight() const;
  void set_numberOfTilesHeight(float value);
  float get_numberOfTilesWidth() const;
  void set_numberOfTilesWidth(float value);
  float get_amplitude() const;
  void set_amplitude(float value);
  Color3& get_jointColor();
  void set_jointColor(const Color3& value);

public:
  Property<MarbleProceduralTexture, float> numberOfTilesHeight;
  Property<MarbleProceduralTexture, float> numberOfTilesWidth;
  Property<MarbleProceduralTexture, float> amplitude;
  Property<MarbleProceduralTexture, Color3> jointColor;

private:
  float _numberOfTilesHeight;
  float _numberOfTilesWidth;
  float _amplitude;
  Color3 _jointColor;

}; // end of class MarbleProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_MARBLE_PROCEDURAL_TEXTURE_H
