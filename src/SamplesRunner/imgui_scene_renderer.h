#include <babylon/interfaces/irenderable_scene.h>
#include "SamplesRunner/impl/framebuffer_canvas.h"
#include <imgui.h>

namespace BABYLON {
  namespace ImGuiRender {

    class ImGuiSceneRenderer
    {
    public:
      ImGuiSceneRenderer(int width = 640, int height = 480);
      BABYLON::ICanvas * getCanvas();
      void setScene(BABYLON::IRenderableScene * scene);
      void showGui();

    private:
      std::unique_ptr<BABYLON::impl::FramebufferCanvas> _framebuffer_canvas;
      BABYLON::IRenderableScene *_scene = nullptr; // not owned!
    };

  } // namespace ImGui

} // namespace BABYLON

