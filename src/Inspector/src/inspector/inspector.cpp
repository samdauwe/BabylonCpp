#include <babylon/inspector/inspector.h>

// GLFW
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

// ImGui
#include <imgui.h>

// ImGui GL2
#include <examples/opengl2_example/imgui_impl_glfw_gl2.h>

// ImGUI bindings and utils
#include <babylon/imgui/icons_font_awesome.h>
#include <babylon/imgui/imgui_dock.h>

// Inspector
#include <babylon/inspector/actions/action_store.h>
#include <babylon/inspector/tabs/logs_tab.h>
#include <babylon/inspector/tabs/scene_tab.h>
#include <babylon/inspector/tabs/stats_tab.h>

namespace BABYLON {

static ImFont* _font = nullptr;

Inspector::Inspector(GLFWwindow* glfwWindow, Scene* scene)
    : _glfwWindow{glfwWindow}
    , _scene{scene}
    , _actionStore{std::make_unique<ActionStore>()}
    , _showDockingWindow{true}
    , _logsTab{::std::make_unique<LogsTab>()}
    , _sceneTab{nullptr}
    , _statsTab{nullptr}
{
}

Inspector::~Inspector()
{
}

Scene* Inspector::scene() const
{
  return _scene;
}

void Inspector::setScene(Scene* scene)
{
  _scene = scene;
  // Reset tabs
  _sceneTab = ::std::make_unique<SceneTab>(*this);
  _statsTab = ::std::make_unique<StatsTab>(*this);
}

void Inspector::intialize()
{
  // Setup ImGui binding
  ImGui::CreateContext();
  ImGui_ImplGlfwGL2_Init(_glfwWindow, true);
  // ImGui docking (Auto-load the imgui.ini settings)
  ImGui::InitDock();
  // Loads fonts
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontDefault();
  static ImWchar ranges[] = {0xf000, 0xf3ff, 0};
  ImFontConfig config;
  config.MergeMode = true;
  _font            = io.Fonts->AddFontFromFileTTF(
    "..//assets/fonts/fontawesome-webfont.ttf", 16.0f, &config, ranges);
  // Setup style
  ImGui::StyleColorsDark();
  // Actions
  _addActions();
}

void Inspector::render()
{
  // New ImGUI frame
  ImGui_ImplGlfwGL2_NewFrame();
  // Push Font
  ImGui::PushFont(_font);
  // Render main menu bar
  if (ImGui::BeginMainMenuBar()) {
    _fileMenu();
    {
      static std::ostringstream oss;
      oss.str("");
      oss << std::fixed << std::setprecision(1);
      oss << "FPS: " << ImGui::GetIO().Framerate << std::ends;
      oss << std::resetiosflags(std::ios_base::fixed
                                | std::ios_base::floatfield);
      const char* stats = oss.str().data();
      auto statsSize    = ImGui::CalcTextSize(stats);
      ImGui::SameLine(ImGui::GetContentRegionMax().x - statsSize.x);
      ImGui::Text("%s", stats);
    }
    _menuHeight = static_cast<int>(ImGui::GetWindowSize().y);
    ImGui::EndMainMenuBar();
  }
  // Render dock widgets
  _renderDockGUI();
  // Pop font
  ImGui::PopFont();
  // Rendering
  ImGui::Render();
  ImGui_ImplGlfwGL2_RenderDrawData(ImGui::GetDrawData());
}

void Inspector::dispose()
{
  // Dispose tabs
  if (_logsTab) {
    _logsTab->dispose();
  }
  if (_sceneTab) {
    _sceneTab->dispose();
  }
  if (_statsTab) {
    _statsTab->dispose();
  }
  // Shotdown ImGui
  ImGui_ImplGlfwGL2_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(_glfwWindow);
}

void Inspector::_fileMenu()
{
  if (!ImGui::BeginMenu("File")) {
    return;
  }
  _doMenuItem(*_actionStore->getAction("exit"), true);
  ImGui::EndMenu();
}

void Inspector::_addActions()
{
  _actionStore->addAction("exit", ICON_FA_POWER_OFF, "Exit", "Alt+F4",
                          [this]() {});
}

void Inspector::_doMenuItem(InspectorAction& a, bool enabled)
{
  if (ImGui::MenuItem(a.iconWithLabel.c_str(), a.shortcut, a.isSelected(),
                      enabled)) {
    a.invoke();
  }
}

void Inspector::_renderDockGUI()
{
  if (ImGui::GetIO().DisplaySize.y <= 0) {
    return;
  }

  if (!_logsTab || !_sceneTab || !_statsTab) {
    return;
  }

  // Setup root docking window size
  auto pos  = ImVec2(0, _menuHeight);
  auto size = ImGui::GetIO().DisplaySize;
  size.y -= pos.y;

  ImGui::SetNextWindowPos(pos, ImGuiSetCond_Always);
  ImGui::SetNextWindowSize(size, ImGuiSetCond_Always);
  if (ImGui::Begin("Inspector", &_showDockingWindow,
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
                     | ImGuiWindowFlags_NoScrollbar
                     | ImGuiWindowFlags_NoScrollWithMouse
                     | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
    // Dock layout by hard-coded or .ini file
    ImGui::BeginDockspace();

    // Render logs dock
    if (_logsTab) {
      _logsTab->render();
    }

    // Render scene dock
    if (_sceneTab) {
      _sceneTab->render();
    }

    // Render statistics dock
    if (_statsTab) {
      _statsTab->render();
    }

    ImGui::EndDockspace();
  }
  ImGui::End();
}

} // end of namespace BABYLON
