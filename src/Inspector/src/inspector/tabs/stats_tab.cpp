#include <babylon/inspector/tabs/stats_tab.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/imgui/imgui_dock.h>
#include <babylon/inspector/inspector.h>
#include <babylon/instrumentation/engine_instrumentation.h>
#include <babylon/instrumentation/scene_instrumentation.h>

namespace BABYLON {

StatsTab::StatsTab(Inspector& inspector)
    : Tab{"Stats"}
    , _inspector{inspector}
    , _sceneInstrumentation{nullptr}
    , _engineInstrumentation{nullptr}
{
  _scene  = inspector.scene();
  _engine = _scene->getEngine();
  _glInfo = _engine->getGlInfo();

  _connectToInstrumentation();
}

StatsTab::~StatsTab()
{
}

void StatsTab::_connectToInstrumentation()
{
  if (_sceneInstrumentation) {
    return;
  }

  _sceneInstrumentation = ::std::make_unique<SceneInstrumentation>(_scene);
  _sceneInstrumentation->captureActiveMeshesEvaluationTime = true;
  _sceneInstrumentation->captureRenderTargetsRenderTime    = true;
  _sceneInstrumentation->captureFrameTime                  = true;
  _sceneInstrumentation->captureRenderTime                 = true;
  _sceneInstrumentation->captureInterFrameTime             = true;
  _sceneInstrumentation->captureParticlesRenderTime        = true;
  _sceneInstrumentation->captureSpritesRenderTime          = true;
  _sceneInstrumentation->capturePhysicsTime                = true;
  _sceneInstrumentation->captureAnimationsTime             = true;

  _engineInstrumentation = ::std::make_unique<EngineInstrumentation>(_engine);
  _engineInstrumentation->captureGPUFrameTime = true;
}

void StatsTab::render()
{
  if (!_scene || !_engine || !_sceneInstrumentation
      || !_engineInstrumentation) {
    return;
  }

  static auto statsSize         = ImGui::CalcTextSize("4096x2160");
  const auto engineCapabilities = _engine->getCaps();

  if (ImGui::BeginDock(name.c_str())) {
    float offsetX = ImGui::GetContentRegionMax().x - statsSize.x;
    // Engine version and framerate
    ImGui::TextWrapped("BabylonCpp v%s -", Engine::Version().c_str());
    ImGui::SameLine();
    // Color "color-top" #f29766 -> rgba(242, 151, 102, 1)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.59f, 0.4f, 1.0f));
    ImGui::TextWrapped("%.f fps", static_cast<double>(_engine->getFps()));
    ImGui::PopStyleColor();
    // OpengGL Info
    if (ImGui::CollapsingHeader("OpenGL Info", "OpenGL Info", true, true)) {
      ImGui::TextWrapped("Version: %s", _glInfo.version.c_str());
      ImGui::TextWrapped("Renderer: %s", _glInfo.renderer.c_str());
      ImGui::TextWrapped("Vendor: %s", _glInfo.vendor.c_str());
    }
    // Count
    if (ImGui::CollapsingHeader("Count", "Count", true, true)) {
      // Total meshes
      ImGui::TextWrapped("Total meshes");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->meshes.size());
      // Draw calls
      ImGui::TextWrapped("Draw calls");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld",
                         _sceneInstrumentation->drawCallsCounter().current());
      // Draw calls
      ImGui::TextWrapped("Texture collisions");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%ld", _sceneInstrumentation->textureCollisionsCounter().current());
      // Total lights
      ImGui::TextWrapped("Total lights");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->lights.size());
      // Total vertices
      ImGui::TextWrapped("Total vertices");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->getTotalVertices());
      // Total materials
      ImGui::TextWrapped("Total materials");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->materials.size());
      // Total textures
      ImGui::TextWrapped("Total textures");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->textures.size());
      // Active meshes
      ImGui::TextWrapped("Active meshes");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->getActiveMeshes().size());
      // Active indices
      ImGui::TextWrapped("Active indices");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->getActiveIndices());
      // Active bones
      ImGui::TextWrapped("Active bones");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->getActiveBones());
      // Active particles
      ImGui::TextWrapped("Active particles");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld", _scene->getActiveParticles());
    }
    // Duration
    if (ImGui::CollapsingHeader("Duration", "Duration", true, true)) {
      // Meshes selection
      ImGui::TextWrapped("Meshes selection");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%ld ms",
        _sceneInstrumentation->activeMeshesEvaluationTimeCounter().current());
      // Render targets
      ImGui::TextWrapped("Render targets");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%ld ms",
        _sceneInstrumentation->renderTargetsRenderTimeCounter().current());
      // Particles
      ImGui::TextWrapped("Particles");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%ld ms",
        _sceneInstrumentation->particlesRenderTimeCounter().current());
      // Sprites
      ImGui::TextWrapped("Sprites");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%ld ms", _sceneInstrumentation->spritesRenderTimeCounter().current());
      // Animations
      ImGui::TextWrapped("Animations");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%ld ms", _sceneInstrumentation->animationsTimeCounter().current());
      // Physics
      ImGui::TextWrapped("Physics");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld ms",
                         _sceneInstrumentation->physicsTimeCounter().current());
      // Render
      ImGui::TextWrapped("Render");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld ms",
                         _sceneInstrumentation->renderTimeCounter().current());
      // Frame
      ImGui::TextWrapped("Frame");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%ld ms",
                         _sceneInstrumentation->frameTimeCounter().current());

      // Inter-frame
      ImGui::TextWrapped("Inter-frame");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%ld ms", _sceneInstrumentation->interFrameTimeCounter().current());
      // GPU Frame time
      ImGui::TextWrapped("GPU Frame time");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%.2f ms",
                         _engineInstrumentation->gpuFrameTimeCounter().current()
                           * 0.000001);
      // GPU Frame time (average)
      ImGui::TextWrapped("GPU Frame time (average)");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%.2f ms",
                         _engineInstrumentation->gpuFrameTimeCounter().average()
                           * 0.000001);
      // Potential FPS
      ImGui::TextWrapped("Potential FPS");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%.2f", (1000.0 / _sceneInstrumentation->frameTimeCounter().current()));
      // Resolution
      ImGui::TextWrapped("Resolution");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%dx%d", _engine->getRenderWidth(),
                         _engine->getRenderHeight());
    }
    // Extensions
    if (ImGui::CollapsingHeader("Extension", "Extensions")) {
      // Std derivates
      ImGui::TextWrapped("Std derivates");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s",
                         engineCapabilities.standardDerivatives ? "Yes" : "No");
      // Compressed textures
      ImGui::TextWrapped("Compressed textures");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s", engineCapabilities.s3tc ? "Yes" : "No");
      // Hardware instances
      ImGui::TextWrapped("Hardware instances");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s",
                         engineCapabilities.instancedArrays ? "Yes" : "No");
      // Texture float
      ImGui::TextWrapped("Texture float");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s", engineCapabilities.textureFloat ? "Yes" : "No");
      // 32bits indices
      ImGui::TextWrapped("32bits indices");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s", engineCapabilities.uintIndices ? "Yes" : "No");
      // Fragment depth
      ImGui::TextWrapped("Fragment depth");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%s", engineCapabilities.fragmentDepthSupported ? "Yes" : "No");
      // High precision shaders
      ImGui::TextWrapped("High precision shaders");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped(
        "%s", engineCapabilities.highPrecisionShaderSupported ? "Yes" : "No");
      // Draw buffers
      ImGui::TextWrapped("Draw buffers");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s", engineCapabilities.drawBuffersExtension ? "Yes" :
                                                                         "No");
      // Vertex array object
      ImGui::TextWrapped("Vertex array object");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s",
                         engineCapabilities.vertexArrayObject ? "Yes" : "No");
      // Timer query
      ImGui::TextWrapped("Timer query");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s", engineCapabilities.timerQuery ? "Yes" : "No");
    }
    // Caps.
    if (ImGui::CollapsingHeader("Caps.", "Caps.")) {
      // Stencil
      ImGui::TextWrapped("Stencil");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%s",
                         _engine->isStencilEnable() ? "Enabled" : "Disabled");
      // Max textures units
      ImGui::TextWrapped("Max textures units");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%d", engineCapabilities.maxTexturesImageUnits);
      // Max textures size
      ImGui::TextWrapped("Max textures size");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%d", engineCapabilities.maxTextureSize);
      // Max anisotropy
      ImGui::TextWrapped("Max anisotropy");
      ImGui::SameLine(offsetX);
      ImGui::TextWrapped("%d", engineCapabilities.maxAnisotropy);
    }
  }
  ImGui::EndDock();
}

void StatsTab::dispose()
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

} // end of namespace BABYLON
