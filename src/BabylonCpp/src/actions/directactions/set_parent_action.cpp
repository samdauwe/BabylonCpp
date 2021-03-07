#include <babylon/actions/directactions/set_parent_action.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/ianimatable.h>

namespace BABYLON {

SetParentAction::SetParentAction(unsigned int iTriggerOptions, const IAnimatablePtr& target,
                                 IAnimatable* parent, Condition* condition)
    : Action(iTriggerOptions, condition), _target{target}, _parent{parent}
{
}

SetParentAction::~SetParentAction() = default;

void SetParentAction::_prepare()
{
}

void SetParentAction::execute(const IActionEventPtr& /*evt*/)
{
  // if (_target->parent() == _parent) {
  //  return;
  //}

  Matrix invertParentWorldMatrix = _parent->getWorldMatrix().copy();
  invertParentWorldMatrix.invert();

  // _target.position = Vector3::TransformCoordinates(_target.position,
  // invertParentWorldMatrix);

  // _target.parent = _parent;
}

json SetParentAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
