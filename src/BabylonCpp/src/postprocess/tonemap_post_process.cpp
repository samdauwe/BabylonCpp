#include <babylon/postprocess/tonemap_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

TonemapPostProcess::TonemapPostProcess(const std::string& _name,
                                       const TonemappingOperator& operator_,
                                       float exposureAdjustment, Camera* camera,
                                       unsigned int samplingMode,
                                       Engine* engine,
                                       unsigned int textureFormat)
    : PostProcess{_name,        "tonemap",    {"_ExposureAdjustment"},
                  {},           1.f,          camera,
                  samplingMode, engine,       true,
                  "",           textureFormat}
    , _operator{operator_}
{
  std::ostringstream defines;
  defines << "#define ";

  if (_operator == TonemappingOperator::HABLE) {
    defines << "HABLE_TONEMAPPING";
  }
  else if (_operator == TonemappingOperator::REINHARD) {
    defines << "REINHARD_TONEMAPPING";
  }
  else if (_operator == TonemappingOperator::HEJIDAWSON) {
    defines << "OPTIMIZED_HEJIDAWSON_TONEMAPPING";
  }
  else if (_operator == TonemappingOperator::PHOTOGRAPHIC) {
    defines << "PHOTOGRAPHIC_TONEMAPPING";
  }

  // Sadly a second call to create the effect.
  updateEffect(defines.str());

  setOnApply([&](Effect* effect) {
    effect->setFloat("_ExposureAdjustment", exposureAdjustment);
  });
}

TonemapPostProcess::~TonemapPostProcess()
{
}

} // end of namespace BABYLON
