#include <babylon/samples/sample_launcher.h>

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
#include <stdio.h>
#include <stdlib.h>

// Babylon
#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/time.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/impl/canvas.h>
#include <babylon/mesh/abstract_mesh.h>

// Inspector
#ifdef WITH_INSPECTOR
#include <babylon/inspector/inspector.h>
#endif

namespace BABYLON {
namespace Samples {

const SampleLauncher::ResolutionSize SampleLauncher::DESIGN_RESOLUTION_SIZE
  = std::make_pair(1280, 720);
const SampleLauncher::ResolutionSize SampleLauncher::SMALL_RESOLUTION_SIZE
  // = std::make_pair(240, 160);
  = std::make_pair(800, 600);
const SampleLauncher::ResolutionSize SampleLauncher::MEDIUM_RESOLUTION_SIZE
  = std::make_pair(1024, 768);
const SampleLauncher::ResolutionSize SampleLauncher::LARGE_RESOLUTION_SIZE
  = std::make_pair(2048, 1536);
const SampleLauncher::ResolutionSize SampleLauncher::FULL_RESOLUTION_SIZE
  = std::make_pair(0, 0);

static Window _sceneWindow;
#ifdef WITH_INSPECTOR
static Window _inspectorWindow;
#endif

static std::unordered_map<int, std::string>& GetGLFWKeyMap()
{
  static std::unordered_map<int, std::string> glfwKeyMap{
    // Inknown key
    {GLFW_KEY_UNKNOWN, "Unknown"}, // -1
    // Non-alphanumeric characters
    {GLFW_KEY_SPACE, "Space"},           // 32
    {GLFW_KEY_APOSTROPHE, "Apostrophe"}, // 39 /* ' */
    {GLFW_KEY_COMMA, "Comma"},           // 44 /* , */
    {GLFW_KEY_MINUS, "Minus"},           // 45 /* - */
    {GLFW_KEY_PERIOD, "Period"},         // 46 /* . */
    {GLFW_KEY_SLASH, "Slash"},           // 47 /* / */
    // Digits
    {GLFW_KEY_0, "Digit0"}, // 48
    {GLFW_KEY_1, "Digit1"}, // 49
    {GLFW_KEY_2, "Digit2"}, // 50
    {GLFW_KEY_3, "Digit3"}, // 51
    {GLFW_KEY_4, "Digit4"}, // 52
    {GLFW_KEY_5, "Digit5"}, // 53
    {GLFW_KEY_6, "Digit6"}, // 54
    {GLFW_KEY_7, "Digit7"}, // 55
    {GLFW_KEY_8, "Digit8"}, // 56
    {GLFW_KEY_9, "Digit9"}, // 57
    // ; and =
    {GLFW_KEY_SEMICOLON, "Semicolon"}, // 59 /* ; */
    {GLFW_KEY_EQUAL, "Equal"},         // 61 /* = */
    // A-Z
    {GLFW_KEY_A, "KeyA"}, // 65
    {GLFW_KEY_B, "KeyB"}, // 66
    {GLFW_KEY_C, "KeyC"}, // 67
    {GLFW_KEY_D, "KeyD"}, // 68
    {GLFW_KEY_E, "KeyE"}, // 69
    {GLFW_KEY_F, "KeyF"}, // 70
    {GLFW_KEY_G, "KeyG"}, // 71
    {GLFW_KEY_H, "KeyH"}, // 72
    {GLFW_KEY_I, "KeyI"}, // 73
    {GLFW_KEY_J, "KeyJ"}, // 74
    {GLFW_KEY_K, "KeyK"}, // 75
    {GLFW_KEY_L, "KeyL"}, // 76
    {GLFW_KEY_M, "KeyM"}, // 77
    {GLFW_KEY_N, "KeyN"}, // 78
    {GLFW_KEY_O, "KeyO"}, // 79
    {GLFW_KEY_P, "KeyP"}, // 80
    {GLFW_KEY_Q, "KeyQ"}, // 81
    {GLFW_KEY_R, "KeyR"}, // 82
    {GLFW_KEY_S, "KeyS"}, // 83
    {GLFW_KEY_T, "KeyT"}, // 84
    {GLFW_KEY_U, "KeyU"}, // 85
    {GLFW_KEY_V, "KeyV"}, // 86
    {GLFW_KEY_W, "KeyW"}, // 87
    {GLFW_KEY_X, "KeyX"}, // 88
    {GLFW_KEY_Y, "KeyY"}, // 89
    {GLFW_KEY_Z, "KeyZ"}, // 90
    // Special keys
    {GLFW_KEY_LEFT_BRACKET, "LeftBracket"},   // 91 /* [ */
    {GLFW_KEY_BACKSLASH, "Backslash"},        // 92 /* \ */
    {GLFW_KEY_RIGHT_BRACKET, "RightBracket"}, // 93 /* ] */
    {GLFW_KEY_GRAVE_ACCENT, "Accent"},        // 96 /* ` */
    {GLFW_KEY_ESCAPE, "Escape"},              // 256
    {GLFW_KEY_ENTER, "Enter"},                // 257
    {GLFW_KEY_TAB, "Tab"},                    // 258
    {GLFW_KEY_BACKSPACE, "Backspace"},        // 259
    {GLFW_KEY_INSERT, "Insert"},              // 260
    {GLFW_KEY_DELETE, "Delete"},              // 261
    {GLFW_KEY_RIGHT, "ArrowRight"},           // 262
    {GLFW_KEY_LEFT, "ArrowLeft"},             // 263
    {GLFW_KEY_DOWN, "ArrowDown"},             // 264
    {GLFW_KEY_UP, "ArrowUp"},                 // 265
    {GLFW_KEY_PAGE_UP, "PageUp"},             // 266
    {GLFW_KEY_PAGE_DOWN, "PageDown"},         // 267
    {GLFW_KEY_HOME, "Home"},                  // 268
    {GLFW_KEY_END, "End"},                    // 269
    {GLFW_KEY_CAPS_LOCK, "CapsLock"},         // 280
    {GLFW_KEY_SCROLL_LOCK, "ScrollLock"},     // 281
    {GLFW_KEY_NUM_LOCK, "NumLock"},           // 282
    {GLFW_KEY_PRINT_SCREEN, "PrintScreen"},   // 283
    {GLFW_KEY_PAUSE, "Pause"},                // 284
    // Function keys
    {GLFW_KEY_F1, "F1"},   // 290
    {GLFW_KEY_F2, "F2"},   // 291
    {GLFW_KEY_F3, "F3"},   // 292
    {GLFW_KEY_F4, "F4"},   // 293
    {GLFW_KEY_F5, "F5"},   // 294
    {GLFW_KEY_F6, "F6"},   // 295
    {GLFW_KEY_F7, "F7"},   // 296
    {GLFW_KEY_F8, "F8"},   // 297
    {GLFW_KEY_F9, "F9"},   // 298
    {GLFW_KEY_F10, "F10"}, // 299
    {GLFW_KEY_F11, "F11"}, // 300
    {GLFW_KEY_F12, "F12"}, // 301
    {GLFW_KEY_F13, "F13"}, // 302
    {GLFW_KEY_F14, "F14"}, // 303
    {GLFW_KEY_F15, "F15"}, // 304
    {GLFW_KEY_F16, "F16"}, // 305
    {GLFW_KEY_F17, "F17"}, // 306
    {GLFW_KEY_F18, "F18"}, // 307
    {GLFW_KEY_F19, "F19"}, // 308
    {GLFW_KEY_F20, "F20"}, // 309
    {GLFW_KEY_F21, "F21"}, // 310
    {GLFW_KEY_F22, "F22"}, // 311
    {GLFW_KEY_F23, "F23"}, // 312
    {GLFW_KEY_F24, "F24"}, // 313
    {GLFW_KEY_F25, "F25"}, // 314
    // Numpad keys
    {GLFW_KEY_KP_0, "Numpad0"},                // 320
    {GLFW_KEY_KP_1, "Numpad1"},                // 321
    {GLFW_KEY_KP_2, "Numpad2"},                // 322
    {GLFW_KEY_KP_3, "Numpad3"},                // 323
    {GLFW_KEY_KP_4, "Numpad4"},                // 324
    {GLFW_KEY_KP_5, "Numpad5"},                // 325
    {GLFW_KEY_KP_6, "Numpad6"},                // 326
    {GLFW_KEY_KP_7, "Numpad7"},                // 327
    {GLFW_KEY_KP_8, "Numpad8"},                // 328
    {GLFW_KEY_KP_9, "Numpad9"},                // 329
    {GLFW_KEY_KP_DECIMAL, "NumpadDecimal"},    // 330
    {GLFW_KEY_KP_DIVIDE, "NumpadDivide"},      // 331
    {GLFW_KEY_KP_MULTIPLY, "NumpadMultiply"},  // 332
    {GLFW_KEY_KP_SUBTRACT, "NumpadSubstract"}, // 333
    {GLFW_KEY_KP_ADD, "NumpadAdd"},            // 334
    {GLFW_KEY_KP_ENTER, "NumpadEnter"},        // 335
    {GLFW_KEY_KP_EQUAL, "NumpadEqual"},        // 336
    // Special keys
    {GLFW_KEY_LEFT_SHIFT, "ShiftLeft"},       // 340
    {GLFW_KEY_LEFT_CONTROL, "ControlLeft"},   // 341
    {GLFW_KEY_LEFT_ALT, "AltLeft"},           // 342
    {GLFW_KEY_LEFT_SUPER, "MetaLeft"},        // 343
    {GLFW_KEY_RIGHT_SHIFT, "ShiftRight"},     // 344
    {GLFW_KEY_RIGHT_CONTROL, "ControlRight"}, // 345
    {GLFW_KEY_RIGHT_ALT, "AltRight"},         // 346
    {GLFW_KEY_RIGHT_SUPER, "MetaRight"},      // 347
    {GLFW_KEY_MENU, "ContextMenu"},           // 348
  };
  /// Return key map
  return glfwKeyMap;
}

static std::string GLFWKeyToString(int glfwKey)
{
  static auto& glfwKeyMap = GetGLFWKeyMap();
  return stl_util::contains(glfwKeyMap, glfwKey) ? glfwKeyMap[glfwKey] :
                                                   "Unknown";
}

static int RemapGLFWKeyCode(int glfwKey)
{
  // Remap GLFW keycode to JS keycode
  if (glfwKey == GLFW_KEY_RIGHT) {
    return 39;
  }
  else if (glfwKey == GLFW_KEY_LEFT) {
    return 37;
  }
  else if (glfwKey == GLFW_KEY_DOWN) {
    return 40;
  }
  else if (glfwKey == GLFW_KEY_UP) {
    return 38;
  }

  return glfwKey;
}

static float RescaleMouseScroll(double offset, float scaleFactor = 120.f)
{
  return static_cast<float>(offset) * scaleFactor;
}

static void GLFWErrorCallback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error occured, Error id: %i, Description: %s\n", error,
          description);
}

