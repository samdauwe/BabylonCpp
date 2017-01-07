#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT MarbleProceduralTexture : public ProceduralTexture {

public:
  MarbleProceduralTexture(const std::string& name, const Size& size,
                          Scene* scene, Texture* fallbackTexture = nullptr,
                          bool generateMipMaps = false);
  ~MarbleProceduralTexture();

  void updateShaderUniforms();
  float numberOfTilesHeight() const;
  void setNumberOfTilesHeight(float value);
  float numberOfTilesWidth() const;
  void setNumberOfTilesWidth(float value);
  float amplitude() const;
  void setAmplitude(float value);
  Color3& jointColor();
  void setJointColor(const Color3& value);

private:
  float _numberOfTilesHeight;
  float _numberOfTilesWidth;
  float _amplitude;
  Color3 _jointColor;

}; // end of class MarbleProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_MARBLE_PROCEDURAL_TEXTURE_H
