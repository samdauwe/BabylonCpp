#include "imgui.h"

#include <vector>
#include <string>
#include <map>
#include <functional>

namespace BABYLON {

enum class DockableWindowId {
  Scene3d,
  SamplesCodeViewer,
  SampleBrowser,
  PlaygroundEditor,
  Logs,
  Inspector
};

enum class LayoutMode { Scene, SceneAndBrowser, Dev };

using GuiRenderFunction = std::function<void(void)>;

class BabylonStudioLayout
{
public:
  BabylonStudioLayout();

  void registerGuiRenderFunction(DockableWindowId id, GuiRenderFunction fn);

  void renderGui();
  void renderMenu();

  bool isVisible(DockableWindowId id);
  void setVisible(DockableWindowId id, bool v);
  void FocusWindow(DockableWindowId id);

  void PrepareLayout(ImGuiID fullDockSpaceId);
  void ApplyLayoutMode(LayoutMode mode);

private:
  std::vector<DockableWindowId> dockableWindowsList();
  std::string Label(DockableWindowId id);
  const char* Label_c(DockableWindowId id);
  void DoDock(DockableWindowId id, ImGuiID dockId);
  ImGuiID DoSplit(ImGuiID * io_dockToSplit, ImGuiDir_ splitDirection, float ratio);
  bool IsShowingSceneOnly();

  struct DockableWindowInfo {
    std::string Label;
    bool IsVisible;
    GuiRenderFunction RenderFunction;
  };

  std::map<DockableWindowId, DockableWindowInfo> _dockableWindowsInfos;
  LayoutMode _layoutMode;
};

} // namespace BABYLON
