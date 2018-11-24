#ifndef BABYLON_MESH_CSG_CSG_H
#define BABYLON_MESH_CSG_CSG_H

#include <babylon/babylon_api.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/csg/polygon.h>

namespace BABYLON {

class Material;
class Mesh;
class Scene;
using MaterialPtr = std::shared_ptr<Material>;
using MeshPtr     = std::shared_ptr<Mesh>;

namespace CSG {

class CSG;
using CSGPtr = std::unique_ptr<CSG>;

/**
 * @brief Class for building Constructive Solid Geometry.
 */
class BABYLON_SHARED_EXPORT CSG {

public:
  CSG();
  CSG(const CSG& otherCSG);
  ~CSG();

  /**
   * @brief Convert the BABYLON.Mesh to BABYLON.CSG.
   * @param mesh The BABYLON.Mesh to convert to BABYLON.CSG
   * @returns A new BABYLON.CSG from the BABYLON.Mesh
   */
  static CSGPtr FromMesh(const MeshPtr& mesh);

  /**
   * @brief Clones, or makes a deep copy, of the BABYLON.CSG.
   * @returns A new BABYLON.CSG
   */
  CSGPtr clone() const;

  /**
   * @brief Unions this CSG with another CSG.
   * @param csg The CSG to union against this CSG
   * @returns The unioned CSG
   */
  CSG _union(const CSGPtr& csg);

  /**
   * @brief Unions this CSG with another CSG in place.
   * @param csg The CSG to union against this CSG
   */
  void unionInPlace(const CSGPtr& csg);

  /**
   * @brief Subtracts this CSG with another CSG.
   * @param csg The CSG to subtract against this CSG
   * @returns A new BABYLON.CSG
   */
  CSG subtract(const CSGPtr& csg);

  /**
   * @brief Subtracts this CSG with another CSG in place.
   * @param csg The CSG to subtact against this CSG
   */
  void subtractInPlace(const CSGPtr& csg);

  /**
   * @brief Intersect this CSG with another CSG.
   * @param csg The CSG to intersect against this CSG
   * @returns A new BABYLON.CSG
   */
  CSG intersect(const CSGPtr& csg);

  /**
   * @brief Intersects this CSG with another CSG in place.
   * @param csg The CSG to intersect against this CSG
   */
  void intersectInPlace(const CSGPtr& csg);

  /**
   * @brief Return a new BABYLON.CSG solid with solid and empty space switched.
   * This solid is not modified.
   * @returns A new BABYLON.CSG solid with solid and empty space switched
   */
  CSGPtr inverse();

  /**
   * @brief Inverses the BABYLON.CSG in place.
   */
  void inverseInPlace();

  /**
   * @brief This is used to keep meshes transformations so they can be restored
   * when we build back a Babylon Mesh.
   * NB : All CSG operations are performed in world coordinates
   * @param csg The BABYLON.CSG to copy the transform attributes from
   * @returns This BABYLON.CSG
   */
  CSG& copyTransformAttributes(const CSG& csg);

  /**
   * @brief Build Raw mesh from CSG.
   * Coordinates here are in world space
   * @param name The name of the mesh geometry
   * @param scene The BABYLON.Scene
   * @param keepSubMeshes Specifies if the submeshes should be kept
   * @returns A new BABYLON.Mesh
   */
  MeshPtr buildMeshGeometry(const std::string& name, Scene* scene,
                            bool keepSubMeshes);

  /**
   * @brief Build Mesh from CSG taking material and transforms into account.
   * @param name The name of the BABYLON.Mesh
   * @param material The material of the BABYLON.Mesh
   * @param scene The BABYLON.Scene
   * @param keepSubMeshes Specifies if submeshes should be kept
   * @returns The new BABYLON.Mesh
   */
  MeshPtr toMesh(const std::string& name, const MaterialPtr& material,
                 Scene* scene, bool keepSubMeshes = false);

private:
  /**
   * @brief Construct a BABYLON.CSG solid from a list of `BABYLON.CSG.Polygon`.
   * instances.
   * @param polygons Polygons used to construct a BABYLON.CSG solid
   */
  static CSGPtr FromPolygons(const std::vector<Polygon>& polygons);

public:
  /**
   * The world matrix
   */
  Matrix matrix;

  /**
   * Stores the position
   */
  Vector3 position;

  /**
   * Stores the rotation
   */
  Vector3 rotation;

  /**
   * Stores the rotation quaternion
   */
  std::optional<Quaternion> rotationQuaternion;

  /**
   * Stores the scaling vector
   */
  Vector3 scaling;

private:
  static unsigned int currentCSGMeshId;
  std::vector<Polygon> _polygons;

}; // end of class CSG

} // end of namespace CSG
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_CSG_CSG_H
