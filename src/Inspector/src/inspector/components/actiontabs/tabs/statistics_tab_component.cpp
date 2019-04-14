#include <babylon/inspector/components/actiontabs/tabs/statistics_tab_component.h>

#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/inspector/components/actiontabs/line_container_component.h>
#include <babylon/inspector/components/actiontabs/lines/boolean_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/value_line_component.h>
#include <babylon/instrumentation/engine_instrumentation.h>
#include <babylon/instrumentation/scene_instrumentation.h>

namespace BABYLON {

StatisticsTabComponent::StatisticsTabComponent(
  const IPaneComponentProps& iProps)
    : PaneComponent{iProps}
    , _sceneInstrumentation{nullptr}
    , _engineInstrumentation{nullptr}
{
  componentWillMount();
}

StatisticsTabComponent::~StatisticsTabComponent()
{
  componentWillUnmount();
}

void StatisticsTabComponent::componentWillMount()
{
  const auto scene = props.scene;

  if (!scene) {
    return;
  }

  _sceneInstrumentation = std::make_unique<SceneInstrumentation>(scene);
  _sceneInstrumentation->captureActiveMeshesEvaluationTime = true;
  _sceneInstrumentation->captureRenderTargetsRenderTime    = true;
  _sceneInstrumentation->captureFrameTime                  = true;
  _sceneInstrumentation->captureRenderTime                 = true;
  _sceneInstrumentation->captureInterFrameTime             = true;
  _sceneInstrumentation->captureParticlesRenderTime        = true;
  _sceneInstrumentation->captureSpritesRenderTime          = true;
  _sceneInstrumentation->capturePhysicsTime                = true;
  _sceneInstrumentation->captureAnimationsTime             = true;

  _engineInstrumentation
    = std::make_unique<EngineInstrumentation>(scene->getEngine());
  _engineInstrumentation->captureGPUFrameTime = true;
}

void StatisticsTabComponent::componentWillUnmount()
{
  if (_sceneInstrumentation) {
    _sceneInstrumentation->dispose();
    _sceneInstrumentation = nullptr;
  }

  if (_engineInstrumentation) {
    _engineInstrumentation->dispose();
    _engineInstrumentation = nullptr;
  }
}

void StatisticsTabComponent::render()
{
  const auto scene = props.scene;

  if (!scene || !_sceneInstrumentation || !_engineInstrumentation) {
    return;
  }

  const auto engine                 = scene->getEngine();
  const auto& sceneInstrumentation  = _sceneInstrumentation;
  const auto& engineInstrumentation = _engineInstrumentation;
  const auto& caps                  = engine->getCaps();

  ImGui::Columns(2, "general info", false);
  {
    TextLineComponent::render(
      "Version", Engine::Version(),
      ImVec4(113 / 255.f, 159 / 255.f, 255 / 255.f, 1.0f));
    ValueLineComponent::render("FPS", engine->getFps(), std::nullopt, 0);
  }
  ImGui::Columns(1);

  // --- COUNT ---
  static auto countClosed = false;
  if (LineContainerComponent::begin("COUNT", countClosed)) {
    TextLineComponent::render("Total meshes",
                              std::to_string(scene->meshes.size()));
    TextLineComponent::render("Active meshes",
                              std::to_string(scene->getActiveMeshes().size()));
    TextLineComponent::render("Active indices",
                              std::to_string(scene->getActiveIndices()));
    TextLineComponent::render("Active faces",
                              std::to_string((scene->getActiveIndices() / 3)));
    TextLineComponent::render("Active bones",
                              std::to_string(scene->getActiveBones()));
    TextLineComponent::render("Active particles",
                              std::to_string(scene->getActiveParticles()));
    TextLineComponent::render(
      "Draw calls",
      std::to_string(sceneInstrumentation->drawCallsCounter().current()));
    TextLineComponent::render("Total lights",
                              std::to_string(scene->lights.size()));
    TextLineComponent::render("Total vertices",
                              std::to_string(scene->getTotalVertices()));
    TextLineComponent::render("Total materials",
                              std::to_string(scene->materials.size()));
    TextLineComponent::render("Total textures",
                              std::to_string(scene->textures.size()));
    LineContainerComponent::end();
  }
  // --- FRAME STEPS DURATION ---
  static auto frameStepsDurationClosed = false;
  if (LineContainerComponent::begin("FRAME STEPS DURATION",
                                    frameStepsDurationClosed)) {
    ValueLineComponent::render(
      "Absolute FPS",
      (1000.f / sceneInstrumentation->frameTimeCounter().current()),
      std::nullopt, 0);
    ValueLineComponent::render(
      "Meshes selection",
      sceneInstrumentation->activeMeshesEvaluationTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Render targets",
      sceneInstrumentation->renderTargetsRenderTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Particles", sceneInstrumentation->particlesRenderTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Sprites", sceneInstrumentation->spritesRenderTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Animations", sceneInstrumentation->animationsTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Physics", sceneInstrumentation->physicsTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Render", sceneInstrumentation->renderTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Frame total", sceneInstrumentation->frameTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Inter-frame", sceneInstrumentation->interFrameTimeCounter().current(),
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "GPU Frame time",
      engineInstrumentation->gpuFrameTimeCounter().current() * 0.000001f,
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "GPU Frame time (average)",
      static_cast<float>(engineInstrumentation->gpuFrameTimeCounter().average())
        * 0.000001f,
      std::nullopt, std::nullopt, "ms");
    LineContainerComponent::end();
  }
  // --- SYSTEM INFO ---
  static auto systemInfoClosed = false;
  if (LineContainerComponent::begin("SYSTEM INFO", systemInfoClosed)) {
    TextLineComponent::render(
      "Resolution",
      String::concat(engine->getRenderWidth(), "x", engine->getRenderHeight()));
    TextLineComponent::render("WebGL version", std::to_string(static_cast<int>(
                                                 engine->webGLVersion())));
    BooleanLineComponent::render("Std derivatives", caps.standardDerivatives);
    BooleanLineComponent::render("Compressed textures", caps.s3tc.has_value());
    BooleanLineComponent::render("Hardware instances", caps.instancedArrays);
    BooleanLineComponent::render("Texture float", caps.textureFloat);
    BooleanLineComponent::render("Texture half-float", caps.textureHalfFloat);
    BooleanLineComponent::render("Render to texture float",
                                 caps.textureFloatRender);
    BooleanLineComponent::render("Render to texture half-float",
                                 caps.textureHalfFloatRender);
    BooleanLineComponent::render("32bits indices", caps.uintIndices);
    BooleanLineComponent::render("Fragment depth", caps.fragmentDepthSupported);
    BooleanLineComponent::render("High precision shaders",
                                 caps.highPrecisionShaderSupported);
    BooleanLineComponent::render("Draw buffers", caps.drawBuffersExtension);
    BooleanLineComponent::render("Vertex array object", caps.vertexArrayObject);
    BooleanLineComponent::render("Timer query", caps.timerQuery != nullptr);
    BooleanLineComponent::render("Stencil", engine->isStencilEnable());
    ValueLineComponent::render("Max textures units",
                               caps.maxTexturesImageUnits);
    ValueLineComponent::render("Max textures size", caps.maxTextureSize);
    ValueLineComponent::render("Max anisotropy",
                               static_cast<int>(caps.maxAnisotropy));
    LineContainerComponent::end();
  }
  // --- OPENGL INFO ---
  static auto openGLInfoClosed = false;
  if (LineContainerComponent::begin("OPENGL INFO", openGLInfoClosed)) {
    TextLineComponent::render("Version: ", engine->getGlInfo().version);
    TextLineComponent::render("Driver: ", engine->getGlInfo().renderer);
    TextLineComponent::render("Vendor: ", engine->getGlInfo().vendor);
    LineContainerComponent::end();
  }
}

} // end of namespace BABYLON
