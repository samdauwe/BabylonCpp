#include <babylon/proceduraltextureslibrary/marble/marble_procedural_texture.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/marble/marble_procedural_texture_fragment_fx.h>

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
    , numberOfTilesHeight{this,
                          &MarbleProceduralTexture::get_numberOfTilesHeight,
                          &MarbleProceduralTexture::set_numberOfTilesHeight}
    , numberOfTilesWidth{this, &MarbleProceduralTexture::get_numberOfTilesWidth,
                         &MarbleProceduralTexture::set_numberOfTilesWidth}
    , amplitude{this, &MarbleProceduralTexture::get_amplitude,
                &MarbleProceduralTexture::set_amplitude}
    , jointColor{this, &MarbleProceduralTexture::get_jointColor,
                 &MarbleProceduralTexture::set_jointColor}
    , _numberOfTilesHeight{3.f}
    , _numberOfTilesWidth{3.f}
    , _amplitude{9.f}
    , _jointColor{Color3(0.72f, 0.72f, 0.72f)}
{
  // Fragment shader
  Effect::ShadersStore()["marbleProceduralTexturePixelShader"]
    = marbleProceduralTexturePixelShader;

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

float MarbleProceduralTexture::get_numberOfTilesHeight() const
{
  return _numberOfTilesHeight;
}

void MarbleProceduralTexture::set_numberOfTilesHeight(float value)
{
  _numberOfTilesHeight = value;
  updateShaderUniforms();
}

float MarbleProceduralTexture::get_numberOfTilesWidth() const
{
  return _numberOfTilesWidth;
}

void MarbleProceduralTexture::set_numberOfTilesWidth(float value)
{
  _numberOfTilesWidth = value;
  updateShaderUniforms();
}

float MarbleProceduralTexture::get_amplitude() const
{
  return _amplitude;
}

void MarbleProceduralTexture::set_amplitude(float value)
{
  _amplitude = value;
  updateShaderUniforms();
}

Color3& MarbleProceduralTexture::get_jointColor()
{
  return _jointColor;
}

void MarbleProceduralTexture::set_jointColor(const Color3& value)
{
  _jointColor = value;
  updateShaderUniforms();
}

Json::object MarbleProceduralTexture::serialize() const
{
  return Json::object();
}

std::unique_ptr<MarbleProceduralTexture>
MarbleProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                               Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
