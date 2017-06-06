#ifndef BABYLON_EXTENSIONS_EDIT_CONTROL_ACT_H
#define BABYLON_EXTENSIONS_EDIT_CONTROL_ACT_H

#include <babylon/babylon_global.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

class Act {

public:
  Act(AbstractMesh* mesh);
  Act(const Act& otherAct);
  Act(Act&& otherAct);
  Act& operator=(const Act& otherAct);
  Act& operator=(Act&& otherAct);
  ~Act();

  void perform(AbstractMesh* mesh);

private:
  Vector3 p;
  bool rSet;
  Quaternion r;
  bool rESet;
  Vector3 rE;
  Vector3 s;

}; // end of class Act

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_EDIT_CONTROL_ACT_H
