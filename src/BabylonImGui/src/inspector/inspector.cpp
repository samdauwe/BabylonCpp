#include <babylon/inspector/inspector.h>

#include <iomanip>
#include <sstream>

// glad
#include <glad/glad.h>

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
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// ImGUI bindings and utils
#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>

// BabylonCpp
#include <babylon/core/string.h>

// Inspector
#include <babylon/inspector/actions/action_store.h>
#include <babylon/inspector/components/actiontabs/action_tabs_component.h>
#include <babylon/inspector/components/global_state.h>
#include <babylon/inspector/components/sceneexplorer/scene_explorer_component.h>

#ifdef _WIN32
// See warning about windows mixing dll and stl in this file!
#include <babylon/core/logging/log_levels_statics.cpp.h> 
#endif

namespace BABYLON {

static ImFont* _fontRegular = nullptr;
static ImFont* _fontSolid   = nullptr;

std::function<void(const std::string &)> Inspector::OnSampleChanged;

Inspector::Inspector(GLFWwindow* glfwWindow, Scene* scene) // KK remove glfwWindow
    : _glfwWindow{glfwWindow}
    , _scene{scene}
    , _actionStore{std::make_unique<ActionStore>()}
    , _globalState{GlobalState::New()}
    , _sceneExplorerHost{nullptr}
    , _actionTabsHost{nullptr}
{
  // Create action tabs
  IActionTabsComponentProps actionTabsComponentProps;
  actionTabsComponentProps.scene       = scene;
  actionTabsComponentProps.globalState = _globalState;
  _actionTabsHost
    = std::make_unique<ActionTabsComponent>(actionTabsComponentProps);
}

Inspector::~Inspector() = default;

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
  _actionTabsHost->setScene(scene);
}

void Inspector::imgui_initialize()
{
  // Initialize glad
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    fprintf(stderr, "Failed to initialize glad\n");
    return;
  }

  // Setup Dear ImGui context
  ImGui::CreateContext();
  imgui_LoadFontAwesome();
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(_glfwWindow, true);
#if __APPLE__
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);
  // Actions
  _addActions();
}

void Inspector::imgui_LoadFontAwesome()
{
  // Loads fonts
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontDefault();
  static ImWchar ranges[] = { 0xf000, 0xf82f, 0 };
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
}

void Inspector::render()
{
  _renderInspector();
}

void Inspector::_showFps()
{
  char stats[1000];
  sprintf(stats, "FPS: %.1lf", ImGui::GetIO().Framerate);
  //auto statsSize = ImGui::CalcTextSize(stats);
  //ImGui::SameLine(ImGui::GetContentRegionMax().x - statsSize.x);
  ImGui::Text("%s", stats);
}

void Inspector::imgui_dispose()
{
  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(_glfwWindow);
}

void Inspector::_fileMenu()
{
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
  // Render the scene explorer
  if (_sceneExplorerHost)
    _sceneExplorerHost->render();

  // Render the action tabs
  if (_actionTabsHost)
    _actionTabsHost->render();
}

} // end of namespace BABYLON
