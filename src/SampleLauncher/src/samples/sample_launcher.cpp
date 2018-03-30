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
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/impl/canvas.h>

// Inspector
#include <babylon/inspector/inspector.h>

namespace BABYLON {
namespace Samples {

const SampleLauncher::ResolutionSize SampleLauncher::DESIGN_RESOLUTION_SIZE
  = std::make_pair(1280, 720);
const SampleLauncher::ResolutionSize SampleLauncher::SMALL_RESOLUTION_SIZE
  //  = std::make_pair(240, 160);
  = std::make_pair(800, 600);
const SampleLauncher::ResolutionSize SampleLauncher::MEDIUM_RESOLUTION_SIZE
  = std::make_pair(1024, 768);
const SampleLauncher::ResolutionSize SampleLauncher::LARGE_RESOLUTION_SIZE
  = std::make_pair(2048, 1536);
const SampleLauncher::ResolutionSize SampleLauncher::FULL_RESOLUTION_SIZE
  = std::make_pair(0, 0);

static Window _sceneWindow;
static Window _inspectorWindow;

static void GLFWErrorCallback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error occured, Error id: %i, Description: %s\n", error,
          description);
}

static void GLFWKeyCallback(GLFWwindow* window, int key, int /*scancode*/,
                            int action, int /*mods*/)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
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
    , _defaultWinResY{options.size.second}
    , _showInspectorWindow{options.showInspectorWindow}
    , _inspector{nullptr}
    , _useOpenGLES{false}
{
  _sceneWindow                 = Window();
  _sceneWindow.title           = options.title;
  _sceneWindow.sceneIntialized = false;
  _sceneWindow.renderCanvas    = std::make_unique<BABYLON::impl::Canvas>();
  _sceneWindow.renderableScene = nullptr;
  _sceneWindow.lastTime        = glfwGetTime();
  if (options.showInspectorWindow) {
    _inspectorWindow          = Window();
    _inspectorWindow.title    = "Inspector";
    _inspectorWindow.lastTime = glfwGetTime();
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

int SampleLauncher::run()
{
  _sampleLauncherState = State::RUNNING;
  // Check if there is a renderable scene
  if (!_sceneWindow.renderableScene) {
    _sampleLauncherState = SampleLauncher::State::ERROR;
    return 1;
  }

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
    if (_showInspectorWindow && _inspector) {
      // Make the window's context current
      glfwMakeContextCurrent(_inspectorWindow.glfwWindow);
      // Render inspector window
      _inspector->render();
      // Swap front and back buffers
      glfwSwapBuffers(_inspectorWindow.glfwWindow);
    }
    // Timeout
    // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    // Process events
    glfwPollEvents();
    // Check if should close
    _sampleLauncherState
      = (glfwWindowShouldClose(_sceneWindow.glfwWindow)
         || (_showInspectorWindow ?
               glfwWindowShouldClose(_inspectorWindow.glfwWindow) :
               false)
         || (_sampleLauncherState == State::FINISHED)) ?
          State::FINISHED :
          _sampleLauncherState;
  }

  return 0;
}

void SampleLauncher::destroy()
{
  // Cleanup window(s)
  glfwDestroyWindow(_sceneWindow.glfwWindow);
  if (_showInspectorWindow && _inspector) {
    _inspector->dispose();
  }
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
  if (_showInspectorWindow && _inspector && _sceneWindow.renderableScene) {
    auto scene = _sceneWindow.renderableScene->getScene();
    _inspector->setScene(scene);
  }
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
  if (_useOpenGLES) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  }
  else {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  }

  // Create the scene window
  CreateWindow(_sceneWindow, _defaultWinResX, _defaultWinResY,
               _sceneWindow.title.c_str(), nullptr, nullptr);

  // Create the inspector window
  if (_showInspectorWindow) {
    CreateWindow(_inspectorWindow, _defaultWinResX / 2, _defaultWinResY,
                 _inspectorWindow.title.c_str(), nullptr, &_sceneWindow);
    _inspector = ::std::make_unique<Inspector>(_inspectorWindow.glfwWindow);
    _inspector->intialize();
  }

  return 0;
}

void SampleLauncher::CreateWindow(Window& window, int width, int height,
                                  const string_t& title, GLFWmonitor* monitor,
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

  // Setup callbacks
  glfwSetKeyCallback(window.glfwWindow, GLFWKeyCallback);
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
