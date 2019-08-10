#include <imgui_utils/app_runner/imgui_runner.h>
#include <imgui_utils/app_runner/imgui_runner_demo.h>
#include "SamplesRunner/impl/framebuffer_canvas.h"
#include "SamplesRunner/HelloScene.h"

struct SceneWithFramebuffer
{
  std::unique_ptr<BABYLON::impl::FramebufferCanvas> _canvas;
  std::unique_ptr<BABYLON::IRenderableScene> _scene;
};

struct AppContext
{
  AppContext()
  {
    _sceneWithFramebuffer._canvas = std::make_unique<BABYLON::impl::FramebufferCanvas>();
    _sceneWithFramebuffer._canvas->clientWidth = 640;
    _sceneWithFramebuffer._canvas->clientHeight = 480;
    _sceneWithFramebuffer._canvas->initializeFrameBuffer();
    _sceneWithFramebuffer._scene = std::make_unique<SamplesRunner::HelloScene>(_sceneWithFramebuffer._canvas.get());
    _sceneWithFramebuffer._scene->initialize();
  }
  SceneWithFramebuffer _sceneWithFramebuffer;
};



void MyGui(SceneWithFramebuffer & sceneWithFramebuffer)
{
  ImVec2 canvasSize(
    static_cast<float>(sceneWithFramebuffer._canvas->width),
    static_cast<float>(sceneWithFramebuffer._canvas->height));

  ImGui::Begin("Win");
  ImGui::Text("Hello");

  sceneWithFramebuffer._canvas->bind();
  sceneWithFramebuffer._scene->render();
  sceneWithFramebuffer._canvas->unbind();

  auto id = sceneWithFramebuffer._canvas->TextureId();
  ImGui::Image((void*)id, canvasSize);

  ImGui::Text("end");
  ImGui::End();
}


int main(int, char**)
{
  std::unique_ptr<AppContext> appContext;  
  auto initScene = [&]() {
    appContext = std::make_unique<AppContext>();
  };

  auto showGui = [&]() {
    MyGui(appContext->_sceneWithFramebuffer);
  };

  ImGuiUtils::ImGuiRunner::WindowParams params;
  ImGuiUtils::ImGuiRunner::RunGui(showGui, params, initScene);
  return 0;
}
