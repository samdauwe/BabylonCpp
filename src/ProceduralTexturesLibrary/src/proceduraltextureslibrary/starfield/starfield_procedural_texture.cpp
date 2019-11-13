#include <babylon/proceduraltextureslibrary/starfield/starfield_procedural_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/starfield/starfield_procedural_texture_fragment_fx.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

StarfieldProceduralTexture::StarfieldProceduralTexture(const std::string& iName,
                                                       const Size& size,
                                                       Scene* scene,
                                                       Texture* fallbackTexture,
                                                       bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "starfieldProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , time{this, &StarfieldProceduralTexture::get_time,
           &StarfieldProceduralTexture::set_time}
    , alpha{this, &StarfieldProceduralTexture::get_alpha,
            &StarfieldProceduralTexture::set_alpha}
    , beta{this, &StarfieldProceduralTexture::get_beta,
           &StarfieldProceduralTexture::set_beta}
    , zoom{this, &StarfieldProceduralTexture::get_zoom,
           &StarfieldProceduralTexture::set_zoom}
    , formuparam{this, &StarfieldProceduralTexture::get_formuparam,
                 &StarfieldProceduralTexture::set_formuparam}
    , stepsize{this, &StarfieldProceduralTexture::get_stepsize,
               &StarfieldProceduralTexture::set_stepsize}
    , tile{this, &StarfieldProceduralTexture::get_tile,
           &StarfieldProceduralTexture::set_tile}
    , brightness{this, &StarfieldProceduralTexture::get_brightness,
                 &StarfieldProceduralTexture::set_brightness}
    , darkmatter{this, &StarfieldProceduralTexture::get_darkmatter,
                 &StarfieldProceduralTexture::set_darkmatter}
    , distfading{this, &StarfieldProceduralTexture::get_distfading,
                 &StarfieldProceduralTexture::set_distfading}
    , saturation{this, &StarfieldProceduralTexture::get_saturation,
                 &StarfieldProceduralTexture::set_saturation}
    , _time{1.f}
    , _alpha{0.5f}
    , _beta{0.8f}
    , _zoom{0.8f}
    , _formuparam{0.53f}
    , _stepsize{0.1f}
    , _tile{0.850f}
    , _brightness{0.0015f}
    , _darkmatter{0.400f}
    , _distfading{0.730f}
    , _saturation{0.850f}
{
  // Fragment shader
  Effect::ShadersStore()["starfieldProceduralTexturePixelShader"]
    = starfieldProceduralTexturePixelShader;

  updateShaderUniforms();
}

StarfieldProceduralTexture::~StarfieldProceduralTexture() = default;

void StarfieldProceduralTexture::updateShaderUniforms()
{
  setFloat("time", _time);
  setFloat("alpha", _alpha);
  setFloat("beta", _beta);
  setFloat("zoom", _zoom);
  setFloat("formuparam", _formuparam);
  setFloat("stepsize", _stepsize);
  setFloat("tile", _tile);
  setFloat("brightness", _brightness);
  setFloat("darkmatter", _darkmatter);
  setFloat("distfading", _distfading);
  setFloat("saturation", _saturation);
}

float StarfieldProceduralTexture::get_time() const
{
  return _time;
}

void StarfieldProceduralTexture::set_time(float value)
{
  _time = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_alpha() const
{
  return _alpha;
}

void StarfieldProceduralTexture::set_alpha(float value)
{
  _alpha = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_beta() const
{
  return _beta;
}

void StarfieldProceduralTexture::set_beta(float value)
{
  _beta = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_zoom() const
{
  return _zoom;
}

void StarfieldProceduralTexture::set_zoom(float value)
{
  _zoom = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_formuparam() const
{
  return _formuparam;
}

void StarfieldProceduralTexture::set_formuparam(float value)
{
  _formuparam = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_stepsize() const
{
  return _stepsize;
}

void StarfieldProceduralTexture::set_stepsize(float value)
{
  _stepsize = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_tile() const
{
  return _tile;
}

void StarfieldProceduralTexture::set_tile(float value)
{
  _tile = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_brightness() const
{
  return _brightness;
}

void StarfieldProceduralTexture::set_brightness(float value)
{
  _brightness = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_darkmatter() const
{
  return _darkmatter;
}

void StarfieldProceduralTexture::set_darkmatter(float value)
{
  _darkmatter = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_distfading() const
{
  return _distfading;
}

void StarfieldProceduralTexture::set_distfading(float value)
{
  _distfading = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::get_saturation() const
{
  return _saturation;
}

void StarfieldProceduralTexture::set_saturation(float value)
{
  _saturation = value;
  updateShaderUniforms();
}

json StarfieldProceduralTexture::serialize() const
{
  return nullptr;
}

std::unique_ptr<StarfieldProceduralTexture>
StarfieldProceduralTexture::Parse(const json& /*parsedTexture*/,
                                  Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
