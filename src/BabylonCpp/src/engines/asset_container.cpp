#include <babylon/engines/asset_container.h>

#include <babylon/actions/abstract_action_manager.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/animation_group.h>
#include <babylon/audio/sound.h>
#include <babylon/audio/sound_track.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/iscene_serializable_component.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/light.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/transform_node.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/probes/reflection_probe.h>

namespace BABYLON {

AssetContainer::AssetContainer(Scene* iScene) : scene{iScene}, _wasAddedToScene{false}
{
  scene->onDisposeObservable.add([this](Scene* /*scene*/, EventState & /*es*/) -> void {
    if (!_wasAddedToScene) {
      dispose();
    }
  });
}

AssetContainer::~AssetContainer() = default;

void AssetContainer::addAllToScene()
{
  _wasAddedToScene = true;

  for (const auto& o : cameras) {
    scene->addCamera(o);
  }
  for (const auto& o : lights) {
    scene->addLight(o);
  }
  for (const auto& o : meshes) {
    scene->addMesh(o);
  }
  for (const auto& o : skeletons) {
    scene->addSkeleton(o);
  }
  for (const auto& o : animations) {
    scene->addAnimation(o);
  }
  for (const auto& o : animationGroups) {
    scene->addAnimationGroup(o);
  }
  for (const auto& o : multiMaterials) {
    scene->addMultiMaterial(o);
  }
  for (const auto& o : materials) {
    scene->addMaterial(o);
  }
  for (const auto& o : morphTargetManagers) {
    scene->addMorphTargetManager(o);
  }
  for (const auto& o : geometries) {
    scene->addGeometry(o);
  }
  for (const auto& o : transformNodes) {
    scene->addTransformNode(o);
  }
  for (const auto& o : actionManagers) {
    scene->addActionManager(o);
  }
  for (const auto& o : textures) {
    scene->addTexture(o);
  }
  for (const auto& o : reflectionProbes) {
    scene->addReflectionProbe(o);
  }

  if (environmentTexture()) {
    scene->environmentTexture = environmentTexture();
  }

  for (const auto& component : scene->_serializableComponents) {
    component->addFromContainer(*this);
  }
}

void AssetContainer::removeAllFromScene()
{
  _wasAddedToScene = false;

  for (const auto& o : cameras) {
    scene->removeCamera(o);
  }
  for (const auto& o : lights) {
    scene->removeLight(o);
  }
  for (const auto& o : meshes) {
    scene->removeMesh(o);
  }
  for (const auto& o : skeletons) {
    scene->removeSkeleton(o);
  }
  for (const auto& o : animations) {
    scene->removeAnimation(o);
  }
  for (const auto& o : animationGroups) {
    scene->removeAnimationGroup(o);
  }
  for (const auto& o : multiMaterials) {
    scene->removeMultiMaterial(o);
  }
  for (const auto& o : materials) {
    scene->removeMaterial(o);
  }
  for (const auto& o : morphTargetManagers) {
    scene->removeMorphTargetManager(o);
  }
  for (const auto& o : geometries) {
    scene->removeGeometry(o);
  }
  for (const auto& o : transformNodes) {
    scene->removeTransformNode(o);
  }
  for (const auto& o : actionManagers) {
    scene->removeActionManager(o);
  }
  for (const auto& o : textures) {
    scene->removeTexture(o);
  }
  for (const auto& o : reflectionProbes) {
    scene->removeReflectionProbe(o);
  }

  if (environmentTexture() == scene->environmentTexture()) {
    scene->environmentTexture = nullptr;
  }

  for (const auto& component : scene->_serializableComponents) {
    component->removeFromContainer(*this);
  }
}

void AssetContainer::dispose()
{
  for (const auto& o : cameras) {
    o->dispose();
  }
  cameras.clear();

  for (const auto& o : lights) {
    o->dispose();
  }
  lights.clear();

  for (const auto& o : meshes) {
    scene->removeMesh(o);
  }
  meshes.clear();

  for (const auto& o : skeletons) {
    o->dispose();
  }
  skeletons.clear();

  for (const auto& o : animationGroups) {
    o->dispose();
  }
  animationGroups.clear();

  for (const auto& o : multiMaterials) {
    o->dispose();
  }
  multiMaterials.clear();

  for (const auto& o : materials) {
    o->dispose();
  }
  materials.clear();

  for (const auto& o : geometries) {
    o->dispose();
  }
  geometries.clear();

  for (const auto& o : transformNodes) {
    o->dispose();
  }
  transformNodes.clear();

  for (const auto& o : actionManagers) {
    o->dispose();
  }
  actionManagers.clear();

  for (const auto& o : textures) {
    o->dispose();
  }
  textures.clear();

  for (const auto& o : reflectionProbes) {
    o->dispose();
  }
  reflectionProbes.clear();

  if (environmentTexture()) {
    environmentTexture()->dispose();
    environmentTexture = nullptr;
  }

  for (const auto& component : scene->_serializableComponents) {
    component->removeFromContainer(*this, true);
  }
}

MeshPtr AssetContainer::createRootMesh()
{
  const auto rootMesh = Mesh::New("assetContainerRootMesh", scene);
  for (const auto& m : meshes) {
    if (!m->parent) {
      rootMesh->addChild(*m);
    }
  }
  meshes.insert(meshes.begin(), rootMesh);
  return rootMesh;
}

} // end of namespace BABYLON
