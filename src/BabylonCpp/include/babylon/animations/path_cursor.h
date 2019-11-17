#ifndef BABYLON_ANIMATIONS_PATH_CURSOR_H
#define BABYLON_ANIMATIONS_PATH_CURSOR_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/math/path2.h>

namespace BABYLON {

class Animation;
class Vector3;

/**
 * @brief A cursor which tracks a point on a path.
 */
class BABYLON_SHARED_EXPORT PathCursor {

public:
  /**
   * @brief Initializes the path cursor.
   * @param path The path to track
   */
  PathCursor(const Path2& path);
  PathCursor(const PathCursor& otherPathCursor);
  PathCursor(PathCursor&& otherPathCursor);
  PathCursor& operator=(const PathCursor& otherPathCursor);
  PathCursor& operator=(PathCursor&& otherPathCursor);
  ~PathCursor(); // = default

  /**
   * @brief Gets the cursor point on the path.
   * @returns A point on the path cursor at the cursor location
   */
  [[nodiscard]] Vector3 getPoint() const;

  /**
   * @brief Moves the cursor ahead by the step amount.
   * @param step The amount to move the cursor forward
   * @returns This path cursor
   */
  PathCursor& moveAhead(float step = 0.002f);

  /**
   * @brief Moves the cursor behind by the step amount.
   * @param step The amount to move the cursor back
   * @returns This path cursor
   */
  PathCursor& moveBack(float step = 0.002f);

  /**
   * @brief Moves the cursor by the step amount.
   * If the step amount is greater than one, an exception is thrown
   * @param step The amount to move the cursor
   * @returns This path cursor
   */
  PathCursor& move(float step);

  /**
   * @brief Executes a function on change.
   * @param f A path cursor onchange callback
   * @returns This path cursor
   */
  PathCursor& onchange(const std::function<void(const PathCursor& cursor)>& f);

private:
  /**
   * @brief Ensures that the value is limited between zero and one.
   * @returns This path cursor
   */
  PathCursor ensureLimits();

  /**
   * @brief Runs onchange callbacks on change (used by the animation engine).
   * @returns This path cursor
   */
  PathCursor& raiseOnChange();

private:
  /**
   * Stores path cursor callbacks for when an onchange event is triggered
   */
  std::vector<std::function<void(const PathCursor& cursor)>> _onchange;

  /**
   * The path to track
   */
  Path2 path;

  /**
   * The value of the path cursor
   */
  float value;

  /**
   * The animation array of the path cursor
   */
  std::vector<Animation*> animations;

}; // end of class PathCursor

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_PATH_CURSOR_H
