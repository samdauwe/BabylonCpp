#include <babylon/inspector/components/sceneexplorer/scene_explorer_component.h>

#include <string>
#include <vector>

// ImGui
#include <imgui.h>

#include <babylon/engine/scene.h>
#include <babylon/inspector/components/sceneexplorer/scene_graph_component.h>

namespace BABYLON {

SceneExplorerComponent::SceneExplorerComponent(
  const ISceneExplorerComponentProps& iProps)
    : props{iProps}
    , _sceneGraphComponent{std::make_unique<SceneGraphComponent>(iProps.scene)}
{
}

SceneExplorerComponent::~SceneExplorerComponent()
{
}

void SceneExplorerComponent::render()
{
  // Filter
  {
    static ImGuiTextFilter filter;
    filter.Draw();
    const std::vector<std::string> lines;
    for (const auto& line : lines) {
      if (filter.PassFilter(line.c_str())) {
        ImGui::BulletText("%s", line.c_str());
      }
    }
  }

  // The Scene Graph
  if (_sceneGraphComponent) {
    _sceneGraphComponent->render();
  }
}

} // end of namespace BABYLON
