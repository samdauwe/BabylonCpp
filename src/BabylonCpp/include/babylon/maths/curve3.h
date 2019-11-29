#ifndef BABYLON_MATHS_CURVE3_H
#define BABYLON_MATHS_CURVE3_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Vector3;

/**
 * @brief A Curve3 object is a logical object, so not a mesh, to handle curves
 * in the 3D geometric space. A Curve3 is designed from a series of successive
 * Vector3.
 * @see https://doc.babylonjs.com/how_to/how_to_use_curve3
 */
class BABYLON_SHARED_EXPORT Curve3 {

public:
  Curve3();
  /**
   * @brief A Curve3 object is a logical object, so not a mesh, to handle curves
   * in the 3D geometric space. A Curve3 is designed from a series of successive
   * Vector3. Tuto :
   * https://doc.babylonjs.com/how_to/how_to_use_curve3#curve3-object
   * @param points points which make up the curve
   */
  Curve3(const std::vector<Vector3>& points);
  Curve3(const Curve3& otherCurve);
  Curve3(Curve3&& otherCurve);
  Curve3& operator=(const Curve3& otherCurve);
  Curve3& operator=(Curve3&& otherCurve);
  ~Curve3(); // = default
  [[nodiscard]] Curve3 copy() const;
  [[nodiscard]] std::unique_ptr<Curve3> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Curve3& curve);

  /**
   * @returns the Curve3 stored array of successive Vector3
   */
  std::vector<Vector3>& getPoints();

  /**
   * @returns the computed length (float) of the curve.
   */
  [[nodiscard]] float length() const;

  /**
   * @brief Returns a new instance of Curve3 object : var curve =
   * curveA.continue(curveB); This new Curve3 is built by translating and
   * sticking the curveB at the end of the curveA. curveA and curveB keep
   * unchanged.
   * @param curve the curve to continue from this curve
   * @returns the newly constructed curve
   */
  Curve3 continueCurve3(Curve3& curve) const;

  /** Statics **/

  /**
   * @brief Returns a Curve3 object along a Quadratic Bezier curve :
   * https://doc.babylonjs.com/how_to/how_to_use_curve3#quadratic-bezier-curve
   * @param v0 (Vector3) the origin point of the Quadratic Bezier
   * @param v1 (Vector3) the control point
   * @param v2 (Vector3) the end point of the Quadratic Bezier
   * @param nbPoints (integer) the wanted number of points in the curve
   * @returns the created Curve3
   */
  static Curve3 CreateQuadraticBezier(const Vector3& v0, const Vector3& v1, const Vector3& v2,
                                      int nbPoints);

  /**
   * @brief Returns a Curve3 object along a Cubic Bezier curve :
   * https://doc.babylonjs.com/how_to/how_to_use_curve3#cubic-bezier-curve
   * @param v0 (Vector3) the origin point of the Cubic Bezier
   * @param v1 (Vector3) the first control point
   * @param v2 (Vector3) the second control point
   * @param v3 (Vector3) the end point of the Cubic Bezier
   * @param nbPoints (integer) the wanted number of points in the curve
   * @returns the created Curve3
   */
  static Curve3 CreateCubicBezier(const Vector3& v0, const Vector3& v1, const Vector3& v2,
                                  const Vector3& v3, int nbPoints);

  /**
   * @brief Returns a Curve3 object along a Hermite Spline curve :
   * https://doc.babylonjs.com/how_to/how_to_use_curve3#hermite-spline
   * @param p1 (Vector3) the origin point of the Hermite Spline
   * @param t1 (Vector3) the tangent vector at the origin point
   * @param p2 (Vector3) the end point of the Hermite Spline
   * @param t2 (Vector3) the tangent vector at the end point
   * @param nbPoints (integer) the wanted number of points in the curve
   * @returns the created Curve3
   */
  static Curve3 CreateHermiteSpline(const Vector3& p1, const Vector3& t1, const Vector3& p2,
                                    const Vector3& t2, int nbPoints);

  /**
   * @brief Returns a Curve3 object along a CatmullRom Spline curve :
   * @param points (array of Vector3) the points the spline must pass through.
   * At least, four points required
   * @param nbPoints (integer) the wanted number of points between each curve
   * control points
   * @param closed (boolean) optional with default false, when true forms a
   * closed loop from the points
   * @returns the created Curve3
   */
  static Curve3 CreateCatmullRomSpline(const std::vector<Vector3>& points, size_t nbPoints,
                                       bool closed = false);

private:
  float computeLength(const std::vector<Vector3>& path);

private:
  std::vector<Vector3> _points;
  float _length;

}; // end of class Curve3

std::ostream& operator<<(std::ostream& os, const Curve3& curve);

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS_CURVE3_H
