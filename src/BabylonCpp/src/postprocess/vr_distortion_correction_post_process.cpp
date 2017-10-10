#include <babylon/postprocess/vr_distortion_correction_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

VRDistortionCorrectionPostProcess::VRDistortionCorrectionPostProcess(
  const string_t& iName, Camera* camera, bool isRightEye,
  const VRCameraMetrics& vrMetrics)
    : PostProcess{iName,
                  "vrDistortionCorrection",
                  {"LensCenter", "Scale", "ScaleIn", "HmdWarpParam"},
                  {},
                  vrMetrics.postProcessScaleFactor,
                  camera,
                  TextureConstants::BILINEAR_SAMPLINGMODE,
                  nullptr,
                  false}
    , _isRightEye{isRightEye}
    , _distortionFactors{vrMetrics.distortionK}
    , _postProcessScaleFactor{vrMetrics.postProcessScaleFactor}
    , _lensCenterOffset{vrMetrics.lensCenterOffset}
{
  adaptScaleToCurrentViewport = true;

  onSizeChangedObservable.add([&](PostProcess*, EventState&) {
    aspectRatio = static_cast<float>(width) * 0.5f / static_cast<float>(height);
    _scaleIn    = Vector2(2.f, 2.f / aspectRatio);
    _scaleFactor
      = Vector2(0.5f * (1.f / _postProcessScaleFactor),
                0.5f * (1.f / _postProcessScaleFactor) * aspectRatio);
    _lensCenter = Vector2(_isRightEye ? 0.5f - _lensCenterOffset * 0.5f :
                                        0.5f + _lensCenterOffset * 0.5f,
                          0.5f);
  });

  onApplyObservable.add([&](Effect* effect, EventState&) {
    effect->setFloat2("LensCenter", _lensCenter.x, _lensCenter.y);
    effect->setFloat2("Scale", _scaleFactor.x, _scaleFactor.y);
    effect->setFloat2("ScaleIn", _scaleIn.x, _scaleIn.y);
    effect->setFloat4("HmdWarpParam", _distortionFactors[0],
                      _distortionFactors[1], _distortionFactors[2],
                      _distortionFactors[3]);
  });
}

VRDistortionCorrectionPostProcess::~VRDistortionCorrectionPostProcess()
{
}

} // end of namespace BABYLON
