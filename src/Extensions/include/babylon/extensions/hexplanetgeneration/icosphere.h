#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_ICOSPHERE_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_ICOSPHERE_H

#include <babylon/babylon_global.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct Tile;

struct BABYLON_SHARED_EXPORT IcoNode {
  IcoNode(const Vector3& p);
  IcoNode(const Vector3& p, const std::vector<size_t>& e);
  IcoNode(const IcoNode& other);
  IcoNode(IcoNode&& other);
  IcoNode& operator=(const IcoNode& other);
  IcoNode& operator=(IcoNode&& other);
  Vector3 p;
  std::vector<size_t> e;
  std::vector<size_t> f;
}; // end of struct IcoNode

struct BABYLON_SHARED_EXPORT IcoEdge {
  IcoEdge(const std::vector<size_t>& n);
  IcoEdge(const IcoEdge& other);
  IcoEdge(IcoEdge&& other);
  IcoEdge& operator=(const IcoEdge& other);
  IcoEdge& operator=(IcoEdge&& other);
  std::vector<size_t> n;
  std::vector<size_t> f;
  std::vector<size_t> subdivided_n;
  std::vector<size_t> subdivided_e;
}; // end of struct IcoEdge

struct BABYLON_SHARED_EXPORT IcoFace {
  IcoFace(const std::vector<size_t>& n, const std::vector<size_t>& e);
  IcoFace(const IcoFace& other);
  IcoFace(IcoFace&& other);
  IcoFace& operator=(const IcoFace& other);
  IcoFace& operator=(IcoFace&& other);
  std::vector<size_t> n;
  std::vector<size_t> e;
  Vector3 centroid;
  BoundingSphere boundingSphere;
  std::vector<Tile*> children;
}; // end of struct IcoEdge

struct BABYLON_SHARED_EXPORT IcosahedronMesh {
  std::vector<IcoNode> nodes;
  std::vector<IcoEdge> edges;
  std::vector<IcoFace> faces;
  IcosahedronMesh();
  IcosahedronMesh(const IcosahedronMesh& other);
  IcosahedronMesh(IcosahedronMesh&& other);
  IcosahedronMesh& operator=(const IcosahedronMesh& other);
  IcosahedronMesh& operator=(IcosahedronMesh&& other);
}; // end of struct IcosahedronMesh

struct IRandomFunction;

struct BABYLON_SHARED_EXPORT Icosphere {

  using RotationPredicateType = std::function<bool(
    const IcoNode&, const IcoNode&, const IcoNode&, const IcoNode&)>;

  static IcosahedronMesh generateIcosahedronMesh(size_t icosahedronSubdivision,
                                                 float topologyDistortionRate,
                                                 IRandomFunction& random);
  static void generateIcosahedron(IcosahedronMesh& mesh);
  static void generateSubdividedIcosahedron(size_t degree,
                                            IcosahedronMesh& mesh);
  static void correctFaceIndices();
  static Vector3 calculateFaceCentroid(const Vector3& pa, const Vector3& pb,
                                       const Vector3& pc);
  static size_t getEdgeOppositeFaceIndex(const IcoEdge& edge, size_t faceIndex);
  static size_t getFaceOppositeNodeIndex(const IcoFace& face,
                                         const IcoEdge& edge);
  static bool conditionalRotateEdge(IcosahedronMesh& icosahedronMesh,
                                    size_t edgeIndex,
                                    RotationPredicateType& predicate);
  static size_t findNextFaceIndex(const IcosahedronMesh& icosahedronMesh,
                                  size_t nodeIndex, size_t faceIndex);
  static bool distortMesh(IcosahedronMesh& mesh, size_t degree,
                          IRandomFunction& random);
  static float relaxMesh(IcosahedronMesh& mesh, float multiplier);

}; // end of struct Icosphere

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_ICOSPHERE_H
