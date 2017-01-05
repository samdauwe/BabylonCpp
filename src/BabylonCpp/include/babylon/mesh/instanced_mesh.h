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

  IReflect::Type type() const override;

  /** Methods **/
  bool receiveShadows() const;
  Material* material() const;
  float visibility() const;
  Skeleton* skeleton() override;
  unsigned int renderingGroupId() const;
  size_t getTotalVertices() const override;
  Mesh* sourceMesh() const;
  Float32Array getVerticesData(unsigned int kind,
                               bool copyWhenShared = false) override;
  bool isVerticesDataPresent(unsigned int kind) override;
  Uint32Array getIndices(bool copyWhenShared = false) override;
  std::vector<Vector3> _positions();
  void refreshBoundingInfo();
  void _preActivate() override;
  void _activate(int renderId) override;
  AbstractMesh* getLOD(Camera* camera,
                       BoundingSphere* boundingSphere = nullptr) override;
  void _syncSubMeshes();
  bool _generatePointsArray() override;

  /** Clone **/
  InstancedMesh* clone(const std::string& name, Node* newParent,
                       bool doNotCloneChildren);

  /** Dispose **/
  void dispose(bool doNotRecurse = false) override;

protected:
  InstancedMesh(const std::string& name, Mesh* source);

private:
  Mesh* _sourceMesh;
  Mesh* _currentLOD;

}; // end of class InstancedMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_INSTANCED_MESH_H
