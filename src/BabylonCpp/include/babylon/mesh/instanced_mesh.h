#ifndef BABYLON_MESH_INSTANCED_MESH_H
#define BABYLON_MESH_INSTANCED_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

/**
 * @brief Represents an instance based on a source mesh.
 */
class BABYLON_SHARED_EXPORT InstancedMesh : public AbstractMesh {

public:
  template <typename... Ts>
  static InstancedMesh* New(Ts&&... args)
  {
    auto mesh = new InstancedMesh(std::forward<Ts>(args)...);
    mesh->addToScene(static_cast<std::unique_ptr<AbstractMesh>>(mesh));

    return mesh;
  }
  ~InstancedMesh();

  /**
   * @brief Returns the string "InstancedMesh"
   */
  const char* getClassName() const override;

  IReflect::Type type() const override;

  /** Methods **/
  bool receiveShadows() const;
  Material* material() const;
  float visibility() const;
  Skeleton* skeleton() override;
  unsigned int renderingGroupId() const;

  /**
   * @brief Returns the total number of vertices (integer).
   */
  size_t getTotalVertices() const override;

  Mesh* sourceMesh() const;

  /**
   * @brief Returns Float32Array of the requested kind of data : positons,
   * normals, uvs, etc.
   */
  Float32Array getVerticesData(unsigned int kind,
                               bool copyWhenShared = false) override;

  /**
   * @brief Returns if the mesh owns the requested kind of data.
   */
  bool isVerticesDataPresent(unsigned int kind) override;

  /**
   * @brief Returns an array of indices (IndicesArray).
   */
  IndicesArray getIndices(bool copyWhenShared = false) override;

  std::vector<Vector3> _positions();

  /**
   * @brief Sets a new updated BoundingInfo to the mesh.
   * @returns The mesh.
   */
  InstancedMesh& refreshBoundingInfo();

  void _preActivate() override;

  void _activate(int renderId) override;

  /**
   * @brief Returns the current associated LOD AbstractMesh.
   */
  AbstractMesh* getLOD(Camera* camera,
                       BoundingSphere* boundingSphere = nullptr) override;

  InstancedMesh& _syncSubMeshes();

  bool _generatePointsArray() override;

  /**
   * @brief Creates a new InstancedMesh from the current mesh.
   * @param name (string) : the cloned mesh name
   * @param  newParent (optional Node) : the optional Node to parent the clone
   * to.
   * @param  doNotCloneChildren (optional boolean, default `false`) : if `true`
   * the model children aren't cloned.
   * @returns The clone.
   */
  InstancedMesh* clone(const std::string& name, Node* newParent,
                       bool doNotCloneChildren = false);

  /**
   * @brief Disposes the InstancedMesh.
   */
  void dispose(bool doNotRecurse = false) override;

protected:
  InstancedMesh(const std::string& name, Mesh* source);

private:
  Mesh* _sourceMesh;
  Mesh* _currentLOD;

}; // end of class InstancedMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_INSTANCED_MESH_H
