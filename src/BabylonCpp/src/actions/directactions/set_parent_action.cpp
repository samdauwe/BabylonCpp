#include <babylon/actions/directactions/set_parent_action.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/core/json.h>

namespace BABYLON {

SetParentAction::SetParentAction(unsigned int triggerOptions,
                                 IAnimatable* target, IAnimatable* parent,
                                 Condition* condition)
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

Json::object SetParentAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object(
      {Json::Pair("name", "SetParentAction"),
       Json::Pair(
         "properties",
         Json::array({{Json::value(Action::_GetTargetProperty(_target)),
                       Json::value(Action::_GetTargetProperty(_parent))}}))}),
    parent);
}

} // end of namespace BABYLON
