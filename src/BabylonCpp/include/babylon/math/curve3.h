#ifndef BABYLON_MATH_CURVE3_H
#define BABYLON_MATH_CURVE3_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents an Curve3.
 */
class BABYLON_SHARED_EXPORT Curve3 {

public:
  Curve3();
  /**
   * @brief A Curve3 object is a logical object, so not a mesh, to handle curves
   * in the 3D geometric space.
   * A Curve3 is designed from a series of successive Vector3.
   * Tuto : http://doc.babylonjs.com/tutorials/How_to_use_Curve3#curve3-object
   */
  Curve3(const vector_t<Vector3>& points);
  Curve3(const Curve3& otherCurve);
  Curve3(Curve3&& otherCurve);
  Curve3& operator=(const Curve3& otherCurve);
  Curve3& operator=(Curve3&& otherCurve);
  ~Curve3();
  Curve3 copy() const;
  unique_ptr_t<Curve3> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Curve3& curve);

  /**
   * @brief Returns the Curve3 stored array of successive Vector3
   */
  vector_t<Vector3>& getPoints();

  /**
   * @brief Returns the computed length (float) of the curve.
   */
  float length() const;

  /**
   * Returns a new instance of Curve3 object : var curve =
   * curveA.continue(curveB);
   * This new Curve3 is built by translating and sticking the curveB at the end
   * of the curveA.
   * curveA and curveB keep unchanged.
   */
  Curve3 continueCurve3(Curve3& curve) const;

  /** Statics **/

  /**
   * @brief Returns a Curve3 object along a Quadratic Bezier curve :
   * http://doc.babylonjs.com/tutorials/How_to_use_Curve3#quadratic-bezier-curve
   * @param v0 (Vector3) the origin point of the Quadratic Bezier
   * @param v1 (Vector3) the control point
   * @param v2 (Vector3) the end point of the Quadratic Bezier
   * @param nbPoints (integer) the wanted number of points in the curve
   */
  static Curve3 CreateQuadraticBezier(const Vector3& v0, const Vector3& v1,
                                      const Vector3& v2, int nbPoints);

  /**
   * R@brief eturns a Curve3 object along a Cubic Bezier curve :
   * http://doc.babylonjs.com/tutorials/How_to_use_Curve3#cubic-bezier-curve
   * @param v0 (Vector3) the origin point of the Cubic Bezier
   * @param v1 (Vector3) the first control point
   * @param v2 (Vector3) the second control point
   * @param v3 (Vector3) the end point of the Cubic Bezier
   * @param nbPoints (integer) the wanted number of points in the curve
   */
  static Curve3 CreateCubicBezier(const Vector3& v0, const Vector3& v1,
                                  const Vector3& v2, const Vector3& v3,
                                  int nbPoints);

  /**
   * @brief Returns a Curve3 object along a Hermite Spline curve :
   * http://doc.babylonjs.com/tutorials/How_to_use_Curve3#hermite-spline
   * @param p1 (Vector3) the origin point of the Hermite Spline
   * @param t1 (Vector3) the tangent vector at the origin point
   * @param p2 (Vector3) the end point of the Hermite Spline
   * @param t2 (Vector3) the tangent vector at the end point
   * @param nbPoints (integer) the wanted number of points in the curve
   */
  static Curve3 CreateHermiteSpline(const Vector3& p1, const Vector3& t1,
                                    const Vector3& p2, const Vector3& t2,
                                    int nbPoints);

  /**
   * @brief Returns a Curve3 object along a CatmullRom Spline curve :
   * @param points (array of Vector3) the points the spline must pass through.
   * At least, four points required.
   * @param nbPoints (integer) the wanted number of points between each curve
   * control points.
   */
  static Curve3 CreateCatmullRomSpline(const vector_t<Vector3> points,
                                       size_t nbPoints);

private:
  float computeLength(const vector_t<Vector3>& path);

private:
  vector_t<Vector3> _points;
  float _length;

}; // end of class Curve3

std::ostream& operator<<(std::ostream& os, const Curve3& curve);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_CURVE3_H