static void GLFWKeyCallback(GLFWwindow* window, int key, int /*scancode*/,
                            int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
    return;
  }

  if (_sceneWindow.glfwWindow == window && _sceneWindow.renderCanvas) {
    // Determine modifier
    bool ctrlKey = (mods & GLFW_MOD_CONTROL);
    bool altKey  = (mods & GLFW_MOD_ALT);
    // Readable key name
    auto code = GLFWKeyToString(key);
    // Remap GLFW keycode to JS code
    auto keyCode = RemapGLFWKeyCode(key);
    // Raise event
    if (action == GLFW_PRESS) {
      _sceneWindow.renderCanvas->onKeyDown(ctrlKey, altKey, keyCode, code);
    }
    else if (action == GLFW_RELEASE) {
      _sceneWindow.renderCanvas->onKeyUp(ctrlKey, altKey, keyCode, code);
    }
  }
}

static void GLFWCursorPositionCallback(GLFWwindow* window, double xpos,
                                       double ypos)
{
  if (_sceneWindow.glfwWindow == window && _sceneWindow.renderCanvas) {
    // Determine modifier
    bool ctrlKey = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == 1)
                   || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == 1);
    bool shiftKey = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == 1)
                    || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == 1);
    // Raise event
    _sceneWindow.renderCanvas->onMouseMove(
      ctrlKey, shiftKey, static_cast<int>(xpos), static_cast<int>(ypos));
  }
}

