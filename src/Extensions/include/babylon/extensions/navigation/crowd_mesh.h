#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/extensions/entitycomponentsystem/component.h>

namespace BABYLON {

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

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
