#ifndef BABYLON_MESHES_BASE_SUB_MESH_H
#define BABYLON_MESHES_BASE_SUB_MESH_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Effect;
struct MaterialDefines;
using EffectPtr          = std::shared_ptr<Effect>;
using MaterialDefinesPtr = std::shared_ptr<MaterialDefines>;

/**
 * @brief Base class for submeshes.
 */
class BABYLON_SHARED_EXPORT BaseSubMesh {

public:
  BaseSubMesh();
  ~BaseSubMesh(); // = default

  /**
   * @brief Sets associated effect (effect used to render this submesh).
   * @param effect defines the effect to associate with
   * @param defines defines the set of defines used to compile this effect
   */
  void setEffect(const EffectPtr& effect, const MaterialDefinesPtr& defines = nullptr);

protected:
  /**
   * @brief Gets material defines used by the effect associated to the sub mesh.
   */
  MaterialDefinesPtr& get_materialDefines();

  /**
   * @brief Sets material defines used by the effect associated to the sub mesh.
   */
  void set_materialDefines(const MaterialDefinesPtr& defines);

  /**
   * @brief Gets the associated effect.
   */
  EffectPtr& get_effect();

public:
  /**
   * Hidden
   */
  MaterialDefinesPtr _materialDefines;

  /**
   * Hidden
   */
  EffectPtr _materialEffect;

  /**
   * Gets or sets material defines used by the effect associated to the sub mesh
   */
  Property<BaseSubMesh, MaterialDefinesPtr> materialDefines;

  /**
   * Gets associated effect
   */
  ReadOnlyProperty<BaseSubMesh, EffectPtr> effect;

}; // end of class BaseSubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BASE_SUB_MESH_H
