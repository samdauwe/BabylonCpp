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
  MirrorTexture(const std::string& name, const ISize& size, Scene* scene,
                bool generateMipMaps = false,
                unsigned int type = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
                unsigned int samplingMode
                = TextureConstants::BILINEAR_SAMPLINGMODE,
                bool generateDepthBuffer = true);
  ~MirrorTexture();

  void setBlurRatio(float value);
  float blurRatio() const;
  void setBlurKernel(float value);
  void setBlurKernelX(float value);
  float blurKernelX() const;
  void setBlurKernelY(float value);
  float blurKernelY() const;

  std::unique_ptr<MirrorTexture> clone() const;
  Json::object serialize() const;

private:
  void _preparePostProcesses();

public:
  Plane mirrorPlane;

private:
  Matrix _transformMatrix;
  Matrix _mirrorMatrix;
  Matrix _savedViewMatrix;

  std::unique_ptr<BlurPostProcess> _blurX;
  std::unique_ptr<BlurPostProcess> _blurY;
  float _blurKernelX;
  float _blurKernelY;
  float _blurRatio;

}; // end of class MirrorTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H
