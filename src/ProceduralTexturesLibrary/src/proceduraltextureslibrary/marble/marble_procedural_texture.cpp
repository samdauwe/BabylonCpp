#include <babylon/proceduraltextureslibrary/marble/marble_procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

MarbleProceduralTexture::MarbleProceduralTexture(const std::string& iName,
                                                 const Size& size, Scene* scene,
                                                 Texture* fallbackTexture,
                                                 bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "marbleProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , _numberOfTilesHeight{3.f}
    , _numberOfTilesWidth{3.f}
    , _amplitude{9.f}
    , _jointColor{Color3(0.72f, 0.72f, 0.72f)}
{
  updateShaderUniforms();
}

MarbleProceduralTexture::~MarbleProceduralTexture()
{
}

void MarbleProceduralTexture::updateShaderUniforms()
{
  setFloat("numberOfTilesHeight", _numberOfTilesHeight);
  setFloat("numberOfTilesWidth", _numberOfTilesWidth);
  setFloat("amplitude", _amplitude);
  setColor3("jointColor", _jointColor);
}

float MarbleProceduralTexture::numberOfTilesHeight() const
{
  return _numberOfTilesHeight;
}

void MarbleProceduralTexture::setNumberOfTilesHeight(float value)
{
  _numberOfTilesHeight = value;
  updateShaderUniforms();
}

float MarbleProceduralTexture::numberOfTilesWidth() const
{
  return _numberOfTilesWidth;
}

void MarbleProceduralTexture::setNumberOfTilesWidth(float value)
{
  _numberOfTilesWidth = value;
  updateShaderUniforms();
}

float MarbleProceduralTexture::amplitude() const
{
  return _amplitude;
}

void MarbleProceduralTexture::setAmplitude(float value)
{
  _amplitude = value;
  updateShaderUniforms();
}

Color3& MarbleProceduralTexture::jointColor()
{
  return _jointColor;
}

void MarbleProceduralTexture::setJointColor(const Color3& value)
{
  _jointColor = value;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
