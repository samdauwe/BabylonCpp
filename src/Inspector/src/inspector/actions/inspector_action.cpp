#include <babylon/inspector/actions/inspector_action.h>

#include <babylon/core/string.h>

#include <iostream>

namespace BABYLON {

InspectorAction::InspectorAction()
    : id{"\0"}, icon{"\0"}, label{"\0"}, iconWithLabel{"\0"}, shortcut{"\0"}
{
}

InspectorAction::InspectorAction(const char* iId, const char* iIcon,
                                 const char* iLabel, const char* iShortcut,
                                 const SA::delegate<void()>& iCallback)
    : id{iId}
    , icon{iIcon}
    , label{iLabel}
    , iconWithLabel{String::printf("%s %s", iIcon, iLabel)}
    , shortcut{iShortcut}
    , callback{iCallback}
    , isSelected{SA::delegate<bool()>::create<&InspectorAction::FalseConst>()}
{
}

InspectorAction::InspectorAction(const InspectorAction& rhs)
    : id{rhs.id}
    , icon{rhs.icon}
    , label{rhs.label}
    , iconWithLabel{rhs.iconWithLabel}
    , shortcut{rhs.shortcut}
    , callback{rhs.callback}
    , isSelected{rhs.isSelected}
{
}

InspectorAction::InspectorAction(InspectorAction&& rhs)
    : id{std::move(rhs.id)}
    , icon{std::move(rhs.icon)}
    , label{std::move(rhs.label)}
    , iconWithLabel{std::move(rhs.iconWithLabel)}
    , shortcut{std::move(rhs.shortcut)}
    , callback{std::move(rhs.callback)}
    , isSelected{std::move(rhs.isSelected)}
{
}

InspectorAction& InspectorAction::operator=(const InspectorAction& rhs)
{
  if (&rhs != this) {
    id            = rhs.id;
    icon          = rhs.icon;
    label         = rhs.label;
    iconWithLabel = rhs.iconWithLabel;
    shortcut      = rhs.shortcut;
    callback      = rhs.callback;
    isSelected    = rhs.isSelected;
  }

  return *this;
}

InspectorAction& InspectorAction::operator=(InspectorAction&& rhs)
{
  if (&rhs != this) {
    std::swap(id, rhs.id);
    std::swap(icon, rhs.icon);
    std::swap(label, rhs.label);
    std::swap(iconWithLabel, rhs.iconWithLabel);
    std::swap(shortcut, rhs.shortcut);
    std::swap(callback, rhs.callback);
    std::swap(isSelected, rhs.isSelected);
  }

  return *this;
}

InspectorAction::~InspectorAction()
{
}

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
