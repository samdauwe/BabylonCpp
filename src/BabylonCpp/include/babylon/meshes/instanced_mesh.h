#ifndef BABYLON_MESHES_INSTANCED_MESH_H
#define BABYLON_MESHES_INSTANCED_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

class InstancedMesh;
class Mesh;
using InstancedMeshPtr = std::shared_ptr<InstancedMesh>;
using MeshPtr          = std::shared_ptr<Mesh>;

/**
 * @brief Creates an instance based on a source mesh.
 */
class BABYLON_SHARED_EXPORT InstancedMesh : public AbstractMesh {

public:
  template <typename... Ts>
  static InstancedMeshPtr New(Ts&&... args)
  {
    auto mesh = std::shared_ptr<InstancedMesh>(new InstancedMesh(std::forward<Ts>(args)...));
    mesh->addToScene(mesh);
    mesh->syncSubMeshes(mesh);

    return mesh;
  }
  ~InstancedMesh() override; // = default

  void syncSubMeshes(const InstancedMeshPtr& instancedMesh);

  /**
   * @brief Returns the string "InstancedMesh"
   */
  std::string getClassName() const override;

  Type type() const override;

  /** Methods **/

  void _resyncLightSources();
  void _resyncLighSource(const LightPtr& light);
  void _removeLightSource(const LightPtr& light, bool dispose = false);

  /**
   * @brief Returns the total number of vertices (integer).
   */
  size_t getTotalVertices() const override;

  /**
   * @brief Returns a positive integer : the total number of indices in this
   * mesh geometry.
   * @returns the numner of indices or zero if the mesh has no geometry.
   */
  size_t getTotalIndices() const;

  /**
   * @brief Creates a new InstancedMesh object from the mesh model.
   * @see https://doc.babylonjs.com/how_to/how_to_use_instances
   * @param name defines the name of the new instance
   * @returns a new InstancedMesh
   */
  InstancedMeshPtr createInstance(const std::string& name);

  /**
   * @brief Is this node ready to be used/rendered.
   * @param completeCheck defines if a complete check (including materials and
   * lights) has to be done (false by default)
   * @return {boolean} is it ready
   */
  bool isReady(bool completeCheck = false, bool forceInstanceSupport = false) override;

  /**
   * @brief Returns an array of integers or a typed array (Int32Array,
   * Uint32Array, Uint16Array) populated with the mesh indices.
   * @param kind kind of verticies to retreive (eg. positons, normals, uvs,
   * etc.)
   * @param copyWhenShared If true (default false) and and if the mesh geometry
   * is shared among some other meshes, the returned array is a copy of the
   * internal one.
   * @returns a float array or a Float32Array of the requested kind of data :
   * positons, normals, uvs, etc.
   */
  Float32Array getVerticesData(const std::string& kind, bool copyWhenShared = false,
                               bool forceCopy = false) override;

  /**
   * @brief Sets the vertex data of the mesh geometry for the requested `kind`.
   * If the mesh has no geometry, a new Geometry object is set to the mesh and
   * then passed this vertex data.
   * The `data` are either a numeric array either a Float32Array.
   * The parameter `updatable` is passed as is to the underlying Geometry object
   * constructor (if initianilly none) or updater.
   * The parameter `stride` is an optional positive integer, it is usually
   * automatically deducted from the `kind` (3 for positions or normals, 2 for
   * UV, etc).
   * Note that a new underlying VertexBuffer object is created each call.
   * If the `kind` is the `PositionKind`, the mesh BoundingInfo is renewed, so
   * the bounding box and sphere, and the mesh World Matrix is recomputed.
   *
   * Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   *
   * @returns The Mesh.
   */
  AbstractMesh* setVerticesData(const std::string& kind, const Float32Array& data,
                                bool updatable                      = false,
                                const std::optional<size_t>& stride = std::nullopt) override;

  /**
   * @brief Updates the existing vertex data of the mesh geometry for the
   * requested `kind`.
   * If the mesh has no geometry, it is simply returned as it is.
   * The `data` are either a numeric array either a Float32Array.
   * No new underlying VertexBuffer object is created.
   * If the `kind` is the `PositionKind` and if `updateExtends` is true, the
   * mesh BoundingInfo is renewed, so the bounding box and sphere, and the mesh
   * World Matrix is recomputed.
   * If the parameter `makeItUnique` is true, a new global geometry is created
   * from this positions and is set to the mesh.
   *
   * Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   *
   * @returns The Mesh.
   */
  AbstractMesh* updateVerticesData(const std::string& kind, const Float32Array& data,
                                   bool updateExtends = false, bool makeItUnique = false) override;

