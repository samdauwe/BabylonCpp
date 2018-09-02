#ifndef BABYLON_MESH_SUB_MESH_H
#define BABYLON_MESH_SUB_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/culling/icullable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>
#include <babylon/mesh/base_sub_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT SubMesh : public BaseSubMesh, public ICullable {

public:
  template <typename... Ts>
  static shared_ptr_t<SubMesh> New(Ts&&... args)
  {
    auto subMeshRawPtr = new SubMesh(::std::forward<Ts>(args)...);
    auto subMesh       = static_cast<shared_ptr_t<SubMesh>>(subMeshRawPtr);
    subMesh->addToMesh(subMesh);

    return subMesh;
  }
  virtual ~SubMesh();

  void addToMesh(const shared_ptr_t<SubMesh>& newSubMesh);
  bool isGlobal() const;

  /**
   * @brief Returns the submesh BoudingInfo object.
   */
  BoundingInfo& getBoundingInfo() const;

  /**
   * @brief Sets the submesh BoundingInfo.
   * @returns The SubMesh.
   */
  SubMesh& setBoundingInfo(const BoundingInfo& boundingInfo);

  /**
   * @brief Returns the mesh of the current submesh.
   */
  AbstractMeshPtr& getMesh();

  /**
   * @brief Returns the rendering mesh of the submesh.
   */
  MeshPtr& getRenderingMesh();

  /**
   * @brief Returns the submesh material.
   */
  MaterialPtr getMaterial();

  /** Methods **/

  /**
   * @brief Sets a new updated BoundingInfo object to the submesh.
   * @returns The SubMesh.
   */
  SubMesh& refreshBoundingInfo();

  /**
   * @brief Hidden
   */
  bool _checkCollision(const Collider& collider);

  /**
   * @brief Updates the submesh BoundingInfo.
   * @returns The Submesh.
   */
  SubMesh& updateBoundingInfo(const Matrix& world);

  /**
   * @brief Returns if the submesh bounding box intersects the frustum defined
   * by the passed array of planes.
   */
  bool isInFrustum(const array_t<Plane, 6>& frustumPlanes,
                   unsigned int strategy = 0) override;

  /**
   * @brief Returns if the submesh bounding box is completely inside the frustum
   * defined by the passed array of planes.
   */
  bool
  isCompletelyInFrustum(const array_t<Plane, 6>& frustumPlanes) const override;

  /**
   * @brief Renders the submesh.
   * @returns The Submesh.
   */
  SubMesh& render(bool enableAlphaMode);

  /**
   * @brief Returns a new Index Buffer.
   * @returns The WebGLBuffer.
   */
  GL::IGLBuffer* getLinesIndexBuffer(const Uint32Array& indices,
                                     Engine* engine);

  /**
   * @brief Returns if the passed Ray intersects the submesh bounding box.
   */
  bool canIntersects(const Ray& ray) const;

  /**
   * @brief Returns an object IntersectionInfo.
   */
  unique_ptr_t<IntersectionInfo> intersects(Ray& ray,
                                            const vector_t<Vector3>& positions,
                                            const Uint32Array& indices,
                                            bool fastCheck);

  /**
   * @brief Hidden
   */
  void _rebuild();

  /** Clone **/

  /**
   * @brief Creates a new Submesh from the passed Mesh.
   */
  SubMeshPtr clone(const AbstractMeshPtr& newMesh,
                   Mesh* newRenderingMesh) const;

  /** Dispose **/

  /**
   * @brief Disposes the Submesh.
   */
  void dispose();

  /** Statics **/

  static SubMeshPtr AddToMesh(unsigned int materialIndex,
                              unsigned int verticesStart, size_t verticesCount,
                              unsigned int indexStart, size_t indexCount,
                              const AbstractMeshPtr& mesh,
                              Mesh* renderingMesh    = nullptr,
                              bool createBoundingBox = true);

  /**
   * @brief Creates a new Submesh from the passed parameters.
   * @param materialIndex (integer) : the index of the main mesh material.
   * @param startIndex (integer) : the index where to start the copy in the mesh
   * indices array.
   * @param indexCount (integer) : the number of indices to copy then from the
   * startIndex.
   * @param mesh (Mesh) : the main mesh to create the submesh from.
   * @param renderingMesh (optional Mesh) : rendering mesh.
   * @return The created SubMesh object.
   */
  static SubMeshPtr CreateFromIndices(unsigned int materialIndex,
                                      unsigned int startIndex,
                                      size_t indexCount,
                                      const AbstractMeshPtr& mesh,
                                      Mesh* renderingMesh = nullptr);

protected:
  SubMesh(unsigned int materialIndex, unsigned int verticesStart,
          size_t verticesCount, unsigned int indexStart, size_t indexCount,
          const AbstractMeshPtr& mesh, Mesh* renderingMesh = nullptr,
          bool createBoundingBox = true);

public:
  unsigned int materialIndex;
  unsigned int verticesStart;
  size_t verticesCount;
  unsigned int indexStart;
  size_t indexCount;
  bool createBoundingBox;
  size_t linesIndexCount;
  /** Hidden */
  vector_t<Vector3> _lastColliderWorldVertices;
  /** Hidden */
  vector_t<Plane> _trianglePlanes;
  /** Hidden */
  Matrix _lastColliderTransformMatrix;
  /** Hidden */
  int _renderId;
  /** Hidden */
  int _alphaIndex;
  /** Hidden */
  float _distanceToCamera;
  /** Hidden */
  size_t _id;

private:
  AbstractMeshPtr _mesh;
  MeshPtr _renderingMesh;
  unique_ptr_t<BoundingInfo> _boundingInfo;
  unique_ptr_t<GL::IGLBuffer> _linesIndexBuffer;
  MaterialPtr _currentMaterial;

}; // end of class SubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SUB_MESH_H
