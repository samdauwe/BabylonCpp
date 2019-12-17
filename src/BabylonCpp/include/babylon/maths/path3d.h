#ifndef BABYLON_MATHS_PATH3D_H
#define BABYLON_MATHS_PATH3D_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

/**
 * @brief Represents a 3D path made up of multiple 3D points.
 */
class BABYLON_SHARED_EXPORT Path3D {

private:
  // holds interpolated point data
  struct PointAtData {
    std::optional<float> id        = 0.f;
    Vector3 point                  = Vector3::Zero();
    size_t previousPointArrayIndex = 0;

    float position    = 0.f;
    float subPosition = 0.f;

    bool interpolateReady      = false;
    Matrix interpolationMatrix = Matrix::Identity();
  }; // end of struct PointAtData

public:
  Path3D();
  /**
   * @brief new Path3D(path, normal, raw)
   * Creates a Path3D. A Path3D is a logical math object, so not a mesh.
   * please read the description in the tutorial :
   * https://doc.babylonjs.com/how_to/how_to_use_path3d
   * @param path an array of Vector3, the curve axis of the Path3D
   * @param firstNormal (options) Vector3, the first wanted normal to the curve. Ex (0, 1, 0) for a
   * vertical normal.
   * @param raw (optional, default false) : boolean, if true the returned Path3D isn't normalized.
   * Useful to depict path acceleration or speed.
   * @param alignTangentsWithPath (optional, default false) : boolean, if true the tangents will be
   * aligned with the path.
   */
  Path3D(const std::vector<Vector3>& path, const std::optional<Vector3>& firstNormal = std::nullopt,
         bool raw = false, bool alignTangentsWithPath = false);
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
   * @brief Returns the Path3D array of successive Vector3 designing its curve.
   * @returns the Path3D array of successive Vector3 designing its curve.
   */
  std::vector<Vector3>& getPoints();

  /**
   * @returns the computed length (float) of the path.
   */
  float length() const;

  /**
   *@brief Returns an array populated with tangent vectors on each Path3D curve point.
   * @returns an array populated with tangent vectors on each Path3D curve point.
   */
  std::vector<Vector3>& getTangents();

  /**
   * @brief Returns an array populated with normal vectors on each Path3D curve point.
   * @returns an array populated with normal vectors on each Path3D curve point.
   */
  std::vector<Vector3>& getNormals();

  /**
   * @brief Returns an array populated with binormal vectors on each Path3D curve point.
   * @returns an array populated with binormal vectors on each Path3D curve point.
   */
  std::vector<Vector3>& getBinormals();

  /**
   * @brief Returns an array populated with distances (float) of the i-th point from the first curve
   * point.
   * @returns an array populated with distances (float) of the i-th point from the first curve
   * point.
   */
  Float32Array& getDistances();

  /**
   * @brief Returns an interpolated point along this path
   * @param position the position of the point along this path, from 0.0 to 1.0
   * @returns a new Vector3 as the point
   */
  Vector3 getPointAt(float position);

  /**
   * @brief Returns the tangent vector of an interpolated Path3D curve point at the specified
   * position along this path.
   * @param position the position of the point along this path, from 0.0 to 1.0
   * @param interpolated (optional, default false) : boolean, if true returns an interpolated
   * tangent instead of the tangent of the previous path point.
   * @returns a tangent vector corresponding to the interpolated Path3D curve point, if not
   * interpolated, the tangent is taken from the precomputed tangents array.
   */
  Vector3 getTangentAt(float position, bool interpolated = false);

  /**
   * @brief Returns the tangent vector of an interpolated Path3D curve point at the specified
   * position along this path.
   * @param position the position of the point along this path, from 0.0 to 1.0
   * @param interpolated (optional, default false) : boolean, if true returns an interpolated normal
   * instead of the normal of the previous path point.
   * @returns a normal vector corresponding to the interpolated Path3D curve point, if not
   * interpolated, the normal is taken from the precomputed normals array.
   */
  Vector3 getNormalAt(float position, bool interpolated = false);

  /**
   * @brief Returns the binormal vector of an interpolated Path3D curve point at the specified
   * position along this path.
   * @param position the position of the point along this path, from 0.0 to 1.0
   * @param interpolated (optional, default false) : boolean, if true returns an interpolated
   * binormal instead of the binormal of the previous path point.
   * @returns a binormal vector corresponding to the interpolated Path3D curve point, if not
   * interpolated, the binormal is taken from the precomputed binormals array.
   */
  Vector3 getBinormalAt(float position, bool interpolated = false);

