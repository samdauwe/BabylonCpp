#ifndef BABYLON_MATH_VIEWPORT_H
#define BABYLON_MATH_VIEWPORT_H

#include <memory>
#include <ostream>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class used to represent a viewport on screen.
 */
class BABYLON_SHARED_EXPORT Viewport {

public:
  /**
   * @brief Creates a Viewport object located at (x, y) and sized (width,
   * height)
   * @param x defines viewport left coordinate
   * @param y defines viewport top coordinate
   * @param width defines the viewport width
   * @param height defines the viewport height
   */
  Viewport();
  Viewport(float x, float y, float width, float height);
  Viewport(const Viewport& otherViewport);
  Viewport(Viewport&& otherViewport);
  Viewport& operator=(const Viewport& otherViewport);
  Viewport& operator=(Viewport&& otherViewport);
  ~Viewport();
  Viewport copy() const;

  /**
   * @brief Returns a new Viewport copied from the current one.
   * @returns a new Viewport
   */
  std::unique_ptr<Viewport> clone() const;

  friend std::ostream& operator<<(std::ostream& os, const Viewport& viewport);

  /** Methods **/

  /**
   * @brief Creates a new viewport using absolute sizing (from 0-> width, 0->
   * height instead of 0->1).
   * @param renderWidthOrEngine defines either an engine or the rendering width
   * @param renderHeight defines the rendering height
   * @returns a new Viewport
   */
  Viewport toGlobal(int renderWidth, int renderHeight) const;

public:
  /**
   * viewport left coordinate
   */
  float x;

  /**
   * viewport top coordinate
   */
  float y;

  /**
   * viewport width
   */
  float width;

  /**
   * viewport height
   */
  float height;

}; // end of class Viewport

std::ostream& operator<<(std::ostream& os, const Viewport& viewport);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VIEWPORT_H
