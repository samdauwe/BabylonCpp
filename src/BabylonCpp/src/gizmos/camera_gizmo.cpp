#include <babylon/gizmos/camera_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/box_builder.h>
#include <babylon/meshes/builders/cylinder_builder.h>
#include <babylon/meshes/builders/lines_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

float CameraGizmo::_Scale = 0.05f;

CameraGizmo::CameraGizmo(const UtilityLayerRendererPtr& gizmoLayer)
    : Gizmo{gizmoLayer}
    , displayFrustum{this, &CameraGizmo::get_displayFrustum, &CameraGizmo::set_displayFrustum}
    , camera{this, &CameraGizmo::get_camera, &CameraGizmo::set_camera}
    , material{this, &CameraGizmo::get_material}
    , _cameraMesh{nullptr}
    , _cameraLinesMesh{nullptr}
    , _pointerObserver{nullptr}
    , _camera{nullptr}
{
  _material = StandardMaterial::New("cameraGizmoMaterial", gizmoLayer->utilityLayerScene.get());
  _material->diffuseColor  = Color3(0.5f, 0.5f, 0.5f);
  _material->specularColor = Color3(0.1f, 0.1f, 0.1f);

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [this](PointerInfo* pointerInfo, EventState& /*es*/) -> void {
      if (!_camera) {
        return;
      }

      const auto childMeshes = _rootMesh->getChildMeshes();
      _isHovered = (stl_util::index_of(childMeshes, pointerInfo->pickInfo.pickedMesh) != -1);
      if (_isHovered && pointerInfo->pointerEvent.button == 0) {
        onClickedObservable.notifyObservers(_camera.get());
      }
    },
    static_cast<int>(PointerEventTypes::POINTERDOWN));
}

CameraGizmo::~CameraGizmo() = default;

bool CameraGizmo::get_displayFrustum() const
{
  return _cameraLinesMesh->isEnabled();
}

void CameraGizmo::set_displayFrustum(bool value)
{
  _cameraLinesMesh->setEnabled(value);
}

void CameraGizmo::set_camera(const CameraPtr& iCamera)
{
  _camera      = iCamera;
  attachedNode = iCamera;
  if (iCamera) {
    // Create the mesh for the given camera
    if (_cameraMesh) {
      _cameraMesh->dispose();
    }
    if (_cameraLinesMesh) {
      _cameraLinesMesh->dispose();
    }
    _cameraMesh      = CameraGizmo::_CreateCameraMesh(gizmoLayer->utilityLayerScene.get());
    _cameraLinesMesh = CameraGizmo::_CreateCameraFrustum(gizmoLayer->utilityLayerScene.get());

    for (const auto& m : _cameraMesh->getChildMeshes(false)) {
      m->material = _material;
    }
    _cameraMesh->parent = _rootMesh.get();

    _cameraLinesMesh->parent = _rootMesh.get();

    if (gizmoLayer->utilityLayerScene->activeCamera()
        && gizmoLayer->utilityLayerScene->activeCamera()->maxZ < iCamera->maxZ * 1.5f) {
      gizmoLayer->utilityLayerScene->activeCamera()->maxZ = iCamera->maxZ * 1.5f;
    }

    /*if (!attachedNode->reservedDataStore) {
        attachedNode!.reservedDataStore = {};
    }
    attachedNode!.reservedDataStore.cameraGizmo = this;*/

    // Add lighting to the camera gizmo
    auto gizmoLight = gizmoLayer->_getSharedGizmoLight();
    gizmoLight->includedOnlyMeshes
      = stl_util::concat(gizmoLight->includedOnlyMeshes(), _cameraMesh->getChildMeshes(false));

    _update();
  }
}

CameraPtr& CameraGizmo::get_camera()
{
  return _camera;
}

StandardMaterialPtr& CameraGizmo::get_material()
{
  return _material;
}

void CameraGizmo::_update()
{
  Gizmo::_update();
  if (!_camera) {
    return;
  }

  // frustum matrix
  _camera->getProjectionMatrix().invertToRef(_invProjection);
  _cameraLinesMesh->setPivotMatrix(_invProjection, false);

  _cameraLinesMesh->scaling().x = 1.f / _rootMesh->scaling().x;
  _cameraLinesMesh->scaling().y = 1.f / _rootMesh->scaling().y;
  _cameraLinesMesh->scaling().z = 1.f / _rootMesh->scaling().z;

  // take care of coordinate system in camera scene to properly display the mesh with the good Y
  // axis orientation in this scene
  _cameraMesh->parent = nullptr;
  _cameraMesh->rotation().y
    = Math::PI * 0.5f * (_camera->getScene()->useRightHandedSystem() ? 1.f : -1.f);
  _cameraMesh->parent = _rootMesh.get();
}

