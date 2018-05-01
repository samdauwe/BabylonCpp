#include <babylon/inspector/tabs/property_tab.h>

#include <babylon/babylon_version.h>
#include <babylon/core/logging.h>
#include <babylon/imgui/imgui_dock.h>
#include <babylon/imgui/imgui_utils.h>
#include <babylon/inspector/adapters/light_adapter.h>

namespace BABYLON {

PropertyTab::PropertyTab(const string_t& name, Inspector& inspector)
    : Tab{name}, _inspector{inspector}
{
}

PropertyTab::~PropertyTab()
{
}

void PropertyTab::render()
{
  if (ImGui::BeginDock(name.c_str())) {
    float width = ImGui::GetContentRegionMax().x;
    float sz1   = ImGui::GetContentRegionMax().y / 2.f;
    float sz2   = ImGui::GetContentRegionMax().y / 2.f;
    ImGui::Splitter(false, 1.0f, &sz1, &sz2, 8, 8, width);
    // Render the tree panel
    if (ImGui::BeginChild("top", ImVec2(width, sz1), true)) {
      _renderTree();
    }
    ImGui::EndChild();
    // Render the properties panel
    if (ImGui::BeginChild("bottom", ImVec2(width, sz2), true)) {
      _renderProperties();
    }
    ImGui::EndChild();
  }
  ImGui::EndDock();
}

void PropertyTab::dispose()
{
}

} // end of namespace BABYLON
