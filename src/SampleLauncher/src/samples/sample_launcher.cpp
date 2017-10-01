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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// Babylon
#include <babylon/babylon_stl_util.h>
#include <babylon/impl/canvas.h>

// Sample Runner
#include <babylon/interfaces/icanvas.h>

// Samples
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

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

static void error_callback(int /*error*/, const char* description)
{
  fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/,
                         int action, int /*mods*/)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

SampleLauncher::SampleLauncher(const std::string& title,
                               const ResolutionSize& size)
    : _editorState{State::UNINITIALIZED}
    , _monitor{nullptr}
    , _window{nullptr}
    , _winResX{size.first}
    , _winResY{size.second}
    , _title{title}
    , _fullscreen{false}
    , _lastTime{glfwGetTime()}
    , _frameCount{0}
    , _sceneIntialized{false}
    , _renderCanvas{std::make_unique<BABYLON::impl::Canvas>()}
    , _renderableScene{nullptr}
{
}

SampleLauncher::~SampleLauncher()
{
  destroy();
}

bool SampleLauncher::intialize()
{
  if (_editorState != State::UNINITIALIZED) {
    return false;
  }

  // Initialization
  initGLFW();
  initGL();

  _editorState = State::INITIALIZED;

  return true;
}

int SampleLauncher::run()
{
  _editorState = State::RUNNING;
  // Check if there is a renderable scene
  if (!_renderableScene) {
    // _editorState = SampleLauncher::State::ERROR;
    return 1;
  }

  int width = 0, height = 0;
  while (_editorState == State::RUNNING) {
    // Create new frame
    glfwGetFramebufferSize(_window, &_winResX, &_winResY);
    // FPS
    updateWindowFPS();
    // Update canvas size
    glfwGetFramebufferSize(_window, &width, &height);
    _renderCanvas->setFrameSize(width, height);
    _renderableScene->getEngine()->resize();
    // Render Scene
    _renderableScene->render();
    // Swap front and back buffers
    glfwSwapBuffers(_window);
    // Swap front and back buffers
    // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    glfwPollEvents();
    // Check if should close
    _editorState
      = (glfwWindowShouldClose(_window) || (_editorState == State::FINISHED)) ?
          State::FINISHED :
          _editorState;
  }

  return 0;
}

void SampleLauncher::destroy()
{
  glfwDestroyWindow(_window);
  glfwTerminate();
  _editorState = State::DESTROYED;
}

ICanvas* SampleLauncher::getRenderCanvas()
{
  return _renderCanvas.get();
}

void SampleLauncher::setRenderableScene(
  std::unique_ptr<IRenderableScene>& renderableScene)
{
  if (renderableScene && !_renderableScene) {
    renderableScene->initialize();
    _renderableScene = std::move(renderableScene);
    // Update the title
    char title[256];
    title[255] = '\0';
    snprintf(title, 255, "%s: %s", _title.c_str(), _renderableScene->getName());
    glfwSetWindowTitle(_window, title);
  }
}

int SampleLauncher::initGLFW()
{
  // Initialize error handling
  glfwSetErrorCallback(error_callback);

  // Initialize the library
  if (!glfwInit()) {
    // Handle initialization failure
    fprintf(stderr, "Failed to initialize GLFW\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  // Check if full screen mode is requested
  if ((_winResX == FULL_RESOLUTION_SIZE.first)
      && (_winResY == FULL_RESOLUTION_SIZE.second)) {
    _monitor                     = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(_monitor);
    _winResX                     = videoMode->width;
    _winResY                     = videoMode->height;

    glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
    glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
    glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
    glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
    glfwWindowHint(GLFW_ALPHA_BITS, videoMode->redBits);
    glfwWindowHint(GLFW_DEPTH_BITS, videoMode->redBits * 4);
  }

  // Draw smooth line with antialias
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  // Set context and api
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Create a windowed mode window and its OpenGL context
  _window
    = glfwCreateWindow(_winResX, _winResY, _title.c_str(), _monitor, nullptr);
  if (!_window) {
    glfwTerminate();
    fprintf(stderr, "Failed to create window\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  // Make the window's context current
  glfwMakeContextCurrent(_window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(_window, key_callback);

  return 0;
}

bool SampleLauncher::initGL()
{
  // Initialize 3D context
  if (!_renderCanvas->initializeContext3d()) {
    std::cerr << "failed to init GLEW" << std::endl;
    return false;
  }

  // Set canvas size
  int width, height;
  glfwGetFramebufferSize(_window, &width, &height);
  _renderCanvas->setFrameSize(width, height);

  return true;
}

void SampleLauncher::updateWindowFPS()
{
  // Measure speed
  double currentTime = glfwGetTime();
  ++_frameCount;

  if (currentTime - _lastTime >= 1.0) {
    char title[256];
    title[255] = '\0';
    snprintf(title, 255, "%s: %s - [%3.2f fps]", _title.c_str(),
             _renderableScene->getName(), static_cast<double>(_frameCount));

    glfwSetWindowTitle(_window, title);

    _frameCount = 0;
    _lastTime += 1.0;
  }
}

} // end of namespace Samples
} // end of namespace BABYLON
