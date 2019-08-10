#include <imgui_utils/app_runner/imgui_runner.h>
#include <imgui_utils/app_runner/imgui_runner_demo.h>
#include "SamplesRunner/impl/framebuffer_canvas.h"
#include "SamplesRunner/HelloScene.h"

#ifdef _MSC_VER
#pragma warning(disable: 4312)
#endif // _MSC_VER

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


//static void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
//{
//  if (_sceneWindow.glfwWindow == window) {
//    _sceneWindow.width = width;
//    _sceneWindow.height = height;
//    // Update canvas size
//    if (_sceneWindow.renderCanvas) {
//      _sceneWindow.renderCanvas->setFrameSize(width, height);
//      _sceneWindow.renderableScene->getEngine()->resize();
//    }
//  }
//}

float DistVec2(ImVec2 p1, ImVec2 p2)
{
  float dx = p2.x - p1.x;
  float dy = p2.y - p1.y;
  return sqrt(dx * dx + dy * dy);
}

void MyGui(SceneWithFramebuffer & sceneWithFramebuffer)
{
  ImVec2 canvasSize(
    static_cast<float>(sceneWithFramebuffer._canvas->width),
    static_cast<float>(sceneWithFramebuffer._canvas->height));

  ImGui::Begin("Win", NULL, ImGuiWindowFlags_NoMove);
  ImGui::Text("Hello");

  sceneWithFramebuffer._canvas->bind();
  sceneWithFramebuffer._scene->render();
  sceneWithFramebuffer._canvas->unbind();

  auto id = sceneWithFramebuffer._canvas->TextureId();
  ImGui::Image(reinterpret_cast<void*>(id), canvasSize);

  int maxKey = 512;
  bool ctrlKey = ImGui::GetIO().KeyCtrl;
  bool altKey = ImGui::GetIO().KeyAlt;
  bool shiftKey = ImGui::GetIO().KeyShift;
  for (int keyCode = 0; keyCode < maxKey; keyCode++)
  {
    static std::string code = "dummy";
    if (ImGui::IsKeyPressed(keyCode))
      sceneWithFramebuffer._canvas->onKeyDown(ctrlKey, altKey, keyCode, code);
    if (ImGui::IsKeyReleased(keyCode))
      sceneWithFramebuffer._canvas->onKeyUp(ctrlKey, altKey, keyCode, code);
  }
  
  if (ImGui::IsItemHovered())
  {
    static ImVec2 lastMousePos(0., 0.);
    ImVec2 mousePos = ImGui::GetMousePos();
    int xpos = static_cast<int>(mousePos.x);
    int ypos = static_cast<int>(mousePos.y);
    if (DistVec2(mousePos,lastMousePos) > 0.1f)
    {
      sceneWithFramebuffer._canvas->onMouseMove(ctrlKey, shiftKey, xpos, ypos);
      lastMousePos = mousePos;
    }

    for (int mouseButton = 0; mouseButton < 3; mouseButton++)
    {
      if (ImGui::IsMouseClicked(mouseButton))
        sceneWithFramebuffer._canvas->onMouseButtonDown(ctrlKey, shiftKey, xpos, ypos, BABYLON::MouseButtonType(mouseButton));
      if (ImGui::IsMouseReleased(mouseButton))
        sceneWithFramebuffer._canvas->onMouseButtonUp(ctrlKey, shiftKey, xpos, ypos, BABYLON::MouseButtonType(mouseButton));
    }
  }

  // Mouse wheel is not handled!
  //_sceneWindow.renderCanvas->onMouseWheel(
  //  ctrlKey, shiftKey, static_cast<int>(xpos), static_cast<int>(ypos),
  //  RescaleMouseScroll(yoffset));


  static char truc[1000];
  ImGui::InputText("truc", truc, 1000);
  ImGui::Text("end");

  //ImGui::IsKeyPressed()

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
