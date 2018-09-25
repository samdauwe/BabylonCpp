#include <babylon/gizmos/scale_gizmo.h>

#include <babylon/gizmos/axis_scale_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {

ScaleGizmo::ScaleGizmo(const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , xGizmo{std::make_unique<AxisScaleGizmo>(
        Vector3(1.f, 0.f, 0.f), Color3::Green().scale(0.5f), iGizmoLayer)}
    , yGizmo{std::make_unique<AxisScaleGizmo>(
        Vector3(0.f, 1.f, 0.f), Color3::Red().scale(0.5f), iGizmoLayer)}
    , zGizmo{std::make_unique<AxisScaleGizmo>(
        Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f), iGizmoLayer)}
    , snapDistance{this, &ScaleGizmo::get_snapDistance,
                   &ScaleGizmo::set_snapDistance}
    , scaleRatio{this, &ScaleGizmo::get_scaleRatio, &ScaleGizmo::set_scaleRatio}
    , updateGizmoRotationToMatchAttachedMesh{
        this, &ScaleGizmo::get_updateGizmoRotationToMatchAttachedMesh,
        &ScaleGizmo::set_updateGizmoRotationToMatchAttachedMesh}
{
  // Create uniform scale gizmo
  _uniformGizmo = std::make_unique<AxisScaleGizmo>(
    Vector3(0.f, 1.f, 0.f), Color3::Yellow().scale(0.5f), gizmoLayer);
  _uniformGizmo->updateGizmoRotationToMatchAttachedMesh = false;
  _uniformGizmo->uniformScaling                         = true;
  PolyhedronOptions options;
  options.type    = 1;
  auto octahedron = Mesh::CreatePolyhedron(
    "", options, _uniformGizmo->gizmoLayer->utilityLayerScene.get());
  octahedron->scaling().scaleInPlace(0.007f);
  _uniformGizmo->setCustomMesh(octahedron, true);

  attachedMesh = nullptr;
}

ScaleGizmo::~ScaleGizmo()
{
}

void ScaleGizmo::set_attachedMesh(const AbstractMeshPtr& mesh)
{
  if (xGizmo) {
    xGizmo->attachedMesh        = mesh;
    yGizmo->attachedMesh        = mesh;
    zGizmo->attachedMesh        = mesh;
    _uniformGizmo->attachedMesh = mesh;
  }
}

void ScaleGizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  if (xGizmo) {
    xGizmo->updateGizmoRotationToMatchAttachedMesh = value;
    yGizmo->updateGizmoRotationToMatchAttachedMesh = value;
    zGizmo->updateGizmoRotationToMatchAttachedMesh = value;
  }
}

bool ScaleGizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return xGizmo->updateGizmoRotationToMatchAttachedMesh;
}

void ScaleGizmo::set_snapDistance(float value)
{
  if (xGizmo) {
    xGizmo->snapDistance        = value;
    yGizmo->snapDistance        = value;
    zGizmo->snapDistance        = value;
    _uniformGizmo->snapDistance = value;
  }
}

float ScaleGizmo::get_snapDistance() const
{
  return xGizmo->snapDistance;
}

void ScaleGizmo::set_scaleRatio(float value)
{
  if (xGizmo) {
    xGizmo->scaleRatio        = value;
    yGizmo->scaleRatio        = value;
    zGizmo->scaleRatio        = value;
    _uniformGizmo->scaleRatio = value;
  }
}

float ScaleGizmo::get_scaleRatio() const
{
  return xGizmo->scaleRatio;
}

void ScaleGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  xGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  yGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  zGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  _uniformGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
