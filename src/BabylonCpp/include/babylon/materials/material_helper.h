#ifndef BABYLON_MATERIALS_MATERIAL_HELPER_H
#define BABYLON_MATERIALS_MATERIAL_HELPER_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

class AbstractMesh;
class BaseTexture;
class Effect;
struct IEffectCreationOptions;
class EffectFallbacks;
class Engine;
class Light;
struct MaterialDefines;
struct PrePassConfiguration;
class Scene;
class UniformBuffer;
using BaseTexturePtr          = std::shared_ptr<BaseTexture>;
using EffectPtr               = std::shared_ptr<Effect>;
using LightPtr                = std::shared_ptr<Light>;
using PrePassConfigurationPtr = std::shared_ptr<PrePassConfiguration>;

struct BABYLON_SHARED_EXPORT PrepareDefinesForLightsState {
  bool needNormals     = false;
  bool needRebuild     = false;
  bool shadowEnabled   = false;
  bool specularEnabled = false;
  bool lightmapMode    = false;
}; // end of struct PrepareDefinesForLightsState

/**
 * @brief "Static Class" containing the most commonly used helper while dealing with material for
 * rendering purpose.
 *
 * It contains the basic tools to help defining defines, binding uniform for the common part of the
 * materials.
 *
 * This works by convention in BabylonJS but is meant to be use only with shader following the in
 * place naming rules and conventions.
 */
struct BABYLON_SHARED_EXPORT MaterialHelper {

  /**
   * @brief Bind the current view position to an effect.
   * @param effect The effect to be bound
   * @param scene The scene the eyes position is used from
   * @param variableName name of the shader variable that will hold the eye position
   */
  static void BindEyePosition(Effect* effect, Scene* scene,
                              const std::string& variableName = "vEyePosition");

  /**
   * @brief Helps preparing the defines values about the UVs in used in the effect. UVs are shared
   * as much as we can accross channels in the shaders.
   * @param texture The texture we are preparing the UVs for
   * @param defines The defines to update
   * @param key The channel key "diffuse", "specular"... used in the shader
   */
  static void PrepareDefinesForMergedUV(const BaseTexturePtr& texture, MaterialDefines& defines,
                                        const std::string& key);

  /**
   * @brief Binds a texture matrix value to its corrsponding uniform
   * @param texture The texture to bind the matrix for
   * @param uniformBuffer The uniform buffer receivin the data
   * @param key The channel key "diffuse", "specular"... used in the shader
   */
  static void BindTextureMatrix(BaseTexture& texture, UniformBuffer& uniformBuffer,
                                const std::string& key);

  /**
   * @brief Gets the current status of the fog (should it be enabled?).
   * @param mesh defines the mesh to evaluate for fog support
   * @param scene defines the hosting scene
   * @returns true if fog must be enabled
   */
  static bool GetFogState(AbstractMesh* mesh, Scene* scene);

  /**
   * @brief Helper used to prepare the list of defines associated with misc. values for shader
   * compilation
   * @param mesh defines the current mesh
   * @param scene defines the current scene
   * @param useLogarithmicDepth defines if logarithmic depth has to be turned on
   * @param pointsCloud defines if point cloud rendering has to be turned on
   * @param fogEnabled defines if fog has to be turned on
   * @param alphaTest defines if alpha testing has to be turned on
   * @param defines defines the current list of defines
   */
  static void PrepareDefinesForMisc(AbstractMesh* mesh, Scene* scene, bool useLogarithmicDepth,
                                    bool pointsCloud, bool fogEnabled, bool alphaTest,
                                    MaterialDefines& defines);

  /**
   * @brief Helper used to prepare the list of defines associated with frame values for shader
   * compilation
   * @param scene defines the current scene
   * @param engine defines the current engine
   * @param defines specifies the list of active defines
   * @param useInstances defines if instances have to be turned on
   * @param useClipPlane defines if clip plane have to be turned on
   * @param useInstances defines if instances have to be turned on
   * @param useThinInstances defines if thin instances have to be turned on
   */
  static void PrepareDefinesForFrameBoundValues(Scene* scene, Engine* engine,
                                                MaterialDefines& defines, bool useInstances,
                                                std::optional<bool> useClipPlane = std::nullopt,
                                                bool useThinInstances            = false);

