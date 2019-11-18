#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_BRICK_BRICK_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_BRICK_BRICK_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BrickProceduralTexture;
using BrickProceduralTexturePtr = std::shared_ptr<BrickProceduralTexture>;

class BABYLON_SHARED_EXPORT BrickProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static BrickProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<BrickProceduralTexture>(
      new BrickProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~BrickProceduralTexture() override; // = default

  void updateShaderUniforms();

  /**
   * @brief Serializes this brick procedural texture.
   * @returns a serialized brick procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a Brick Procedural Texture from parsed brick procedural
   * texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing brick procedural texture
   * information
   * @returns a parsed Brick Procedural Texture
   */
  static std::unique_ptr<BrickProceduralTexture> Parse(const json& parsedTexture, Scene* scene,
                                                       const std::string& rootUrl);

protected:
  BrickProceduralTexture(const std::string& name, const Size& size, Scene* scene,
                         Texture* fallbackTexture = nullptr, bool generateMipMaps = true);

  float get_numberOfBricksHeight() const;
  void set_numberOfBricksHeight(float value);
  float get_numberOfBricksWidth() const;
  void set_numberOfBricksWidth(float value);
  Color3& get_jointColor();
  void set_jointColor(const Color3& value);
  Color3& get_brickColor();
  void set_brickColor(const Color3& value);

public:
  Property<BrickProceduralTexture, float> numberOfBricksHeight;
  Property<BrickProceduralTexture, float> numberOfBricksWidth;
  Property<BrickProceduralTexture, Color3> jointColor;
  Property<BrickProceduralTexture, Color3> brickColor;

private:
  float _numberOfBricksHeight;
  float _numberOfBricksWidth;
  Color3 _jointColor;
  Color3 _brickColor;

}; // end of class BrickProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_BRICK_BRICK_PROCEDURAL_TEXTURE_H
