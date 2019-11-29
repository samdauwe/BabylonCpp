#include <babylon/debug/bone_axes_viewer.h>

#include <babylon/bones/bone.h>
#include <babylon/maths/axis.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Debug {

BoneAxesViewer::BoneAxesViewer(Scene* iScene, Bone* iBone, Mesh* iMesh,
                               float iScaleLines)
    : AxesViewer{iScene, iScaleLines}
    , mesh{iMesh}
    , bone{iBone}
    , pos{Vector3::Zero()}
    , xaxis{Vector3::Zero()}
    , yaxis{Vector3::Zero()}
    , zaxis{Vector3::Zero()}
{
}

BoneAxesViewer::~BoneAxesViewer() = default;

void BoneAxesViewer::update()
{
  if (!mesh || !bone) {
    return;
  }

  bone->_markAsDirtyAndCompose();
  bone->getAbsolutePositionToRef(mesh, pos);
  bone->getDirectionToRef(Axis::X(), xaxis, mesh);
  bone->getDirectionToRef(Axis::Y(), yaxis, mesh);
  bone->getDirectionToRef(Axis::Z(), zaxis, mesh);

  AxesViewer::update(pos, xaxis, yaxis, zaxis);
}

void BoneAxesViewer::dispose()
{
  if (mesh) {
    mesh = nullptr;
    bone = nullptr;

    AxesViewer::dispose();
  }
}

} // end of namespace Debug
} // end of namespace BABYLON
