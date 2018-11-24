#include <babylon/materials/textures/procedurals/custom_procedural_texture.h>

#include <babylon/core/json_util.h>

#include <babylon/engine/scene.h>

namespace BABYLON {

CustomProceduralTexture::CustomProceduralTexture(const std::string& name,
                                                 const std::string& texturePath,
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

void CustomProceduralTexture::loadJson(const std::string& /*jsonUrl*/)
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
    for (auto& sampler2D : json_util::get_array<json>(_config, "sampler2Ds")) {
      const auto sample2Dname
        = json_util::get_string(sampler2D, "sample2Dname");
      const auto textureRelativeUrl
        = json_util::get_string(sampler2D, "textureRelativeUrl");
      setTexture(
        sample2Dname,
        Texture::New(_texturePath + "/" + textureRelativeUrl, getScene()));
    }
  }
}

void CustomProceduralTexture::updateShaderUniforms()
{
  if (_configSet) {
    for (auto& uniform : json_util::get_array<json>(_config, "uniforms")) {
      const auto uniformType = json_util::get_string(uniform, "type");
      const auto uniformName = json_util::get_string(uniform, "name");

      if (uniformType == "float") {
        setFloat(uniformName, json_util::get_number<float>(uniform, "value"));
      }
      else if (uniformType == "color3") {
        setColor3(uniformName,
                  Color3(json_util::get_number<float>(uniform, "r"),
                         json_util::get_number<float>(uniform, "g"),
                         json_util::get_number<float>(uniform, "b")));
      }
      else if (uniformType == "color4") {
        setColor4(uniformName,
                  Color4(json_util::get_number<float>(uniform, "r"),
                         json_util::get_number<float>(uniform, "g"),
                         json_util::get_number<float>(uniform, "b"),
                         json_util::get_number<float>(uniform, "a")));
      }
      else if (uniformType == "vector2") {
        setVector2(uniformName,
                   Vector2(json_util::get_number<float>(uniform, "x"),
                           json_util::get_number<float>(uniform, "y")));
      }
      else if (uniformType == "vector3") {
        setVector3(uniformName,
                   Vector3(json_util::get_number<float>(uniform, "x"),
                           json_util::get_number<float>(uniform, "y"),
                           json_util::get_number<float>(uniform, "z")));
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
