#include <babylon/debug/axes_viewer.h>

#include <babylon/materials/material.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

AxesViewer::AxesViewer(Scene* iScene, float iScaleLines)
    : scene{iScene}
    , scaleLines{iScaleLines}
    , _xline{Vector3::Zero(), Vector3::Zero()}
    , _yline{Vector3::Zero(), Vector3::Zero()}
    , _zline{Vector3::Zero(), Vector3::Zero()}
{
  _xmesh = Mesh::CreateLines("xline", _xline, scene, true);
  _ymesh = Mesh::CreateLines("yline", _yline, scene, true);
  _zmesh = Mesh::CreateLines("zline", _zline, scene, true);

  _xmesh->renderingGroupId = 2;
  _ymesh->renderingGroupId = 2;
  _zmesh->renderingGroupId = 2;

  _xmesh->material()->checkReadyOnlyOnce = true;
  _xmesh->color                          = Color3(1.f, 0.f, 0.f);
  _ymesh->material()->checkReadyOnlyOnce = true;
  _ymesh->color                          = Color3(0.f, 1.f, 0.f);
  _zmesh->material()->checkReadyOnlyOnce = true;
  _zmesh->color                          = Color3(0.f, 0.f, 1.f);
}

AxesViewer::~AxesViewer()
{
}

void AxesViewer::update(const Vector3& position, const Vector3& xaxis,
                        const Vector3& yaxis, const Vector3& zaxis)
{
  {
    auto& point1 = _xline[0];
    auto& point2 = _xline[1];
    point1.x     = position.x;
    point1.y     = position.y;
    point1.z     = position.z;
    point2.x     = point1.x + xaxis.x * scaleLines;
    point2.y     = point1.y + xaxis.y * scaleLines;
    point2.z     = point1.z + xaxis.z * scaleLines;
    Mesh::CreateLines("xline", _xline, nullptr, false, _xmesh);
  }

  {
    auto& point1 = _yline[0];
    auto& point2 = _yline[1];
    point1.x     = position.x;
    point1.y     = position.y;
    point1.z     = position.z;
    point2.x     = point1.x + yaxis.x * scaleLines;
    point2.y     = point1.y + yaxis.y * scaleLines;
    point2.z     = point1.z + yaxis.z * scaleLines;
    Mesh::CreateLines("yline", _yline, nullptr, false, _ymesh);
  }

  {
    auto& point1 = _zline[0];
    auto& point2 = _zline[1];
    point1.x     = position.x;
    point1.y     = position.y;
    point1.z     = position.z;
    point2.x     = point1.x + zaxis.x * scaleLines;
    point2.y     = point1.y + zaxis.y * scaleLines;
    point2.z     = point1.z + zaxis.z * scaleLines;
    Mesh::CreateLines("zline", _zline, nullptr, false, _zmesh);
  }
}

void AxesViewer::dispose()
{
  if (_xmesh) {
    _xmesh->dispose();
    _ymesh->dispose();
    _zmesh->dispose();

    _xmesh = nullptr;
    _ymesh = nullptr;
    _zmesh = nullptr;

    _xline.clear();
    _yline.clear();
    _zline.clear();

    scene = nullptr;
  }
}

} // end of namespace BABYLON
