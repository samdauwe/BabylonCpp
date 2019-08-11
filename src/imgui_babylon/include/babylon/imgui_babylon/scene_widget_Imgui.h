#include <babylon/interfaces/irenderable_scene.h>
#include "babylon/GL/framebuffer_canvas.h"
#include <imgui.h>
#include <babylon/babylon_api.h>

namespace BABYLON {
  class SceneWidget_ImGui
  {
  public:
    SceneWidget_ImGui(ImVec2 size = ImVec2(0.f, 0.f));
    BABYLON::ICanvas * getCanvas();
    void setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene);
    BABYLON::IRenderableScene * getRenderableScene();
    BABYLON::Scene * getScene();
    void render();

  private:
    std::unique_ptr<BABYLON::impl::FramebufferCanvas> _framebuffer_canvas;
    std::shared_ptr<BABYLON::IRenderableScene> _renderableScene;
  };

} // namespace BABYLON

