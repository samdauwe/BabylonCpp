#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_GRASS_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_GRASS_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class GrassProceduralTexture;
using GrassProceduralTexturePtr = std::shared_ptr<GrassProceduralTexture>;

class BABYLON_SHARED_EXPORT GrassProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static GrassProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<GrassProceduralTexture>(
      new GrassProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~GrassProceduralTexture(); // = default

  void updateShaderUniforms();

  /**
   * @brief Serializes this grass procedural texture.
   * @returns a serialized grass procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a Grass Procedural Texture from parsed grass procedural
   * texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing grass procedural texture
   * information
   * @returns a parsed Grass Procedural Texture
   */
  static std::unique_ptr<GrassProceduralTexture>
  Parse(const json& parsedTexture, Scene* scene, const std::string& rootUrl);

protected:
  GrassProceduralTexture(const std::string& name, const Size& size,
                         Scene* scene, Texture* fallbackTexture = nullptr,
                         bool generateMipMaps = false);

  std::vector<Color3>& get_grassColors();
  void set_grassColors(const std::vector<Color3>& value);
  Color3& get_groundColor();
  void set_groundColor(const Color3& value);

public:
  Property<GrassProceduralTexture, std::vector<Color3>> grassColors;
  Property<GrassProceduralTexture, Color3> groundColor;

private:
  std::vector<Color3> _grassColors;
  Color3 _groundColor;

}; // end of class GrassProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_GRASS_PROCEDURAL_TEXTURE_H
