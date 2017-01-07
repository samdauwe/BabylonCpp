#include <babylon/proceduraltextureslibrary/grass/grass_procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

GrassProceduralTexture::GrassProceduralTexture(const std::string& iName,
                                               const Size& size, Scene* scene,
                                               Texture* fallbackTexture,
                                               bool generateMipMaps)

    : ProceduralTexture{iName,
                        size,
                        "grassProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , _herb1{Color3(0.29f, 0.38f, 0.02f)}
    , _herb2{Color3(0.36f, 0.49f, 0.09f)}
    , _herb3{Color3(0.51f, 0.60f, 0.28f)}
    , _groundColor{Color3(1.f, 1.f, 1.f)}
{
  _grassColors.clear();
  _grassColors.emplace_back(Color3(0.29f, 0.38f, 0.02f));
  _grassColors.emplace_back(Color3(0.36f, 0.49f, 0.09f));
  _grassColors.emplace_back(Color3(0.51f, 0.60f, 0.28f));

  updateShaderUniforms();
}

GrassProceduralTexture::~GrassProceduralTexture()
{
}

void GrassProceduralTexture::updateShaderUniforms()
{
  setColor3("herb1Color", _grassColors[0]);
  setColor3("herb2Color", _grassColors[1]);
  setColor3("herb3Color", _grassColors[2]);
  setColor3("groundColor", _groundColor);
}

std::vector<Color3>& GrassProceduralTexture::grassColors()
{
  return _grassColors;
}

void GrassProceduralTexture::setGrassColors(const std::vector<Color3>& value)
{
  _grassColors = value;
  updateShaderUniforms();
}

Color3& GrassProceduralTexture::groundColor()
{
  return _groundColor;
}

void GrassProceduralTexture::setGroundColor(const Color3& value)
{
  _groundColor = value;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
