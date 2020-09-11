#include <babylon/misc/pivot_tools.h>

#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

size_t PivotTools::_PivotCached                = 0;
Vector3 PivotTools::_OldPivotPoint             = Vector3();
Vector3 PivotTools::_PivotTranslation          = Vector3();
Vector3 PivotTools::_PivotTmpVector            = Vector3();
bool PivotTools::_PivotPostMultiplyPivotMatrix = false;

void PivotTools::_RemoveAndStorePivotPoint(const AbstractMeshPtr& mesh)
{
  if (mesh && PivotTools::_PivotCached == 0) {
    // Save old pivot and set pivot to 0,0,0
    mesh->getPivotPointToRef(PivotTools::_OldPivotPoint);
    PivotTools::_PivotPostMultiplyPivotMatrix = mesh->_postMultiplyPivotMatrix;
    if (!PivotTools::_OldPivotPoint.equalsToFloats(0.f, 0.f, 0.f)) {
      mesh->setPivotMatrix(Matrix::IdentityReadOnly());
      PivotTools::_OldPivotPoint.subtractToRef(mesh->getPivotPoint(),
                                               PivotTools::_PivotTranslation);
      PivotTools::_PivotTmpVector.copyFromFloats(1.f, 1.f, 1.f);
      PivotTools::_PivotTmpVector.subtractInPlace(mesh->scaling());
      PivotTools::_PivotTmpVector.multiplyInPlace(PivotTools::_PivotTranslation);
      mesh->position().addInPlace(PivotTools::_PivotTmpVector);
    }
  }
  PivotTools::_PivotCached++;
}

void PivotTools::_RestorePivotPoint(const AbstractMeshPtr& mesh)
{
  if (mesh && !PivotTools::_OldPivotPoint.equalsToFloats(0.f, 0.f, 0.f)
      && PivotTools::_PivotCached == 1) {
    mesh->setPivotPoint(PivotTools::_OldPivotPoint);
    mesh->_postMultiplyPivotMatrix = PivotTools::_PivotPostMultiplyPivotMatrix;
    PivotTools::_PivotTmpVector.copyFromFloats(1.f, 1.f, 1.f);
    PivotTools::_PivotTmpVector.subtractInPlace(mesh->scaling());
    PivotTools::_PivotTmpVector.multiplyInPlace(PivotTools::_PivotTranslation);
    mesh->position().subtractInPlace(PivotTools::_PivotTmpVector);
  }
  PivotTools::_PivotCached--;
}

} // end of namespace BABYLON
