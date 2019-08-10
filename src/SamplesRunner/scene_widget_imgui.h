#include <babylon/interfaces/irenderable_scene.h>
#include "SamplesRunner/impl/framebuffer_canvas.h"
#include <imgui.h>

namespace BABYLON {
  class SceneWidget_ImGui
  {
  public:
    SceneWidget_ImGui(ImVec2 size = ImVec2(0.f, 0.f));
    BABYLON::ICanvas * getCanvas();
    void setScene(std::shared_ptr<BABYLON::IRenderableScene> scene);
    void render();

  private:
    std::unique_ptr<BABYLON::impl::FramebufferCanvas> _framebuffer_canvas;
    std::shared_ptr<BABYLON::IRenderableScene> _scene;
  };

} // namespace BABYLON

