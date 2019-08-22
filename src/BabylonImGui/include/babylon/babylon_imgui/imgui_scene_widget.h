#include <babylon/interfaces/irenderable_scene.h>
#include "babylon/GL/framebuffer_canvas.h"
#include <imgui.h>
#include <babylon/babylon_api.h>

namespace BABYLON {
  class ImGuiSceneWidget
  {
  public:
    ImGuiSceneWidget(ImVec2 size = ImVec2(0.f, 0.f));

    BABYLON::GL::FramebufferCanvas * getCanvas();
    void setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene);
    BABYLON::IRenderableScene * getRenderableScene();
    BABYLON::Scene * getScene();
    void render();

  private:
    void initializeFramebufferCanvas();

  private:
    std::unique_ptr<BABYLON::GL::FramebufferCanvas> _framebuffer_canvas;
    std::shared_ptr<BABYLON::IRenderableScene> _renderableScene;
    ImVec2 _size;
  };

} // namespace BABYLON

