#include <babylon/debug/physics_viewer.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/instanced_mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {

PhysicsViewer::PhysicsViewer(Scene* scene)
    : _scene{scene ? scene : Engine::LastCreatedScene()}
    , _renderFunction{nullptr}
    , _debugBoxMesh{nullptr}
    , _debugSphereMesh{nullptr}
    , _debugMaterial{nullptr}
{
  auto physicEngine = _scene->getPhysicsEngine();

  if (physicEngine) {
    _physicsEnginePlugin = physicEngine->getPhysicsPlugin();
  }
}

PhysicsViewer::~PhysicsViewer()
{
}

void PhysicsViewer::_updateDebugMeshes()
{
  auto plugin = _physicsEnginePlugin;

  for (size_t i = 0; i < _numMeshes; ++i) {
    auto& impostor = _impostors[i];

    if (!impostor) {
      continue;
    }

    if (impostor->isDisposed()) {
      hideImpostor(_impostors[i--]);
    }
    else {
      auto& mesh = _meshes[i];

      if (mesh && plugin) {
        plugin->syncMeshWithImpostor(mesh, impostor);
      }
    }
  }
}

void PhysicsViewer::showImpostor(PhysicsImpostor* impostor)
{
  if (!_scene) {
    return;
  }

  for (size_t i = 0; i < _numMeshes; ++i) {
    if (_impostors[i] == impostor) {
      return;
    }
  }

  auto debugMesh = _getDebugMesh(impostor, _scene);

  if (debugMesh) {
    _impostors[_numMeshes] = impostor;
    _meshes[_numMeshes]    = debugMesh;

    if (_numMeshes == 0) {
      // _renderFunction = [this](){_updateDebugMeshes();};
      // _scene->registerBeforeRender(_renderFunction);
    }

    ++_numMeshes;
  }
}

void PhysicsViewer::hideImpostor(PhysicsImpostor* impostor)
{
  if (!impostor || !_scene) {
    return;
  }

  auto removed = false;

  for (size_t i = 0; i < _numMeshes; ++i) {
    if (_impostors[i] == impostor) {
      auto& mesh = _meshes[i];

      if (!mesh) {
        continue;
      }

      _scene->removeMesh(mesh);
      mesh->dispose();
      --_numMeshes;
      if (_numMeshes > 0) {
        _meshes[i]             = _meshes[_numMeshes];
        _impostors[i]          = _impostors[_numMeshes];
        _meshes[_numMeshes]    = nullptr;
        _impostors[_numMeshes] = nullptr;
      }
      else {
        _meshes[0]    = nullptr;
        _impostors[0] = nullptr;
      }
      removed = true;
      break;
    }
  }

  if (removed && _numMeshes == 0) {
    // _scene->unregisterBeforeRender(_renderFunction);
  }
}

Material* PhysicsViewer::_getDebugMaterial(Scene* scene)
{
  if (!_debugMaterial) {
    _debugMaterial = StandardMaterial::New("", scene);
    _debugMaterial->setWireframe(true);
  }

  return _debugMaterial;
}

AbstractMesh* PhysicsViewer::_getDebugBoxMesh(Scene* scene)
{
  if (!_debugBoxMesh) {
    BoxOptions options(1.f);
    _debugBoxMesh
      = MeshBuilder::CreateBox("physicsBodyBoxViewMesh", options, scene);
    _debugBoxMesh->renderingGroupId = 1;
    _debugBoxMesh->setRotationQuaternion(Quaternion::Identity());
    _debugBoxMesh->setMaterial(_getDebugMaterial(scene));
    scene->removeMesh(_debugBoxMesh);
  }

  return _debugBoxMesh->createInstance("physicsBodyBoxViewInstance");
}

AbstractMesh* PhysicsViewer::_getDebugSphereMesh(Scene* scene)
{
  if (!_debugSphereMesh) {
    SphereOptions options(1.f);
    _debugSphereMesh
      = MeshBuilder::CreateSphere("physicsBodySphereViewMesh", options, scene);
    _debugSphereMesh->renderingGroupId = 1;
    _debugSphereMesh->setRotationQuaternion(Quaternion::Identity());
    _debugSphereMesh->setMaterial(_getDebugMaterial(scene));
    scene->removeMesh(_debugSphereMesh);
  }

  return _debugSphereMesh->createInstance("physicsBodyBoxViewInstance");
}

AbstractMesh* PhysicsViewer::_getDebugMesh(PhysicsImpostor* impostor,
                                           Scene* scene)
{
  AbstractMesh* mesh = nullptr;

  if (impostor->type() == PhysicsImpostor::BoxImpostor) {
    mesh = _getDebugBoxMesh(scene);
    impostor->getBoxSizeToRef(mesh->scaling());
  }
  else if (impostor->type() == PhysicsImpostor::SphereImpostor) {
    mesh              = _getDebugSphereMesh(scene);
    auto radius       = impostor->getRadius();
    mesh->scaling().x = radius * 2.f;
    mesh->scaling().y = radius * 2.f;
    mesh->scaling().z = radius * 2.f;
  }

  return mesh;
}

void PhysicsViewer::dispose()
{
  for (size_t i = 0; i < _numMeshes; ++i) {
    hideImpostor(_impostors[i]);
  }

  if (_debugBoxMesh) {
    _debugBoxMesh->dispose();
  }
  if (_debugSphereMesh) {
    _debugSphereMesh->dispose();
  }
  if (_debugMaterial) {
    _debugMaterial->dispose();
  }

  _impostors.clear();
  _scene               = nullptr;
  _physicsEnginePlugin = nullptr;
}

} // end of namespace BABYLON
