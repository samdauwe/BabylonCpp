#include <babylon/actions/directactions/switch_boolean_action.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/animation.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/ianimatable.h>

namespace BABYLON {

SwitchBooleanAction::SwitchBooleanAction(unsigned int iTtriggerOptions,
                                         const IAnimatablePtr& target,
                                         const std::string& iPropertyPath, Condition* condition)
    : Action(iTtriggerOptions, condition)
    , propertyPath{iPropertyPath}
    , _target{target}
    , _effectiveTarget{target}
{
}

SwitchBooleanAction::~SwitchBooleanAction() = default;

void SwitchBooleanAction::_prepare()
{
  _effectiveTarget = _getEffectiveTarget(_effectiveTarget, propertyPath);
  _property        = _getProperty(propertyPath);
}

void SwitchBooleanAction::execute(const std::optional<IActionEvent>& /*evt*/)
{
  auto prop = _effectiveTarget->getProperty({_property});
  if (prop && (prop.animationType() == Animation::ANIMATIONTYPE_BOOL)) {
    prop = !prop.get<bool>();
    _effectiveTarget->setProperty({_property}, prop);
  }
}

json SwitchBooleanAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
