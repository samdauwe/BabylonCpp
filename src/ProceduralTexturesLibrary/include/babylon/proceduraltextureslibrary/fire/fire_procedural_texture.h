#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_FIRE_FIRE_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_FIRE_FIRE_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector2.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT FireProceduralTexture : public ProceduralTexture {

public:
  FireProceduralTexture(const std::string& name, const Size& size, Scene* scene,
                        Texture* fallbackTexture = nullptr,
                        bool generateMipMaps     = false);
  ~FireProceduralTexture();

  void updateShaderUniforms();
  void render(bool useCameraPostProcess);
  static std::vector<Color3> PurpleFireColors();
  static std::vector<Color3> GreenFireColors();
  static std::vector<Color3> RedFireColors();
  static std::vector<Color3> BlueFireColors();
  std::vector<Color3>& fireColors();
  void setFireColors(const std::vector<Color3>& value);
  float time() const;
  void setTime(float value);
  Vector2& speed();
  void setSpeed(const Vector2& value);
  float alphaThreshold() const;
  void setAlphaThreshold(float value);

private:
  float _time;
  Vector2 _speed;
  bool _autoGenerateTime;
  std::vector<Color3> _fireColors;
  float _alphaThreshold;

}; // end of class FireProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_FIRE_FIRE_PROCEDURAL_TEXTURE_H
