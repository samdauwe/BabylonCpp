#include <babylon/materials/effect_shaders_store.h>

#include <babylon/shaders/anaglyph_fragment_fx.h>
#include <babylon/shaders/background_fragment_fx.h>
#include <babylon/shaders/background_vertex_fx.h>
#include <babylon/shaders/black_and_white_fragment_fx.h>
#include <babylon/shaders/bloom_merge_fragment_fx.h>
#include <babylon/shaders/blur_fragment_fx.h>
#include <babylon/shaders/chromatic_aberration_fragment_fx.h>
#include <babylon/shaders/circle_of_confusion_fragment_fx.h>
#include <babylon/shaders/color_fragment_fx.h>
#include <babylon/shaders/color_vertex_fx.h>
#include <babylon/shaders/color_correction_fragment_fx.h>
#include <babylon/shaders/convolution_fragment_fx.h>
#include <babylon/shaders/default_fragment_fx.h>
#include <babylon/shaders/default_vertex_fx.h>
#include <babylon/shaders/depth_fragment_fx.h>
#include <babylon/shaders/depth_vertex_fx.h>
#include <babylon/shaders/depth_box_blur_fragment_fx.h>
#include <babylon/shaders/depth_of_field_fragment_fx.h>
#include <babylon/shaders/depth_of_field_merge_fragment_fx.h>
#include <babylon/shaders/display_pass_fragment_fx.h>
#include <babylon/shaders/extract_highlights_fragment_fx.h>
#include <babylon/shaders/filter_fragment_fx.h>
#include <babylon/shaders/fxaa_fragment_fx.h>
#include <babylon/shaders/fxaa_vertex_fx.h>
#include <babylon/shaders/geometry_fragment_fx.h>
#include <babylon/shaders/geometry_vertex_fx.h>
#include <babylon/shaders/glow_blur_post_process_fragment_fx.h>
#include <babylon/shaders/glow_map_generation_fragment_fx.h>
#include <babylon/shaders/glow_map_generation_vertex_fx.h>
#include <babylon/shaders/glow_map_merge_fragment_fx.h>
#include <babylon/shaders/glow_map_merge_vertex_fx.h>
#include <babylon/shaders/gpu_render_particles_fragment_fx.h>
#include <babylon/shaders/gpu_render_particles_vertex_fx.h>
#include <babylon/shaders/gpu_update_particles_fragment_fx.h>
#include <babylon/shaders/gpu_update_particles_vertex_fx.h>
#include <babylon/shaders/grain_fragment_fx.h>
#include <babylon/shaders/highlights_fragment_fx.h>
#include <babylon/shaders/image_processing_fragment_fx.h>
#include <babylon/shaders/kernel_blur_fragment_fx.h>
#include <babylon/shaders/kernel_blur_vertex_fx.h>
#include <babylon/shaders/layer_fragment_fx.h>
#include <babylon/shaders/layer_vertex_fx.h>
#include <babylon/shaders/lens_flare_fragment_fx.h>
#include <babylon/shaders/lens_flare_vertex_fx.h>
#include <babylon/shaders/lens_highlights_fragment_fx.h>
#include <babylon/shaders/line_fragment_fx.h>
#include <babylon/shaders/line_vertex_fx.h>
#include <babylon/shaders/noise_fragment_fx.h>
#include <babylon/shaders/outline_fragment_fx.h>
#include <babylon/shaders/outline_vertex_fx.h>
#include <babylon/shaders/particles_fragment_fx.h>
#include <babylon/shaders/particles_vertex_fx.h>
#include <babylon/shaders/pass_fragment_fx.h>
#include <babylon/shaders/pbr_fragment_fx.h>
#include <babylon/shaders/pbr_vertex_fx.h>
#include <babylon/shaders/postprocess_vertex_fx.h>
#include <babylon/shaders/procedural_vertex_fx.h>
#include <babylon/shaders/refraction_fragment_fx.h>
#include <babylon/shaders/rgbd_decode_fragment_fx.h>
#include <babylon/shaders/rgbd_encode_fragment_fx.h>
#include <babylon/shaders/shadow_map_fragment_fx.h>
#include <babylon/shaders/shadow_map_vertex_fx.h>
#include <babylon/shaders/sharpen_fragment_fx.h>
#include <babylon/shaders/sprites_fragment_fx.h>
#include <babylon/shaders/sprites_vertex_fx.h>
#include <babylon/shaders/ssao_fragment_fx.h>
#include <babylon/shaders/ssao2_fragment_fx.h>
#include <babylon/shaders/ssao_combine_fragment_fx.h>
#include <babylon/shaders/standard_fragment_fx.h>
#include <babylon/shaders/stereoscopic_interlace_fragment_fx.h>
#include <babylon/shaders/tonemap_fragment_fx.h>
#include <babylon/shaders/volumetric_light_scattering_fragment_fx.h>
#include <babylon/shaders/volumetric_light_scattering_pass_fragment_fx.h>
#include <babylon/shaders/vr_distortion_correction_fragment_fx.h>

