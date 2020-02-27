#ifndef BABYLON_MESHES_SUB_MESH_H
#define BABYLON_MESHES_SUB_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/culling/icullable.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/plane.h>
#include <babylon/meshes/base_sub_mesh.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

class IntersectionInfo;
class SubMesh;
class WebGLDataBuffer;
using SubMeshPtr         = std::shared_ptr<SubMesh>;
using WebGLDataBufferPtr = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT SubMesh : public BaseSubMesh, public ICullable {

public:
  using TrianglePickingPredicate
    = std::function<bool(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Ray& ray)>;

public:
  template <typename... Ts>
  static SubMeshPtr New(Ts&&... args)
  {
    auto subMesh = std::shared_ptr<SubMesh>(new SubMesh(std::forward<Ts>(args)...));
    subMesh->addToMesh(subMesh);

    return subMesh;
  }
  ~SubMesh() override; // = default

  void addToMesh(const std::shared_ptr<SubMesh>& newSubMesh);
  [[nodiscard]] bool isGlobal() const;

  /**
   * @brief Returns the submesh BoudingInfo object.
   */
  BoundingInfoPtr& getBoundingInfo();

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
   * @param data defines an optional position array to use to determine the bounding info
   * @returns the SubMesh
   */
  SubMesh& refreshBoundingInfo(const Float32Array& data = {});

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
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes, unsigned int strategy = 0) override;

  /**
   * @brief Returns if the submesh bounding box is completely inside the frustum
   * defined by the passed array of planes.
   */
  bool isCompletelyInFrustum(const std::array<Plane, 6>& frustumPlanes) override;

  /**
   * @brief Renders the submesh.
   * @returns The Submesh.
   */
  SubMesh& render(bool enableAlphaMode);

  /**
   * @brief Returns a new Index Buffer.
   * @returns The WebGLBuffer.
   */
  WebGLDataBufferPtr& _getLinesIndexBuffer(const IndicesArray& indices, Engine* engine);

  /**
   * @brief Returns if the passed Ray intersects the submesh bounding box.
   */
  bool canIntersects(const Ray& ray);

  /**
   * @brief Intersects current submesh with a ray.
   * @param ray defines the ray to test
   * @param positions defines mesh's positions array
   * @param indices defines mesh's indices array
   * @param fastCheck defines if only bounding info should be used
   * @param trianglePredicate defines an optional predicate used to select faces when a mesh
   * intersection is detected
   * @returns intersection info or null if no intersection
   */
  std::optional<IntersectionInfo>
  intersects(Ray& ray, const std::vector<Vector3>& positions, const IndicesArray& indices,
             bool fastCheck = false, const TrianglePickingPredicate& trianglePredicate = nullptr);

  /**
   * @brief Hidden
   */
  void _rebuild();

  /** Clone **/

  /**
   * @brief Creates a new Submesh from the passed Mesh.
   */
  SubMeshPtr clone(const AbstractMeshPtr& newMesh, const MeshPtr& newRenderingMesh);

  /** Dispose **/

  /**
   * @brief Disposes the Submesh.
   */
  void dispose();

  /**
   * @brief Gets the class name
   * @returns the string "SubMesh".
   */
  [[nodiscard]] std::string getClassName() const;

  /** Statics **/

  static SubMeshPtr AddToMesh(unsigned int materialIndex, unsigned int verticesStart,
                              size_t verticesCount, unsigned int indexStart, size_t indexCount,
                              const AbstractMeshPtr& mesh, const MeshPtr& renderingMesh = nullptr,
                              bool createBoundingBox = true);

  /**
   * @brief Creates a new Submesh from the passed parameters.
   * @param materialIndex (integer) : the index of the main mesh material.
   * @param startIndex (integer) : the index where to start the copy in the mesh indices array.
   * @param indexCount (integer) : the number of indices to copy then from the startIndex.
   * @param mesh (Mesh) : the main mesh to create the submesh from.
   * @param renderingMesh (optional Mesh) : rendering mesh.
   * @return The created SubMesh object.
   */
  static SubMeshPtr CreateFromIndices(unsigned int materialIndex, unsigned int startIndex,
                                      size_t indexCount, const AbstractMeshPtr& mesh,
                                      const MeshPtr& renderingMesh = nullptr);

protected:
  SubMesh(unsigned int materialIndex, unsigned int verticesStart, size_t verticesCount,
          unsigned int indexStart, size_t indexCount, const AbstractMeshPtr& mesh,
          const MeshPtr& renderingMesh = nullptr, bool createBoundingBox = true);

private:
  /** Hidden */
  std::optional<IntersectionInfo> _intersectLines(Ray& ray, const std::vector<Vector3>& positions,
                                                  const IndicesArray& indices,
                                                  float intersectionThreshold,
                                                  bool fastCheck = false);
  /** Hidden */
  std::optional<IntersectionInfo> _intersectUnIndexedLines(Ray& ray,
                                                           const std::vector<Vector3>& positions,
                                                           const IndicesArray& indices,
                                                           float intersectionThreshold,
                                                           bool fastCheck = false);
  /** Hidden */
  std::optional<IntersectionInfo>
  _intersectTriangles(Ray& ray, const std::vector<Vector3>& positions, const IndicesArray& indices,
                      unsigned int step, bool checkStopper, bool fastCheck = false,
                      const TrianglePickingPredicate& trianglePredicate = nullptr);
  /** Hidden */
  std::optional<IntersectionInfo>
  _intersectUnIndexedTriangles(Ray& ray, const std::vector<Vector3>& positions,
                               const IndicesArray& indices, bool fastCheck = false,
                               const TrianglePickingPredicate& trianglePredicate = nullptr);

public:
  /** the material index to use */
  unsigned int materialIndex;
  /** vertex index start */
  unsigned int verticesStart;
  /** vertices count */
  size_t verticesCount;
  /** index start */
  unsigned int indexStart;
  /** indices count */
  size_t indexCount;
  bool createBoundingBox;
  size_t _linesIndexCount;
  /** Hidden */
  std::vector<Vector3> _lastColliderWorldVertices;
  /** Hidden */
  std::vector<Plane> _trianglePlanes;
  /** Hidden */
  std::unique_ptr<Matrix> _lastColliderTransformMatrix;
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
  BoundingInfoPtr _boundingInfo;
  WebGLDataBufferPtr _linesIndexBuffer;
  MaterialPtr _currentMaterial;

}; // end of class SubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_SUB_MESH_H
