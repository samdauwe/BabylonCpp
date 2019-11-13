#ifndef BABYLON_MESHES_INSTANCED_LINE_MESH_H
#define BABYLON_MESHES_INSTANCED_LINE_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/instanced_mesh.h>

namespace BABYLON {

class InstancedLinesMesh;
class LinesMesh;
using InstancedLinesMeshPtr = std::shared_ptr<InstancedLinesMesh>;
using LinesMeshPtr          = std::shared_ptr<LinesMesh>;

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
  ~InstancedLinesMesh(); // = default

  /**
   * @brief Returns the string "InstancedLinesMesh".
   */
  const std::string getClassName() const override;

  /**
   * @brief Enables the edge rendering mode on the mesh.
   * This mode makes the mesh edges visible
   * @param epsilon defines the maximal distance between two angles to detect a
   * face
   * @param checkVerticesInsteadOfIndices indicates that we should check vertex
   * list directly instead of faces
   * @returns the current InstancedLinesMesh
   * @see https://www.babylonjs-playground.com/#19O9TU#0
   */
  InstancedLinesMesh& enableEdgesRendering(float epsilon = 0.95f,
                                           bool checkVerticesInsteadOfIndices
                                           = false);

protected:
  InstancedLinesMesh(const std::string& name, const LinesMeshPtr& source);

}; // end of class InstancedLinesMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_INSTANCED_LINE_MESH_H
