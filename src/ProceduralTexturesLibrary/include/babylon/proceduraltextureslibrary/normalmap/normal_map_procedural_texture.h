#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT NormalMapProceduralTexture
  : public ProceduralTexture {

public:
  NormalMapProceduralTexture(const std::string& name, const Size& size,
                             Scene* scene, Texture* fallbackTexture = nullptr,
                             bool generateMipMaps = false);
  ~NormalMapProceduralTexture();

  void updateShaderUniforms();
  void render(bool useCameraPostProcess = false);
  void resize(const Size& size, bool generateMipMaps = false);
  Texture* baseTexture() const;
  void setBaseTexture(Texture* texture);

private:
  Texture* _baseTexture;

}; // end of class NormalMapProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMAL_MAP_NORMAL_MAP_PROCEDURAL_TEXTURE_H