  /**
   * @brief Returns the distance (float) of an interpolated Path3D curve point at the specified
   * position along this path.
   * @param position the position of the point along this path, from 0.0 to 1.0
   * @returns the distance of the interpolated Path3D curve point at the specified position along
   * this path.
   */
  float getDistanceAt(float position) const;

  /**
   * @brief Returns the array index of the previous point of an interpolated point along this path
   * @param position the position of the point to interpolate along this path, from 0.0 to 1.0
   * @returns the array index
   */
  size_t getPreviousPointIndexAt(float position);

  /**
   * @brief Returns the position of an interpolated point relative to the two path points it lies
   * between, from 0.0 (point A) to 1.0 (point B)
   * @param position the position of the point to interpolate along this path, from 0.0 to 1.0
   * @returns the sub position
   */
  float getSubPositionAt(float position);

  /**
   * @brief Returns the position of the closest virtual point on this path to an arbitrary Vector3,
   * from 0.0 to 1.0
   * @param target the vector of which to get the closest position to
   * @returns the position of the closest virtual point on this path to the target vector
   */
  float getClosestPositionTo(const Vector3& target);

  /**
   * @brief Returns a sub path (slice) of this path
   * @param start the position of the fist path point, from 0.0 to 1.0, or a negative value, which
   * will get wrapped around from the end of the path to 0.0 to 1.0 values
   * @param end the position of the last path point, from 0.0 to 1.0, or a negative value, which
   * will get wrapped around from the end of the path to 0.0 to 1.0 values
   * @returns a sub path (slice) of this path
   */
  Path3D slice(float start = 0.f, float end = 1.f);

  /**
   * @brief Forces the Path3D tangent, normal, binormal and distance recomputation.
   * @param path path which all values are copied into the curves points
   * @param firstNormal which should be projected onto the curve
   * @param alignTangentsWithPath (optional, default false) : boolean, if true the tangents will be
   * aligned with the path
   * @returns the same object updated.
   */
  Path3D& update(const std::vector<Vector3>& path,
                 const std::optional<Vector3>& firstNormal = std::nullopt,
                 bool alignTangentsWithPath                = false);

private:
  /**
   * @brief Computes tangents, normals and binormals.
   */
  void _compute(const std::optional<Vector3>& firstNormal, bool alignTangentsWithPath = false);

  /**
   * @brief Returns the first non null vector from index : curve[index + N].subtract(curve[index])
   */
  Vector3 _getFirstNonNullVector(unsigned int index);

  /**
   * @brief Returns the last non null vector from index : curve[index].subtract(curve[index - N]).
   */
  Vector3 _getLastNonNullVector(unsigned int index);

  /**
   * @brief returns an arbitrary point in the plane defined by the point v0 and the vector vt
   * orthogonal to this plane if va is passed, it returns the va projection on the plane orthogonal
   * to vt at the point v0.
   */
  Vector3 _normalVector(const Vector3& vt, const std::optional<Vector3>& va);

  /**
   * @brief Updates the point at data for an interpolated point along this curve.
   * @param position the position of the point along this curve, from 0.0 to 1.0
   * @interpolateTNB wether to compute the interpolated tangent, normal and binormal
   * @returns the (updated) point at data
   */
  PointAtData& _updatePointAtData(float position, bool interpolateTNB = false);

  /**
   * @brief Updates the point at data from the specified parameters.
   * @param position where along the path the interpolated point is, from 0.0 to 1.0
   * @param point the interpolated point
   * @param parentIndex the index of an existing curve point that is on, or else positionally the
   * first behind, the interpolated point
   */
  PointAtData& _setPointAtData(float position, float subPosition, const Vector3& point,
                               size_t parentIndex, bool interpolateTNB);

  /**
   * @brief Updates the point at interpolation matrix for the tangents, normals and binormals.
   */
  void _updateInterpolationMatrix();

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
  bool _alignTangentsWithPath;
  PointAtData _pointAtData;

}; // end of class Path3D

std::ostream& operator<<(std::ostream& os, const Path3D& path);

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS_PATH3D_H
