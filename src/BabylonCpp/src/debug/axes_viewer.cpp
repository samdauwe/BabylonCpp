#include <babylon/debug/axes_viewer.h>

#include <babylon/gizmos/axis_drag_gizmo.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/vector3.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/transform_node.h>

namespace BABYLON {
namespace Debug {

AxesViewer::AxesViewer(Scene* iScene, float iScaleLines,
                       const std::optional<int>& iRenderingGroupId,
                       TransformNodePtr iXAxis, TransformNodePtr iYAxis,
                       TransformNodePtr iZAxis)
    : scaleLines{iScaleLines}
    , xAxis{this, &AxesViewer::get_xAxis}
    , yAxis{this, &AxesViewer::get_yAxis}
    , zAxis{this, &AxesViewer::get_zAxis}
    , _scaleLinesFactor{4.f}
    , _instanced{false}
{
  if (!iXAxis) {
    auto redColoredMaterial             = StandardMaterial::New("", scene);
    redColoredMaterial->disableLighting = true;
    redColoredMaterial->emissiveColor   = Color3::Red().scale(0.5f);
    iXAxis = AxisDragGizmo::_CreateArrow(scene, redColoredMaterial);
  }

  if (!iYAxis) {
    auto greenColoredMaterial             = StandardMaterial::New("", scene);
    greenColoredMaterial->disableLighting = true;
    greenColoredMaterial->emissiveColor   = Color3::Green().scale(0.5f);
    iYAxis = AxisDragGizmo::_CreateArrow(scene, greenColoredMaterial);
  }

  if (!iZAxis) {
    auto blueColoredMaterial             = StandardMaterial::New("", scene);
    blueColoredMaterial->disableLighting = true;
    blueColoredMaterial->emissiveColor   = Color3::Blue().scale(0.5f);
    iZAxis = AxisDragGizmo::_CreateArrow(scene, blueColoredMaterial);
  }

  _xAxis = iXAxis;
  _xAxis->scaling().setAll(scaleLines * _scaleLinesFactor);
  _yAxis = iYAxis;
  _yAxis->scaling().setAll(scaleLines * _scaleLinesFactor);
  _zAxis = iZAxis;
  _zAxis->scaling().setAll(scaleLines * _scaleLinesFactor);

  if (iRenderingGroupId.has_value()) {
    AxesViewer::_SetRenderingGroupId(_xAxis, *iRenderingGroupId);
    AxesViewer::_SetRenderingGroupId(_yAxis, *iRenderingGroupId);
    AxesViewer::_SetRenderingGroupId(_zAxis, *iRenderingGroupId);
  }

  scene = iScene;
  update(Vector3(), Vector3::Right(), Vector3::Up(), Vector3::Forward());
}

AxesViewer::~AxesViewer() = default;

TransformNodePtr& AxesViewer::get_xAxis()
{
  return _xAxis;
}

TransformNodePtr& AxesViewer::get_yAxis()
{
  return _yAxis;
}

TransformNodePtr& AxesViewer::get_zAxis()
{
  return _zAxis;
}

void AxesViewer::update(const Vector3& position, const Vector3& xaxis,
                        const Vector3& yaxis, const Vector3& zaxis)
{
  _xAxis->position().copyFrom(position);
  _xAxis->setDirection(xaxis);
  _xAxis->scaling().setAll(scaleLines * _scaleLinesFactor);

  _yAxis->position().copyFrom(position);
  _yAxis->setDirection(yaxis);
  _yAxis->scaling().setAll(scaleLines * _scaleLinesFactor);

  _zAxis->position().copyFrom(position);
  _zAxis->setDirection(zaxis);
  _zAxis->scaling().setAll(scaleLines * _scaleLinesFactor);
}

std::unique_ptr<AxesViewer> AxesViewer::createInstance() const
{
  const auto iXAxis = AxisDragGizmo::_CreateArrowInstance(scene, _xAxis);
  const auto iYAxis = AxisDragGizmo::_CreateArrowInstance(scene, _yAxis);
  const auto iZAxis = AxisDragGizmo::_CreateArrowInstance(scene, _zAxis);
  auto axesViewer   = std::make_unique<AxesViewer>(
    scene, scaleLines, std::nullopt, iXAxis, iYAxis, iZAxis);
  axesViewer->_instanced = true;
  return axesViewer;
}

void AxesViewer::dispose()
{
  if (_xAxis) {
    _xAxis->dispose(false, !_instanced);
    _xAxis = nullptr;
  }

  if (_yAxis) {
    _yAxis->dispose(false, !_instanced);
    _yAxis = nullptr;
  }

  if (_zAxis) {
    _zAxis->dispose(false, !_instanced);
    _zAxis = nullptr;
  }

  scene = nullptr;
}

void AxesViewer::_SetRenderingGroupId(const TransformNodePtr& node, int id)
{
  for (const auto& mesh : node->getChildMeshes()) {
    mesh->renderingGroupId = id;
  }
}

} // end of namespace Debug
} // end of namespace BABYLON
