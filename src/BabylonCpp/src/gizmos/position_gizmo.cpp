#include <babylon/gizmos/position_gizmo.h>

#include <babylon/gizmos/axis_drag_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

PositionGizmo::PositionGizmo(UtilityLayerRenderer* iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , _xDrag{::std::make_unique<AxisDragGizmo>(
        iGizmoLayer, Vector3(1.f, 0.f, 0.f), Color3::FromHexString("#00b894"))}
    , _yDrag{::std::make_unique<AxisDragGizmo>(
        iGizmoLayer, Vector3(0.f, 1.f, 0.f), Color3::FromHexString("#d63031"))}
    , _zDrag{::std::make_unique<AxisDragGizmo>(
        iGizmoLayer, Vector3(0.f, 0.f, 1.f), Color3::FromHexString("#0984e3"))}
{
}

PositionGizmo::~PositionGizmo()
{
}

void PositionGizmo::setAttachedMesh(Mesh* mesh)
{
  _xDrag->attachedMesh = mesh;
  _yDrag->attachedMesh = mesh;
  _zDrag->attachedMesh = mesh;
}

void PositionGizmo::dispose(bool /*doNotRecurse*/,
                            bool /*disposeMaterialAndTextures*/)
{
  _xDrag->dispose();
  _yDrag->dispose();
  _zDrag->dispose();
}

} // end of namespace BABYLON
