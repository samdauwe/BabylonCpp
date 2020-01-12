#include <babylon/inspector/actions/inspector_action.h>

#include <babylon/misc/string_tools.h>

#include <iostream>

namespace BABYLON {

InspectorAction::InspectorAction()
    : id{"\0"}, icon{"\0"}, label{"\0"}, iconWithLabel{"\0"}, shortcut{"\0"}
{
}

InspectorAction::InspectorAction(const char* iId, const char* iIcon, const char* iLabel,
                                 const char* iShortcut, const SA::delegate<void()>& iCallback)
    : id{iId}
    , icon{iIcon}
    , label{iLabel}
    , iconWithLabel{StringTools::printf("%s %s", iIcon, iLabel)}
    , shortcut{iShortcut}
    , callback{iCallback}
    , isSelected{SA::delegate<bool()>::create<&InspectorAction::FalseConst>()}
{
}

InspectorAction::InspectorAction(const InspectorAction& rhs) = default;

InspectorAction::InspectorAction(InspectorAction&& rhs) = default;

InspectorAction& InspectorAction::operator=(const InspectorAction& rhs) = default;

InspectorAction& InspectorAction::operator=(InspectorAction&& rhs) = default;

InspectorAction::~InspectorAction() = default;

bool InspectorAction::FalseConst()
{
  return false;
}

void InspectorAction::invoke()
{
  if (callback) {
    callback();
  }
}

} // end of namespace BABYLON
