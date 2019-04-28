#include <babylon/inspector/components/actiontabs/action_tabs_component.h>

#include <imgui.h>

#include <babylon/core/string.h>
#include <babylon/inspector/components/actiontabs/tabs/debug_tab_component.h>
#include <babylon/inspector/components/actiontabs/tabs/logs_tab_component.h>
#include <babylon/inspector/components/actiontabs/tabs/property_grid_tab_component.h>
#include <babylon/inspector/components/actiontabs/tabs/statistics_tab_component.h>
#include <babylon/inspector/components/global_state.h>

#include <iostream>

namespace BABYLON {

ActionTabsComponent::ActionTabsComponent(
  const IActionTabsComponentProps& iProps)
    : props{iProps}
    , _propertyGridTabComponent{nullptr}
    , _debugTabComponent{nullptr}
    , _statisticsTabComponent{nullptr}
    , _logsTabComponent{nullptr}
    , _onSelectionChangeObserver{nullptr}
{
  componentWillMount();
}

ActionTabsComponent::~ActionTabsComponent()
{
  componentWillUnmount();
}

void ActionTabsComponent::setScene(Scene* scene)
{
  // Create tab widgets
  props.scene      = scene;
  _paneProps.scene = props.scene;
  _propertyGridTabComponent
    = std::make_unique<PropertyGridTabComponent>(_paneProps);
  _debugTabComponent = std::make_unique<DebugTabComponent>(_paneProps);
  _statisticsTabComponent
    = std::make_unique<StatisticsTabComponent>(_paneProps);
}

void ActionTabsComponent::componentWillMount()
{
  // Create selection change observer
  _onSelectionChangeObserver
    = props.globalState->onSelectionChangedObservable.add(
      [this](EntityInfo* entity, EventState & /*es*/) -> void {
        if (entity && (entity->uniqueId.has_value() || !entity->name.empty())) {
          _paneProps.selectedEntity
            = {entity->type, *entity->uniqueId, entity->name};
        }
      });

  // Create logs tab widget
  _logsTabComponent = std::make_unique<LogsTabComponent>(_paneProps);
}

void ActionTabsComponent::componentWillUnmount()
{
  if (_onSelectionChangeObserver) {
    props.globalState->onSelectionChangedObservable.remove(
      _onSelectionChangeObserver);
  }
}

void ActionTabsComponent::render()
{
  static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
  if (ImGui::BeginTabBar("ActionTabs", tab_bar_flags)) {
    // Properties tab
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
    // Statistics tab
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
    // Logs Tab
    if (_logsTabComponent) {
      static const auto logsTabLabel
        = String::concat(faInfoCircle, " ", "Logs");
      if (ImGui::BeginTabItem(logsTabLabel.c_str())) {
        if (ImGui::BeginChild("scrollingArea")) {
          _logsTabComponent->render();
          ImGui::EndChild();
        }
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
  }
}

} // end of namespace BABYLON
