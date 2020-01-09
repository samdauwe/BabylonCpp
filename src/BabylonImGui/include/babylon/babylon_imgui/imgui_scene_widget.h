#include <vector>
#include <functional>
#include <babylon/interfaces/irenderable_scene.h>
#include "babylon/GL/framebuffer_canvas.h"
#include <imgui.h>
#include <babylon/babylon_api.h>

namespace BABYLON {
  class BABYLON_SHARED_EXPORT ImGuiSceneWidget
  {
  public:
    ImGuiSceneWidget(ImVec2 size);

    BABYLON::GL::FramebufferCanvas * getCanvas();
    void setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene);
    BABYLON::IRenderableScene * getRenderableScene();
    BABYLON::Scene * getScene();
    void render(ImVec2 size = ImVec2(0.f, 0.f)); // if no size is transmitted, use the size obtained at construction

    // This callback will be called before resizing
    // (a good time to unlink the inspector, since its observed scene will change)
    using CallbackBeforeResize = std::function<void(void)>;
    std::vector<CallbackBeforeResize> OnBeforeResize;

  private:
    void initializeFramebufferCanvas();
    void resizeFramebuffer();

  private:
    std::unique_ptr<BABYLON::GL::FramebufferCanvas> _framebuffer_canvas;
    std::shared_ptr<BABYLON::IRenderableScene> _renderableScene;
    int _width, _height;
  };

} // namespace BABYLON

