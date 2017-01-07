#include <babylon/proceduraltextureslibrary/road/road_procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

RoadProceduralTexture::RoadProceduralTexture(const std::string& iName,
                                             const Size& size, Scene* scene,
                                             Texture* fallbackTexture,
                                             bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "roadProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , _roadColor{Color3(0.53f, 0.53f, 0.53f)}
{
  updateShaderUniforms();
}

RoadProceduralTexture::~RoadProceduralTexture()
{
}

void RoadProceduralTexture::updateShaderUniforms()
{
  setColor3("roadColor", _roadColor);
}

Color3& RoadProceduralTexture::roadColor()
{
  return _roadColor;
}

void RoadProceduralTexture::setRoadColor(const Color3& value)
{
  _roadColor = value;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
