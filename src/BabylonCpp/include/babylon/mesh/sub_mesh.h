#ifndef BABYLON_MESH_SUB_MESH_H
#define BABYLON_MESH_SUB_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/culling/icullable.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT SubMesh : public ICullable, public IDisposable {

public:
  template <typename... Ts>
  static SubMesh* New(Ts&&... args)
  {
    auto subMesh = new SubMesh(std::forward<Ts>(args)...);
    subMesh->addToMesh(static_cast<std::unique_ptr<SubMesh>>(subMesh));

    return subMesh;
  }
  virtual ~SubMesh();

  void addToMesh(std::unique_ptr<SubMesh>&& newSubMesh);
  bool isGlobal() const;
  BoundingInfo* getBoundingInfo() const;
  AbstractMesh* getMesh();
  Mesh* getRenderingMesh();
  Material* getMaterial();

  /** Methods **/
  void refreshBoundingInfo();
  bool _checkCollision(const Collider& collider);
  void updateBoundingInfo(const Matrix& world);
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes) override;
  bool isCompletelyInFrustum(
    const std::array<Plane, 6>& frustumPlanes) const override;
  void render(bool enableAlphaMode);
  GL::IGLBuffer* getLinesIndexBuffer(const Uint32Array& indices,
                                     Engine* engine);
  bool canIntersects(const Ray& ray) const;
  std::unique_ptr<IntersectionInfo>
  intersects(Ray& ray, const std::vector<Vector3>& positions,
             const Uint32Array& indices, bool fastCheck);

  /** Clone **/
  SubMesh* clone(AbstractMesh* newMesh, Mesh* newRenderingMesh) const;

  /** Dispose **/
  void dispose(bool doNotRecurse = false) override;

  /** Statics **/
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

}; // end of class SubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SUB_MESH_H
