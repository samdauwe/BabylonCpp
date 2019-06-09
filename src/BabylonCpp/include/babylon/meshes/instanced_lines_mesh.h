#ifndef BABYLON_MESHES_INSTANCED_LINE_MESH_H
#define BABYLON_MESHES_INSTANCED_LINE_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/instanced_mesh.h>

namespace BABYLON {

class InstancedLinesMesh;
class LinesMesh;
using InstancedLinesMeshPtr = std::shared_ptr<InstancedLinesMesh>;

/**
 * @brief Creates an instance based on a source LinesMesh.
 */
class BABYLON_SHARED_EXPORT InstancedLinesMesh : public InstancedMesh {

public:
  template <typename... Ts>
  static InstancedLinesMeshPtr New(Ts&&... args)
  {
    auto mesh = std::shared_ptr<InstancedLinesMesh>(
      new InstancedLinesMesh(std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~InstancedLinesMesh() override;

  /**
   * @brief Returns the string "InstancedLinesMesh".
   */
  const std::string getClassName() const override;

protected:
  InstancedLinesMesh(const std::string& name, LinesMesh* source);

}; // end of class InstancedLinesMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_INSTANCED_LINE_MESH_H
