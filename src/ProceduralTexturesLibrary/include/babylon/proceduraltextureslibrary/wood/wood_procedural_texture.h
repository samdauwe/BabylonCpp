#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_PROCEDURAL_TEXTURE_H

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
  float ampScale() const;
  void setAmpScale(float value);
  Color3& woodColor();
  void setWoodColor(const Color3& value);

private:
  float _ampScale;
  Color3 _woodColor;

}; // end of class WoodProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_PROCEDURAL_TEXTURE_H