static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action,
                                    int mods)
{
  if (_sceneWindow.glfwWindow == window && _sceneWindow.renderCanvas) {
    // Determine mouse button type
    MouseButtonType buttonType{MouseButtonType::UNDEFINED};
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      buttonType = MouseButtonType::LEFT;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      buttonType = MouseButtonType::MIDDLE;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      buttonType = MouseButtonType::RIGHT;
    }
    // Get cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // Determine modifier
    bool ctrlKey  = (mods & GLFW_MOD_CONTROL);
    bool shiftKey = (mods & GLFW_MOD_SHIFT);
    // Raise event
    if (action == GLFW_PRESS) {
      _sceneWindow.renderCanvas->onMouseButtonDown(
        ctrlKey, shiftKey, static_cast<int>(xpos), static_cast<int>(ypos),
        buttonType);
    }
    else if (action == GLFW_RELEASE) {
      _sceneWindow.renderCanvas->onMouseButtonUp(
        ctrlKey, shiftKey, static_cast<int>(xpos), static_cast<int>(ypos),
        buttonType);
    }
  }
}

static void GLFWScrollCallback(GLFWwindow* window, double /*xoffset*/,
                               double yoffset)
{
  if (_sceneWindow.glfwWindow == window && _sceneWindow.renderCanvas) {
    // Get cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // Determine modifier
    bool ctrlKey = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == 1)
                   || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == 1);
    bool shiftKey = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == 1)
                    || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == 1);
    // Raise event
    _sceneWindow.renderCanvas->onMouseWheel(
      ctrlKey, shiftKey, static_cast<int>(xpos), static_cast<int>(ypos),
      RescaleMouseScroll(yoffset));
  }
}

