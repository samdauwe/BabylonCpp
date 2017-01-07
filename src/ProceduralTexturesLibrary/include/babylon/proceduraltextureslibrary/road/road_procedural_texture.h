#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_ROAD_ROAD_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_ROAD_ROAD_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class BABYLON_SHARED_EXPORT RoadProceduralTexture : public ProceduralTexture {

public:
  RoadProceduralTexture(const std::string& name, const Size& size, Scene* scene,
                        Texture* fallbackTexture = nullptr,
                        bool generateMipMaps     = false);
  ~RoadProceduralTexture();

  void updateShaderUniforms();
  Color3& roadColor();
  void setRoadColor(const Color3& value);

private:
  Color3 _roadColor;

}; // end of class RoadProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_PROCEDURAL_TEXTURES_LIBRARY_ROAD_ROAD_PROCEDURAL_TEXTURE_H
