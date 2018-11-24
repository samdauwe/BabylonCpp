#include <babylon/actions/directactions/set_parent_action.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/ianimatable.h>

namespace BABYLON {

SetParentAction::SetParentAction(unsigned int triggerOptions,
                                 const IAnimatablePtr& target,
                                 IAnimatable* parent, Condition* condition)
    : Action(triggerOptions, condition), _target{target}, _parent{parent}
{
}

SetParentAction::~SetParentAction()
{
}

void SetParentAction::_prepare()
{
}

void SetParentAction::execute(const ActionEvent& /*evt*/)
{
  // if (_target->parent() == _parent) {
  //  return;
  //}

  Matrix invertParentWorldMatrix = _parent->getWorldMatrix()->copy();
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