  /**
   * @brief Prepares the defines for bones.
   * @param mesh The mesh containing the geometry data we will draw
   * @param defines The defines to update
   */
  static void PrepareDefinesForBones(AbstractMesh* mesh, MaterialDefines& defines);

  /**
   * @brief Prepares the defines for morph targets.
   * @param mesh The mesh containing the geometry data we will draw
   * @param defines The defines to update
   */
  static void PrepareDefinesForMorphTargets(AbstractMesh* mesh, MaterialDefines& defines);

  /**
   * @brief Prepares the defines used in the shader depending on the attributes data available in
   * the mesh
   * @param mesh The mesh containing the geometry data we will draw
   * @param defines The defines to update
   * @param useVertexColor Precise whether vertex colors should be used or not (override mesh info)
   * @param useBones Precise whether bones should be used or not (override mesh info)
   * @param useMorphTargets Precise whether morph targets should be used or not (override mesh info)
   * @param useVertexAlpha Precise whether vertex alpha should be used or not (override mesh info)
   * @returns false if defines are considered not dirty and have not been checked
   */
  static bool PrepareDefinesForAttributes(AbstractMesh* mesh, MaterialDefines& defines,
                                          bool useVertexColor, bool useBones,
                                          bool useMorphTargets = false, bool useVertexAlpha = true);

  /**
   * @brief Prepares the defines related to multiview.
   * @param scene The scene we are intending to draw
   * @param defines The defines to update
   */
  static void PrepareDefinesForMultiview(Scene* scene, MaterialDefines& defines);

  /**
   * @brief Prepares the defines related to the prepass.
   * @param scene The scene we are intending to draw
   * @param defines The defines to update
   * @param canRenderToMRT Indicates if this material renders to several textures in the prepass
   */
  static void PrepareDefinesForPrePass(Scene* scene, MaterialDefines& defines, bool canRenderToMRT);

  /**
   * @brief Prepares the defines related to the light information passed in parameter
   * @param scene The scene we are intending to draw
   * @param mesh The mesh the effect is compiling for
   * @param light The light the effect is compiling for
   * @param lightIndex The index of the light
   * @param defines The defines to update
   * @param specularSupported Specifies whether specular is supported or not (override lights data)
   * @param state Defines the current state regarding what is needed (normals, etc...)
   */
  static void PrepareDefinesForLight(Scene* scene, AbstractMesh* mesh, const LightPtr& light,
                                     unsigned int lightIndex, MaterialDefines& defines,
                                     bool specularSupported, PrepareDefinesForLightsState& state);

  /**
   * @brief Prepares the defines related to the light information passed in parameter.
   * @param scene The scene we are intending to draw
   * @param mesh The mesh the effect is compiling for
   * @param defines The defines to update
   * @param specularSupported Specifies whether specular is supported or not (override lights data)
   * @param maxSimultaneousLights Specfies how manuy lights can be added to the effect at max
   * @param disableLighting Specifies whether the lighting is disabled (override scene and light)
   * @returns true if normals will be required for the rest of the effect
   */
  static bool PrepareDefinesForLights(Scene* scene, AbstractMesh* mesh, MaterialDefines& defines,
                                      bool specularSupported,
                                      unsigned int maxSimultaneousLights = 4,
                                      bool disableLighting               = false);

  /**
   * @brief Prepares the uniforms and samplers list to be used in the effect (for a specific light).
   * @param lightIndex defines the light index
   * @param uniformsList The uniform list
   * @param samplersList The sampler list
   * @param projectedLightTexture defines if projected texture must be used
   */
  static void PrepareUniformsAndSamplersForLight(unsigned int lightIndex,
                                                 std::vector<std::string>& uniformsList,
                                                 std::vector<std::string>& samplersList,
                                                 bool projectedLightTexture = false);

