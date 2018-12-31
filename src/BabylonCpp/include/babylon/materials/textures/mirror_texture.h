#ifndef BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

class BlurPostProcess;
class ImageProcessingConfiguration;
class MirrorTexture;
using BlurPostProcessPtr = std::shared_ptr<BlurPostProcess>;
using MirrorTexturePtr   = std::shared_ptr<MirrorTexture>;

/**
 * @brief Mirror texture can be used to simulate the view from a mirror in a
 * scene. It will dynamically be rendered every frame to adapt to the camera
 * point of view. You can then easily use it as a reflectionTexture on a flat
 * surface. In case the surface is not a plane, please consider relying on
 * reflection probes.
 * @see https://doc.babylonjs.com/how_to/reflect#mirrors
 */
class BABYLON_SHARED_EXPORT MirrorTexture : public RenderTargetTexture {

public:
  template <typename... Ts>
  static MirrorTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<MirrorTexture>(
      new MirrorTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~MirrorTexture() override;

  /**
   * @brief Clone the mirror texture.
   * @returns the cloned texture
   */
  MirrorTexturePtr clone();

  /**
   * @brief Serialize the texture to a JSON representation you could use in
   * Parse later on
   * @returns the serialized JSON representation
   */
  json serialize() const;

  /**
   * @brief Dispose the texture and release its associated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Instantiates a Mirror Texture.
   * Mirror texture can be used to simulate the view from a mirror in a scene.
   * It will dynamically be rendered every frame to adapt to the camera point of
   * view. You can then easily use it as a reflectionTexture on a flat surface.
   * In case the surface is not a plane, please consider relying on reflection
   * probes.
   * @see https://doc.babylonjs.com/how_to/reflect#mirrors
   * @param name
   * @param size
   * @param scene
   * @param generateMipMaps
   * @param type
   * @param samplingMode
   * @param generateDepthBuffer
   */
  MirrorTexture(const std::string& name, const std::variant<ISize, float>& size,
                Scene* scene, bool generateMipMaps = false,
                unsigned int type = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
                unsigned int samplingMode
                = TextureConstants::BILINEAR_SAMPLINGMODE,
                bool generateDepthBuffer = true);

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
  /**
   * Define the reflection plane we want to use. The mirrorPlane is usually set
   * to the constructed reflector. It is possible to directly set the
   * mirrorPlane by directly using a BABYLON.Plane(a, b, c, d) where a, b and c
   * give the plane normal vector (a, b, c) and d is a scalar displacement from
   * the mirrorPlane to the origin. However in all but the very simplest of
   * situations it is more straight forward to set it to the reflector as stated
   * in the doc.
   * @see https://doc.babylonjs.com/how_to/reflect#mirrors
   */
  Plane mirrorPlane;

  /**
   * Define the blur ratio used to blur the reflection if needed.
   */
  Property<MirrorTexture, float> blurRatio;

  /**
   * Define the adaptive blur kernel used to blur the reflection if needed.
   * This will autocompute the closest best match for the `blurKernel`
   */
  WriteOnlyProperty<MirrorTexture, float> adaptiveBlurKernel;

  /**
   * Define the blur kernel used to blur the reflection if needed.
   * Please consider using `adaptiveBlurKernel` as it could find the closest
   * best value for you.
   */
  WriteOnlyProperty<MirrorTexture, float> blurKernel;

  /**
   * Define the blur kernel on the X Axis used to blur the reflection if needed.
   * Please consider using `adaptiveBlurKernel` as it could find the closest
   * best value for you.
   */
  Property<MirrorTexture, float> blurKernelX;

  /**
   * Define the blur kernel on the Y Axis used to blur the reflection if needed.
   * Please consider using `adaptiveBlurKernel` as it could find the closest
   * best value for you.
   */
  Property<MirrorTexture, float> blurKernelY;

private:
  Scene* scene;
  Matrix _transformMatrix;
  Matrix _mirrorMatrix;
  Matrix _savedViewMatrix;

  BlurPostProcessPtr _blurX;
  BlurPostProcessPtr _blurY;
  float _adaptiveBlurKernel;
  float _blurKernelX;
  float _blurKernelY;
  float _blurRatio;

  Observer<ImageProcessingConfiguration>::Ptr
    _imageProcessingConfigChangeObserver;

}; // end of class MirrorTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H
