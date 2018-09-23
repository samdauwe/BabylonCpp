#ifndef BABYLON_MESH_CSG_CSG_H
#define BABYLON_MESH_CSG_CSG_H

#include <babylon/babylon_api.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Material;
class Mesh;
class Scene;
using MaterialPtr = std::shared_ptr<Material>;
using MeshPtr     = std::shared_ptr<Mesh>;

namespace CSG {

class Polygon;

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT CSG {

public:
  CSG();
  ~CSG();

  // Convert BABYLON.Mesh to BABYLON.CSG
  static std::unique_ptr<CSG> FromMesh(Mesh* mesh);

  std::unique_ptr<CSG> clone() const;
  CSG& _union(const CSG& csg);
  void unionInPlace(CSG* csg);
  CSG& subtract(const CSG& csg);
  void subtractInPlace(CSG* csg);
  CSG& intersect(const CSG& csg);
  void intersectInPlace(CSG* csg);

  // Return a new BABYLON.CSG solid with solid and empty space switched. This
  // solid is
  // not modified.
  std::unique_ptr<CSG> inverse();

  void inverseInPlace();

  // This is used to keep meshes transformations so they can be restored
  // when we build back a Babylon Mesh
  // NB : All CSG operations are performed in world coordinates
  CSG& copyTransformAttributes(const CSG& csg);

  // Build Raw mesh from CSG
  // Coordinates here are in world space
  MeshPtr buildMeshGeometry(const std::string& name, Scene* scene,
                            bool keepSubMeshes);

  // Build Mesh from CSG taking material and transforms into account
  MeshPtr toMesh(const std::string& name, const MaterialPtr& material,
                 Scene* scene, bool keepSubMeshes = false);

private:
  // Construct a BABYLON.CSG solid from a list of `BABYLON.CSG.Polygon`
  // instances.
  static std::unique_ptr<CSG>
  FromPolygons(const std::vector<Polygon>& polygons);

public:
  Matrix matrix;
  Vector3 position;
  std::optional<Vector3> rotation;
  std::optional<Quaternion> rotationQuaternion;
  Vector3 scaling;

private:
  static unsigned int currentCSGMeshId;
  std::vector<Polygon> _polygons;

}; // end of class CSG

} // end of namespace CSG
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_CSG_CSG_H
