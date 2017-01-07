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
  float numberOfBricksHeight() const;
  void setNumberOfBricksHeight(float value);
  float numberOfBricksWidth() const;
  void setNumberOfBricksWidth(float value);
  Color3& jointColor();
  void setJointColor(const Color3& value);
  Color3& brickColor();
  void setBrickColor(const Color3& value);

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
