#ifndef BABYLON_MATERIALS_MATERIAL_HELPER_H
#define BABYLON_MATERIALS_MATERIAL_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MaterialHelper {

public:
  static bool PrepareDefinesForLights(Scene* scene, AbstractMesh* mesh,
                                      MaterialDefines& defines,
                                      unsigned int maxSimultaneousLights = 4,
                                      int SPECULARTERM = -1, int SHADOWS = -1,
                                      int SHADOWFULLFLOAT = -1);
  static void
  PrepareUniformsAndSamplersList(std::vector<std::string>& uniformsList,
                                 std::vector<std::string>& samplersList,
                                 MaterialDefines& defines,
                                 unsigned int maxSimultaneousLights = 4);
  static void HandleFallbacksForShadows(MaterialDefines& defines,
                                        EffectFallbacks* fallbacks,
                                        unsigned int maxSimultaneousLights = 4);
  static void PrepareAttributesForBones(std::vector<std::string>& attribs,
                                        AbstractMesh* mesh,
                                        std::size_t numBoneInfluencers,
                                        EffectFallbacks* fallbacks);
  static void PrepareAttributesForInstances(std::vector<std::string>& attribs,
                                            MaterialDefines& defines,
                                            int INSTANCES = -1);

  // Bindings
  static bool BindLightShadow(Light* light, Scene* scene, AbstractMesh* mesh,
                              unsigned int lightIndex, Effect* effect,
                              bool depthValuesAlreadySet);
  static void BindLightProperties(Light* light, Effect* effect,
                                  unsigned int lightIndex);
  static void BindLights(Scene* scene, AbstractMesh* mesh, Effect* effect,
                         bool specularTerm,
                         unsigned int maxSimultaneousLights = 4);
  static void BindFogParameters(Scene* scene, AbstractMesh* mesh,
                                Effect* effect);
  static void BindBonesParameters(AbstractMesh* mesh, Effect* effect);
  static void BindLogDepth(bool logarithmicDepth, Effect* effect, Scene* scene);
  static void BindClipPlane(Effect* effect, Scene* scene);

}; // end of class MaterialHelper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_HELPER_H
