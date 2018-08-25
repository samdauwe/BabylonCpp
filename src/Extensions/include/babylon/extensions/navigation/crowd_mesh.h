#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/entitycomponentsystem/component.h>

namespace BABYLON {
namespace Extensions {

/**
 * @brief A component holding the mesh reference of an entity.
 */
struct BABYLON_SHARED_EXPORT CrowdMesh : public ECS::Component {
  AbstractMeshPtr mesh;
}; // end of struct CrowdMesh

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_H
