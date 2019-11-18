#ifndef BABYLON_MATH_PATH3D_H
#define BABYLON_MATH_PATH3D_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Represents a 3D path made up of multiple 3D points.
 */
class BABYLON_SHARED_EXPORT Path3D {

public:
  Path3D();
  /**
   * @brief new Path3D(path, normal, raw)
   * Creates a Path3D. A Path3D is a logical math object, so not a mesh.
   * please read the description in the tutorial :
   * https://doc.babylonjs.com/how_to/how_to_use_path3d
   * @param path an array of Vector3, the curve axis of the Path3D
   * @param firstNormal (options) Vector3, the first wanted normal to the curve.
   * Ex (0, 1, 0) for a vertical normal.
   * @param raw (optional, default false) : boolean, if true the returned Path3D
   * isn't normalized. Useful to depict path acceleration or speed.
   */
  Path3D(const std::vector<Vector3>& path, const std::optional<Vector3>& firstNormal = std::nullopt,
         bool raw = false);
  Path3D(const Path3D& otherPath);
  Path3D(Path3D&& otherPath);
  Path3D& operator=(const Path3D& otherPath);
  Path3D& operator=(Path3D&& otherPath);
  ~Path3D(); // = default
  [[nodiscard]] Path3D copy() const;
  [[nodiscard]] std::unique_ptr<Path3D> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Path3D& path);

  /**
   * @brief Returns the Path3D array of successive Vector3 designing its curve.
   * @returns the Path3D array of successive Vector3 designing its curve.
   */
  std::vector<Vector3>& getCurve();

  /**
   *@brief Returns an array populated with tangent vectors on each Path3D curve
   *point.
   * @returns an array populated with tangent vectors on each Path3D curve
   *point.
   */
  std::vector<Vector3>& getTangents();

  /**
   * @brief Returns an array populated with normal vectors on each Path3D curve
   * point.
   * @returns an array populated with normal vectors on each Path3D curve point.
   */
  std::vector<Vector3>& getNormals();

  /**
   * @brief Returns an array populated with binormal vectors on each Path3D
   * curve point.
   * @returns an array populated with binormal vectors on each Path3D curve
   * point.
   */
  std::vector<Vector3>& getBinormals();

  /**
   * @brief Returns an array populated with distances (float) of the i-th point
   * from the first curve point.
   * @returns an array populated with distances (float) of the i-th point from
   * the first curve point.
   */
  Float32Array& getDistances();

  /**
   * @brief Forces the Path3D tangent, normal, binormal and distance
   * recomputation.
   * @param path path which all values are copied into the curves points
   * @param firstNormal which should be projected onto the curve
   * @returns the same object updated.
   */
  Path3D& update(const std::vector<Vector3>& path,
                 const std::optional<Vector3>& firstNormal = std::nullopt);

private:
  /**
   * @brief Computes tangents, normals and binormals.
   */
  void _compute(const std::optional<Vector3>& firstNormal);

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
  Vector3 _normalVector(const Vector3& vt, const std::optional<Vector3>& va);

public:
  /**
   * an array of Vector3, the curve axis of the Path3D
   */
  std::vector<Vector3> path;

private:
  std::vector<Vector3> _curve;
  Float32Array _distances;
  std::vector<Vector3> _tangents;
  std::vector<Vector3> _normals;
  std::vector<Vector3> _binormals;
  bool _raw;

}; // end of class Path3D

std::ostream& operator<<(std::ostream& os, const Path3D& path);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_PATH3D_H
