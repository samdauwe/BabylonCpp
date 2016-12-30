#ifndef BABYLON_MATH_PATH2_H
#define BABYLON_MATH_PATH2_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a Path2.
 */
class BABYLON_SHARED_EXPORT Path2 {

public:
  Path2(float x, float y);
  Path2(const Path2& otherPath);
  Path2(Path2&& otherPath);
  Path2& operator=(const Path2& otherPath);
  Path2& operator=(Path2&& otherPath);
  ~Path2();
  Path2 copy() const;
  std::unique_ptr<Path2> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Path2& path);

  Path2& addLineTo(float x, float y);
  Path2& addArcTo(float midX, float midY, float endX, float endY,
                  unsigned int numberOfSegments = 36);
  Path2& close();
  float length() const;
  std::vector<Vector2>& getPoints();
  const std::vector<Vector2>& getPoints() const;
  Vector2 getPointAtLengthPosition(float normalizedLengthPosition) const;

  /** Statics **/
  static Path2 StartingAt(float x, float y);

public:
  bool closed;

private:
  std::vector<Vector2> _points;
  float _length;

}; // end of class Path2

std::ostream& operator<<(std::ostream& os, const Path2& path);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_PATH2_H