namespace BABYLON {

EffectShadersStore::EffectShadersStore()
{
}

EffectShadersStore::~EffectShadersStore()
{
}

unordered_map_t<string_t, string_t>& EffectShadersStore::shaders()
{
  return _shaders;
}

const unordered_map_t<string_t, string_t>&
EffectShadersStore::shaders() const
{
  return _shaders;
}

unordered_map_t<string_t, string_t> EffectShadersStore::_shaders
  = {{"anaglyphPixelShader", anaglyphPixelShader},
     {"backgroundPixelShader", backgroundPixelShader},
     {"backgroundVertexShader", backgroundVertexShader},
     {"blackAndWhitePixelShader", blackAndWhitePixelShader},
     {"bloomMergePixelShader", bloomMergePixelShader},
     {"blurPixelShader", blurPixelShader},
     {"chromaticAberrationPixelShader", chromaticAberrationPixelShader},
     {"circleOfConfusionPixelShader", circleOfConfusionPixelShader},
     {"colorPixelShader", colorPixelShader},
     {"colorVertexShader", colorVertexShader},
     {"colorCorrectionPixelShader", colorCorrectionPixelShader},
     {"convolutionPixelShader", convolutionPixelShader},
     {"defaultPixelShader", defaultPixelShader},
     {"defaultVertexShader", defaultVertexShader},
     {"depthPixelShader", depthPixelShader},
     {"depthVertexShader", depthVertexShader},
     {"depthBoxBlurPixelShader", depthBoxBlurPixelShader},
     {"depthOfFieldPixelShader", depthOfFieldPixelShader},
     {"depthOfFieldMergePixelShader", depthOfFieldMergePixelShader},
     {"displayPassPixelShader", displayPassPixelShader},
     {"extractHighlightsPixelShader", extractHighlightsPixelShader},
     {"filterPixelShader", filterPixelShader},
     {"fxaaPixelShader", fxaaPixelShader},
     {"fxaaVertexShader", fxaaVertexShader},
     {"geometryPixelShader", geometryPixelShader},
     {"geometryVertexShader", geometryVertexShader},
     {"glowBlurPostProcessPixelShader", glowBlurPostProcessPixelShader},
     {"glowMapGenerationPixelShader", glowMapGenerationPixelShader},
     {"glowMapGenerationVertexShader", glowMapGenerationVertexShader},
     {"glowMapMergePixelShader", glowMapMergePixelShader},
     {"glowMapMergeVertexShader", glowMapMergeVertexShader},
     {"gpuRenderParticlesPixelShader", gpuRenderParticlesPixelShader},
     {"gpuRenderParticlesVertexShader", gpuRenderParticlesVertexShader},
     {"gpuUpdateParticlesPixelShader", gpuUpdateParticlesPixelShader},
     {"gpuUpdateParticlesVertexShader", gpuUpdateParticlesVertexShader},
     {"grainPixelShader", grainPixelShader},
     {"highlightsPixelShader", highlightsPixelShader},
     {"imageProcessingPixelShader", imageProcessingPixelShader},
     {"kernelBlurPixelShader", kernelBlurPixelShader},
     {"kernelBlurVertexShader", kernelBlurVertexShader},
     {"layerPixelShader", layerPixelShader},
     {"layerVertexShader", layerVertexShader},
     {"lensFlarePixelShader", lensFlarePixelShader},
     {"lensFlareVertexShader", lensFlareVertexShader},
     {"lensHighlightsPixelShader", lensHighlightsPixelShader},
     {"linePixelShader", linePixelShader},
     {"lineVertexShader", lineVertexShader},
     {"noisePixelShader", noisePixelShader},
     {"outlinePixelShader", outlinePixelShader},
     {"outlineVertexShader", outlineVertexShader},
     {"particlesPixelShader", particlesPixelShader},
     {"particlesVertexShader", particlesVertexShader},
     {"passPixelShader", passPixelShader},
     {"pbrPixelShader", pbrPixelShader},
     {"pbrVertexShader", pbrVertexShader},
     {"postprocessVertexShader", postprocessVertexShader},
     {"proceduralVertexShader", proceduralVertexShader},
     {"refractionPixelShader", refractionPixelShader},
     {"rgbdDecodePixelShader", rgbdDecodePixelShader},
     {"rgbdEncodePixelShader", rgbdEncodePixelShader},
     {"shadowMapPixelShader", shadowMapPixelShader},
     {"shadowMapVertexShader", shadowMapVertexShader},
     {"sharpenPixelShader", sharpenPixelShader},
     {"spritesPixelShader", spritesPixelShader},
     {"spritesVertexShader", spritesVertexShader},
     {"ssaoPixelShader", ssaoPixelShader},
     {"ssao2PixelShader", ssao2PixelShader},
     {"ssaoCombinePixelShader", ssaoCombinePixelShader},
     {"standardPixelShader", standardPixelShader},
     {"stereoscopicInterlacePixelShader", stereoscopicInterlacePixelShader},
     {"tonemapPixelShader", tonemapPixelShader},
     {"volumetricLightScatteringPixelShader",
      volumetricLightScatteringPixelShader},
     {"volumetricLightScatteringPassPixelShader",
      volumetricLightScatteringPassPixelShader},
     {"vrDistortionCorrectionPixelShader", vrDistortionCorrectionPixelShader}};

} // end of namespace BABYLON
