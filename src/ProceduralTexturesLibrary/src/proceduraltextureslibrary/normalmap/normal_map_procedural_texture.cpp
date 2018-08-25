#include <babylon/proceduraltextureslibrary/normalmap/normal_map_procedural_texture.h>

#include <babylon/core/json.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

NormalMapProceduralTexture::NormalMapProceduralTexture(const std::string& iName,
                                                       const Size& size,
                                                       Scene* scene,
                                                       Texture* fallbackTexture,
                                                       bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "normalMapProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , baseTexture{this, &NormalMapProceduralTexture::get_baseTexture,
                  &NormalMapProceduralTexture::set_baseTexture}
    , _baseTexture{nullptr}
{
  updateShaderUniforms();
}

NormalMapProceduralTexture::~NormalMapProceduralTexture()
{
}

void NormalMapProceduralTexture::updateShaderUniforms()
{
  setTexture("baseSampler", _baseTexture);
  setFloat("size", static_cast<float>(getRenderSize().width));
}

void NormalMapProceduralTexture::render(bool useCameraPostProcess)
{
  ProceduralTexture::render(useCameraPostProcess);
}

void NormalMapProceduralTexture::resize(const Size& size, bool generateMipMaps)
{
  ProceduralTexture::resize(size, generateMipMaps);

  // We need to update the "size" uniform
  updateShaderUniforms();
}

TexturePtr& NormalMapProceduralTexture::get_baseTexture()
{
  return _baseTexture;
}

void NormalMapProceduralTexture::set_baseTexture(const TexturePtr& texture)
{
  _baseTexture = texture;
  updateShaderUniforms();
}

Json::object NormalMapProceduralTexture::serialize() const
{
  return Json::object();
}

unique_ptr_t<NormalMapProceduralTexture>
NormalMapProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                                  Scene* /*scene*/, const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