void CameraGizmo::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  onClickedObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  if (_cameraMesh) {
    _cameraMesh->dispose();
  }
  if (_cameraLinesMesh) {
    _cameraLinesMesh->dispose();
  }
  _material->dispose();
  Gizmo::dispose();
}

MeshPtr CameraGizmo::_CreateCameraMesh(Scene* scene)
{
  auto root = Mesh::New("rootCameraGizmo", scene);

  auto mesh    = Mesh::New(root->name, scene);
  mesh->parent = root.get();

  BoxOptions boxOptions;
  boxOptions.width  = 1.f;
  boxOptions.height = 0.8f;
  boxOptions.depth  = 0.5f;
  auto box          = BoxBuilder::CreateBox(root->name, boxOptions, scene);
  box->parent       = mesh.get();

  CylinderOptions cyl1Options;
  cyl1Options.height         = 0.5f;
  cyl1Options.diameterTop    = 0.8f;
  cyl1Options.diameterBottom = 0.8f;
  auto cyl1                  = CylinderBuilder::CreateCylinder(root->name, cyl1Options, scene);
  cyl1->parent               = mesh.get();
  cyl1->position().y         = 0.3f;
  cyl1->position().x         = -0.6f;
  cyl1->rotation().x         = Math::PI * 0.5f;

  CylinderOptions cyl2Options;
  cyl2Options.height         = 0.5f;
  cyl1Options.diameterTop    = 0.6f;
  cyl1Options.diameterBottom = 0.6f;
  auto cyl2                  = CylinderBuilder::CreateCylinder(root->name, cyl2Options, scene);
  cyl2->parent               = mesh.get();
  cyl2->position().y         = 0.5f;
  cyl2->position().x         = 0.4f;
  cyl2->rotation().x         = Math::PI * 0.5f;

  CylinderOptions cyl3Options;
  cyl3Options.height         = 0.5f;
  cyl1Options.diameterTop    = 0.5f;
  cyl1Options.diameterBottom = 0.5f;
  auto cyl3                  = CylinderBuilder::CreateCylinder(root->name, cyl3Options, scene);
  cyl3->parent               = mesh.get();
  cyl3->position().y         = 0.f;
  cyl3->position().x         = 0.6f;
  cyl3->rotation().z         = Math::PI * 0.5f;

  root->scaling().scaleInPlace(CameraGizmo::_Scale);
  mesh->position().x = -0.9f;

  return root;
}

MeshPtr CameraGizmo::_CreateCameraFrustum(Scene* scene)
{
  auto root    = Mesh::New("rootCameraGizmo", scene);
  auto mesh    = Mesh::New(root->name, scene);
  mesh->parent = root.get();

  for (unsigned y = 0; y < 4; y += 2) {
    for (unsigned x = 0; x < 4; x += 2) {
      {
        LinesOptions options;
        options.points = {Vector3(-1.f + x, -1.f + y, -1.f), Vector3(-1.f + x, -1.f + y, 1.f)};
        auto line      = LinesBuilder::CreateLines("lines", options, scene);
        line->parent   = mesh.get();
        line->alwaysSelectAsActiveMesh = true;
        line->isPickable               = false;
      }
      {
        LinesOptions options;
        options.points = {Vector3(-1.f, -1.f + x, -1.f + y), Vector3(1.f, -1.f + x, -1.f + y)};
        auto line      = LinesBuilder::CreateLines("lines", options, scene);
        line->parent   = mesh.get();
        line->alwaysSelectAsActiveMesh = true;
        line->isPickable               = false;
      }
      {
        LinesOptions options;
        options.points = {Vector3(-1.f + x, -1.f, -1.f + y), Vector3(-1.f + x, 1.f, -1.f + y)};
        auto line      = LinesBuilder::CreateLines("lines", options, scene);
        line->parent   = mesh.get();
        line->alwaysSelectAsActiveMesh = true;
        line->isPickable               = false;
      }
    }
  }

  return root;
}

} // end of namespace BABYLON
