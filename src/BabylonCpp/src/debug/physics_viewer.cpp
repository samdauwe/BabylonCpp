#include <babylon/debug/physics_viewer.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/builders/box_builder.h>
#include <babylon/meshes/builders/cylinder_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/physics/iphysics_enabled_object.h>
#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {
namespace Debug {

PhysicsViewer::PhysicsViewer(Scene* scene)
    : _scene{scene ? scene : Engine::LastCreatedScene()}
    , _renderFunction{nullptr}
    , _utilityLayer{nullptr}
    , _debugBoxMesh{nullptr}
    , _debugSphereMesh{nullptr}
    , _debugCylinderMesh{nullptr}
    , _debugMaterial{nullptr}
{
  auto physicEngine = _scene->getPhysicsEngine();

  if (physicEngine) {
    _physicsEnginePlugin = physicEngine->getPhysicsPlugin();
  }

  _utilityLayer = UtilityLayerRenderer::New(_scene, false);
  _utilityLayer->pickUtilitySceneFirst                       = false;
  _utilityLayer->utilityLayerScene->autoClearDepthAndStencil = true;
}

PhysicsViewer::~PhysicsViewer()
{
}

void PhysicsViewer::_updateDebugMeshes()
{
  auto& plugin = _physicsEnginePlugin;

  for (size_t i = 0; i < _numMeshes; ++i) {
    auto& impostor = _impostors[i];

    if (!impostor) {
      continue;
    }

    if (impostor->isDisposed()) {
      hideImpostor(_impostors[i--]);
    }
    else {
      if (impostor->physicsImposterType == PhysicsImpostor::MeshImpostor) {
        continue;
      }
      auto& mesh = _meshes[i];

      if (mesh && plugin) {
        plugin->syncMeshWithImpostor(mesh.get(), *impostor);
      }
    }
  }
}

AbstractMeshPtr PhysicsViewer::showImpostor(PhysicsImpostor* impostor,
                                            const MeshPtr& targetMesh)
{
  if (!_scene) {
    return nullptr;
  }

  for (size_t i = 0; i < _numMeshes; ++i) {
    if (_impostors[i] == impostor) {
      return nullptr;
    }
  }

  auto debugMesh = _getDebugMesh(impostor, targetMesh);

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

  return debugMesh;
}

void PhysicsViewer::hideImpostor(PhysicsImpostor* impostor)
{
  if (!impostor || !_scene || !_utilityLayer) {
    return;
  }

  auto removed            = false;
  auto& utilityLayerScene = _utilityLayer->utilityLayerScene;

  for (size_t i = 0; i < _numMeshes; ++i) {
    if (_impostors[i] == impostor) {
      auto& mesh = _meshes[i];

      if (!mesh) {
        continue;
      }

      utilityLayerScene->removeMesh(mesh);
      mesh->dispose();

      _debugMeshMeshes.erase(
        std::remove(_debugMeshMeshes.begin(), _debugMeshMeshes.end(), mesh),
        _debugMeshMeshes.end());

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
    _debugMaterial                  = StandardMaterial::New("", scene);
    _debugMaterial->wireframe       = true;
    _debugMaterial->emissiveColor   = Color3::White();
    _debugMaterial->disableLighting = true;
  }

  return _debugMaterial;
}

AbstractMeshPtr PhysicsViewer::_getDebugBoxMesh(Scene* scene)
{
  if (!_debugBoxMesh) {
    BoxOptions options;
    options.size = 1.f;
    _debugBoxMesh
      = BoxBuilder::CreateBox("physicsBodyBoxViewMesh", options, scene);
    _debugBoxMesh->rotationQuaternion = Quaternion::Identity();
    _debugBoxMesh->material           = _getDebugMaterial(scene);
    _debugBoxMesh->setEnabled(false);
  }

  return _debugBoxMesh->createInstance("physicsBodyBoxViewInstance");
}

AbstractMeshPtr PhysicsViewer::_getDebugSphereMesh(Scene* scene)
{
  if (!_debugSphereMesh) {
    SphereOptions options;
    options.diameter = 1.f;
    _debugSphereMesh = SphereBuilder::CreateSphere("physicsBodySphereViewMesh",
                                                   options, scene);
    _debugSphereMesh->rotationQuaternion = Quaternion::Identity();
    _debugSphereMesh->material           = _getDebugMaterial(scene);
    _debugSphereMesh->setEnabled(false);
  }

  return _debugSphereMesh->createInstance("physicsBodyBoxViewInstance");
}

AbstractMeshPtr PhysicsViewer::_getDebugCylinderMesh(Scene* scene)
{
  if (!_debugCylinderMesh) {
    CylinderOptions options;
    options.diameterTop    = 1.f;
    options.diameterBottom = 1.f;
    options.height         = 1.f;
    _debugCylinderMesh     = CylinderBuilder::CreateCylinder(
      "physicsBodyCylinderViewMesh", options, scene);
    _debugCylinderMesh->rotationQuaternion = Quaternion::Identity();
    _debugCylinderMesh->material           = _getDebugMaterial(scene);
    _debugCylinderMesh->setEnabled(false);
  }

  return _debugCylinderMesh->createInstance("physicsBodyBoxViewInstance");
}

AbstractMeshPtr PhysicsViewer::_getDebugMeshMesh(const MeshPtr& mesh,
                                                 Scene* scene)
{
  auto wireframeOver      = Mesh::New(mesh->name, scene, nullptr, mesh.get());
  wireframeOver->position = Vector3::Zero();
  wireframeOver->setParent(mesh.get());
  wireframeOver->material = _getDebugMaterial(scene);

  _debugMeshMeshes.emplace_back(wireframeOver);

  return wireframeOver;
}

AbstractMeshPtr PhysicsViewer::_getDebugMesh(PhysicsImpostor* impostor,
                                             const MeshPtr& targetMesh)
{
  if (!_utilityLayer) {
    return nullptr;
  }

  // Only create child impostor debug meshes when evaluating the parent
  if (targetMesh && targetMesh->parent()
      && static_cast<Mesh*>(targetMesh->parent())->physicsImpostor()) {
    return nullptr;
  }

  AbstractMeshPtr mesh   = nullptr;
  auto utilityLayerScene = _utilityLayer->utilityLayerScene.get();

  switch (impostor->physicsImposterType) {
    case PhysicsImpostor::BoxImpostor: {
      mesh = _getDebugBoxMesh(utilityLayerScene);
      impostor->getBoxSizeToRef(mesh->scaling);
    } break;
    case PhysicsImpostor::SphereImpostor: {
      mesh              = _getDebugSphereMesh(utilityLayerScene);
      auto radius       = impostor->getRadius();
      mesh->scaling().x = radius * 2.f;
      mesh->scaling().y = radius * 2.f;
      mesh->scaling().z = radius * 2.f;
    } break;
    case PhysicsImpostor::MeshImpostor:
      if (targetMesh) {
        mesh = _getDebugMeshMesh(targetMesh, utilityLayerScene);
      }
      break;
    case PhysicsImpostor::NoImpostor:
      if (targetMesh) {
        // Handle compound impostors
        std::vector<MeshPtr> filteredChildMeshes;
        for (const auto& c : targetMesh->getChildMeshes()) {
          if (c->physicsImpostor()) {
            filteredChildMeshes.emplace_back(std::static_pointer_cast<Mesh>(c));
          }
        }
        for (const auto& m : filteredChildMeshes) {
          auto a    = _getDebugBoxMesh(utilityLayerScene);
          a->parent = m.get();
        }
      }
      break;
    case PhysicsImpostor::CylinderImpostor: {
      mesh              = _getDebugCylinderMesh(utilityLayerScene);
      auto bi           = impostor->object->getBoundingInfo();
      mesh->scaling().x = bi->boundingBox.maximum.x - bi->boundingBox.minimum.x;
      mesh->scaling().y = bi->boundingBox.maximum.y - bi->boundingBox.minimum.y;
      mesh->scaling().z = bi->boundingBox.maximum.z - bi->boundingBox.minimum.z;
    } break;
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
  if (_debugCylinderMesh) {
    _debugCylinderMesh->dispose();
  }
  if (_debugMaterial) {
    _debugMaterial->dispose();
  }

  _impostors.clear();
  _scene               = nullptr;
  _physicsEnginePlugin = nullptr;

  if (_utilityLayer) {
    _utilityLayer->dispose();
    _utilityLayer = nullptr;
  }
}

} // end of namespace Debug
} // end of namespace BABYLON
