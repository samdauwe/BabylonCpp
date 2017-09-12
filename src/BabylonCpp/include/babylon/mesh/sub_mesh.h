#ifndef BABYLON_MESH_SUB_MESH_H
#define BABYLON_MESH_SUB_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/culling/icullable.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>
#include <babylon/mesh/base_sub_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT SubMesh : public BaseSubMesh,
                                      public ICullable,
                                      public IDisposable {

public:
  template <typename... Ts>
  static SubMesh* New(Ts&&... args)
  {
    auto subMesh = new SubMesh(::std::forward<Ts>(args)...);
    subMesh->addToMesh(static_cast<std::unique_ptr<SubMesh>>(subMesh));

    return subMesh;
  }
  virtual ~SubMesh();

  void addToMesh(std::unique_ptr<SubMesh>&& newSubMesh);
  bool isGlobal() const;

  /**
   * @brief Returns the submesh BoudingInfo object.
   */
  BoundingInfo* getBoundingInfo() const;

  /**
   * @brief Sets the submesh BoundingInfo.
   * @returns The SubMesh.
   */
  SubMesh& setBoundingInfo(const BoundingInfo& boundingInfo);

  /**
   * @brief Returns the mesh of the current submesh.
   */
  AbstractMesh* getMesh();

  /**
   * @brief Returns the rendering mesh of the submesh.
   */
  Mesh* getRenderingMesh();

  /**
   * @brief Returns the submesh material.
   */
  Material* getMaterial();

  /** Methods **/

  /**
   * @brief Sets a new updated BoundingInfo object to the submesh.
   * @returns The SubMesh.
   */
  SubMesh& refreshBoundingInfo();

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
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes) override;

  /**
   * @brief Returns if the submesh bounding box is completely inside the frustum
   * defined by the passed array of planes.
   */
  bool isCompletelyInFrustum(
    const std::array<Plane, 6>& frustumPlanes) const override;

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
  std::unique_ptr<IntersectionInfo>
  intersects(Ray& ray, const std::vector<Vector3>& positions,
             const Uint32Array& indices, bool fastCheck);

  /** Clone **/

  /**
   * @brief Creates a new Submesh from the passed Mesh.
   */
  SubMesh* clone(AbstractMesh* newMesh, Mesh* newRenderingMesh) const;

  /** Dispose **/

  /**
   * @brief Disposes the Submesh.
   */
  void dispose(bool doNotRecurse = false) override;

  /** Statics **/

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
  static SubMesh* CreateFromIndices(unsigned int materialIndex,
                                    unsigned int startIndex, size_t indexCount,
                                    AbstractMesh* mesh,
                                    Mesh* renderingMesh = nullptr);

protected:
  SubMesh(unsigned int materialIndex, unsigned int verticesStart,
          size_t verticesCount, unsigned int indexStart, size_t indexCount,
          AbstractMesh* mesh, Mesh* renderingMesh = nullptr,
          bool createBoundingBox = true);

public:
  unsigned int materialIndex;
  unsigned int verticesStart;
  size_t verticesCount;
  unsigned int indexStart;
  size_t indexCount;
  bool createBoundingBox;
  size_t linesIndexCount;
  std::vector<Vector3> _lastColliderWorldVertices;
  std::vector<Plane> _trianglePlanes;
  Matrix _lastColliderTransformMatrix;
  int _renderId;
  int _alphaIndex;
  float _distanceToCamera;
  size_t _id;

private:
  AbstractMesh* _mesh;
  Mesh* _renderingMesh;
  std::unique_ptr<BoundingInfo> _boundingInfo;
  std::unique_ptr<GL::IGLBuffer> _linesIndexBuffer;
  Material* _currentMaterial;

}; // end of class SubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SUB_MESH_H
