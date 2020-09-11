#ifndef BABYLON_MISC_PIVOT_TOOLS_H
#define BABYLON_MISC_PIVOT_TOOLS_H

#include <babylon/babylon_api.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

/**
 * @brief Class containing a set of static utilities functions for managing Pivots.
 * @hidden
 */
class BABYLON_SHARED_EXPORT PivotTools {

private:
  // Stores the state of the pivot cache (_oldPivotPoint, _pivotTranslation)
  // store/remove pivot point should only be applied during their outermost calls
  static size_t _PivotCached;
  static Vector3 _OldPivotPoint;
  static Vector3 _PivotTranslation;
  static Vector3 _PivotTmpVector;
  static bool _PivotPostMultiplyPivotMatrix;

public:
  /**
   * @brief Hidden
   */
  static void _RemoveAndStorePivotPoint(const AbstractMeshPtr& mesh);

  /**
   * @brief Hidden
   */
  static void _RestorePivotPoint(const AbstractMeshPtr& mesh);

}; // end of class PivotTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_PIVOT_TOOLS_H
