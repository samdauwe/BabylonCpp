#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/maths/color4.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class CloudProceduralTexture;
using CloudProceduralTexturePtr = std::shared_ptr<CloudProceduralTexture>;

class BABYLON_SHARED_EXPORT CloudProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static CloudProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<CloudProceduralTexture>(
      new CloudProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~CloudProceduralTexture() override; // = default

  void updateShaderUniforms();

  /**
   * @brief Serializes this cloud procedural texture.
   * @returns a serialized cloud procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a Cloud Procedural Texture from parsed cloud procedural
   * texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing cloud procedural texture
   * information
   * @returns a parsed Cloud Procedural Texture
   */
  static std::unique_ptr<CloudProceduralTexture> Parse(const json& parsedTexture, Scene* scene,
                                                       const std::string& rootUrl);

protected:
  CloudProceduralTexture(const std::string& name, const Size& size, Scene* scene,
                         Texture* fallbackTexture = nullptr, bool generateMipMaps = true);

  Color4& get_skyColor();
  void set_skyColor(const Color4& value);
  Color4& get_cloudColor();
  void set_cloudColor(const Color4& value);

public:
  Property<CloudProceduralTexture, Color4> skyColor;
  Property<CloudProceduralTexture, Color4> cloudColor;

private:
  Color4 _skyColor;
  Color4 _cloudColor;

}; // end of class CloudProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_H
