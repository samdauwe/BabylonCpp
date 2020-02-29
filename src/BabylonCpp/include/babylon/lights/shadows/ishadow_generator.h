#ifndef BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H

#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vector>

#include <babylon/babylon_api.h>

using json = nlohmann::json;

namespace BABYLON {

class Effect;
struct MaterialDefines;
class Matrix;
class RenderTargetTexture;
class ShadowGenerator;
class SubMesh;
using EffectPtr              = std::shared_ptr<Effect>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

struct ShadowGeneratorCompileOptions {
  bool useInstances = false;
}; // end of struct ShadowGeneratorCompileOptions

/**
 * @briefInterface to implement to create a shadow generator compatible with BJS.
 */
struct BABYLON_SHARED_EXPORT IShadowGenerator {
  virtual ~IShadowGenerator() = default;
  /**
   * @brief Gets the main RTT containing the shadow map (usually storing depth from the light point
   * of view).
   * @returns The render target texture if present otherwise, null
   */
  virtual RenderTargetTexturePtr getShadowMap() = 0;

  /**
   * @brief Determine wheter the shadow generator is ready or not (mainly all effects and related
   * post processes needs to be ready).
   * @param subMesh The submesh we want to render in the shadow map
   * @param useInstances Defines wether will draw in the map using instances
   * @returns true if ready otherwise, false
   */
  virtual bool isReady(SubMesh* subMesh, bool useInstances = false) = 0;

  /**
   * @brief Prepare all the defines in a material relying on a shadow map at the specified light
   * index.
   * @param defines Defines of the material we want to update
   * @param lightIndex Index of the light in the enabled light list of the material
   */
  virtual void prepareDefines(MaterialDefines& defines, unsigned int lightIndex) = 0;

  /**
   * @brief Binds the shadow related information inside of an effect (information like near, far,
   * darkness... defined in the generator but impacting the effect). It implies the unifroms
   * available on the materials are the standard BJS ones.
   * @param lightIndex Index of the light in the enabled light list of the material owning the
   * effect
   * @param effect The effect we are binfing the information for
   */
  virtual void bindShadowLight(const std::string& lightIndex, const EffectPtr& effect) = 0;

  /**
   * @brief Gets the transformation matrix used to project the meshes into the map from the light
   * point of view. (eq to shadow prjection matrix * light transform matrix)
   * @returns The transform matrix used to create the shadow map
   */
  virtual Matrix getTransformMatrix() = 0;

  /**
   * @brief Recreates the shadow map dependencies like RTT and post processes. This can be used
   * during the switch between Cube and 2D textures for instance.
   */
  virtual void recreateShadowMap() = 0;

  /**
   * @brief Forces all the attached effect to compile to enable rendering only once ready vs. lazyly
   * compiling effects.
   * @param options Sets of optional options forcing the compilation with different modes
   * @returns A promise that resolves when the compilation completes
   */
  virtual void
  forceCompilationSync(const std::function<void(IShadowGenerator* generator)>& onCompiled,
                       const ShadowGeneratorCompileOptions& options)
    = 0;

  /**
   * @brief Serializes the shadow generator setup to a json object.
   * @returns The serialized JSON object
   */
  [[nodiscard]] virtual json serialize() const = 0;

  /**
   * @brief Disposes the Shadow map and related Textures and effects.
   */
  virtual void dispose() = 0;

}; // end of class IShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H
