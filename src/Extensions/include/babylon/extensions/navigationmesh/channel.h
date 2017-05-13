#ifndef BABYLON_EXTENSIONS_NAVIGATION_MESH_CHANNEL_H
#define BABYLON_EXTENSIONS_NAVIGATION_MESH_CHANNEL_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/navigationmesh/navigation_structs.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

/**
* @brief
*/
class BABYLON_SHARED_EXPORT Channel {

public:
  Channel();
  ~Channel();

  void push(const Vector3& p);
  void push(const Vector3& p1, const Vector3& p2);
  std::vector<Vector3>& stringPull();

private:
  bool _vequal(const Vector3& a, const Vector3& b);
  float _triarea2(const Vector3& a, const Vector3& b, const Vector3& c);

public:
  std::vector<Portal> portals;
  std::vector<Vector3> path;

}; // end of class Channel

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_MESH_CHANNEL_H
