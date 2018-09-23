#include <babylon/materials/effect_includes_shaders_store.h>

#include <babylon/shaders/shadersinclude/background_fragment_declaration_fx.h>
#include <babylon/shaders/shadersinclude/background_ubo_declaration_fx.h>
#include <babylon/shaders/shadersinclude/background_vertex_declaration_fx.h>
#include <babylon/shaders/shadersinclude/bones300_declaration_fx.h>
#include <babylon/shaders/shadersinclude/bones_declaration_fx.h>
#include <babylon/shaders/shadersinclude/bones_vertex_fx.h>
#include <babylon/shaders/shadersinclude/bump_fragment_functions_fx.h>
#include <babylon/shaders/shadersinclude/bump_fragment_fx.h>
#include <babylon/shaders/shadersinclude/bump_vertex_declaration_fx.h>
#include <babylon/shaders/shadersinclude/bump_vertex_fx.h>
#include <babylon/shaders/shadersinclude/clip_plane_fragment_declaration2_fx.h>
#include <babylon/shaders/shadersinclude/clip_plane_fragment_declaration_fx.h>
#include <babylon/shaders/shadersinclude/clip_plane_fragment_fx.h>
#include <babylon/shaders/shadersinclude/clip_plane_vertex_declaration2_fx.h>
#include <babylon/shaders/shadersinclude/clip_plane_vertex_declaration_fx.h>
#include <babylon/shaders/shadersinclude/clip_plane_vertex_fx.h>
#include <babylon/shaders/shadersinclude/default_fragment_declaration_fx.h>
#include <babylon/shaders/shadersinclude/default_ubo_declaration_fx.h>
#include <babylon/shaders/shadersinclude/default_vertex_declaration_fx.h>
#include <babylon/shaders/shadersinclude/depth_pre_pass_fx.h>
#include <babylon/shaders/shadersinclude/fog_fragment_declaration_fx.h>
#include <babylon/shaders/shadersinclude/fog_fragment_fx.h>
#include <babylon/shaders/shadersinclude/fog_vertex_declaration_fx.h>
#include <babylon/shaders/shadersinclude/fog_vertex_fx.h>
#include <babylon/shaders/shadersinclude/fresnel_function_fx.h>
#include <babylon/shaders/shadersinclude/harmonics_functions_fx.h>
#include <babylon/shaders/shadersinclude/helper_functions_fx.h>
#include <babylon/shaders/shadersinclude/image_processing_declaration_fx.h>
#include <babylon/shaders/shadersinclude/image_processing_functions_fx.h>
#include <babylon/shaders/shadersinclude/instances300_declaration_fx.h>
#include <babylon/shaders/shadersinclude/instances_declaration_fx.h>
#include <babylon/shaders/shadersinclude/instances_vertex_fx.h>
#include <babylon/shaders/shadersinclude/kernel_blur_fragment2_fx.h>
#include <babylon/shaders/shadersinclude/kernel_blur_fragment_fx.h>
#include <babylon/shaders/shadersinclude/kernel_blur_varying_declaration_fx.h>
#include <babylon/shaders/shadersinclude/kernel_blur_vertex_fx.h>
#include <babylon/shaders/shadersinclude/light_fragment_declaration_fx.h>
#include <babylon/shaders/shadersinclude/light_fragment_fx.h>
#include <babylon/shaders/shadersinclude/light_ubo_declaration_fx.h>
#include <babylon/shaders/shadersinclude/lights_fragment_functions_fx.h>
#include <babylon/shaders/shadersinclude/log_depth_declaration_fx.h>
#include <babylon/shaders/shadersinclude/log_depth_fragment_fx.h>
#include <babylon/shaders/shadersinclude/log_depth_vertex_fx.h>
#include <babylon/shaders/shadersinclude/morph_targets_vertex_declaration_fx.h>
#include <babylon/shaders/shadersinclude/morph_targets_vertex_fx.h>
#include <babylon/shaders/shadersinclude/morph_targets_vertex_global_declaration_fx.h>
#include <babylon/shaders/shadersinclude/mrt_fragment_declaration_fx.h>
#include <babylon/shaders/shadersinclude/pbr_fragment_declaration_fx.h>
#include <babylon/shaders/shadersinclude/pbr_functions_fx.h>
#include <babylon/shaders/shadersinclude/pbr_light_functions_fx.h>
#include <babylon/shaders/shadersinclude/pbr_ubo_declaration_fx.h>
#include <babylon/shaders/shadersinclude/pbr_vertex_declaration_fx.h>
#include <babylon/shaders/shadersinclude/point_cloud_vertex_declaration_fx.h>
#include <babylon/shaders/shadersinclude/point_cloud_vertex_fx.h>
#include <babylon/shaders/shadersinclude/reflection_function_fx.h>
#include <babylon/shaders/shadersinclude/shadows_fragment_functions_fx.h>
#include <babylon/shaders/shadersinclude/shadows_vertex_fx.h>

