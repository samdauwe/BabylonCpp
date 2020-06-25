#include <babylon/materials/effect_wrapper.h>

#include <babylon/materials/effect.h>
#include <babylon/materials/effect_wrapper_creation_options.h>
#include <babylon/materials/ieffect_creation_options.h>

namespace BABYLON {

EffectWrapper::EffectWrapper(const EffectWrapperCreationOptions& creationOptions)
{
  std::unordered_map<std::string, std::string> effectCreationOptions{};
  auto uniformNames = creationOptions.uniformNames;
  if (!creationOptions.vertexShader.empty()) {
    effectCreationOptions["fragmentSource"] = creationOptions.fragmentShader;
    effectCreationOptions["vertexSource"]   = creationOptions.vertexShader;
    effectCreationOptions["spectorName"]
      = !creationOptions.name.empty() ? creationOptions.name : "effectWrapper";
  }
  else {
    // Default scale to use in post process vertex shader.
    uniformNames.emplace_back("scale");

    effectCreationOptions["fragmentSource"] = creationOptions.fragmentShader;
    effectCreationOptions["vertexSource"]   = "postprocess";
    effectCreationOptions["spectorName"]
      = !creationOptions.name.empty() ? creationOptions.name : "effectWrapper";

    // Sets the default scale to identity for the post process vertex shader.
    onApplyObservable.add(
      [this](void*, EventState & /*es*/) -> void { effect->setFloat2("scale", 1.f, 1.f); });
  }

  const std::vector<std::string> fallbackAttributeNames = {"position"};

  IEffectCreationOptions options;
  options.attributes = !creationOptions.attributeNames.empty() ? creationOptions.attributeNames :
                                                                 fallbackAttributeNames;
  options.uniformsNames = uniformNames;
  options.samplers      = creationOptions.samplerNames;

  effect = Effect ::New(effectCreationOptions, options, creationOptions.engine);
}

EffectWrapper::~EffectWrapper() = default;

void EffectWrapper::dispose()
{
  effect->dispose();
}

} // end of namespace BABYLON
