#ifndef BABYLON_ANIMATIONS_PATH_CURSOR_H
#define BABYLON_ANIMATIONS_PATH_CURSOR_H

#include <babylon/babylon_global.h>
#include <babylon/math/path2.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PathCursor {

public:
  PathCursor(const Path2& path);
  PathCursor(const PathCursor& otherPathCursor);
  ~PathCursor();

  Vector3 getPoint() const;
  PathCursor& moveAhead(float step = 0.002f);
  PathCursor& moveBack(float step = 0.002f);
  PathCursor& move(float step);

  // used by animation engine
  PathCursor& markAsDirty(const std::string& propertyName);

private:
  PathCursor ensureLimits();
  PathCursor& raiseOnChange();
  PathCursor& onchange(std::function<void(const PathCursor& cursor)>& f);

private:
  std::vector<std::function<void(const PathCursor& cursor)>> _onchange;
  Path2 path;
  float value;
  std::vector<Animation*> animations;

}; // end of class PathCursor

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_PATH_CURSOR_H
