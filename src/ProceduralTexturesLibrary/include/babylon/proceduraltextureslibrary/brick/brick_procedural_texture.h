#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_BRICK_BRICK_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_BRICK_BRICK_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT BrickProceduralTexture : public ProceduralTexture {

public:
  BrickProceduralTexture(const std::string& name, const Size& size,
                         Scene* scene, Texture* fallbackTexture = nullptr,
                         bool generateMipMaps = true);
  ~BrickProceduralTexture();

  void updateShaderUniforms();

  /**
   * @brief Serializes this brick procedural texture.
   * @returns a serialized brick procedural texture object
   */
  Json::object serialize() const;

  /**
   * @brief Creates a Brick Procedural Texture from parsed brick procedural
   * texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing brick procedural texture
   * information
   * @returns a parsed Brick Procedural Texture
   */
  static unique_ptr_t<BrickProceduralTexture>
  Parse(const Json::value& parsedTexture, Scene* scene,
        const string_t& rootUrl);

protected:
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
