#include <babylon/inspector/components/actiontabs/tabs/statistics_tab_component.h>

#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
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

  static auto statsSize = ImGui::CalcTextSize("4096x2160");
  const auto offsetX    = ImGui::GetContentRegionMax().x - statsSize.x;

  TextLineComponent::render(
    "Version", Engine::Version(), offsetX,
    ImVec4(113 / 255.f, 159 / 255.f, 255 / 255.f, 1.0f));
  ValueLineComponent::render("FPS", engine->getFps(), offsetX, std::nullopt, 0);

  // --- COUNT ---
  static auto countOpened = true;
  ImGui::SetNextTreeNodeOpen(countOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("COUNT")) {
    TextLineComponent::render("Total meshes",
                              std::to_string(scene->meshes.size()), offsetX);
    TextLineComponent::render("Active meshes",
                              std::to_string(scene->getActiveMeshes().size()),
                              offsetX);
    TextLineComponent::render(
      "Active indices", std::to_string(scene->getActiveIndices()), offsetX);
    TextLineComponent::render(
      "Active faces", std::to_string((scene->getActiveIndices() / 3)), offsetX);
    TextLineComponent::render("Active bones",
                              std::to_string(scene->getActiveBones()), offsetX);
    TextLineComponent::render(
      "Active particles", std::to_string(scene->getActiveParticles()), offsetX);
    TextLineComponent::render(
      "Draw calls",
      std::to_string(sceneInstrumentation->drawCallsCounter().current()),
      offsetX);
    TextLineComponent::render(
      "Texture collisions",
      std::to_string(
        sceneInstrumentation->textureCollisionsCounter().current()),
      offsetX);
    TextLineComponent::render("Total lights",
                              std::to_string(scene->lights.size()), offsetX);
    TextLineComponent::render(
      "Total vertices", std::to_string(scene->getTotalVertices()), offsetX);
    TextLineComponent::render("Total materials",
                              std::to_string(scene->materials.size()), offsetX);
    TextLineComponent::render("Total textures",
                              std::to_string(scene->textures.size()), offsetX);
    countOpened = true;
  }
  else {
    countOpened = false;
  }
  // --- FRAME STEPS DURATION ---
  static auto frameStepsDurationOpened = true;
  ImGui::SetNextTreeNodeOpen(frameStepsDurationOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("FRAME STEPS DURATION")) {
    ValueLineComponent::render(
      "Absolute FPS",
      (1000.f / sceneInstrumentation->frameTimeCounter().current()), offsetX,
      std::nullopt, 0);
    ValueLineComponent::render(
      "Meshes selection",
      sceneInstrumentation->activeMeshesEvaluationTimeCounter().current(),
      offsetX, std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Render targets",
      sceneInstrumentation->renderTargetsRenderTimeCounter().current(), offsetX,
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Particles", sceneInstrumentation->particlesRenderTimeCounter().current(),
      offsetX, std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Sprites", sceneInstrumentation->spritesRenderTimeCounter().current(),
      offsetX, std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Animations", sceneInstrumentation->animationsTimeCounter().current(),
      offsetX, std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Physics", sceneInstrumentation->physicsTimeCounter().current(), offsetX,
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Render", sceneInstrumentation->renderTimeCounter().current(), offsetX,
      std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Frame total", sceneInstrumentation->frameTimeCounter().current(),
      offsetX, std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "Inter-frame", sceneInstrumentation->interFrameTimeCounter().current(),
      offsetX, std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "GPU Frame time",
      engineInstrumentation->gpuFrameTimeCounter().current() * 0.000001f,
      offsetX, std::nullopt, std::nullopt, "ms");
    ValueLineComponent::render(
      "GPU Frame time (average)",
      static_cast<float>(engineInstrumentation->gpuFrameTimeCounter().average())
        * 0.000001f,
      offsetX, std::nullopt, std::nullopt, "ms");
    frameStepsDurationOpened = true;
  }
  else {
    frameStepsDurationOpened = false;
  }
  // --- SYSTEM INFO ---
  static auto systemInfoOpened = true;
  ImGui::SetNextTreeNodeOpen(systemInfoOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("SYSTEM INFO", false)) {
    TextLineComponent::render(
      "Resolution",
      String::concat(engine->getRenderWidth(), "x", engine->getRenderHeight()),
      offsetX);
    TextLineComponent::render(
      "WebGL version", std::to_string(static_cast<int>(engine->webGLVersion())),
      offsetX);
    BooleanLineComponent::render("Std derivatives", caps.standardDerivatives,
                                 offsetX);
    BooleanLineComponent::render("Compressed textures", caps.s3tc.has_value(),
                                 offsetX);
    BooleanLineComponent::render("Hardware instances", caps.instancedArrays,
                                 offsetX);
    BooleanLineComponent::render("Texture float", caps.textureFloat, offsetX);
    BooleanLineComponent::render("Texture half-float", caps.textureHalfFloat,
                                 offsetX);
    BooleanLineComponent::render("Render to texture float",
                                 caps.textureFloatRender, offsetX);
    BooleanLineComponent::render("Render to texture half-float",
                                 caps.textureHalfFloatRender, offsetX);
    BooleanLineComponent::render("32bits indices", caps.uintIndices, offsetX);
    BooleanLineComponent::render("Fragment depth", caps.fragmentDepthSupported,
                                 offsetX);
    BooleanLineComponent::render("High precision shaders",
                                 caps.highPrecisionShaderSupported, offsetX);
    BooleanLineComponent::render("Draw buffers", caps.drawBuffersExtension,
                                 offsetX);
    BooleanLineComponent::render("Vertex array object", caps.vertexArrayObject,
                                 offsetX);
    BooleanLineComponent::render("Timer query", caps.timerQuery != nullptr,
                                 offsetX);
    BooleanLineComponent::render("Stencil", engine->isStencilEnable(), offsetX);
    ValueLineComponent::render("Max textures units", caps.maxTexturesImageUnits,
                               offsetX);
    ValueLineComponent::render("Max textures size", caps.maxTextureSize,
                               offsetX);
    ValueLineComponent::render("Max anisotropy",
                               static_cast<int>(caps.maxAnisotropy), offsetX);
    systemInfoOpened = true;
  }
  else {
    systemInfoOpened = false;
  }
  // --- OPENGL INFO ---
  static auto openGLInfoOpened = true;
  ImGui::SetNextTreeNodeOpen(openGLInfoOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("OPENGL INFO")) {
    TextLineComponent::render(
      String::concat("Version: ", engine->getGlInfo().version));
    TextLineComponent::render(
      String::concat("Driver: ", engine->getGlInfo().renderer));
    TextLineComponent::render(
      String::concat("Vendor: ", engine->getGlInfo().vendor));
    openGLInfoOpened = true;
  }
  else {
    openGLInfoOpened = false;
  }
}

} // end of namespace BABYLON
