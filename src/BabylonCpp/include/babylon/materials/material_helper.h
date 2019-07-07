#ifndef BABYLON_MATERIALS_MATERIAL_HELPER_H
#define BABYLON_MATERIALS_MATERIAL_HELPER_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class AbstractMesh;
class BaseTexture;
class Effect;
struct EffectCreationOptions;
class EffectFallbacks;
class Engine;
class Light;
struct MaterialDefines;
class Scene;
class UniformBuffer;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;
using EffectPtr      = std::shared_ptr<Effect>;

/**
 * @brief "Static Class" containing the most commonly used helper while dealing
 * with material for rendering purpose.
 *
 * It contains the basic tools to help defining defines, binding uniform for the
 * common part of the materials.
 *
 * This works by convention in BabylonJS but is meant to be use only with shader
 * following the in place naming rules and conventions.
 */
struct BABYLON_SHARED_EXPORT MaterialHelper {

  /**
   * @brief Bind the current view position to an effect.
   * @param effect The effect to be bound
   * @param scene The scene the eyes position is used from
   */
  static void BindEyePosition(const EffectPtr& effect, Scene* scene);

  /**
   * @brief Helps preparing the defines values about the UVs in used in the
   * effect. UVs are shared as much as we can accross channels in the shaders.
   * @param texture The texture we are preparing the UVs for
   * @param defines The defines to update
   * @param key The channel key "diffuse", "specular"... used in the shader
   */
  static void PrepareDefinesForMergedUV(const BaseTexturePtr& texture,
                                        MaterialDefines& defines,
                                        const std::string& key);

  /**
   * @brief Binds a texture matrix value to its corrsponding uniform
   * @param texture The texture to bind the matrix for
   * @param uniformBuffer The uniform buffer receivin the data
   * @param key The channel key "diffuse", "specular"... used in the shader
   */
  static void BindTextureMatrix(BaseTexture& texture,
                                UniformBuffer& uniformBuffer,
                                const std::string& key);

  /**
   * @brief Helper used to prepare the list of defines associated with misc.
   * values for shader compilation
   * @param mesh defines the current mesh
   * @param scene defines the current scene
   * @param useLogarithmicDepth defines if logarithmic depth has to be turned on
   * @param pointsCloud defines if point cloud rendering has to be turned on
   * @param fogEnabled defines if fog has to be turned on
   * @param alphaTest defines if alpha testing has to be turned on
   * @param defines defines the current list of defines
   */
  static void PrepareDefinesForMisc(AbstractMesh* mesh, Scene* scene,
                                    bool useLogarithmicDepth, bool pointsCloud,
                                    bool fogEnabled, bool alphaTest,
                                    MaterialDefines& defines);

  /**
   * @brief Helper used to prepare the list of defines associated with frame
   * values for shader compilation
   * @param scene defines the current scene
   * @param engine defines the current engine
   * @param defines specifies the list of active defines
   * @param useInstances defines if instances have to be turned on
   * @param useClipPlane defines if clip plane have to be turned on
   */
  static void PrepareDefinesForFrameBoundValues(Scene* scene, Engine* engine,
                                                MaterialDefines& defines,
                                                bool useInstances,
                                                std::optional<bool> useClipPlane
                                                = std::nullopt);

  /**
   * @brief Prepares the defines used in the shader depending on the attributes
   * data available in the mesh
   * @param mesh The mesh containing the geometry data we will draw
   * @param defines The defines to update
   * @param useVertexColor Precise whether vertex colors should be used or not
   * (override mesh info)
   * @param useBones Precise whether bones should be used or not (override mesh
   * info)
   * @param useMorphTargets Precise whether morph targets should be used or not
   * (override mesh info)
   * @param useVertexAlpha Precise whether vertex alpha should be used or not
   * (override mesh info)
   * @returns false if defines are considered not dirty and have not been
   * checked
   */
  static bool PrepareDefinesForAttributes(AbstractMesh* mesh,
                                          MaterialDefines& defines,
                                          bool useVertexColor, bool useBones,
                                          bool useMorphTargets = false,
                                          bool useVertexAlpha  = true);

  /**
   * @brief Prepares the defines related to multiview.
   * @param scene The scene we are intending to draw
   * @param defines The defines to update
   */
  static void PrepareDefinesForMultiview(Scene* scene,
                                         MaterialDefines& defines);

  /**
   * @brief Prepares the defines related to the light information passed in
   * parameter
   * @param scene The scene we are intending to draw
   * @param mesh The mesh the effect is compiling for
   * @param defines The defines to update
   * @param specularSupported Specifies whether specular is supported or not
   * (override lights data)
   * @param maxSimultaneousLights Specfies how manuy lights can be added to the
   * effect at max
   * @param disableLighting Specifies whether the lighting is disabled (override
   * scene and light)
   * @returns true if normals will be required for the rest of the effect
   */
  static bool PrepareDefinesForLights(Scene* scene, AbstractMesh* mesh,
                                      MaterialDefines& defines,
                                      bool specularSupported,
                                      unsigned int maxSimultaneousLights = 4,
                                      bool disableLighting = false);

  /**
   * @brief Prepares the uniforms and samplers list to be used in the effect.
   * This can automatically remove from the list uniforms that won t be acctive
   * due to defines being turned off.
   * @param uniformsListOrOptions The uniform names to prepare or an
   * EffectCreationOptions containing the liist and extra information
   * @param samplersList The samplers list
   * @param defines The defines helping in the list generation
   * @param maxSimultaneousLights The maximum number of simultanous light
   * allowed in the effect
   */
  static void
  PrepareUniformsAndSamplersList(std::vector<std::string>& uniformsList,
                                 std::vector<std::string>& samplersList,
                                 MaterialDefines& defines,
                                 unsigned int maxSimultaneousLights = 4);

