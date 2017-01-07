#include <babylon/proceduraltextureslibrary/brick/brick_procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

BrickProceduralTexture::BrickProceduralTexture(const std::string& iName,
                                               const Size& size, Scene* scene,
                                               Texture* fallbackTexture,
                                               bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "brickProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , _numberOfBricksHeight{15.f}
    , _numberOfBricksWidth{5.f}
    , _jointColor{Color3(0.72f, 0.72f, 0.72f)}
    , _brickColor{Color3(0.77f, 0.47f, 0.40f)}
{
  updateShaderUniforms();
}

BrickProceduralTexture::~BrickProceduralTexture()
{
}

void BrickProceduralTexture::updateShaderUniforms()
{
  setFloat("numberOfBricksHeight", _numberOfBricksHeight);
  setFloat("numberOfBricksWidth", _numberOfBricksWidth);
  setColor3("brickColor", _brickColor);
  setColor3("jointColor", _jointColor);
}

float BrickProceduralTexture::numberOfBricksHeight() const
{
  return _numberOfBricksHeight;
}

void BrickProceduralTexture::setNumberOfBricksHeight(float value)
{
  _numberOfBricksHeight = value;
  updateShaderUniforms();
}

float BrickProceduralTexture::numberOfBricksWidth() const
{
  return _numberOfBricksWidth;
}

void BrickProceduralTexture::setNumberOfBricksWidth(float value)
{
  _numberOfBricksWidth = value;
  updateShaderUniforms();
}

Color3& BrickProceduralTexture::jointColor()
{
  return _jointColor;
}

void BrickProceduralTexture::setJointColor(const Color3& value)
{
  _jointColor = value;
  updateShaderUniforms();
}

Color3& BrickProceduralTexture::brickColor()
{
  return _brickColor;
}

void BrickProceduralTexture::setBrickColor(const Color3& value)
{
  _brickColor = value;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