static void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
{
  if (_sceneWindow.glfwWindow == window) {
    _sceneWindow.width  = width;
    _sceneWindow.height = height;
    // Update canvas size
    if (_sceneWindow.renderCanvas) {
      _sceneWindow.renderCanvas->setFrameSize(width, height);
      _sceneWindow.renderableScene->getEngine()->resize();
    }
  }
}

SampleLauncher::SampleLauncher(const SampleLauncherOptions& options)
    : _sampleLauncherState{State::UNINITIALIZED}
    , _defaultWinResX{options.size.first}
    , _defaultWinResY
{
  options.size.second
}
#ifdef WITH_INSPECTOR
, _showInspectorWindow{options.showInspectorWindow}, _inspector
{
  nullptr
}
#endif
{
  _sceneWindow                 = Window();
  _sceneWindow.title           = options.title;
  _sceneWindow.sceneIntialized = false;
  _sceneWindow.renderCanvas    = std::make_unique<BABYLON::impl::Canvas>();
  _sceneWindow.renderableScene = nullptr;
  if (options.showInspectorWindow) {
#ifdef WITH_INSPECTOR
    _inspectorWindow       = Window();
    _inspectorWindow.title = "Inspector";
#else
    BABYLON_LOG_ERROR("SampleLauncher",
                      "Could not show inspector because it is currenlty "
                      "disabled in the build!");
#endif
  }
}

SampleLauncher::~SampleLauncher()
{
  destroy();
}

bool SampleLauncher::intialize()
{
  if (_sampleLauncherState != State::UNINITIALIZED) {
    return false;
  }

  // Initialization
  initGLFW();
  initGL(_sceneWindow);

  // Update sample launcher state
  _sampleLauncherState = State::INITIALIZED;

  return true;
}