  /**
   * @brief Prepares the uniforms and samplers list to be used in the effect.
   * This can automatically remove from the list uniforms that won t be acctive
   * due to defines being turned off.
   * @param uniformsListOrOptions The uniform names to prepare or an
   * EffectCreationOptions containing the liist and extra information
   * @param samplersList The samplers list
   * @param defines The defines helping in the list generation
   * @param maxSimultaneousLights The maximum number of simultanous light
   * allowed in the effect
   */
  static void PrepareUniformsAndSamplersList(EffectCreationOptions& options);

  /**
   * @brief This helps decreasing rank by rank the shadow quality (0 being the
   * highest rank and quality)
   * @param defines The defines to update while falling back
   * @param fallbacks The authorized effect fallbacks
   * @param maxSimultaneousLights The maximum number of lights allowed
   * @param rank the current rank of the Effect
   * @returns The newly affected rank
   */
  static unsigned int HandleFallbacksForShadows(
    MaterialDefines& defines, EffectFallbacks& fallbacks,
    unsigned int maxSimultaneousLights = 4, unsigned int rank = 0);

  /**
   * @brief Prepares the list of attributes required for morph targets according
   * to the effect defines.
   * @param attribs The current list of supported attribs
   * @param mesh The mesh to prepare the morph targets attributes for
   * @param defines The current Defines of the effect
   */
  static void
  PrepareAttributesForMorphTargets(std::vector<std::string>& attribs,
                                   AbstractMesh* mesh,
                                   MaterialDefines& defines);

  /**
   * @brief Prepares the list of attributes required for bones according to the
   * effect defines.
   * @param attribs The current list of supported attribs
   * @param mesh The mesh to prepare the bones attributes for
   * @param defines The current Defines of the effect
   * @param fallbacks The current efffect fallback strategy
   */
  static void PrepareAttributesForBones(std::vector<std::string>& attribs,
                                        AbstractMesh* mesh,
                                        MaterialDefines& defines,
                                        EffectFallbacks& fallbacks);

  /**
   * @brief Prepares the list of attributes required for instances according to
   * the effect defines.
   * @param attribs The current list of supported attribs
   * @param defines The current Defines of the effect
   */
  static void PrepareAttributesForInstances(std::vector<std::string>& attribs,
                                            MaterialDefines& defines);

  /**
   * @brief Prepares the list of attributes required for instances according to
   * the effect defines.
   * @param attribs The current list of supported attribs
   * @param defines The current Defines of the effect
   */
  static void PrepareAttributesForInstances(std::vector<std::string>& attribs,
                                            std::vector<std::string>& defines);

  /**
   * @brief Binds the light shadow information to the effect for the given mesh.
   * @param light The light containing the generator
   * @param mesh The mesh we are binding the information to render
   * @param lightIndex The light index in the effect used to render the mesh
   * @param effect The effect we are binding the data to
   */
  static void BindLightShadow(Light& light, AbstractMesh& mesh,
                              const std::string& lightIndex,
                              const EffectPtr& effect);

  /**
   * @brief Binds the light information to the effect.
   * @param light The light containing the generator
   * @param effect The effect we are binding the data to
   * @param lightIndex The light index in the effect used to render
   */
  static void BindLightProperties(Light& light, const EffectPtr& effect,
                                  unsigned int lightIndex);

  /**
   * @brief Binds the lights information from the scene to the effect for the
   * given mesh.
   * @param scene The scene the lights belongs to
   * @param mesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   * @param defines The generated defines for the effect
   * @param maxSimultaneousLights The maximum number of light that can be bound
   * to the effect
   * @param usePhysicalLightFalloff Specifies whether the light falloff is
   * defined physically or not
   */
  static void BindLights(Scene* scene, AbstractMesh* mesh,
                         const EffectPtr& effect, MaterialDefines& defines,
                         unsigned int maxSimultaneousLights = 4,
                         bool usePhysicalLightFalloff       = false);

  /**
   * @brief Binds the fog information from the scene to the effect for the given
   * mesh.
   * @param scene The scene the lights belongs to
   * @param mesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   * @param linearSpace Defines if the fog effect is applied in linear space
   */
  static void BindFogParameters(Scene* scene, AbstractMesh* mesh,
                                const EffectPtr& effect,
                                bool linearSpace = false);

  /**
   * @brief Binds the bones information from the mesh to the effect.
   * @param mesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   */
  static void BindBonesParameters(AbstractMesh* mesh, const EffectPtr& effect);

  /**
   * @brief Binds the morph targets information from the mesh to the effect.
   * @param abstractMesh The mesh we are binding the information to render
   * @param effect The effect we are binding the data to
   */
  static void BindMorphTargetParameters(AbstractMesh* abstractMesh,
                                        const EffectPtr& effect);

  /**
   * @brief Binds the logarithmic depth information from the scene to the effect
   * for the given defines.
   * @param defines The generated defines used in the effect
   * @param effect The effect we are binding the data to
   * @param scene The scene we are willing to render with logarithmic scale for
   */
  static void BindLogDepth(MaterialDefines& defines, const EffectPtr& effect,
                           Scene* scene);

  /**
   * @brief Binds the clip plane information from the scene to the effect.
   * @param scene The scene the clip plane information are extracted from
   * @param effect The effect we are binding the data to
   */
  static void BindClipPlane(const EffectPtr& effect, Scene* scene);

  static Color3 _tempFogColor;

}; // end of struct MaterialHelper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_HELPER_H
