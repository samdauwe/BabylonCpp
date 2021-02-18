#ifndef BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

using json = nlohmann::json;

namespace BABYLON {

struct MaterialDefines;
class Matrix;
class ShadowGenerator;
class SubMesh;
FWD_CLASS_SPTR(Effect)
FWD_CLASS_SPTR(RenderTargetTexture)

struct ShadowGeneratorCompileOptions {
  bool useInstances = false;
}; // end of struct ShadowGeneratorCompileOptions

/**
 * @brief Interface to implement to create a shadow generator compatible with BJS.
 */
struct BABYLON_SHARED_EXPORT IShadowGenerator {

  virtual ~IShadowGenerator() = default;

  /**
   * Gets or set the id of the shadow generator. It will be the one from the light if not defined
   */
  std::string id;

  /**
   * @brief Gets the main RTT containing the shadow map (usually storing depth from the light point
   * of view).
   * @returns The render target texture if present otherwise, null
   */
  virtual RenderTargetTexturePtr getShadowMap() = 0;

  /**
   * @brief Determine whether the shadow generator is ready or not (mainly all effects and related
   * post processes needs to be ready).
   * @param subMesh The submesh we want to render in the shadow map
   * @param useInstances Defines whether will draw in the map using instances
   * @param isTransparent Indicates that isReady is called for a transparent subMesh
   * @returns true if ready otherwise, false
   */
  virtual bool isReady(SubMesh* subMesh, bool useInstances = false, bool isTransparent = false) = 0;

  /**
   * @brief Prepare all the defines in a material relying on a shadow map at the specified light
   * index.
   * @param defines Defines of the material we want to update
   * @param lightIndex Index of the light in the enabled light list of the material
   */
  virtual void prepareDefines(MaterialDefines& defines, unsigned int lightIndex) = 0;

  /**
   * @brief Binds the shadow related information inside of an effect (information like near, far,
   * darkness... defined in the generator but impacting the effect). It implies the uniforms
   * available on the materials are the standard BJS ones.
   * @param lightIndex Index of the light in the enabled light list of the material owning the
   * effect
   * @param effect The effect we are binding the information for
   */
  virtual void bindShadowLight(const std::string& lightIndex, Effect* effect) = 0;

  /**
   * @brief Gets the transformation matrix used to project the meshes into the map from the light
   * point of view. (eq to shadow projection matrix * light transform matrix)
   * @returns The transform matrix used to create the shadow map
   */
  virtual Matrix getTransformMatrix() = 0;

  /**
   * @brief Recreates the shadow map dependencies like RTT and post processes. This can be used
   * during the switch between Cube and 2D textures for instance.
   */
  virtual void recreateShadowMap() = 0;

  /**
   * @brief Forces all the attached effect to compile to enable rendering only once ready vs. lazily
   * compiling effects.
   * @param onCompiled Callback triggered at the and of the effects compilation
   * @param options Sets of optional options forcing the compilation with different modes
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
