#include <babylon/inspector/components/actiontabs/action_tabs_component.h>

#include <imgui.h>
#include <babylon/misc/string_tools.h>
#include <babylon/inspector/components/actiontabs/tabs/debug_tab_component.h>
#include <babylon/inspector/components/actiontabs/tabs/property_grid_tab_component.h>
#include <babylon/inspector/components/actiontabs/tabs/statistics_tab_component.h>
#include <babylon/inspector/components/global_state.h>

#include <iostream>

namespace BABYLON {

std::unordered_map<std::string, std::string> _ActionTabsComponent_Labels{
  {"Properties", std::string(ICON_FA_FILE_ALT) + " Properties"},
  {"Debug", std::string(ICON_FA_BUG) + " Debug"},
  {"Statistics", std::string(ICON_FA_CHART_BAR) + " Statistics"},
};

ActionTabsComponent::ActionTabsComponent(
  const IActionTabsComponentProps& iProps)
    : props{iProps}
    , _propertyGridTabComponent{nullptr}
    , _debugTabComponent{nullptr}
    , _statisticsTabComponent{nullptr}
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
  if (ImGui::BeginTabBar("Actions Tabs"))
  {
    // Properties
    if (_propertyGridTabComponent) {
      if (ImGui::BeginTabItem(_ActionTabsComponent_Labels.at("Properties").c_str())) {
        _propertyGridTabComponent->render();
        ImGui::EndTabItem();
      }
    }

    // Debug
    if (_debugTabComponent) {
      if (ImGui::BeginTabItem(_ActionTabsComponent_Labels.at("Debug").c_str())) {
        _debugTabComponent->render();
        ImGui::EndTabItem();
      }
    }

    // Statistics
    if (_statisticsTabComponent) {
      if (ImGui::BeginTabItem(_ActionTabsComponent_Labels.at("Statistics").c_str())) {
        _statisticsTabComponent->render();
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
  }
}

} // end of namespace BABYLON
