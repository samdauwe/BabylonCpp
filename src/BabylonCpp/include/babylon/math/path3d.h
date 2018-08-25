#ifndef BABYLON_MATH_PATH3D_H
#define BABYLON_MATH_PATH3D_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Represents an Path3D.
 */
class BABYLON_SHARED_EXPORT Path3D {

public:
  Path3D();
  /**
   * @brief Creates a Path3D. A Path3D is a logical math object, so not a mesh.
   * path : an array of Vector3, the curve axis of the Path3D
   * normal (optional) : Vector3, the first wanted normal to the curve. Ex (0,
   *                     1, 0) for a vertical normal.
   * raw (optional, default false) : boolean, if true the returned Path3D isn't
   *                                 normalized. Useful to depict path
   *                                 acceleration or speed.
   */
  Path3D(const vector_t<Vector3>& path,
         const nullable_t<Vector3>& firstNormal = nullopt_t, bool raw = false);
  Path3D(const Path3D& otherPath);
  Path3D(Path3D&& otherPath);
  Path3D& operator=(const Path3D& otherPath);
  Path3D& operator=(Path3D&& otherPath);
  ~Path3D();
  Path3D copy() const;
  unique_ptr_t<Path3D> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Path3D& path);

  /**
   * @brief Returns the Path3D array of successive Vector3 designing its curve.
   */
  vector_t<Vector3>& getCurve();

  /**
   * Returns an array populated with tangent vectors on each Path3D curve point.
   */
  vector_t<Vector3>& getTangents();

  /**
   * @brief Returns an array populated with normal vectors on each Path3D curve
   * point.
   */
  vector_t<Vector3>& getNormals();

  /**
   * @brief Returns an array populated with binormal vectors on each Path3D
   * curve point.
   */
  vector_t<Vector3>& getBinormals();

  /**
   * @brief Returns an array populated with distances (float) of the i-th point
   * from the first curve point.
   */
  Float32Array& getDistances();

  /**
   * @brief Forces the Path3D tangent, normal, binormal and distance
   * recomputation.
   * @returns The same object updated.
   */
  Path3D& update(const vector_t<Vector3>& path,
                 const nullable_t<Vector3>& firstNormal = nullopt_t);

private:
  /**
   * @brief Computes tangents, normals and binormals.
   */
  void _compute(const nullable_t<Vector3>& firstNormal);

  /**
   * @brief Returns the first non null vector from index : curve[index +
   * N].subtract(curve[index])
   */
  Vector3 _getFirstNonNullVector(unsigned int index);

  /**
   * @brief Returns the last non null vector from index :
   * curve[index].subtract(curve[index - N]).
   */
  Vector3 _getLastNonNullVector(unsigned int index);

  /**
   * @brief returns an arbitrary point in the plane defined by the point v0 and
   * the vector vt orthogonal to this plane if va is passed, it returns the va
   * projection on the plane orthogonal to vt at the point v0.
   */
  Vector3 _normalVector(const Vector3& v0, const Vector3& vt,
                        const nullable_t<Vector3>& va);

public:
  vector_t<Vector3> path;

private:
  vector_t<Vector3> _curve;
  Float32Array _distances;
  vector_t<Vector3> _tangents;
  vector_t<Vector3> _normals;
  vector_t<Vector3> _binormals;
  bool _raw;

}; // end of class Path3D

std::ostream& operator<<(std::ostream& os, const Path3D& path);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_PATH3D_H
