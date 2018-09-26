#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_ROADMAP_VERTEX_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_ROADMAP_VERTEX_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/extensions/navigation/rvo2/vector2.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT CrowdRoadmapVertex {
  RVO2::Vector2 position;
  Uint32Array neighbors;
  Float32Array distToGoal;
}; // end of struct CrowdRoadmapVertex

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_CROWD_ROADMAP_VERTEX_H
