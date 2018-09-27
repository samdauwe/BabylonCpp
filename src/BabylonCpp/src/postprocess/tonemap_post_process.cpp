#include <babylon/postprocess/tonemap_post_process.h>

#include <sstream>

#include <babylon/materials/effect.h>

#include <babylon/core/variant.h>

namespace BABYLON {

TonemapPostProcess::TonemapPostProcess(
  const std::string& iName, const TonemappingOperator& operator_,
  float exposureAdjustment, const CameraPtr& camera, unsigned int samplingMode,
  Engine* engine, unsigned int textureFormat)
    : PostProcess{iName,
                  "tonemap",
                  {"_ExposureAdjustment"},
                  {},
                  ToVariant<float, PostProcessOptions>(1.f),
                  camera,
                  samplingMode,
                  engine,
                  true,
                  "",
                  textureFormat}
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

  setOnApply([&](Effect* effect, EventState&) {
    effect->setFloat("_ExposureAdjustment", exposureAdjustment);
  });
}

TonemapPostProcess::~TonemapPostProcess()
{
}

} // end of namespace BABYLON
