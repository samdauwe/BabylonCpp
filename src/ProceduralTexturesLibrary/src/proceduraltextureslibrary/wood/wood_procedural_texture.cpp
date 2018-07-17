#include <babylon/proceduraltextureslibrary/wood/wood_procedural_texture.h>

#include <babylon/core/json.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

WoodProceduralTexture::WoodProceduralTexture(const std::string& iName,
                                             const Size& size, Scene* scene,
                                             Texture* fallbackTexture,
                                             bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "woodProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , ampScale{this, &WoodProceduralTexture::get_ampScale,
               &WoodProceduralTexture::set_ampScale}
    , woodColor{this, &WoodProceduralTexture::get_woodColor,
                &WoodProceduralTexture::set_woodColor}
    , _ampScale{100.f}
    , _woodColor{Color3(0.32f, 0.17f, 0.09f)}
{
  updateShaderUniforms();
}

WoodProceduralTexture::~WoodProceduralTexture()
{
}

void WoodProceduralTexture::updateShaderUniforms()
{
  setFloat("ampScale", _ampScale);
  setColor3("woodColor", _woodColor);
}

float WoodProceduralTexture::get_ampScale() const
{
  return _ampScale;
}

void WoodProceduralTexture::set_ampScale(float value)
{
  _ampScale = value;
  updateShaderUniforms();
}

Color3& WoodProceduralTexture::get_woodColor()
{
  return _woodColor;
}

void WoodProceduralTexture::set_woodColor(const Color3& value)
{
  _woodColor = value;
  updateShaderUniforms();
}

Json::object WoodProceduralTexture::serialize() const
{
  return Json::object();
}

unique_ptr_t<WoodProceduralTexture>
WoodProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                             Scene* /*scene*/, const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
