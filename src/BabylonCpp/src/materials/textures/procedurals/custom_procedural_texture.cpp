#include <babylon/materials/textures/procedurals/custom_procedural_texture.h>

#include <babylon/engine/scene.h>

namespace BABYLON {

CustomProceduralTexture::CustomProceduralTexture(const string_t& name,
                                                 const string_t& texturePath,
                                                 int size, Scene* scene,
                                                 Texture* fallbackTexture,
                                                 bool generateMipMaps)
    : ProceduralTexture{name,           size, nullptr, scene, fallbackTexture,
                        generateMipMaps}
    , _animate{true}
    , _time{0.f}
{
  _texturePath = texturePath;

  // Try to load json
  _configSet = false;
  loadJson(texturePath);
  set_refreshRate(1);
}

CustomProceduralTexture::~CustomProceduralTexture()
{
}

void CustomProceduralTexture::loadJson(const string_t& /*jsonUrl*/)
{
}

bool CustomProceduralTexture::isReady()
{
  if (!ProceduralTexture::isReady()) {
    return false;
  }

  for (auto& item : _textures) {
    auto& texture = item.second;

    if (!texture->isReady()) {
      return false;
    }
  }

  return true;
}

void CustomProceduralTexture::render(bool useCameraPostProcess)
{
  auto scene = getScene();
  if (_animate && scene) {
    _time += scene->getAnimationRatio() * 0.03f;
    updateShaderUniforms();
  }

  ProceduralTexture::render(useCameraPostProcess);
}

void CustomProceduralTexture::updateTextures()
{
  if (_configSet) {
    for (auto& sampler2D : Json::GetArray(_config, "sampler2Ds")) {
      const auto sample2Dname = Json::GetString(sampler2D, "sample2Dname");
      const auto textureRelativeUrl
        = Json::GetString(sampler2D, "textureRelativeUrl");
      setTexture(
        sample2Dname,
        Texture::New(_texturePath + "/" + textureRelativeUrl, getScene()));
    }
  }
}

void CustomProceduralTexture::updateShaderUniforms()
{
  if (_configSet) {
    for (auto& uniform : Json::GetArray(_config, "uniforms")) {
      const auto uniformType = Json::GetString(uniform, "type");
      const auto uniformName = Json::GetString(uniform, "name");

      if (uniformType == "float") {
        setFloat(uniformName, Json::GetNumber<float>(uniform, "value", 0.f));
      }
      else if (uniformType == "color3") {
        setColor3(uniformName,
                  Color3(Json::GetNumber<float>(uniform, "r", 0.f),
                         Json::GetNumber<float>(uniform, "g", 0.f),
                         Json::GetNumber<float>(uniform, "b", 0.f)));
      }
      else if (uniformType == "color4") {
        setColor4(uniformName,
                  Color4(Json::GetNumber<float>(uniform, "r", 0.f),
                         Json::GetNumber<float>(uniform, "g", 0.f),
                         Json::GetNumber<float>(uniform, "b", 0.f),
                         Json::GetNumber<float>(uniform, "a", 0.f)));
      }
      else if (uniformType == "vector2") {
        setVector2(uniformName,
                   Vector2(Json::GetNumber<float>(uniform, "x", 0.f),
                           Json::GetNumber<float>(uniform, "y", 0.f)));
      }
      else if (uniformType == "vector3") {
        setVector3(uniformName,
                   Vector3(Json::GetNumber<float>(uniform, "x", 0.f),
                           Json::GetNumber<float>(uniform, "y", 0.f),
                           Json::GetNumber<float>(uniform, "z", 0.f)));
      }
    }
  }

  setFloat("time", _time);
}

bool CustomProceduralTexture::animate() const
{
  return _animate;
}

void CustomProceduralTexture::setAnimate(bool value)
{
  _animate = value;
}

} // end of namespace BABYLON
