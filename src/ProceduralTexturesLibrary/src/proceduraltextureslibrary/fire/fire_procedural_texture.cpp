#include <babylon/proceduraltextureslibrary/fire/fire_procedural_texture.h>

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
  if (_autoGenerateTime) {
    _time += getScene()->getAnimationRatio() * 0.03f;
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

std::vector<Color3>& FireProceduralTexture::fireColors()
{
  return _fireColors;
}

void FireProceduralTexture::setFireColors(const std::vector<Color3>& value)
{
  _fireColors = value;
  updateShaderUniforms();
}

float FireProceduralTexture::time() const
{
  return _time;
}

void FireProceduralTexture::setTime(float value)
{
  _time = value;
  updateShaderUniforms();
}

Vector2& FireProceduralTexture::speed()
{
  return _speed;
}

void FireProceduralTexture::setSpeed(const Vector2& value)
{
  _speed = value;
  updateShaderUniforms();
}

float FireProceduralTexture::alphaThreshold() const
{
  return _alphaThreshold;
}

void FireProceduralTexture::setAlphaThreshold(float value)
{
  _alphaThreshold = value;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
