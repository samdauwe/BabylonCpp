#include <babylon/proceduraltextureslibrary/fire/fire_procedural_texture.h>

#include <babylon/core/json.h>
#include <babylon/engine/scene.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

FireProceduralTexture::FireProceduralTexture(const std::string& iName,
                                             const Size& size, Scene* scene,
                                             Texture* fallbackTexture,
                                             bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "fireProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , autoGenerateTime{this, &FireProceduralTexture::get_autoGenerateTime,
                       &FireProceduralTexture::set_autoGenerateTime}
    , fireColors{this, &FireProceduralTexture::get_fireColors,
                 &FireProceduralTexture::set_fireColors}
    , time{this, &FireProceduralTexture::get_time,
           &FireProceduralTexture::set_time}
    , speed{this, &FireProceduralTexture::get_speed,
            &FireProceduralTexture::set_speed}
    , alphaThreshold{this, &FireProceduralTexture::get_alphaThreshold,
                     &FireProceduralTexture::set_alphaThreshold}
    , _time{0.f}
    , _speed{Vector2(0.5f, 0.3f)}
    , _autoGenerateTime{true}
    , _alphaThreshold{0.5f}
{
  _fireColors = FireProceduralTexture::RedFireColors();
  updateShaderUniforms();
}

FireProceduralTexture::~FireProceduralTexture()
{
}

void FireProceduralTexture::updateShaderUniforms()
{
  setFloat("time", _time);
  setVector2("speed", _speed);
  setColor3("c1", _fireColors[0]);
  setColor3("c2", _fireColors[1]);
  setColor3("c3", _fireColors[2]);
  setColor3("c4", _fireColors[3]);
  setColor3("c5", _fireColors[4]);
  setColor3("c6", _fireColors[5]);
  setFloat("alphaThreshold", _alphaThreshold);
}

void FireProceduralTexture::render(bool useCameraPostProcess)
{
  auto scene = getScene();
  if (_autoGenerateTime && scene) {
    _time += scene->getAnimationRatio() * 0.03f;
    updateShaderUniforms();
  }
  ProceduralTexture::render(useCameraPostProcess);
}

std::vector<Color3> FireProceduralTexture::PurpleFireColors()
{
  return {Color3(0.5f, 0.0f, 1.f), //
          Color3(0.9f, 0.0f, 1.f), //
          Color3(0.2f, 0.0f, 1.f), //
          Color3(1.0f, 0.9f, 1.f), //
          Color3(0.1f, 0.1f, 1.f), //
          Color3(0.9f, 0.9f, 1.f)};
}

std::vector<Color3> FireProceduralTexture::GreenFireColors()
{
  return {Color3(0.5f, 1.0f, 0.f), //
          Color3(0.5f, 1.0f, 0.f), //
          Color3(0.3f, 0.4f, 0.f), //
          Color3(0.5f, 1.0f, 0.f), //
          Color3(0.2f, 0.0f, 0.f), //
          Color3(0.5f, 1.0f, 0.f)};
}

std::vector<Color3> FireProceduralTexture::RedFireColors()
{
  return {Color3(0.5f, 0.0f, 0.1f), //
          Color3(0.9f, 0.0f, 0.0f), //
          Color3(0.2f, 0.0f, 0.0f), //
          Color3(1.0f, 0.9f, 0.0f), //
          Color3(0.1f, 0.1f, 0.1f), //
          Color3(0.9f, 0.9f, 0.9f)};
}

std::vector<Color3> FireProceduralTexture::BlueFireColors()
{
  return {Color3(0.1f, 0.0f, 0.5f), //
          Color3(0.0f, 0.0f, 0.5f), //
          Color3(0.1f, 0.0f, 0.2f), //
          Color3(0.0f, 0.0f, 1.0f), //
          Color3(0.1f, 0.2f, 0.3f), //
          Color3(0.0f, 0.2f, 0.9f)};
}

bool FireProceduralTexture::get_autoGenerateTime() const
{
  return _autoGenerateTime;
}

void FireProceduralTexture::set_autoGenerateTime(bool value)
{
  _autoGenerateTime = value;
}

std::vector<Color3>& FireProceduralTexture::get_fireColors()
{
  return _fireColors;
}

void FireProceduralTexture::set_fireColors(const std::vector<Color3>& value)
{
  _fireColors = value;
  updateShaderUniforms();
}

float FireProceduralTexture::get_time() const
{
  return _time;
}

void FireProceduralTexture::set_time(float value)
{
  _time = value;
  updateShaderUniforms();
}

Vector2& FireProceduralTexture::get_speed()
{
  return _speed;
}

void FireProceduralTexture::set_speed(const Vector2& value)
{
  _speed = value;
  updateShaderUniforms();
}

float FireProceduralTexture::get_alphaThreshold() const
{
  return _alphaThreshold;
}

void FireProceduralTexture::set_alphaThreshold(float value)
{
  _alphaThreshold = value;
  updateShaderUniforms();
}

Json::object FireProceduralTexture::serialize() const
{
  return Json::object();
}

unique_ptr_t<FireProceduralTexture>
FireProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                             Scene* /*scene*/, const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