  /**
   * @brief Prepares the uniforms and samplers list to be used in the effect (for a specific light).
   * @param lightIndex defines the light index
   * @param uniformsList The uniform list
   * @param samplersList The sampler list
   * @param projectedLightTexture defines if projected texture must be used
   * @param uniformBuffersList defines an optional list of uniform buffers
   */
  static void PrepareUniformsAndSamplersForLight(unsigned int lightIndex,
                                                 std::vector<std::string>& uniformsList,
                                                 std::vector<std::string>& samplersList,
                                                 std::vector<std::string>& uniformBuffersList,
                                                 bool hasUniformBuffersList = false,
                                                 bool projectedLightTexture = false);

  /**
   * @brief Prepares the uniforms and samplers list to be used in the effect.
   * @param uniformsListOrOptions The uniform names to prepare or an EffectCreationOptions
   * containing the list and extra information
   * @param samplersList The sampler list
   * @param defines The defines helping in the list generation
   * @param maxSimultaneousLights The maximum number of simultanous light allowed in the effect
   */
  static void PrepareUniformsAndSamplersList(std::vector<std::string>& uniformsList,
                                             std::vector<std::string>& samplersList,
                                             MaterialDefines& defines,
                                             unsigned int maxSimultaneousLights = 4);

  /**
   * @brief Prepares the uniforms and samplers list to be used in the effect. This can automatically
   * remove from the list uniforms that won t be acctive due to defines being turned off.
   * @param uniformsListOrOptions The uniform names to prepare or an EffectCreationOptions
   * containing the list and extra information
   * @param samplersList The samplers list
   * @param defines The defines helping in the list generation
   * @param maxSimultaneousLights The maximum number of simultanous light allowed in the effect
   */
  static void PrepareUniformsAndSamplersList(IEffectCreationOptions& options);

  /**
   * @brief This helps decreasing rank by rank the shadow quality (0 being the highest rank and
   * quality)
   * @param defines The defines to update while falling back
   * @param fallbacks The authorized effect fallbacks
   * @param maxSimultaneousLights The maximum number of lights allowed
   * @param rank the current rank of the Effect
   * @returns The newly affected rank
   */
  static unsigned int HandleFallbacksForShadows(MaterialDefines& defines,
                                                EffectFallbacks& fallbacks,
                                                unsigned int maxSimultaneousLights = 4,
                                                unsigned int rank                  = 0);

  /**
   * @brief Prepares the list of attributes required for morph targets according to the effect
   * defines.
   * @param attribs The current list of supported attribs
   * @param mesh The mesh to prepare the morph targets attributes for
   * @param influencers The number of influencers
   */
  static void PrepareAttributesForMorphTargetsInfluencers(std::vector<std::string>& attribs,
                                                          AbstractMesh* mesh,
                                                          unsigned int influencers);

  /**
   * @brief Prepares the list of attributes required for morph targets according to the effect
   * defines.
   * @param attribs The current list of supported attribs
   * @param mesh The mesh to prepare the morph targets attributes for
   * @param defines The current Defines of the effect
   */
  static void PrepareAttributesForMorphTargets(std::vector<std::string>& attribs,
                                               AbstractMesh* mesh, MaterialDefines& defines);

  /**
   * @brief Prepares the list of attributes required for bones according to the effect defines.
   * @param attribs The current list of supported attribs
   * @param mesh The mesh to prepare the bones attributes for
   * @param defines The current Defines of the effect
   * @param fallbacks The current efffect fallback strategy
   */
  static void PrepareAttributesForBones(std::vector<std::string>& attribs, AbstractMesh* mesh,
                                        MaterialDefines& defines, EffectFallbacks& fallbacks);

  /**
   * @brief Check and prepare the list of attributes required for instances according to the effect
   * defines.
   * @param attribs The current list of supported attribs
   * @param defines The current MaterialDefines of the effect
   */
  static void PrepareAttributesForInstances(std::vector<std::string>& attribs,
                                            MaterialDefines& defines);

