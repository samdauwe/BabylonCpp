#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT GrassProceduralTexture : public ProceduralTexture {

public:
  GrassProceduralTexture(const std::string& name, const Size& size,
                         Scene* scene, Texture* fallbackTexture = nullptr,
                         bool generateMipMaps = false);
  ~GrassProceduralTexture();

  void updateShaderUniforms();
  std::vector<Color3>& grassColors();
  void setGrassColors(const std::vector<Color3>& value);
  Color3& groundColor();
  void setGroundColor(const Color3& value);

private:
  std::vector<Color3> _grassColors;
  Color3 _herb1;
  Color3 _herb2;
  Color3 _herb3;
  Color3 _groundColor;

}; // end of class GrassProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_PROCEDURAL_TEXTURE_H
