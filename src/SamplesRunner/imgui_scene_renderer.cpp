#include "SamplesRunner/impl/framebuffer_canvas.h"
#include "SamplesRunner/imgui_scene_renderer.h"
#include <imgui.h>

namespace BABYLON {
namespace ImGuiRender {

  float distVec2(ImVec2 p1, ImVec2 p2)
  {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
  }

  ImGuiSceneRenderer::ImGuiSceneRenderer(int width, int height)
  {
    _framebuffer_canvas = std::make_unique<BABYLON::impl::FramebufferCanvas>();
    _framebuffer_canvas->clientWidth = width;
    _framebuffer_canvas->clientHeight = height;
    _framebuffer_canvas->initializeFrameBuffer();
  }

  BABYLON::ICanvas * ImGuiSceneRenderer::getCanvas()
  {
    return _framebuffer_canvas.get();
  }

  void ImGuiSceneRenderer::setScene(BABYLON::IRenderableScene * scene)
  {
    _scene = scene;
    _scene->initialize();
  }

  void ImGuiSceneRenderer::showGui()
  {
    ImVec2 canvasSize(
      static_cast<float>(_framebuffer_canvas->width),
      static_cast<float>(_framebuffer_canvas->height));

    _framebuffer_canvas->bind();
    _scene->render();
    _framebuffer_canvas->unbind();

    auto id = _framebuffer_canvas->TextureId();
    ImGui::Image(reinterpret_cast<void*>(id), canvasSize);

    int maxKey = 512;
    bool ctrlKey = ImGui::GetIO().KeyCtrl;
    bool altKey = ImGui::GetIO().KeyAlt;
    bool shiftKey = ImGui::GetIO().KeyShift;
    for (int keyCode = 0; keyCode < maxKey; keyCode++)
    {
      static std::string code = "dummy";
      if (ImGui::IsKeyPressed(keyCode))
        _framebuffer_canvas->onKeyDown(ctrlKey, altKey, keyCode, code);
      if (ImGui::IsKeyReleased(keyCode))
        _framebuffer_canvas->onKeyUp(ctrlKey, altKey, keyCode, code);
    }

    if (ImGui::IsItemHovered())
    {
      static ImVec2 lastMousePos(0., 0.);
      ImVec2 mousePos = ImGui::GetMousePos();
      int xpos = static_cast<int>(mousePos.x);
      int ypos = static_cast<int>(mousePos.y);
      if (distVec2(mousePos, lastMousePos) > 0.1f)
      {
        _framebuffer_canvas->onMouseMove(ctrlKey, shiftKey, xpos, ypos);
        lastMousePos = mousePos;
      }

      for (int mouseButton = 0; mouseButton < 3; mouseButton++)
      {
        if (ImGui::IsMouseClicked(mouseButton))
          _framebuffer_canvas->onMouseButtonDown(ctrlKey, shiftKey, xpos, ypos, BABYLON::MouseButtonType(mouseButton));
        if (ImGui::IsMouseReleased(mouseButton))
          _framebuffer_canvas->onMouseButtonUp(ctrlKey, shiftKey, xpos, ypos, BABYLON::MouseButtonType(mouseButton));
      }
    }

    // Mouse wheel is not handled!
    //_sceneWindow.renderCanvas->onMouseWheel(
    //  ctrlKey, shiftKey, static_cast<int>(xpos), static_cast<int>(ypos),
    //  RescaleMouseScroll(yoffset));


  }




} // namespace ImGui

} // namespace BABYLON

