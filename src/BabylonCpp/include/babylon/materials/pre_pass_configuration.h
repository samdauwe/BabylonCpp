#ifndef BABYLON_MATERIALS_PRE_PASS_CONFIGURATION_H
#define BABYLON_MATERIALS_PRE_PASS_CONFIGURATION_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/matrix.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(Effect)
FWD_CLASS_SPTR(Mesh)

/**
 * @brief Configuration needed for prepass-capable materials.
 */
struct BABYLON_SHARED_EXPORT PrePassConfiguration {

  PrePassConfiguration();
  ~PrePassConfiguration(); // = default

  /**
   * Previous world matrices of meshes carrying this material
   * Used for computing velocity
   */
  std::unordered_map<size_t, Matrix> previousWorldMatrices;

  /**
   * Previous view project matrix
   * Used for computing velocity
   */
  std::optional<Matrix> previousViewProjection;

  /**
   * Previous bones of meshes carrying this material
   * Used for computing velocity
   */
  std::unordered_map<unsigned int, Float32Array> previousBones;

  /**
   * @brief Add the required uniforms to the current list.
   * @param uniforms defines the current uniform list.
   */
  static void AddUniforms(std::vector<std::string>& uniforms);

  /**
   * @brief Add the required samplers to the current list.
   * @param samplers defines the current sampler list.
   */
  static void AddSamplers(std::vector<std::string>& samplers);

  /**
   * @brief Binds the material data.
   * @param effect defines the effect to update
   * @param scene defines the scene the material belongs to.
   * @param mesh The mesh
   * @param world World matrix of this mesh
   * @param isFrozen Is the material frozen
   */
  void bindForSubMesh(const EffectPtr& effect, Scene* scene, Mesh* mesh, const Matrix& world,
                      bool isFrozen);

}; // end of struct PrePassConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PRE_PASS_CONFIGURATION_H
