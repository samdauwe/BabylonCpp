#include <babylon/proceduraltextureslibrary/brick/brick_procedural_texture.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/brick/brick_procedural_texture_fragment_fx.h>

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
    , numberOfBricksHeight{this,
                           &BrickProceduralTexture::get_numberOfBricksHeight,
                           &BrickProceduralTexture::set_numberOfBricksHeight}
    , numberOfBricksWidth{this,
                          &BrickProceduralTexture::get_numberOfBricksWidth,
                          &BrickProceduralTexture::set_numberOfBricksWidth}
    , jointColor{this, &BrickProceduralTexture::get_jointColor,
                 &BrickProceduralTexture::set_jointColor}
    , brickColor{this, &BrickProceduralTexture::get_brickColor,
                 &BrickProceduralTexture::set_brickColor}
    , _numberOfBricksHeight{15.f}
    , _numberOfBricksWidth{5.f}
    , _jointColor{Color3(0.72f, 0.72f, 0.72f)}
    , _brickColor{Color3(0.77f, 0.47f, 0.40f)}
{
  // Fragment shader
  Effect::ShadersStore()["brickProceduralTexturePixelShader"]
    = brickProceduralTexturePixelShader;

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

float BrickProceduralTexture::get_numberOfBricksHeight() const
{
  return _numberOfBricksHeight;
}

void BrickProceduralTexture::set_numberOfBricksHeight(float value)
{
  _numberOfBricksHeight = value;
  updateShaderUniforms();
}

float BrickProceduralTexture::get_numberOfBricksWidth() const
{
  return _numberOfBricksWidth;
}

void BrickProceduralTexture::set_numberOfBricksWidth(float value)
{
  _numberOfBricksWidth = value;
  updateShaderUniforms();
}

Color3& BrickProceduralTexture::get_jointColor()
{
  return _jointColor;
}

void BrickProceduralTexture::set_jointColor(const Color3& value)
{
  _jointColor = value;
  updateShaderUniforms();
}

Color3& BrickProceduralTexture::get_brickColor()
{
  return _brickColor;
}

void BrickProceduralTexture::BrickProceduralTexture::set_brickColor(
  const Color3& value)
{
  _brickColor = value;
  updateShaderUniforms();
}

Json::object BrickProceduralTexture::serialize() const
{
  return Json::object();
}

std::unique_ptr<BrickProceduralTexture>
BrickProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                              Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
