#ifndef BABYLON_MESH_CSG_VERTEX_H
#define BABYLON_MESH_CSG_VERTEX_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace CSG {

/**
 * @brief class Vertex
 * Represents a vertex of a polygon. Use your own vertex class instead of
 * this one to provide additional features like texture coordinates and vertex
 * colors. Custom vertex classes need to provide a `pos` property and `clone()`,
 * `flip()`, and `interpolate()` methods that behave analogous to the ones
 * defined by `BABYLON.CSG.Vertex`. This class provides `normal` so convenience
 * functions like `BABYLON.CSG.sphere()` can return a smooth vertex normal, but
 * `normal` is not used anywhere else. Same goes for uv, it allows to keep the
 * original vertex uv coordinates of the 2 meshes
 */
class BABYLON_SHARED_EXPORT Vertex {

public:
  Vertex(const Vector3& pos, const Vector3& normal, const Vector2& uv);
  Vertex(const Vertex& otherVertex);
  Vertex(Vertex&& otherVertex);
  Vertex& operator=(const Vertex& otherVertex);
  Vertex& operator=(Vertex&& otherVertex);
  ~Vertex();
  Vertex clone() const;
  std::unique_ptr<Vertex> cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex);
  std::string toString() const;

  // Invert all orientation-specific data (e.g. vertex normal). Called when the
  // orientation of a polygon is flipped.
  void flip();

  // Create a new vertex between this vertex and `other` by linearly
  // interpolating all properties using a parameter of `t`. Subclasses should
  // override this to interpolate additional properties.
  Vertex interpolate(const Vertex& other, float t);

public:
  Vector3 pos;
  Vector3 normal;
  Vector2 uv;

}; // end of class Vertex

} // end of namespace CSG
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_CSG_VERTEX_H
