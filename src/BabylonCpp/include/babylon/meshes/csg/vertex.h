#ifndef BABYLON_MESHES_CSG_VERTEX_H
#define BABYLON_MESHES_CSG_VERTEX_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace CSG {

/**
 * @brief Represents a vertex of a polygon. Use your own vertex class instead of
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
  /**
   * @brief Initializes the vertex.
   * @param pos The position of the vertex
   * @param normal The normal of the vertex
   * @param uv The texture coordinate of the vertex
   */
  Vertex(const Vector3& pos, const Vector3& normal, const Vector2& uv);
  Vertex(const Vertex& otherVertex);
  Vertex(Vertex&& otherVertex);
  Vertex& operator=(const Vertex& otherVertex);
  Vertex& operator=(Vertex&& otherVertex);
  ~Vertex();

  /**
   * @brief Make a clone, or deep copy, of the vertex.
   * @returns A new Vertex
   */
  Vertex clone() const;
  std::unique_ptr<Vertex> cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex);
  std::string toString() const;

  /**
   * @brief Invert all orientation-specific data (e.g. vertex normal). Called
   * when the orientation of a polygon is flipped.
   */
  void flip();

  /**
   * @brief Create a new vertex between this vertex and `other` by linearly
   * interpolating all properties using a parameter of `t`. Subclasses should
   * override this to interpolate additional properties.
   * @param other the vertex to interpolate against
   * @param t The factor used to linearly interpolate between the vertices
   */
  Vertex interpolate(const Vertex& other, float t);

public:
  /**
   * The position of the vertex
   */
  Vector3 pos;

  /**
   * The normal of the vertex
   */
  Vector3 normal;

  /**
   * The texture coordinate of the vertex
   */
  Vector2 uv;

}; // end of class Vertex

} // end of namespace CSG
} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_CSG_VERTEX_H
