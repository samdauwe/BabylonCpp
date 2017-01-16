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
  Vector3 p;
  std::vector<size_t> e;
  std::vector<size_t> f;
}; // end of struct IcoNode

struct BABYLON_SHARED_EXPORT IcoEdge {
  IcoEdge(const std::vector<size_t>& n);
  std::vector<size_t> n;
  std::vector<size_t> f;
  std::vector<size_t> subdivided_n;
  std::vector<size_t> subdivided_e;
}; // end of struct IcoEdge

struct BABYLON_SHARED_EXPORT IcoFace {
  IcoFace(const std::vector<size_t>& n, const std::vector<size_t>& e);
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
}; // end of struct IcosahedronMesh

struct IRandomFunction;

class BABYLON_SHARED_EXPORT Icosphere {

private:
  using RotationPredicateType = std::function<bool(
    const IcoNode&, const IcoNode&, const IcoNode&, const IcoNode&)>;

public:
  Icosphere();
  Icosphere(size_t icosahedronSubdivision, float topologyDistortionRate);

  void generateIcosahedron();
  void generateSubdividedIcosahedron(size_t degree);
  void correctFaceIndices();
  Vector3 calculateFaceCentroid(const Vector3& pa, const Vector3& pb,
                                const Vector3& pc) const;
  size_t getEdgeOppositeFaceIndex(const IcoEdge& edge, size_t faceIndex) const;
  size_t getFaceOppositeNodeIndex(const IcoFace& face,
                                  const IcoEdge& edge) const;
  bool conditionalRotateEdge(IcosahedronMesh& icosahedronMesh, size_t edgeIndex,
                             RotationPredicateType& predicate);
  size_t findNextFaceIndex(size_t nodeIndex, size_t faceIndex) const;
  bool distortMesh(size_t degree, IRandomFunction& random);
  float relaxMesh(float multiplier);
  IcosahedronMesh& icosahedron();

private:
  /** Project a vector onto the plane. This  gives you the element of the input
   * vector that is perpendicularto the normal of the plane.
   */
  Vector3 _projectVectorOnPlane(const Vector3& v,
                                const Vector3& rkNormal) const;

private:
  size_t _icosahedronSubdivision;
  float _topologyDistortionRate;
  IcosahedronMesh _icosahedronMesh;

}; // end of class Icosphere

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_ICOSPHERE_H
