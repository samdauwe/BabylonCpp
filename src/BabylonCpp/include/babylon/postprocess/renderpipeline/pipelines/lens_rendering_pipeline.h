#ifndef BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_LENS_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_LENS_RENDERING_PIPLINE_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

struct LensRenderingPipelineParameters {
  Texture* grain_texture;
  float edge_blur            = 0;
  float grain_amount         = 0;
  float chromatic_aberration = 0;
  float distortion           = 0;
  float dof_gain             = -1;
  float dof_threshold        = 1;
  float dof_focus_distance   = -1;
  float dof_aperture         = 1;
  float dof_darken           = 0;
  bool dof_pentagon          = true;
  bool blur_noise            = true;
}; // end of struct LensRenderingPipelineParameters

/**
 * Chromatic Aberration GLSL Shader
 * Separates very slightly R, G and B colors on the edges of the screen
 * Inspired by Francois Tarlier & Martins Upitis
 *
 * Lens effects can be of the following:
 * - chromatic aberration (slight shift of RGB colors)
 * - blur on the edge of the lens
 * - lens distortion
 * - depth-of-field blur & highlights enhancing
 * - depth-of-field 'bokeh' effect (shapes appearing in blurred areas)
 * - grain effect (noise or custom texture)
 *
 * Two additional texture samplers are needed:
 * - depth map (for depth-of-field)
 * - grain texture
 */
class BABYLON_SHARED_EXPORT LensRenderingPipeline
    : public PostProcessRenderPipeline {

public:
  /**
   * The chromatic aberration PostProcess id in the pipeline
   */
  static constexpr const char* LensChromaticAberrationEffect
    = "LensChromaticAberrationEffect";
  /**
   * The highlights enhancing PostProcess id in the pipeline
   */
  static constexpr const char* HighlightsEnhancingEffect
    = "HighlightsEnhancingEffect";
  /**
   * The depth-of-field PostProcess id in the pipeline
   */
  static constexpr const char* LensDepthOfFieldEffect
    = "LensDepthOfFieldEffect";

public:
  /**
   * Constructor
   *
   * Effect parameters are as follow:
   * {
   *      chromatic_aberration: number;       // from 0 to x (1 for realism)
   *      edge_blur: number;                  // from 0 to x (1 for realism)
   *      distortion: number;                 // from 0 to x (1 for realism)
   *      grain_float amount ;                // from 0 to 1
   *      grain_texture: BABYLON.Texture;     // texture to use for grain
   *                                          // effect; if unset, use random
   *                                          // B&W noise
   *      dof_focus_distance: number;         // depth-of-field: focus distance;
   *                                          // unset to disable (disabled by
   *                                          // default)
   *      dof_aperture: number;               // depth-of-field: focus blur bias
   *                                          // (default: 1)
   *      dof_darken: number;                 // depth-of-field: darken that
   *                                          // which is out of focus (from 0
   *                                          // to 1, disabled by default)
   *      dof_pentagon: boolean;              // depth-of-field: makes a
   *                                          // pentagon-like "bokeh" effect
   *      dof_gain: number;                   // depth-of-field: highlights
   *                                          // gain; unset to disable
   *                                          // (disabled by default)
   *      dof_threshold: number;              // depth-of-field: highlights
   *                                          // threshold (default: 1)
   *      blur_noise: boolean;                // add a little bit of noise to
   *                                          // the blur (default: true)
   * }
   * Note: if an effect parameter is unset, effect is disabled
   *
   * @param {string} name - The rendering pipeline name
   * @param {object} parameters - An object containing all parameters (see
   * above)
   * @param {BABYLON.Scene} scene - The scene linked to this pipeline
   * @param {number} ratio - The size of the postprocesses (0.5 means that your
   * postprocess will have a width = canvas.width 0.5 and a height =
   * canvas.height 0.5)
   * @param {BABYLON.Camera[]} cameras - The array of cameras that the rendering
   * pipeline will be attached to
   */
  LensRenderingPipeline(const string_t& name,
                        const LensRenderingPipelineParameters& parameters,
                        Scene* scene, float ratio = 1.f,
                        const vector_t<Camera*>& cameras = vector_t<Camera*>());
  virtual ~LensRenderingPipeline();

  /** Methods **/
  void setEdgeBlur(float amount);
  void disableEdgeBlur();
  void setGrainAmount(float amount);
  void disableGrain();
  void setChromaticAberration(float amount);
  void disableChromaticAberration();
  void setEdgeDistortion(float amount);
  void disableEdgeDistortion();
  void setFocusDistance(float amount);
  void disableDepthOfField();
  void setAperture(float amount);
  void setDarkenOutOfFocus(float amount);
  void enablePentagonBokeh();
  void disablePentagonBokeh();
  void enableNoiseBlur();
  void disableNoiseBlur();
  void setHighlightsGain(float amount);
  void setHighlightsThreshold(float amount);
  void disableHighlights();

  /**
   * Removes the internal pipeline assets and detaches the pipeline from the
   * scene cameras
   */
  void dispose(bool disableDepthRender         = false,
               bool disposeMaterialAndTextures = false) override;

private:
  /**
   * colors shifting and distortion
   */
  void _createChromaticAberrationPostProcess(float ratio);
  /**
   * highlights enhancing
   */
  void _createHighlightsPostProcess(float ratio);
  /**
   * colors shifting and distortion
   */
  void _createDepthOfFieldPostProcess(float ratio);
  /**
   * creates a black and white random noise texture, 512x512
   */
  void _createGrainTexture();

private:
  Scene* _scene;
  RenderTargetTexture* _depthTexture;
  Texture* _grainTexture;

  PostProcess* _chromaticAberrationPostProcess;
  PostProcess* _highlightsPostProcess;
  PostProcess* _depthOfFieldPostProcess;

  float _edgeBlur;
  float _grainAmount;
  float _chromaticAberration;
  float _distortion;
  float _highlightsGain;
  float _highlightsThreshold;
  float _dofDistance;
  float _dofAperture;
  float _dofDarken;
  bool _dofPentagon;
  bool _blurNoise;

}; // end of class LensRenderingPipeline

} // end of namespace BABYLON

#endif // end of
       // BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_LENS_RENDERING_PIPLINE_H
