#include <babylon/GL/framebuffer_canvas.h>
#include <babylon/babylon_imgui/imgui_scene_widget.h>
#include <cmath>
#include <imgui.h>

#ifdef _MSC_VER
#pragma warning(disable : 4312)
#endif

namespace BABYLON {

float distVec2(ImVec2 p1, ImVec2 p2)
{
  float dx = p2.x - p1.x;
  float dy = p2.y - p1.y;
  return sqrt(dx * dx + dy * dy);
}

ImGuiSceneWidget::ImGuiSceneWidget(ImVec2 size)
{
  _width  = static_cast<int>(size.x);
  _height = static_cast<int>(size.y);
}

void ImGuiSceneWidget::initializeFramebufferCanvas()
{
  _framebuffer_canvas               = std::make_unique<BABYLON::GL::FramebufferCanvas>();
  _framebuffer_canvas->clientWidth  = _width;
  _framebuffer_canvas->clientHeight = _height;
  _framebuffer_canvas->initializeFrameBuffer();
}

BABYLON::GL::FramebufferCanvas* ImGuiSceneWidget::getCanvas()
{
  return _framebuffer_canvas.get();
}

void ImGuiSceneWidget::setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene)
{
  initializeFramebufferCanvas();
  _renderableScene = scene;
  _renderableScene->initialize(_framebuffer_canvas.get());
}

void ImGuiSceneWidget::resizeFramebuffer()
{
  for (auto callback : OnBeforeResize)
    callback();
  initializeFramebufferCanvas();
  _renderableScene->initialize(_framebuffer_canvas.get());
}

BABYLON::IRenderableScene* ImGuiSceneWidget::getRenderableScene()
{
  return _renderableScene.get();
}

BABYLON::Scene* ImGuiSceneWidget::getScene()
{
  return _renderableScene->getScene();
}

void ImGuiSceneWidget::render(ImVec2 size)
{
  // Resize framebuffer if needed
  int width  = static_cast<int>(size.x);
  int height = static_cast<int>(size.y);

  if ((width > 0) && (height > 0)) {
    if ((width != _width) || (height != _height)) {
#if 0
      if (!ImGui::IsMouseDragging() && !ImGui::IsAnyMouseDown()) {
        _width  = width;
        _height = height;
        resizeFramebuffer();
      }
#endif
    }
  }

  ImVec2 canvasSize(static_cast<float>(_framebuffer_canvas->clientWidth),
                    static_cast<float>(_framebuffer_canvas->clientHeight));

  _framebuffer_canvas->bind();
  _renderableScene->render();
  _framebuffer_canvas->unbind();

  auto id          = _framebuffer_canvas->textureId();
  const ImVec2 uv0 = ImVec2(0, 1);
  const ImVec2 uv1 = ImVec2(1, 0);
  ImGui::Image(reinterpret_cast<void*>(id), canvasSize, uv0, uv1);

  int maxKey    = 512;
  bool ctrlKey  = ImGui::GetIO().KeyCtrl;
  bool altKey   = ImGui::GetIO().KeyAlt;
  bool shiftKey = ImGui::GetIO().KeyShift;
  for (int keyCode = 0; keyCode < maxKey; keyCode++) {
    static std::string code = "dummy";
    if (ImGui::IsKeyPressed(keyCode))
      _framebuffer_canvas->onKeyDown(ctrlKey, altKey, keyCode, code);
    if (ImGui::IsKeyReleased(keyCode))
      _framebuffer_canvas->onKeyUp(ctrlKey, altKey, keyCode, code);
  }

  if (ImGui::IsItemHovered()) {
    // @see https://github.com/ocornut/imgui/issues/2486
    ImVec2 windowMin = ImGui::GetWindowContentRegionMin();
    windowMin.x += ImGui::GetWindowPos().x;
    windowMin.y += ImGui::GetWindowPos().y;

    static ImVec2 lastMousePos(0., 0.);
    ImVec2 mousePos = ImGui::GetMousePos();
    int xpos        = static_cast<int>(mousePos.x - windowMin.x);
    int ypos        = static_cast<int>(mousePos.y - windowMin.y);
    if (distVec2(mousePos, lastMousePos) > 0.1f) {
      _framebuffer_canvas->onMouseMove(ctrlKey, shiftKey, xpos, ypos);
      lastMousePos = mousePos;
    }

    for (int mouseButton = 0; mouseButton < 3; mouseButton++) {
      if (ImGui::IsMouseClicked(mouseButton))
        _framebuffer_canvas->onMouseButtonDown(ctrlKey, shiftKey, xpos, ypos,
                                               BABYLON::MouseButtonType(mouseButton));
      if (ImGui::IsMouseReleased(mouseButton))
        _framebuffer_canvas->onMouseButtonUp(ctrlKey, shiftKey, xpos, ypos,
                                             BABYLON::MouseButtonType(mouseButton));
    }

    float mouseWheel = ImGui::GetIO().MouseWheel;
    if (fabs(mouseWheel) > 0.01f)
      _framebuffer_canvas->onMouseWheel(ctrlKey, shiftKey, xpos, ypos, mouseWheel);
  }
}

} // namespace BABYLON
