#include <babylon/extensions/editcontrol/act.h>

#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {
namespace Extensions {

Act::Act(AbstractMesh* mesh) : p{mesh->position()}, rSet{false}, rESet{false}
{
  if (!mesh->rotationQuaternionSet()) {
    rE    = mesh->rotation();
    rESet = true;
  }
  else {
    r    = mesh->rotationQuaternion();
    rSet = true;
  }
  s = mesh->scaling();
}

Act::Act(const Act& otherAct)
    : p{otherAct.p}
    , rSet{otherAct.rSet}
    , r{otherAct.r}
    , rESet{otherAct.rESet}
    , rE{otherAct.rE}
    , s{otherAct.s}
{
}

Act::Act(Act&& otherAct)
    : p{std::move(otherAct.p)}
    , rSet{std::move(otherAct.rSet)}
    , r{std::move(otherAct.r)}
    , rESet{std::move(otherAct.rESet)}
    , rE{std::move(otherAct.rE)}
    , s{std::move(otherAct.s)}
{
}

Act& Act::operator=(const Act& otherAct)
{
  if (&otherAct != this) {
    p     = otherAct.p;
    rSet  = otherAct.rSet;
    r     = otherAct.r;
    rESet = otherAct.rESet;
    rE    = otherAct.rE;
    s     = otherAct.s;
  }

  return *this;
}

Act& Act::operator=(Act&& otherAct)
{
  if (&otherAct != this) {
    p     = std::move(otherAct.p);
    rSet  = std::move(otherAct.rSet);
    r     = std::move(otherAct.r);
    rESet = std::move(otherAct.rESet);
    rE    = std::move(otherAct.rE);
    s     = std::move(otherAct.s);
  }

  return *this;
}

Act::~Act()
{
}

void Act::perform(AbstractMesh* mesh)
{
  mesh->position().copyFrom(p);
  // Check if we are doing euler or quaternion now also check what were we doing
  // when the rotation value was captured and set value accordingly
  if (!mesh->rotationQuaternionSet()) {
    if (rESet) {
      mesh->rotation().copyFrom(rE);
    }
    else {
      mesh->rotation().copyFrom(r.toEulerAngles());
    }
  }
  else {
    if (rSet) {
      mesh->rotationQuaternion().copyFrom(r);
    }
    else {
      mesh->rotationQuaternion().copyFrom(rE.toQuaternion());
    }
  }
  mesh->scaling().copyFrom(s);
}

} // end of namespace Extensions
} // end of namespace BABYLON
