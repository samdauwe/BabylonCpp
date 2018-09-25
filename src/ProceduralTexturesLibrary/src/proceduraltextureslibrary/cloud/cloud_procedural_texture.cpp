#include <babylon/proceduraltextureslibrary/cloud/cloud_procedural_texture.h>

#include <babylon/core/json.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

CloudProceduralTexture::CloudProceduralTexture(const std::string& iName,
                                               const Size& size, Scene* scene,
                                               Texture* fallbackTexture,
                                               bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "cloudProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , skyColor{this, &CloudProceduralTexture::get_skyColor,
               &CloudProceduralTexture::set_skyColor}
    , cloudColor{this, &CloudProceduralTexture::get_cloudColor,
                 &CloudProceduralTexture::set_cloudColor}
    , _skyColor{Color4(0.15f, 0.68f, 1.f, 1.f)}
    , _cloudColor{Color4(1.f, 1.f, 1.f, 1.f)}
{
  updateShaderUniforms();
}

CloudProceduralTexture::~CloudProceduralTexture()
{
}

void CloudProceduralTexture::updateShaderUniforms()
{
  setColor4("skyColor", _skyColor);
  setColor4("cloudColor", _cloudColor);
}

Color4& CloudProceduralTexture::get_skyColor()
{
  return _skyColor;
}

void CloudProceduralTexture::set_skyColor(const Color4& value)
{
  _skyColor = value;
  updateShaderUniforms();
}

Color4& CloudProceduralTexture::get_cloudColor()
{
  return _cloudColor;
}

void CloudProceduralTexture::set_cloudColor(const Color4& value)
{
  _cloudColor = value;
  updateShaderUniforms();
}

Json::object CloudProceduralTexture::serialize() const
{
  return Json::object();
}

std::unique_ptr<CloudProceduralTexture>
CloudProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                              Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
