#include <babylon/debug/physics_viewer.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>

namespace BABYLON {
namespace Debug {

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
        plugin->syncMeshWithImpostor(mesh.get(), *impostor);
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
      _renderFunction = [this](Scene* /*scene*/, EventState& /*es*/) {
        _updateDebugMeshes();
      };
      _scene->registerBeforeRender(_renderFunction);
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
    _scene->unregisterBeforeRender(_renderFunction);
  }
}

MaterialPtr PhysicsViewer::_getDebugMaterial(Scene* scene)
{
  if (!_debugMaterial) {
    _debugMaterial            = StandardMaterial::New("", scene);
    _debugMaterial->wireframe = true;
  }

  return _debugMaterial;
}

AbstractMeshPtr PhysicsViewer::_getDebugBoxMesh(Scene* scene)
{
  if (!_debugBoxMesh) {
    BoxOptions options;
    options.size = 1.f;
    _debugBoxMesh
      = MeshBuilder::CreateBox("physicsBodyBoxViewMesh", options, scene);
    _debugBoxMesh->renderingGroupId   = 1;
    _debugBoxMesh->rotationQuaternion = Quaternion::Identity();
    _debugBoxMesh->material           = _getDebugMaterial(scene);
    scene->removeMesh(_debugBoxMesh);
  }

  return _debugBoxMesh->createInstance("physicsBodyBoxViewInstance");
}

AbstractMeshPtr PhysicsViewer::_getDebugSphereMesh(Scene* scene)
{
  if (!_debugSphereMesh) {
    SphereOptions options;
    options.diameter = 1.f;
    _debugSphereMesh
      = MeshBuilder::CreateSphere("physicsBodySphereViewMesh", options, scene);
    _debugSphereMesh->renderingGroupId   = 1;
    _debugSphereMesh->rotationQuaternion = Quaternion::Identity();
    _debugSphereMesh->material           = _getDebugMaterial(scene);
    scene->removeMesh(_debugSphereMesh);
  }

  return _debugSphereMesh->createInstance("physicsBodyBoxViewInstance");
}

AbstractMeshPtr PhysicsViewer::_getDebugMesh(PhysicsImpostor* impostor,
                                             Scene* scene)
{
  AbstractMeshPtr mesh = nullptr;

  if (impostor->physicsImposterType == PhysicsImpostor::BoxImpostor) {
    mesh = _getDebugBoxMesh(scene);
    impostor->getBoxSizeToRef(mesh->scaling());
  }
  else if (impostor->physicsImposterType == PhysicsImpostor::SphereImpostor) {
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

} // end of namespace Debug
} // end of namespace BABYLON
