#include <babylon/extensions/editcontrol/act_history.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {
namespace Extensions {

ActHist::ActHist(AbstractMesh* iMesh, int capacity)
    : mesh{iMesh}, lastMax{capacity - 1}, last{-1}, current{-1}
{
  if (!mesh->rotationQuaternionSet()) {
    const auto& rotation = mesh->rotation();
    mesh->setRotationQuaternion(
      Quaternion::RotationYawPitchRoll(rotation.y, rotation.x, rotation.z));
  }
  add();
}

ActHist::~ActHist()
{
}

void ActHist::setCapacity(int c)
{
  if (c == 0) {
    BABYLON_LOG_ERROR("ActHist", "capacity should be more than zero");
    return;
  }
  lastMax = c - 1;
  last    = -1;
  current = -1;
  acts.clear();
  add();
}

void ActHist::add()
{
  Act act(mesh);
  if (current < last) {
    stl_util::splice(acts, current + 1);
    last = current;
  }
  if (last == lastMax) {
    acts.erase(acts.begin(), acts.begin() + 1); // Remove the first item
    acts.emplace_back(act);
  }
  else {
    acts.emplace_back(act);
    ++last;
    ++current;
  }
}

void ActHist::undo()
{
  if (current > 0) {
    --current;
    acts[static_cast<unsigned>(current)].perform(mesh);
  }
}

void ActHist::redo()
{
  if (current < last) {
    ++current;
    acts[static_cast<unsigned>(current)].perform(mesh);
  }
}

} // end of namespace Extensions
} // end of namespace BABYLON
