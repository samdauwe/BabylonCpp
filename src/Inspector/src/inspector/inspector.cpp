#include <babylon/inspector/inspector.h>

#include <iomanip>
#include <sstream>

// GLXW
#include <GLXW/glxw.h>

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
#include <babylon/imgui/imgui_impl_glfw.h>
#include <babylon/imgui/imgui_impl_opengl3.h>

// ImGUI bindings and utils
#include <babylon/imgui/icons_font_awesome_5.h>
#include <babylon/imgui/imgui_utils.h>

// BabylonCpp
#include <babylon/core/string.h>

// Inspector
#include <babylon/inspector/actions/action_store.h>
#include <babylon/inspector/components/actiontabs/action_tabs_component.h>
#include <babylon/inspector/components/global_state.h>
#include <babylon/inspector/components/sceneexplorer/scene_explorer_component.h>

namespace BABYLON {

static ImFont* _fontRegular = nullptr;
static ImFont* _fontSolid   = nullptr;

Inspector::Inspector(GLFWwindow* glfwWindow, Scene* scene)
    : _glfwWindow{glfwWindow}
    , _scene{scene}
    , _actionStore{std::make_unique<ActionStore>()}
    , _showInspectorWindow{true}
    , _globalState{GlobalState::New()}
    , _sceneExplorerHost{nullptr}
    , _actionTabsHost{nullptr}
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

  // Create Scene explorer
  ISceneExplorerComponentProps sceneExplorerComponentProps;
  sceneExplorerComponentProps.scene       = scene;
  sceneExplorerComponentProps.globalState = _globalState;
  _sceneExplorerHost
    = std::make_unique<SceneExplorerComponent>(sceneExplorerComponentProps);

  // Create action tabs
  IActionTabsComponentProps actionTabsComponentProps;
  actionTabsComponentProps.scene       = scene;
  actionTabsComponentProps.globalState = _globalState;
  _actionTabsHost
    = std::make_unique<ActionTabsComponent>(actionTabsComponentProps);
}

void Inspector::intialize()
{
  // Initialize GLXW
  if (glxwInit() != 0) {
    fprintf(stderr, "Failed to initialize GLXW\n");
    return;
  }

  // Setup Dear ImGui context
  ImGui::CreateContext();
  // Loads fonts
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontDefault();
  static ImWchar ranges[] = {0xf000, 0xf82f, 0};
  ImFontConfig config;
  config.MergeMode = true;
  auto fontRegularPath
    = String::concat("../assets/fonts/", FONT_ICON_FILE_NAME_FAR);
  _fontRegular = io.Fonts->AddFontFromFileTTF(fontRegularPath.c_str(),
                                              ImGui::IconSize, &config, ranges);
  auto fontSolidPath
    = String::concat("../assets/fonts/", FONT_ICON_FILE_NAME_FAS);
  _fontSolid = io.Fonts->AddFontFromFileTTF(fontSolidPath.c_str(),
                                            ImGui::IconSize, &config, ranges);
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(_glfwWindow, true);
#if __APPLE__
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
#else
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);
  // Actions
  _addActions();
}

void Inspector::render()
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  // Push Font
  _pushFonts();
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
  _renderInspector();
  // Pop font
  _popFonts();
  // Rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Inspector::dispose()
{
  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
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
  _actionStore->addAction("exit", ICON_FA_POWER_OFF, "Exit", "Alt+F4", []() {});
}

void Inspector::_doMenuItem(InspectorAction& a, bool enabled)
{
  if (ImGui::MenuItem(a.iconWithLabel.c_str(), a.shortcut, a.isSelected(),
                      enabled)) {
    a.invoke();
  }
}

void Inspector::_pushFonts()
{
  ImGui::PushFont(_fontRegular);
  ImGui::PushFont(_fontSolid);
}

void Inspector::_popFonts()
{
  ImGui::PopFont();
  ImGui::PopFont();
}

void Inspector::_renderInspector()
{
  if (ImGui::GetIO().DisplaySize.y <= 0) {
    return;
  }

  // Setup window size
  auto pos  = ImVec2(0, _menuHeight);
  auto size = ImGui::GetIO().DisplaySize;
  size.y -= pos.y;

  ImGui::SetNextWindowPos(pos, ImGuiSetCond_Always);
  ImGui::SetNextWindowSize(size, ImGuiSetCond_Always);
  if (ImGui::Begin("INSPECTOR", &_showInspectorWindow,
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
                     | ImGuiWindowFlags_NoScrollbar
                     | ImGuiWindowFlags_NoScrollWithMouse
                     | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
    auto width      = ImGui::GetContentRegionMax().x - 4.f;
    static auto sz1 = ImGui::GetContentRegionMax().y / 2.f;
    auto sz2        = ImGui::GetContentRegionMax().y - sz1 - 8.f;
    ImGui::Splitter(false, 1.f, &sz1, &sz2, 4, 4, width);
    // Render the scene explorer
    if (ImGui::BeginChild("SceneExplorer", ImVec2(width, sz1), true)) {
      if (_sceneExplorerHost) {
        _sceneExplorerHost->render();
      }
    }
    ImGui::EndChild();
    // Render the action tabs
    if (ImGui::BeginChild("ActionTabs", ImVec2(width, sz2), true)) {
      if (_actionTabsHost) {
        _actionTabsHost->render();
      }
    }
    ImGui::EndChild();
  }
  ImGui::End();
}

} // end of namespace BABYLON
