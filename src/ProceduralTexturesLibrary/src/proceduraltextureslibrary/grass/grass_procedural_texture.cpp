#include <babylon/proceduraltextureslibrary/grass/grass_procedural_texture.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/grass/grass_procedural_texture_fragment_fx.h>

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
    , grassColors{this, &GrassProceduralTexture::get_grassColors,
                  &GrassProceduralTexture::set_grassColors}
    , groundColor{this, &GrassProceduralTexture::get_groundColor,
                  &GrassProceduralTexture::set_groundColor}
    , _groundColor{Color3(1.f, 1.f, 1.f)}
{
  // Fragment shader
  Effect::ShadersStore()["grassProceduralTexturePixelShader"]
    = grassProceduralTexturePixelShader;

  _grassColors = {
    Color3(0.29f, 0.38f, 0.02f), //
    Color3(0.36f, 0.49f, 0.09f), //
    Color3(0.51f, 0.60f, 0.28f)  //
  };

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

std::vector<Color3>& GrassProceduralTexture::get_grassColors()
{
  return _grassColors;
}

void GrassProceduralTexture::set_grassColors(const std::vector<Color3>& value)
{
  _grassColors = value;
  updateShaderUniforms();
}

Color3& GrassProceduralTexture::get_groundColor()
{
  return _groundColor;
}

void GrassProceduralTexture::set_groundColor(const Color3& value)
{
  _groundColor = value;
  updateShaderUniforms();
}

Json::object GrassProceduralTexture::serialize() const
{
  return Json::object();
}

std::unique_ptr<GrassProceduralTexture>
GrassProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                              Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
