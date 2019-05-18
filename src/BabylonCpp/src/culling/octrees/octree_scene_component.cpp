#include <babylon/culling/octrees/octree_scene_component.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/collider.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

OctreeSceneComponent::OctreeSceneComponent(Scene* iScene)
    : checksIsEnabled{this, &OctreeSceneComponent::get_checksIsEnabled}
    , _checksIsEnabled{true}
    , _tempRay{Ray(Vector3::Zero(), Vector3(1.f, 1.f, 1.f))}
{
  ISceneComponent::name = OctreeSceneComponent::name;
  scene                 = iScene;

  scene->getActiveMeshCandidates
    = [this]() { return getActiveMeshCandidates(); };

  scene->getActiveSubMeshCandidates
    = [this](AbstractMesh* mesh) { return getActiveSubMeshCandidates(mesh); };
  scene->getCollidingSubMeshCandidates
    = [this](AbstractMesh* mesh, const Collider& collider) {
        return getCollidingSubMeshCandidates(mesh, collider);
      };
  scene->getIntersectingSubMeshCandidates
    = [this](AbstractMesh* mesh, const Ray& localRay) {
        return getIntersectingSubMeshCandidates(mesh, localRay);
      };
}

OctreeSceneComponent::~OctreeSceneComponent()
{
}

bool OctreeSceneComponent::get_checksIsEnabled() const
{
  return _checksIsEnabled;
}

void OctreeSceneComponent::_register()
{
  scene->onMeshRemovedObservable.add(
    [this](AbstractMesh* mesh, EventState& /*es*/) {
      auto sceneOctree = scene->selectionOctree();
      if (sceneOctree != nullptr) {
        sceneOctree->dynamicContent.erase(
          std::remove(sceneOctree->dynamicContent.begin(),
                      sceneOctree->dynamicContent.end(), mesh),
          sceneOctree->dynamicContent.end());
      }
    });

  scene->onMeshImportedObservable.add(
    [this](AbstractMesh* mesh, EventState& /*es*/) {
      auto sceneOctree = scene->selectionOctree();
      if (sceneOctree != nullptr) {
        sceneOctree->addMesh(mesh);
      }
    });
}

std::vector<AbstractMesh*> OctreeSceneComponent::getActiveMeshCandidates()
{
  if (scene->selectionOctree()) {
    auto selection = scene->selectionOctree()->select(scene->frustumPlanes());
    return selection;
  }
  return scene->_getDefaultMeshCandidates();
}

std::vector<SubMesh*>
OctreeSceneComponent::getActiveSubMeshCandidates(AbstractMesh* mesh)
{
  if (mesh->_submeshesOctree && mesh->useOctreeForRenderingSelection) {
    auto intersections = mesh->_submeshesOctree->select(scene->frustumPlanes());
    return intersections;
  }
  return scene->_getDefaultSubMeshCandidates(mesh);
}

std::vector<SubMesh*>
OctreeSceneComponent::getIntersectingSubMeshCandidates(AbstractMesh* mesh,
                                                       const Ray& localRay)
{
  if (mesh->_submeshesOctree && mesh->useOctreeForPicking) {
    Ray::TransformToRef(localRay, mesh->getWorldMatrix(), _tempRay);
    auto intersections = mesh->_submeshesOctree->intersectsRay(_tempRay);

    return intersections;
  }
  return scene->_getDefaultSubMeshCandidates(mesh);
}

std::vector<SubMesh*>
OctreeSceneComponent::getCollidingSubMeshCandidates(AbstractMesh* mesh,
                                                    const Collider& collider)
{
  if (mesh->_submeshesOctree && mesh->useOctreeForCollisions) {
    auto radius = collider._velocityWorldLength
                  + stl_util::max(collider._radius.x, collider._radius.y,
                                  collider._radius.z);
    auto intersections
      = mesh->_submeshesOctree->intersects(collider._basePointWorld, radius);

    return intersections;
  }
  return scene->_getDefaultSubMeshCandidates(mesh);
}

void OctreeSceneComponent::rebuild()
{
  // Nothing to do here.
}

void OctreeSceneComponent::dispose()
{
  // Nothing to do here.
}

} // end of namespace BABYLON