  /**
   * @brief Add the list of attributes required for instances to the attribs array.
   * @param attribs The current list of supported attribs
   */
  static void PushAttributesForInstances(std::vector<std::string>& attribs);

  /**
   * @brief Binds the light information to the effect.
   * @param light The light containing the generator
   * @param effect The effect we are binding the data to
   * @param lightIndex The light index in the effect used to render
   */
  static void BindLightProperties(Light& light, Effect* effect, unsigned int lightIndex);

  /**
   * @brief Binds the lights information from the scene to the effect for the given mesh.
   * @param light Light to bind
   * @param lightIndex Light index
   * @param scene The scene where the light belongs to
   * @param effect The effect we are binding the data to
   * @param useSpecular Defines if specular is supported
   * @param rebuildInParallel Specifies whether the shader is rebuilding in parallel
   */
  static void BindLight(const LightPtr& light, unsigned int lightIndex, Scene* scene,
                        Effect* effect, bool useSpecular, bool rebuildInParallel = false);

  /**
   * @brief Binds the lights information from the scene to the effect for the given mesh.
   * @param scene The scene the lights belongs to
   * @param mesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   * @param defines The generated defines for the effect
   * @param maxSimultaneousLights The maximum number of light that can be bound to the effect
   * @param rebuildInParallel Specifies whether the shader is rebuilding in parallel
   */
  static void BindLights(Scene* scene, AbstractMesh* mesh, Effect* effect, bool defines,
                         unsigned int maxSimultaneousLights = 4, bool rebuildInParallel = false);

  /**
   * @brief Binds the lights information from the scene to the effect for the given mesh.
   * @param scene The scene the lights belongs to
   * @param mesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   * @param defines The generated defines for the effect
   * @param maxSimultaneousLights The maximum number of light that can be bound to the effect
   * @param rebuildInParallel Specifies whether the shader is rebuilding in parallel
   */
  static void BindLights(Scene* scene, AbstractMesh* mesh, Effect* effect, MaterialDefines& defines,
                         unsigned int maxSimultaneousLights = 4, bool rebuildInParallel = false);

  /**
   * @brief Binds the fog information from the scene to the effect for the given mesh.
   * @param scene The scene the lights belongs to
   * @param mesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   * @param linearSpace Defines if the fog effect is applied in linear space
   */
  static void BindFogParameters(Scene* scene, AbstractMesh* mesh, const EffectPtr& effect,
                                bool linearSpace = false);

  /**
   * @brief Binds the bones information from the mesh to the effect.
   * @param mesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   * @param prePassConfiguration Configuration for the prepass, in case prepass is activated
   */
  static void BindBonesParameters(AbstractMesh* mesh, Effect* effect,
                                  const PrePassConfigurationPtr& prePassConfiguration = nullptr);

  /**
   * @brief Copies the bones transformation matrices into the target array and returns the target's
   * reference.
   */
  static Float32Array& _CopyBonesTransformationMatrices(const Float32Array& source,
                                                        Float32Array& target);

  /**
   * @brief Binds the morph targets information from the mesh to the effect.
   * @param abstractMesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   */
  static void BindMorphTargetParameters(AbstractMesh* abstractMesh, Effect* effect);

  /**
   * @brief Binds the logarithmic depth information from the scene to the effect for the given
   * defines.
   * @param defines The generated defines used in the effect
   * @param effect The effect we are binding the data to
   * @param scene The scene we are willing to render with logarithmic scale for
   */
  static void BindLogDepth(MaterialDefines& defines, Effect* effect, Scene* scene);

  /**
   * @brief Binds the clip plane information from the scene to the effect.
   * @param scene The scene the clip plane information are extracted from
   * @param effect The effect we are binding the data to
   */
  static void BindClipPlane(const EffectPtr& effect, Scene* scene);

  static std::unique_ptr<MaterialDefines> _TmpMorphInfluencers;
  static Color3 _tempFogColor;

}; // end of struct MaterialHelper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_HELPER_H
