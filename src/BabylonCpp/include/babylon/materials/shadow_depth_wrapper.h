#ifndef BABYLON_MATERIALS_SHADOW_DEPTH_WRAPPER_H
#define BABYLON_MATERIALS_SHADOW_DEPTH_WRAPPER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/structs.h>
#include <babylon/materials/iioption_shadow_depth_material.h>

namespace BABYLON {

class Material;
class Scene;
class ShadowGenerator;
class SubMesh;
FWD_STRUCT_SPTR(DrawWrapper)
FWD_CLASS_SPTR(Material)

/**
 * @brief Class that can be used to wrap a base material to generate accurate shadows when using
 * custom vertex/fragment code in the base material.
 */
class BABYLON_SHARED_EXPORT ShadowDepthWrapper {

public:
  /**
   * @brief Instantiate a new shadow depth wrapper.
   * It works by injecting some specific code in the vertex/fragment shaders of the base material
   * and is used by a shadow generator to generate the shadow depth map. For more information,
   * please refer to the documentation: https://doc.babylonjs.com/babylon101/shadows
   * @param baseMaterial Material to wrap
   * @param scene Define the scene the material belongs to
   * @param options Options used to create the wrapper
   */
  explicit ShadowDepthWrapper(const MaterialPtr& baseMaterial, Scene* scene,
                              const std::optional<IIOptionShadowDepthMaterial>& options);
  ~ShadowDepthWrapper();

  /**
   * @brief Gets the effect to use to generate the depth map
   * @param subMesh subMesh to get the effect for
   * @param shadowGenerator shadow generator to get the effect for
   * @returns the effect to use to generate the depth map for the subMesh + shadow generator
   * specified
   */
  DrawWrapperPtr getEffect(SubMesh* subMesh, ShadowGenerator* shadowGenerator);

  /**
   * @brief Specifies that the submesh is ready to be used for depth rendering
   * @param subMesh submesh to check
   * @param defines the list of defines to take into account when checking the effect
   * @param shadowGenerator combined with subMesh, it defines the effect to check
   * @param useInstances specifies that instances should be used
   * @returns a boolean indicating that the submesh is ready or not
   */
  bool isReadyForSubMesh(SubMesh* subMesh, const std::vector<std::string>& defines,
                         ShadowGenerator* shadowGenerator, bool useInstances);

  /**
   * @brief Disposes the resources
   */
  void dispose();

protected:
  /**
   * @brief Gets the matrices names.
   */
  std::unordered_map<std::string, std::string>& get__matriceNames();

  /**
   * @brief Gets the standalone status of the wrapper.
   */
  bool get_standalone() const;

  /**
   * @brief Gets the base material the wrapper is built upon.
   */
  MaterialPtr& get_baseMaterial();

public:
  /**
   * Gets the matrices names
   */
  ReadOnlyProperty<ShadowDepthWrapper, std::unordered_map<std::string, std::string>> _matriceNames;

  /**
   * Gets the standalone status of the wrapper
   */
  ReadOnlyProperty<ShadowDepthWrapper, bool> standalone;

  /**
   * Gets the base material the wrapper is built upon
   */
  ReadOnlyProperty<ShadowDepthWrapper, MaterialPtr> baseMaterial;

private:
  class ShadowDepthWrapperImpl;                  // Forward declaration of the implementation class
  std::unique_ptr<ShadowDepthWrapperImpl> _impl; // PIMPL

}; // end of struct ShadowDepthWrapper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_SHADOW_DEPTH_WRAPPER_H
