#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_STAR_FIELD_STAR_FIELD_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_STAR_FIELD_STAR_FIELD_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT StarfieldProceduralTexture
  : public ProceduralTexture {

public:
  StarfieldProceduralTexture(const std::string& name, const Size& size,
                             Scene* scene, Texture* fallbackTexture = nullptr,
                             bool generateMipMaps = false);
  ~StarfieldProceduralTexture();

  void updateShaderUniforms();
  float time() const;
  void setTime(float value);
  float alpha() const;
  void setAlpha(float value);
  float beta() const;
  void setBeta(float value);
  float zoom() const;
  void setZoom(float value);
  float formuparam() const;
  void setFormuparam(float value);
  float stepsize() const;
  void setStepsize(float value);
  float tile() const;
  void setTile(float value);
  float brightness() const;
  void setBrightness(float value);
  float darkmatter() const;
  void setDarkmatter(float value);
  float distfading() const;
  void setDistfading(float value);
  float saturation() const;
  void setSaturation(float value);

private:
  float _time;
  float _alpha;
  float _beta;
  float _zoom;
  float _formuparam;
  float _stepsize;
  float _tile;
  float _brightness;
  float _darkmatter;
  float _distfading;
  float _saturation;

}; // end of class StarfieldProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_STAR_FIELD_STAR_FIELD_PROCEDURAL_TEXTURE_H
