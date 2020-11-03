#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_ROAD_ROAD_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_ROAD_ROAD_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/maths/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class RoadProceduralTexture;
using RoadProceduralTexturePtr = std::shared_ptr<RoadProceduralTexture>;

class BABYLON_SHARED_EXPORT RoadProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static RoadProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<RoadProceduralTexture>(
      new RoadProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~RoadProceduralTexture() override; // = default

  void updateShaderUniforms();

  /**
   * @brief Serializes this road procedural texture.
   * @returns a serialized road procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a Road Procedural Texture from parsed road procedural
   * texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing road procedural texture
   * information
   * @returns a parsed Road Procedural Texture
   */
  static std::unique_ptr<RoadProceduralTexture> Parse(const json& parsedTexture, Scene* scene,
                                                      const std::string& rootUrl);

protected:
  RoadProceduralTexture(const std::string& name, const RenderTargetTextureSize& size, Scene* scene,
                        Texture* fallbackTexture = nullptr, bool generateMipMaps = false);

  Color3& get_roadColor();
  void set_roadColor(const Color3& value);

public:
  Property<RoadProceduralTexture, Color3> roadColor;

private:
  Color3 _roadColor;

}; // end of class RoadProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_ROAD_ROAD_PROCEDURAL_TEXTURE_H
