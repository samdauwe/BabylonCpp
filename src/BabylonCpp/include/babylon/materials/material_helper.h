#ifndef BABYLON_MATERIALS_MATERIAL_HELPER_H
#define BABYLON_MATERIALS_MATERIAL_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MaterialHelper {

public:
  static void PrepareDefinesForMisc(AbstractMesh* mesh, Scene* scene,
                                    bool useLogarithmicDepth, bool pointsCloud,
                                    bool fogEnabled, MaterialDefines& defines,
                                    unsigned int LOGARITHMICDEPTH,
                                    unsigned int POINTSIZE, unsigned int FOG);
  static void PrepareDefinesForFrameBoundValues(
    Scene* scene, Engine* engine, MaterialDefines& defines, bool useInstances,
    unsigned int CLIPPLANE, unsigned int ALPHATEST, unsigned int INSTANCES);
  static void PrepareDefinesForAttributes(
    AbstractMesh* mesh, MaterialDefines& defines, bool useVertexColor,
    bool useBones, bool useMorphTargets, unsigned int NORMAL, unsigned int UV1,
    unsigned int UV2, unsigned int VERTEXCOLOR, unsigned int VERTEXALPHA,
    unsigned int MORPHTARGETS_NORMAL = 0, unsigned int MORPHTARGETS = 0);
  static bool PrepareDefinesForLights(
    Scene* scene, AbstractMesh* mesh, MaterialDefines& defines,
    bool specularSupported, unsigned int maxSimultaneousLights,
    bool disableLighting, unsigned int SPECULARTERM, unsigned int SHADOWFLOAT);
  static void
  PrepareUniformsAndSamplersList(std::vector<std::string>& uniformsList,
                                 std::vector<std::string>& samplersList,
                                 MaterialDefines& defines,
                                 unsigned int maxSimultaneousLights = 4);
  static void PrepareUniformsAndSamplersList(EffectCreationOptions& options);
  static void HandleFallbacksForShadows(MaterialDefines& defines,
                                        EffectFallbacks& fallbacks,
                                        unsigned int maxSimultaneousLights);
  static void
  PrepareAttributesForMorphTargets(std::vector<std::string>& attribs,
                                   AbstractMesh* mesh, MaterialDefines& defines,
                                   unsigned int NORMAL);
  static void PrepareAttributesForBones(std::vector<std::string>& attribs,
                                        AbstractMesh* mesh,
                                        MaterialDefines& defines,
                                        EffectFallbacks& fallbacks);
  static void PrepareAttributesForInstances(std::vector<std::string>& attribs,
                                            MaterialDefines& defines,
                                            unsigned int INSTANCES);

  // Bindings
  static bool BindLightShadow(Light* light, Scene* scene, AbstractMesh* mesh,
                              unsigned int lightIndex, Effect* effect,
                              bool depthValuesAlreadySet);
  static void BindLightProperties(Light* light, Effect* effect,
                                  unsigned int lightIndex);
  static void BindLights(Scene* scene, AbstractMesh* mesh, Effect* effect,
                         MaterialDefines& defines,
                         unsigned int maxSimultaneousLights,
                         unsigned int SPECULARTERM);
  static void BindFogParameters(Scene* scene, AbstractMesh* mesh,
                                Effect* effect);
  static void BindBonesParameters(AbstractMesh* mesh, Effect* effect);
  static void BindMorphTargetParameters(AbstractMesh* abstractMesh,
                                        Effect* effect);
  static void BindLogDepth(MaterialDefines& defines, Effect* effect,
                           Scene* scene, unsigned int LOGARITHMICDEPTH);
  static void BindClipPlane(Effect* effect, Scene* scene);

}; // end of class MaterialHelper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_HELPER_H