int SampleLauncher::run(long runTime)
{
  _sampleLauncherState = State::RUNNING;
  auto startTime       = Time::unixtimeInMs();

  // Check if there is a renderable scene
  if (!_sceneWindow.renderableScene) {
    _sampleLauncherState = SampleLauncher::State::ERRORCONDITION;
    return 1;
  }

#ifdef WITH_INSPECTOR
  auto previousTimestamp = Time::highresTimepointNow();
#endif

  // Run sample
  bool windowClosed = false, maxRunTimeReached = false;
  while (_sampleLauncherState == State::RUNNING) {
    //*** Scene Window ***//
    {
      // Make the window's context current
      glfwMakeContextCurrent(_sceneWindow.glfwWindow);
      // FPS
      updateWindowFPS(_sceneWindow);
      // Render Scene
      _sceneWindow.renderableScene->render();
      // Swap front and back buffers
      glfwSwapBuffers(_sceneWindow.glfwWindow);
    }
    //*** Inspector Window ***//
#ifdef WITH_INSPECTOR
    auto currentTimestamp = Time::highresTimepointNow();
    auto diffMillis = Time::fpTimeDiff<size_t, std::milli>(previousTimestamp,
                                                           currentTimestamp);
    if (_showInspectorWindow && _inspector && diffMillis >= 100ul) {
      previousTimestamp = currentTimestamp;
      // Make the window's context current
      glfwMakeContextCurrent(_inspectorWindow.glfwWindow);
      // Render inspector window
      _inspector->render();
      // Swap front and back buffers
      glfwSwapBuffers(_inspectorWindow.glfwWindow);
    }
#endif
    // Timeout
    // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    // Process events
    glfwPollEvents();
    // Check if should close
    maxRunTimeReached
      = (runTime > 0 && (Time::unixtimeInMs() - startTime > runTime));
    windowClosed = glfwWindowShouldClose(_sceneWindow.glfwWindow);
#ifdef WITH_INSPECTOR
    windowClosed = windowClosed
                   || (_showInspectorWindow ?
                         glfwWindowShouldClose(_inspectorWindow.glfwWindow) :
                         false);
#endif
    _sampleLauncherState = (maxRunTimeReached || windowClosed
                            || (_sampleLauncherState == State::FINISHED)) ?
                             State::FINISHED :
                             _sampleLauncherState;
  }

  return windowClosed ? 0 : 1;
}

void SampleLauncher::destroy()
{
  // Cleanup window(s)
  glfwDestroyWindow(_sceneWindow.glfwWindow);
#ifdef WITH_INSPECTOR
  if (_showInspectorWindow && _inspector) {
    _inspector->dispose();
  }
#endif
  // Terminate GLFW
  glfwTerminate();
  _sampleLauncherState = State::DESTROYED;
}

ICanvas* SampleLauncher::getRenderCanvas()
{
  return _sceneWindow.renderCanvas.get();
}

void SampleLauncher::setRenderableScene(
  std::unique_ptr<IRenderableScene>& renderableScene)
{
  // Main scene window
  if (renderableScene && !_sceneWindow.renderableScene) {
    renderableScene->initialize();
    _sceneWindow.renderableScene = std::move(renderableScene);
    // Update the title
    char title[256];
    title[255] = '\0';
    snprintf(title, 255, "%s: %s", _sceneWindow.title.c_str(),
             _sceneWindow.renderableScene->getName());
    glfwSetWindowTitle(_sceneWindow.glfwWindow, title);
  }
  // Inspector window
#ifdef WITH_INSPECTOR
  if (_showInspectorWindow && _inspector && _sceneWindow.renderableScene) {
    auto scene = _sceneWindow.renderableScene->getScene();
    _inspector->setScene(scene);
  }
#endif
}

