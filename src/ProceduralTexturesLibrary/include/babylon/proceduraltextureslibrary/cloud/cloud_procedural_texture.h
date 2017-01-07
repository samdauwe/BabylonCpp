#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color4.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT CloudProceduralTexture : public ProceduralTexture {

public:
  CloudProceduralTexture(const std::string& name, const Size& size,
                         Scene* scene, Texture* fallbackTexture = nullptr,
                         bool generateMipMaps = true);
  ~CloudProceduralTexture();

  void updateShaderUniforms();
  Color4& skyColor();
  void setSkyColor(const Color4& value);
  Color4& cloudColor();
  void setCloudColor(const Color4& value);

private:
  Color4 _skyColor;
  Color4 _cloudColor;

}; // end of class CloudProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_H
