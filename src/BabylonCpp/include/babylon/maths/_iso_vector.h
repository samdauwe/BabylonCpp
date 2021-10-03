#ifndef BABYLON_MATHS__ISO_VECTOR_H
#define BABYLON_MATHS__ISO_VECTOR_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class Vector3;

/**
 * @brief Class representing an isovector a vector containing 2 INTEGER coordinates
 * x axis is horizontal
 * y axis is 60 deg counter clockwise from positive y axis
 * @hidden
 */
class BABYLON_SHARED_EXPORT _IsoVector {

public:
  /**
   * @brief Creates a new isovector from the given x and y coordinates.
   * @param x defines the first coordinate, must be an integer
   * @param y defines the second coordinate, must be an integer
   */
  _IsoVector();
  _IsoVector(float x, float y);
  _IsoVector(int x, int y);
  ~_IsoVector() = default;

  // Operators

  /**
   * @brief Gets a new IsoVector copied from the IsoVector.
   * @returns a new IsoVector
   */
  _IsoVector clone() const;

  /**
   * @brief Rotates one IsoVector 60 degrees counter clockwise about another.
   * Please note that this is an in place operation.
   * @param other an IsoVector a center of rotation
   * @returns the rotated IsoVector
   */
  _IsoVector& rotate60About(const _IsoVector& other);

  /**
   * @brief Rotates one IsoVector 60 degrees clockwise about another.
   * Please note that this is an in place operation.
   * @param other an IsoVector as center of rotation
   * @returns the rotated IsoVector
   */
  _IsoVector& rotateNeg60About(const _IsoVector& other);

  /**
   * @brief For an equilateral triangle OAB with O at isovector (0, 0) and A at isovector (m, n).
   * Rotates one IsoVector 120 degrees counter clockwise about the center of the triangle.
   * Please note that this is an in place operation.
   * @param m integer a measure a Primary triangle of order (m, n) m > n
   * @param n >= 0 integer a measure for a Primary triangle of order (m, n)
   * @returns the rotated IsoVector
   */
  _IsoVector& rotate120(float m, float n);
  _IsoVector& rotate120(int m, int n);

  /**
   * @brief For an equilateral triangle OAB with O at isovector (0, 0) and A at isovector (m, n).
   * Rotates one IsoVector 120 degrees clockwise about the center of the triangle.
   * Please note that this is an in place operation.
   * @param m integer a measure a Primary triangle of order (m, n) m > n
   * @param n >= 0 integer a measure for a Primary triangle of order (m, n)
   * @returns the rotated IsoVector
   */
  _IsoVector& rotateNeg120(float m, float n);
  _IsoVector& rotateNeg120(int m, int n);

  /**
   * @brief Transforms an IsoVector to one in Cartesian 3D space based on an isovector.
   * @param origin an IsoVector
   * @returns Point as a Vector3
   */
  Vector3 toCartesianOrigin(const _IsoVector& origin, int isoGridSize);

  // Statics

  /**
   * @brief Gets a new IsoVector(0, 0).
   * @returns a new IsoVector
   */
  static _IsoVector Zero();

public:
  /** defines the first coordinate */
  int x;
  /** defines the second coordinate */
  int y;

}; // end of class _IsoVector

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS__ISO_VECTOR_H