namespace BABYLON {

EffectIncludesShadersStore::EffectIncludesShadersStore()
{
}

EffectIncludesShadersStore::~EffectIncludesShadersStore()
{
}

std::unordered_map<std::string, std::string>&
EffectIncludesShadersStore::shaders()
{
  return _shaders;
}

const std::unordered_map<std::string, std::string>&
EffectIncludesShadersStore::shaders() const
{
  return _shaders;
}

std::unordered_map<std::string, std::string>
  EffectIncludesShadersStore::_shaders
  = {{"backgroundFragmentDeclaration", backgroundFragmentDeclaration},
     {"backgroundUboDeclaration", backgroundUboDeclaration},
     {"backgroundVertexDeclaration", backgroundVertexDeclaration},
     {"bones300Declaration", bones300Declaration},
     {"bonesDeclaration", bonesDeclaration},
     {"bonesVertex", bonesVertex},
     {"bumpFragment", bumpFragment},
     {"bumpFragmentFunctions", bumpFragmentFunctions},
     {"bumpVertex", bumpVertex},
     {"bumpVertexDeclaration", bumpVertexDeclaration},
     {"clipPlaneFragment", clipPlaneFragment},
     {"clipPlaneFragmentDeclaration", clipPlaneFragmentDeclaration},
     {"clipPlaneFragmentDeclaration2", clipPlaneFragmentDeclaration2},
     {"clipPlaneVertex", clipPlaneVertex},
     {"clipPlaneVertexDeclaration", clipPlaneVertexDeclaration},
     {"clipPlaneVertexDeclaration2", clipPlaneVertexDeclaration2},
     {"defaultFragmentDeclaration", defaultFragmentDeclaration},
     {"defaultUboDeclaration", defaultUboDeclaration},
     {"defaultVertexDeclaration", defaultVertexDeclaration},
     {"depthPrePass", depthPrePass},
     {"fogFragment", fogFragment},
     {"fogFragmentDeclaration", fogFragmentDeclaration},
     {"fogVertex", fogVertex},
     {"fogVertexDeclaration", fogVertexDeclaration},
     {"fresnelFunction", fresnelFunction},
     {"harmonicsFunctions", harmonicsFunctions},
     {"helperFunctions", helperFunctions},
     {"imageProcessingDeclaration", imageProcessingDeclaration},
     {"imageProcessingFunctions", imageProcessingFunctions},
     {"instances300Declaration", instances300Declaration},
     {"instancesDeclaration", instancesDeclaration},
     {"instancesVertex", instancesVertex},
     {"kernelBlurFragment", kernelBlurFragment},
     {"kernelBlurFragment2", kernelBlurFragment2},
     {"kernelBlurVaryingDeclaration", kernelBlurVaryingDeclaration},
     {"kernelBlurVertex", kernelBlurVertex},
     {"lightFragment", lightFragment},
     {"lightFragmentDeclaration", lightFragmentDeclaration},
     {"lightUboDeclaration", lightUboDeclaration},
     {"lightsFragmentFunctions", lightsFragmentFunctions},
     {"logDepthDeclaration", logDepthDeclaration},
     {"logDepthFragment", logDepthFragment},
     {"logDepthVertex", logDepthVertex},
     {"morphTargetsVertex", morphTargetsVertex},
     {"morphTargetsVertexDeclaration", morphTargetsVertexDeclaration},
     {"morphTargetsVertexGlobalDeclaration",
      morphTargetsVertexGlobalDeclaration},
     {"mrtFragmentDeclaration", mrtFragmentDeclaration},
     {"pbrFragmentDeclaration", pbrFragmentDeclaration},
     {"pbrFunctions", pbrFunctions},
     {"pbrLightFunctions", pbrLightFunctions},
     {"pbrUboDeclaration", pbrUboDeclaration},
     {"pbrVertexDeclaration", pbrVertexDeclaration},
     {"pointCloudVertex", pointCloudVertex},
     {"pointCloudVertexDeclaration", pointCloudVertexDeclaration},
     {"reflectionFunction", reflectionFunction},
     {"shadowsFragmentFunctions", shadowsFragmentFunctions},
     {"shadowsVertex", shadowsVertex}};

} // end of namespace BABYLON
