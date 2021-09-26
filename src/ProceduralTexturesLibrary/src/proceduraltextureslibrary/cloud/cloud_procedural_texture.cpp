#include <babylon/proceduraltextureslibrary/cloud/cloud_procedural_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/cloud/cloud_procedural_texture_fragment_fx.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

CloudProceduralTexture::CloudProceduralTexture(const std::string& iName,
                                               const RenderTargetTextureSize& size, Scene* scene,
                                               Texture* fallbackTexture, bool generateMipMaps)
    : ProceduralTexture{iName,           size,           "cloudProceduralTexture", scene,
                        fallbackTexture, generateMipMaps}
    , skyColor{this, &CloudProceduralTexture::get_skyColor, &CloudProceduralTexture::set_skyColor}
    , cloudColor{this, &CloudProceduralTexture::get_cloudColor,
                 &CloudProceduralTexture::set_cloudColor}
    , amplitude{this, &CloudProceduralTexture::get_amplitude,
                &CloudProceduralTexture::set_amplitude}
    , numOctaves{this, &CloudProceduralTexture::get_numOctaves,
                 &CloudProceduralTexture::set_numOctaves}
    , _skyColor{Color4(0.15f, 0.68f, 1.f, 1.f)}
    , _cloudColor{Color4(1.f, 1.f, 1.f, 1.f)}
    , _amplitude{1.f}
    , _numOctaves{4}
{
  // Fragment shader
  Effect::ShadersStore()["cloudProceduralTexturePixelShader"] = cloudProceduralTexturePixelShader;

  updateShaderUniforms();
}

CloudProceduralTexture::~CloudProceduralTexture() = default;

void CloudProceduralTexture::updateShaderUniforms()
{
  setColor4("skyColor", _skyColor);
  setColor4("cloudColor", _cloudColor);
  setFloat("amplitude", _amplitude);
  setInt("numOctaves", _numOctaves);
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

float CloudProceduralTexture::get_amplitude() const
{
  return _amplitude;
}

void CloudProceduralTexture::set_amplitude(float value)
{
  _amplitude = value;
  updateShaderUniforms();
}

int CloudProceduralTexture::get_numOctaves() const
{
  return _numOctaves;
}

void CloudProceduralTexture::set_numOctaves(int value)
{
  _numOctaves = value;
  updateShaderUniforms();
}

json CloudProceduralTexture::serialize() const
{
  return nullptr;
}

std::unique_ptr<CloudProceduralTexture>
CloudProceduralTexture::Parse(const json& /*parsedTexture*/, Scene* /*scene*/,
                              const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