int SampleLauncher::initGLFW()
{
  // Initialize error handling
  glfwSetErrorCallback(GLFWErrorCallback);

  // Initialize the library
  if (!glfwInit()) {
    // Handle initialization failure
    fprintf(stderr, "Failed to initialize GLFW\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  // Draw smooth line with antialias
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

  // Initialize OpenGL context
#if __APPLE__
  // GL 3.2
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  // GL 3.2
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only
#endif

  // Create the scene window
  CreateGLFWWindow(_sceneWindow, _defaultWinResX, _defaultWinResY,
                   _sceneWindow.title.c_str(), nullptr, nullptr);
  _sceneWindow.lastTime = glfwGetTime();

  // Create the inspector window
#ifdef WITH_INSPECTOR
  if (_showInspectorWindow) {
    CreateGLFWWindow(_inspectorWindow, _defaultWinResX / 2, _defaultWinResY,
                     _inspectorWindow.title.c_str(), nullptr, &_sceneWindow);
    _inspectorWindow.lastTime = glfwGetTime();
    _inspector = std::make_unique<Inspector>(_inspectorWindow.glfwWindow);
    _inspector->intialize();
  }
#endif

  return 0;
}

void SampleLauncher::CreateGLFWWindow(Window& window, int width, int height,
                                      const std::string& title,
                                      GLFWmonitor* monitor,
                                      Window* parentWindow)
{
  // Create new window
  window.glfwWindow  = nullptr;
  window.glfwMonitor = monitor;
  window.width       = width;
  window.height      = height;

  // Check if full screen mode is requested
  if ((width == FULL_RESOLUTION_SIZE.first)
      && (height == FULL_RESOLUTION_SIZE.second)) {
    window.glfwMonitor   = glfwGetPrimaryMonitor();
    const auto videoMode = glfwGetVideoMode(window.glfwMonitor);
    window.fullscreen    = true;
    window.width         = videoMode->width;
    window.height        = videoMode->height;

    glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
    glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
    glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
    glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
    glfwWindowHint(GLFW_ALPHA_BITS, videoMode->redBits);
    glfwWindowHint(GLFW_DEPTH_BITS, videoMode->redBits * 4);
  }

  // Create GLFW window
  if (parentWindow) {
    window.glfwWindow = glfwCreateWindow(width, height, title.c_str(), monitor,
                                         parentWindow->glfwWindow);
  }
  else {
    window.glfwWindow
      = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
  }

  // Confirm that GLFW window was created successfully
  if (!window.glfwWindow) {
    glfwTerminate();
    fprintf(stderr, "Failed to create window\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  // Make the window's context current
  glfwMakeContextCurrent(window.glfwWindow);

  // Enable vsync
  glfwSwapInterval(1);

  // -- Setup callbacks --
  // Key input events
  glfwSetKeyCallback(window.glfwWindow, GLFWKeyCallback);
  // Cursor position
  glfwSetCursorPosCallback(window.glfwWindow, GLFWCursorPositionCallback);
  // Mouse button input
  glfwSetMouseButtonCallback(window.glfwWindow, GLFWMouseButtonCallback);
  // Scroll input
  glfwSetScrollCallback(window.glfwWindow, GLFWScrollCallback);
  // Window resize events
  glfwSetWindowSizeCallback(window.glfwWindow, GLFWWindowSizeCallback);

  // Change the state of the window to intialized
  window.intialized = true;
}

bool SampleLauncher::initGL(Window& window)
{
  // Initialize 3D context
  if (!window.renderCanvas->initializeContext3d()) {
    fprintf(stderr, "Error occured, Failed to initialize 3D context\n");
    return false;
  }

  // Set canvas size
  int width, height;
  glfwGetFramebufferSize(window.glfwWindow, &width, &height);
  window.renderCanvas->setFrameSize(width, height);

  return true;
}

void SampleLauncher::updateWindowFPS(Window& window)
{
  // Measure speed
  double currentTime = glfwGetTime();
  ++window.frameCount;

  if (currentTime - window.lastTime >= 1.0) {
    char title[256];
    title[255] = '\0';
    snprintf(title, 255, "%s: %s - [%3.2f fps]", window.title.c_str(),
             window.renderableScene->getName(),
             static_cast<double>(window.frameCount));

    glfwSetWindowTitle(window.glfwWindow, title);

    window.frameCount = 0;
    window.lastTime += 1.0;
  }
}

} // end of namespace Samples
} // end of namespace BABYLON
