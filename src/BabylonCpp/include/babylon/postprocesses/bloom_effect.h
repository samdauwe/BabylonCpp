#ifndef BABYLON_POSTPROCESSES_BLOOM_EFFECT_H
#define BABYLON_POSTPROCESSES_BLOOM_EFFECT_H

#include <babylon/babylon_api.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_effect.h>

namespace BABYLON {

class BloomEffect;
class BloomMergePostProcess;
class BlurPostProcess;
class ExtractHighlightsPostProcess;
class Scene;
using BloomEffectPtr                  = std::shared_ptr<BloomEffect>;
using BloomMergePostProcessPtr        = std::shared_ptr<BloomMergePostProcess>;
using BlurPostProcessPtr              = std::shared_ptr<BlurPostProcess>;
using ExtractHighlightsPostProcessPtr = std::shared_ptr<ExtractHighlightsPostProcess>;

/**
 * @brief The bloom effect spreads bright areas of an image to simulate
 * artifacts seen in cameras.
 */
class BABYLON_SHARED_EXPORT BloomEffect : public PostProcessRenderEffect {

public:
  template <typename... Ts>
  static BloomEffectPtr New(Ts&&... args)
  {
    return std::shared_ptr<BloomEffect>(new BloomEffect(std::forward<Ts>(args)...));
  }
  virtual ~BloomEffect(); // = default

  /**
   * @brief Disposes each of the internal effects for a given camera.
   * @param camera The camera to dispose the effect on.
   */
  void disposeEffects(Camera* camera);

  /**
   * @brief Hidden Internal
   */
  void _updateEffects();

  /**
   * @brief Internal
   * @returns if all the contained post processes are ready.
   * Hidden
   */
  [[nodiscard]] bool _isReady() const;

protected:
  /**
   * @brief Creates a new instance of @see BloomEffect.
   * @param scene The scene the effect belongs to.
   * @param bloomScale The ratio of the blur texture to the input texture that
   * should be used to compute the bloom.
   * @param bloomKernel The size of the kernel to be used when applying the
   * blur.
   * @param bloomWeight The the strength of bloom.
   * @param pipelineTextureType The type of texture to be used when performing
   * the post processing.
   * @param blockCompilation If compilation of the shader should not be done in
   * the constructor. The updateEffect method can be used to compile the shader
   * at a later time. (default: false)
   */
  BloomEffect(const std::string& name, Scene* scene, float bloomScale, float bloomWeight,
              float bloomKernel, unsigned int pipelineTextureType = 0,
              bool blockCompilation = false);

  /**
   * @brief Gets the luminance threshold to find bright areas of the image to
   * bloom.
   */
  [[nodiscard]] float get_threshold() const;

  /**
   * @brief Sets the luminance threshold to find bright areas of the image to
   * bloom.
   */
  void set_threshold(float value);

  /**
   * @brief The strength of the bloom.
   */
  [[nodiscard]] float get_weight() const;

  /**
   * @brief The strength of the bloom.
   */
  void set_weight(float value);

  /**
   * @brief Specifies the size of the bloom blur kernel, relative to the final
   * output size.
   */
  [[nodiscard]] float get_kernel() const;

  /**
   * @brief Specifies the size of the bloom blur kernel, relative to the final
   * output size.
   */
  void set_kernel(float value);

public:
  float bloomWeight;
  float bloomKernel;
  unsigned int pipelineTextureType;

  /**
   * Hidden Internal
   */
  std::vector<PostProcessPtr> _effects;

  /**
   * Hidden Internal
   */
  ExtractHighlightsPostProcessPtr _downscale;

  /**
   * The luminance threshold to find bright areas of the image to bloom.
   */
  Property<BloomEffect, float> threshold;

  /**
   * The strength of the bloom.
   */
  Property<BloomEffect, float> weight;

  /**
   * Size of the bloom blur kernel, relative to the final output size
   */
  Property<BloomEffect, float> kernel;

private:
  float bloomScale;
  BlurPostProcessPtr _blurX;
  BlurPostProcessPtr _blurY;
  BloomMergePostProcessPtr _merge;

}; // end of class ChromaticAberrationPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_BLOOM_EFFECT_H
