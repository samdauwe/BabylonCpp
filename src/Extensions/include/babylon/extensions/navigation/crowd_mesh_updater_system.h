#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_UPDATER_SYSTEM_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_UPDATER_SYSTEM_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/entitycomponentsystem/system.h>
#include <babylon/extensions/navigation/crowd_agent.h>
#include <babylon/extensions/navigation/crowd_mesh.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT CrowdMeshUpdaterSystem
    : ECS::System<ECS::Requires<CrowdAgent, CrowdMesh>> {

  CrowdMeshUpdaterSystem();
  ~CrowdMeshUpdaterSystem() override; // = default

  void update();

}; // end of struct CrowdMeshUpdaterSystem

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_CROWD_MESH_UPDATER_SYSTEM_H
