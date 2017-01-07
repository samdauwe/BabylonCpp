#include <babylon/proceduraltextureslibrary/starfield/star_field_procedural_texture.h>

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
  updateShaderUniforms();
}

StarfieldProceduralTexture::~StarfieldProceduralTexture()
{
}

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

float StarfieldProceduralTexture::time() const
{
  return _time;
}

void StarfieldProceduralTexture::setTime(float value)
{
  _time = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::alpha() const
{
  return _alpha;
}

void StarfieldProceduralTexture::setAlpha(float value)
{
  _alpha = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::beta() const
{
  return _beta;
}

void StarfieldProceduralTexture::setBeta(float value)
{
  _beta = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::zoom() const
{
  return _zoom;
}

void StarfieldProceduralTexture::setZoom(float value)
{
  _zoom = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::formuparam() const
{
  return _formuparam;
}

void StarfieldProceduralTexture::setFormuparam(float value)
{
  _formuparam = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::stepsize() const
{
  return _stepsize;
}

void StarfieldProceduralTexture::setStepsize(float value)
{
  _stepsize = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::tile() const
{
  return _tile;
}

void StarfieldProceduralTexture::setTile(float value)
{
  _tile = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::brightness() const
{
  return _brightness;
}

void StarfieldProceduralTexture::setBrightness(float value)
{
  _brightness = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::darkmatter() const
{
  return _darkmatter;
}

void StarfieldProceduralTexture::setDarkmatter(float value)
{
  _darkmatter = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::distfading() const
{
  return _distfading;
}

void StarfieldProceduralTexture::setDistfading(float value)
{
  _distfading = value;
  updateShaderUniforms();
}

float StarfieldProceduralTexture::saturation() const
{
  return _saturation;
}

void StarfieldProceduralTexture::setSaturation(float value)
{
  _saturation = value;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
