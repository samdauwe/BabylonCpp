#ifndef BABYLON_MATERIALS_MATERIAL_HELPER_H
#define BABYLON_MATERIALS_MATERIAL_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MaterialHelper {

public:
  static void BindEyePosition(Effect* effect, Scene* scene);
  static void
  PrepareDefinesForMergedUV(BaseTexture* texture, MaterialDefines& defines,
                            unsigned int key, const string_t& keyString,
                            unsigned int MAINUV1, unsigned int MAINUV2);
  static void BindTextureMatrix(BaseTexture& texture,
                                UniformBuffer& uniformBuffer,
                                const string_t& key);
  static void PrepareDefinesForMisc(AbstractMesh* mesh, Scene* scene,
                                    bool useLogarithmicDepth, bool pointsCloud,
                                    bool fogEnabled, MaterialDefines& defines,
                                    unsigned int LOGARITHMICDEPTH,
                                    unsigned int POINTSIZE, unsigned int FOG,
                                    unsigned int NONUNIFORMSCALING);
  static void PrepareDefinesForFrameBoundValues(
    Scene* scene, Engine* engine, MaterialDefines& defines, bool useInstances,
    unsigned int CLIPPLANE, unsigned int ALPHATEST, unsigned int DEPTHPREPASS,
    unsigned int INSTANCES, bool forceAlphaTest = false);
  static bool PrepareDefinesForAttributes(
    AbstractMesh* mesh, MaterialDefines& defines, bool useVertexColor,
    bool useBones, bool useMorphTargets, bool useVertexAlpha,
    unsigned int NORMAL, unsigned int UV1, unsigned int UV2,
    unsigned int VERTEXCOLOR, unsigned int VERTEXALPHA,
    unsigned int MORPHTARGETS_TANGENT = 0, unsigned int MORPHTARGETS_NORMAL = 0,
    unsigned int MORPHTARGETS = 0);
  static bool PrepareDefinesForLights(
    Scene* scene, AbstractMesh* mesh, MaterialDefines& defines,
    bool specularSupported, unsigned int maxSimultaneousLights,
    bool disableLighting, unsigned int SPECULARTERM, unsigned int SHADOWFLOAT);
  static void PrepareUniformsAndSamplersList(vector_t<string_t>& uniformsList,
                                             vector_t<string_t>& samplersList,
                                             MaterialDefines& defines,
                                             unsigned int maxSimultaneousLights
                                             = 4);
  static void PrepareUniformsAndSamplersList(EffectCreationOptions& options);
  static unsigned int HandleFallbacksForShadows(
    MaterialDefines& defines, EffectFallbacks& fallbacks,
    unsigned int maxSimultaneousLights = 4, unsigned int rank = 0);
  static void PrepareAttributesForMorphTargets(vector_t<string_t>& attribs,
                                               AbstractMesh* mesh,
                                               MaterialDefines& defines,
                                               unsigned int NORMAL);
  static void PrepareAttributesForBones(vector_t<string_t>& attribs,
                                        AbstractMesh* mesh,
                                        MaterialDefines& defines,
                                        EffectFallbacks& fallbacks);
  static void PrepareAttributesForInstances(vector_t<string_t>& attribs,
                                            MaterialDefines& defines,
                                            unsigned int INSTANCES);

  // Bindings
  static void BindLightShadow(Light* light, Scene* scene, AbstractMesh* mesh,
                              unsigned int lightIndex, Effect* effect);
  static void BindLightProperties(Light* light, Effect* effect,
                                  unsigned int lightIndex);
  static void BindLights(Scene* scene, AbstractMesh* mesh, Effect* effect,
                         MaterialDefines& defines,
                         unsigned int maxSimultaneousLights,
                         unsigned int SPECULARTERM,
                         bool usePhysicalLightFalloff = false);
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
