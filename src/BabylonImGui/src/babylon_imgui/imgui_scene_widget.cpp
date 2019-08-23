#include "babylon/GL/framebuffer_canvas.h"
#include "babylon/babylon_imgui/imgui_scene_widget.h"
#include <cmath>
#include <imgui.h>

#ifdef _MSC_VER
#pragma warning(disable: 4312)
#endif

namespace BABYLON {

  float distVec2(ImVec2 p1, ImVec2 p2)
  {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
  }

  ImGuiSceneWidget::ImGuiSceneWidget(ImVec2 size) :
    _size(size)
  {
  }

  void ImGuiSceneWidget::initializeFramebufferCanvas()
  {
    ImVec2 realSize(_size);
    _framebuffer_canvas = std::make_unique<BABYLON::GL::FramebufferCanvas>();
    if (realSize.x < 0.1f) {
      realSize = ImGui::GetIO().DisplaySize;
      realSize.y -= 20.f;
    }

    int width = static_cast<int>(realSize.x);
    int height = static_cast<int>(realSize.y);
    _framebuffer_canvas->clientWidth = width;
    _framebuffer_canvas->clientHeight = height;
    _framebuffer_canvas->initializeFrameBuffer();
  }

  BABYLON::GL::FramebufferCanvas * ImGuiSceneWidget::getCanvas()
  {
    return _framebuffer_canvas.get();
  }

  void ImGuiSceneWidget::setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene)
  {
    initializeFramebufferCanvas();

    _renderableScene = scene;
    _renderableScene->initialize(_framebuffer_canvas.get());
  }

  BABYLON::IRenderableScene * ImGuiSceneWidget::getRenderableScene()
  {
    return _renderableScene.get();
  }

  BABYLON::Scene * ImGuiSceneWidget::getScene()
  {
    return _renderableScene->getScene();
  }

  void ImGuiSceneWidget::render()
  {
    ImVec2 canvasSize(
      static_cast<float>(_framebuffer_canvas->width),
      static_cast<float>(_framebuffer_canvas->height));

    _framebuffer_canvas->bind();
    _renderableScene->render();
    _framebuffer_canvas->unbind();

    auto id = _framebuffer_canvas->textureId();
    const ImVec2 uv0 = ImVec2(0, 1);
    const ImVec2 uv1 = ImVec2(1, 0);
    ImGui::Image(reinterpret_cast<void*>(id), canvasSize, uv0, uv1);

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

      float mouseWheel = ImGui::GetIO().MouseWheel;
      if (fabs(mouseWheel) > 0.01f)
        _framebuffer_canvas->onMouseWheel(ctrlKey, shiftKey, xpos, ypos, mouseWheel);
    }
  }

} // namespace BABYLON
