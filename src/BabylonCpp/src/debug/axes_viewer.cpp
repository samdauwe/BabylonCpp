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
  _xmesh->position().copyFrom(position);
  _ymesh->position().copyFrom(position);
  _zmesh->position().copyFrom(position);

  auto point2 = _xline[1];
  point2.x    = xaxis.x * scaleLines;
  point2.y    = xaxis.y * scaleLines;
  point2.z    = xaxis.z * scaleLines;
  Mesh::CreateLines(nullptr, _xline, nullptr, false, _xmesh);

  point2   = _yline[1];
  point2.x = yaxis.x * scaleLines;
  point2.y = yaxis.y * scaleLines;
  point2.z = yaxis.z * scaleLines;
  Mesh::CreateLines(nullptr, _yline, nullptr, false, _ymesh);

  point2   = _zline[1];
  point2.x = zaxis.x * scaleLines;
  point2.y = zaxis.y * scaleLines;
  point2.z = zaxis.z * scaleLines;
  Mesh::CreateLines(nullptr, _zline, nullptr, false, _zmesh);
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
