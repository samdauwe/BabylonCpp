#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class NormalMapProceduralTexture;
using NormalMapProceduralTexturePtr = std::shared_ptr<NormalMapProceduralTexture>;

class BABYLON_SHARED_EXPORT NormalMapProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static NormalMapProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<NormalMapProceduralTexture>(
      new NormalMapProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~NormalMapProceduralTexture() override; // = default

  void updateShaderUniforms();
  void render(bool useCameraPostProcess = false) override;
  void resize(const Size& size, bool generateMipMaps = false);

  /**
   * @brief Serializes this normal map procedural texture.
   * @returns a serialized normal map procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a Normal Map Procedural Texture from parsed normal map
   * procedural texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing normal map procedural
   * texture information
   * @returns a parsed Normal Map Procedural Texture
   */
  static std::unique_ptr<NormalMapProceduralTexture> Parse(const json& parsedTexture, Scene* scene,
                                                           const std::string& rootUrl);

protected:
  NormalMapProceduralTexture(const std::string& name, const Size& size, Scene* scene,
                             Texture* fallbackTexture = nullptr, bool generateMipMaps = false);

  TexturePtr& get_baseTexture();
  void set_baseTexture(const TexturePtr& texture);

public:
  Property<NormalMapProceduralTexture, TexturePtr> baseTexture;

private:
  TexturePtr _baseTexture;

}; // end of class NormalMapProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H
