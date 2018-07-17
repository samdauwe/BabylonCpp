#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_WOOD_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_WOOD_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT WoodProceduralTexture : public ProceduralTexture {

public:
  WoodProceduralTexture(const std::string& name, const Size& size, Scene* scene,
                        Texture* fallbackTexture = nullptr,
                        bool generateMipMaps     = false);
  ~WoodProceduralTexture();

  void updateShaderUniforms();

  /**
   * @brief Serializes this wood procedural texture.
   * @returns a serialized wood procedural texture object
   */
  Json::object serialize() const;

  /**
   * @brief Creates a Wood Procedural Texture from parsed wood procedural
   * texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing wood procedural texture
   * information
   * @returns a parsed Wood Procedural Texture
   */
  static unique_ptr_t<WoodProceduralTexture>
  Parse(const Json::value& parsedTexture, Scene* scene,
        const string_t& rootUrl);

protected:
  float get_ampScale() const;
  void set_ampScale(float value);
  Color3& get_woodColor();
  void set_woodColor(const Color3& value);

public:
  Property<WoodProceduralTexture, float> ampScale;
  Property<WoodProceduralTexture, Color3> woodColor;

private:
  float _ampScale;
  Color3 _woodColor;

}; // end of class WoodProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_WOOD_PROCEDURAL_TEXTURE_H
