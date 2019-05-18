#include <babylon/extensions/navigation/crowd_mesh_updater_system.h>

#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {
namespace Extensions {

CrowdMeshUpdaterSystem::CrowdMeshUpdaterSystem()
{
}

CrowdMeshUpdaterSystem::~CrowdMeshUpdaterSystem()
{
}

void CrowdMeshUpdaterSystem::update()
{
  auto entities = getEntities();
  for (auto& entity : entities) {
    auto crowdAgent              = entity.getComponent<CrowdAgent>();
    auto crowdMesh               = entity.getComponent<CrowdMesh>();
    const auto& position         = crowdAgent.position();
    crowdMesh.mesh->position().x = position.x();
    crowdMesh.mesh->position().z = position.y();
  }
}

} // end of namespace Extensions
} // end of namespace BABYLON
