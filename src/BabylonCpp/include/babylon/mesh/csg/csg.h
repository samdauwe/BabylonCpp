#ifndef BABYLON_MESH_CSG_CSG_H
#define BABYLON_MESH_CSG_CSG_H

#include <babylon/babylon_global.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace CSG {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT CSG {

public:
  CSG();
  ~CSG();

  // Convert BABYLON.Mesh to BABYLON.CSG
  static unique_ptr_t<CSG> FromMesh(Mesh* mesh);

  unique_ptr_t<CSG> clone() const;
  CSG& _union(const CSG& csg);
  void unionInPlace(CSG* csg);
  CSG& subtract(const CSG& csg);
  void subtractInPlace(CSG* csg);
  CSG& intersect(const CSG& csg);
  void intersectInPlace(CSG* csg);

  // Return a new BABYLON.CSG solid with solid and empty space switched. This
  // solid is
  // not modified.
  unique_ptr_t<CSG> inverse();

  void inverseInPlace();

  // This is used to keep meshes transformations so they can be restored
  // when we build back a Babylon Mesh
  // NB : All CSG operations are performed in world coordinates
  CSG& copyTransformAttributes(const CSG& csg);

  // Build Raw mesh from CSG
  // Coordinates here are in world space
  MeshPtr buildMeshGeometry(const string_t& name, Scene* scene,
                            bool keepSubMeshes);

  // Build Mesh from CSG taking material and transforms into account
  MeshPtr toMesh(const string_t& name, const MaterialPtr& material,
                 Scene* scene, bool keepSubMeshes = false);

private:
  // Construct a BABYLON.CSG solid from a list of `BABYLON.CSG.Polygon`
  // instances.
  static unique_ptr_t<CSG> FromPolygons(const vector_t<Polygon>& polygons);

public:
  Matrix matrix;
  Vector3 position;
  Nullable<Vector3> rotation;
  Nullable<Quaternion> rotationQuaternion;
  Vector3 scaling;

private:
  static unsigned int currentCSGMeshId;
  vector_t<Polygon> _polygons;

}; // end of class CSG

} // end of namespace CSG
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_CSG_CSG_H
