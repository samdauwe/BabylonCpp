#include <babylon/materials/occlusion/occlusion_material.h>

namespace BABYLON {

OcclusionMaterial::OcclusionMaterial(const std::string& iName, Scene* scene)
    : ShaderMaterial{iName, scene, "color",
                     IShaderMaterialOptions{
                       false,                                // needAlphaBlending
                       false,                                // needAlphaTesting
                       {"position"},                         // attributes
                       {"world", "viewProjection", "color"}, // uniforms
                       {},                                   // uniformBuffers
                       {},                                   // samplers
                       {},                                   // defines
                     }}
{
  disableColorWrite = true;
  forceDepthWrite   = true;
  setColor4("color", Color4(0.f, 0.f, 0.f, 1.f));
}

OcclusionMaterial::~OcclusionMaterial() = default;

} // end of namespace BABYLON
