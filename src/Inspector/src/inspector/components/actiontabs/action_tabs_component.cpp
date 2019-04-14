#include <babylon/inspector/components/actiontabs/action_tabs_component.h>

#include <imgui.h>

#include <babylon/core/string.h>
#include <babylon/inspector/components/actiontabs/tabs/debug_tab_component.h>
#include <babylon/inspector/components/actiontabs/tabs/property_grid_tab_component.h>
#include <babylon/inspector/components/actiontabs/tabs/statistics_tab_component.h>

namespace BABYLON {

ActionTabsComponent::ActionTabsComponent(
  const IActionTabsComponentProps& iProps)
    : props{iProps}
    , _propertyGridTabComponent{nullptr}
    , _debugTabComponent{nullptr}
    , _statisticsTabComponent{nullptr}
{
  componentWillMount();
}

ActionTabsComponent::~ActionTabsComponent()
{
  componentWillUnmount();
}

void ActionTabsComponent::componentWillMount()
{
  IPaneComponentProps paneProps;
  paneProps.scene = props.scene;

  _propertyGridTabComponent
    = std::make_unique<PropertyGridTabComponent>(paneProps);
  _debugTabComponent      = std::make_unique<DebugTabComponent>(paneProps);
  _statisticsTabComponent = std::make_unique<StatisticsTabComponent>(paneProps);
}

void ActionTabsComponent::componentWillUnmount()
{
}

void ActionTabsComponent::render()
{
  static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
  if (ImGui::BeginTabBar("ActionTabs", tab_bar_flags)) {
    // Properties grid tab
    if (_propertyGridTabComponent) {
      static const auto propertiesTabLabel
        = String::concat(faFileAlt, " ", "Properties");
      if (ImGui::BeginTabItem(propertiesTabLabel.c_str())) {
        if (ImGui::BeginChild("scrollingArea")) {
          _propertyGridTabComponent->render();
          ImGui::EndChild();
        }
        ImGui::EndTabItem();
      }
    }
    // Debug tab
    if (_debugTabComponent) {
      static const auto debugTabLabel = String::concat(faBug, " ", "Debug");
      if (ImGui::BeginTabItem(debugTabLabel.c_str())) {
        if (ImGui::BeginChild("scrollingArea")) {
          _debugTabComponent->render();
          ImGui::EndChild();
        }
        ImGui::EndTabItem();
      }
    }
    if (_statisticsTabComponent) {
      static const auto statisticsTabLabel
        = String::concat(faChartBar, " ", "Statistics");
      if (ImGui::BeginTabItem(statisticsTabLabel.c_str())) {
        if (ImGui::BeginChild("scrollingArea")) {
          _statisticsTabComponent->render();
          ImGui::EndChild();
        }
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
  }
}

} // end of namespace BABYLON
