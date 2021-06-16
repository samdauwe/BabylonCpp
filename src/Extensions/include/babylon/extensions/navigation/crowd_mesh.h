#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/extensions/entitycomponentsystem/component.h>

namespace BABYLON {

FWD_CLASS_SPTR(AbstractMesh)

namespace Extensions {

/**
 * @brief A component holding the mesh reference of an entity.
 */
struct BABYLON_SHARED_EXPORT CrowdMesh : public ECS::Component {
  AbstractMeshPtr mesh = nullptr;
}; // end of struct CrowdMesh

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_H
