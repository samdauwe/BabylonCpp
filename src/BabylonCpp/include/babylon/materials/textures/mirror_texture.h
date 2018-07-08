#ifndef BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MirrorTexture : public RenderTargetTexture {

public:
  MirrorTexture(const string_t& name, const ISize& size, Scene* scene,
                bool generateMipMaps = false,
                unsigned int type = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
                unsigned int samplingMode
                = TextureConstants::BILINEAR_SAMPLINGMODE,
                bool generateDepthBuffer = true);
  ~MirrorTexture() override;

  unique_ptr_t<MirrorTexture> clone();
  Json::object serialize() const;
  void dispose() override;

protected:
  void set_blurRatio(float value);
  float get_blurRatio() const;
  void set_adaptiveBlurKernel(float value);
  void set_blurKernel(float value);
  void set_blurKernelX(float value);
  float get_blurKernelX() const;
  void set_blurKernelY(float value);
  float get_blurKernelY() const;

  void _onRatioRescale();

private:
  void _autoComputeBlurKernel();
  void _updateGammaSpace();
  void _preparePostProcesses();

public:
  Plane mirrorPlane;

  Property<MirrorTexture, float> blurRatio;
  WriteOnlyProperty<MirrorTexture, float> adaptiveBlurKernel;
  WriteOnlyProperty<MirrorTexture, float> blurKernel;
  Property<MirrorTexture, float> blurKernelX;
  Property<MirrorTexture, float> blurKernelY;

private:
  Scene* scene;
  Matrix _transformMatrix;
  Matrix _mirrorMatrix;
  Matrix _savedViewMatrix;

  unique_ptr_t<BlurPostProcess> _blurX;
  unique_ptr_t<BlurPostProcess> _blurY;
  float _adaptiveBlurKernel;
  float _blurKernelX;
  float _blurKernelY;
  float _blurRatio;

  Observer<ImageProcessingConfiguration>::Ptr
    _imageProcessingConfigChangeObserver;

}; // end of class MirrorTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H