  /**
   * @brief Sets the mesh indices.
   * Expects an array populated with integers or a typed array (Int32Array,
   * Uint32Array, Uint16Array).
   * If the mesh has no geometry, a new Geometry object is created and set to
   * the mesh.
   * This method creates a new index buffer each call.
   *
   * @returns The Mesh.
   */
  AbstractMesh* setIndices(const IndicesArray& indices, size_t totalVertices = 0,
                           bool updatable = false) override;

  /**
   * @brief Returns if the mesh owns the requested kind of data.
   */
  bool isVerticesDataPresent(const std::string& kind) const override;

  /**
   * @brief Returns an array of indices (IndicesArray).
   */
  IndicesArray getIndices(bool copyWhenShared = false, bool forceCopy = false) override;

  /**
   * @brief Hidden
   */
  std::vector<Vector3>& _positions();

  /**
   * @brief This method recomputes and sets a new BoundingInfo to the mesh
   * unless it is locked. This means the mesh underlying bounding box and sphere
   * are recomputed.
   * @param applySkeleton defines whether to apply the skeleton before computing
   * the bounding info
   * @returns the current mesh
   */
  InstancedMesh& refreshBoundingInfo(bool applySkeleton = false);

  /**
   * @brief Hidden
   */
  void _preActivate() override;

  /**
   * @brief Hidden
   */
  bool _activate(int renderId, bool intermediateRendering) override;

  /**
   * @brief Hidden
   */
  void _postActivate() override;

  Matrix& getWorldMatrix() override;

  bool isAnInstance() const;

  /**
   * @brief Returns the current associated LOD AbstractMesh.
   */
  AbstractMesh* getLOD(const CameraPtr& camera, BoundingSphere* boundingSphere = nullptr) override;

  /**
   * @brief Hidden
   */
  Mesh* _preActivateForIntermediateRendering(int renderId) override;

  /**
   * @brief Hidden
   */
  InstancedMesh& _syncSubMeshes();

  /**
   * @brief Hidden
   */
  bool _generatePointsArray() override;

  /**
   * @brief Hidden
   */
  AbstractMesh& _updateBoundingInfo();

  /**
   * @brief Creates a new InstancedMesh from the current mesh.
   * @param name (string) : the cloned mesh name
   * @param  newParent (optional Node) : the optional Node to parent the clone to.
   * @param  doNotCloneChildren (optional boolean, default `false`) : if `true` the model children
   * aren't cloned.
   * @returns The clone.
   */
  InstancedMeshPtr clone(const std::string& name, Node* newParent, bool doNotCloneChildren = false);

  /**
   * @brief Disposes the InstancedMesh.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  InstancedMesh(const std::string& name, const MeshPtr& source);

  /**
   * @brief Gets if the source mesh receives shadows.
   */
  bool get_receiveShadows() const override;

  /**
   * @brief Gets the material of the source mesh.
   */
  MaterialPtr& get_material() override;

  /**
   * @brief Gets the visibility of the source mesh.
   */
  float get_visibility() const override;

  /**
   * @brief Gets the skeleton of the source mesh.
   */
  SkeletonPtr& get_skeleton() override;

  /**
   * @brief Gets the rendering ground id of the source mesh.
   */
  int get_renderingGroupId() const override;

  /**
   * @brief Sets the rendering ground id of the source mesh.
   */
  void set_renderingGroupId(int value) override;

  /**
   * @brief Gets the source mesh of the instance.
   */
  MeshPtr& get_sourceMesh();

public:
  /** Hidden */
  int _indexInSourceMeshInstanceArray;

  /**
   * The source mesh of the instance.
   */
  ReadOnlyProperty<InstancedMesh, MeshPtr> sourceMesh;

private:
  MeshPtr _sourceMesh;
  Mesh* _currentLOD;

}; // end of class InstancedMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_INSTANCED_MESH_H
