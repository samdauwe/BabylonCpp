#include <babylon/inspector/components/actiontabs/tabs/propertygrids/scene_property_grid_component.h>

#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/options_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/radio_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/fog_property_grid_component.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/physics/physics_engine.h>

namespace BABYLON {

ScenePropertyGridComponent::ScenePropertyGridComponent(
  const IScenePropertyGridComponentProps& iProps)
    : props{iProps}, _storedEnvironmentTexture{nullptr}, _dummy{std::nullopt}
{
}

ScenePropertyGridComponent::~ScenePropertyGridComponent() = default;

void ScenePropertyGridComponent::setRenderingModes(bool point, bool wireframe)
{
  const auto& scene       = props.scene;
  scene->forcePointsCloud = point;
  scene->forceWireframe   = wireframe;
}

void ScenePropertyGridComponent::switchIBL()
{
  const auto& scene = props.scene;

  if (scene->environmentTexture()) {
    _storedEnvironmentTexture = scene->environmentTexture();
    scene->environmentTexture = nullptr;
  }
  else {
    scene->environmentTexture = _storedEnvironmentTexture;
    _storedEnvironmentTexture = nullptr;
  }
}

void ScenePropertyGridComponent::updateEnvironmentTexture(
  const std::string& /*fileLocation*/)
{
}

void ScenePropertyGridComponent::updateGravity(const Vector3& newValue)
{
  const auto& scene         = props.scene;
  const auto& physicsEngine = scene->getPhysicsEngine();

  if (physicsEngine) {
    physicsEngine->setGravity(newValue);
  }
}

void ScenePropertyGridComponent::updateTimeStep(float newValue)
{
  const auto& scene         = props.scene;
  const auto& physicsEngine = scene->getPhysicsEngine();

  if (physicsEngine) {
    physicsEngine->setTimeStep(newValue);
  }
}

void ScenePropertyGridComponent::render()
{
  const auto& scene = props.scene;

  const auto& physicsEngine = scene->getPhysicsEngine();
  _dummy                    = std::nullopt;

  if (physicsEngine) {
    _dummy->gravity  = physicsEngine->gravity;
    _dummy->timeStep = physicsEngine->getTimeStep();
  }

  const auto& imageProcessing = scene->imageProcessingConfiguration();

  static std::vector<std::pair<const char*, unsigned int>> toneMappingOptions{
    {"Standard", ImageProcessingConfiguration::TONEMAPPING_STANDARD},
    {"ACES", ImageProcessingConfiguration::TONEMAPPING_ACES}};

  static std::vector<std::pair<const char*, unsigned int>> vignetteModeOptions{
    {"Multiply", ImageProcessingConfiguration::VIGNETTEMODE_MULTIPLY()},
    {"Opaque", ImageProcessingConfiguration::VIGNETTEMODE_OPAQUE()}};

  // --- RENDERING MODE ---
  static auto renderingModeContainerOpened = true;
  ImGui::SetNextTreeNodeOpen(renderingModeContainerOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("RENDERING MODE")) {
    if (RadioButtonLineComponent::render("Point", scene->forcePointsCloud())) {
      setRenderingModes(true, false);
    }
    if (RadioButtonLineComponent::render("Wireframe",
                                         scene->forceWireframe())) {
      setRenderingModes(false, true);
    }
    if (RadioButtonLineComponent::render(
          "Solid", !scene->forcePointsCloud() && !scene->forceWireframe())) {
      setRenderingModes(false, false);
    }
    renderingModeContainerOpened = true;
  }
  else {
    renderingModeContainerOpened = false;
  }
  // --- ENVIRONMENT ---
  static auto environmentContainerOpened = true;
  ImGui::SetNextTreeNodeOpen(environmentContainerOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("ENVIRONMENT")) {
    Color3LineComponent::render("Clear color", scene->clearColor);
    CheckBoxLineComponent::render("Clear color enabled", scene->autoClear);
    Color3LineComponent::render("Ambient color", scene->ambientColor);
    if (CheckBoxLineComponent::render("Environment texture (IBL)",
                                      scene->environmentTexture() != nullptr)) {
      switchIBL();
    }
    FogPropertyGridComponent::render(scene);
    environmentContainerOpened = true;
  }
  else {
    environmentContainerOpened = false;
  }
  // --- MATERIAL IMAGE PROCESSING ---
  static auto imageprocessingContainerOpened = true;
  ImGui::SetNextTreeNodeOpen(imageprocessingContainerOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("MATERIAL IMAGE PROCESSING")) {
    auto sliderChange = SliderLineComponent::render(
      "Contrast", imageProcessing->contrast(), 0.f, 4.f, 0.1f, "%.2f");
    if (sliderChange) {
      imageProcessing->contrast = sliderChange.value();
    }
    sliderChange = SliderLineComponent::render(
      "Exposure", imageProcessing->exposure(), 0.f, 4.f, 0.1f, "%.2f");
    if (sliderChange) {
      imageProcessing->exposure = sliderChange.value();
    }
    if (CheckBoxLineComponent::render("Tone mapping",
                                      imageProcessing->toneMappingEnabled())) {
      imageProcessing->toneMappingEnabled
        = !imageProcessing->toneMappingEnabled();
    }
    auto optionChange = OptionsLineComponent::render(
      "Tone mapping type", imageProcessing->toneMappingType(),
      toneMappingOptions);
    if (optionChange) {
      imageProcessing->toneMappingType = optionChange.value();
    }
    if (CheckBoxLineComponent::render("Vignette",
                                      imageProcessing->vignetteEnabled())) {
      imageProcessing->vignetteEnabled = !imageProcessing->vignetteEnabled();
    }
    sliderChange = SliderLineComponent::render("Vignette weight",
                                               imageProcessing->vignetteWeight,
                                               0.f, 4.f, 0.1f, "%.2f");
    if (sliderChange) {
      imageProcessing->vignetteWeight = sliderChange.value();
    }
    sliderChange = SliderLineComponent::render("Vignette stretch",
                                               imageProcessing->vignetteStretch,
                                               0.f, 1.f, 0.1f, "%.2f");
    if (sliderChange) {
      imageProcessing->vignetteStretch = sliderChange.value();
    }
    sliderChange = SliderLineComponent::render(
      "Vignette FOV", imageProcessing->vignetteCameraFov, 0.f, Math::PI, 0.1f,
      "%.2f");
    if (sliderChange) {
      imageProcessing->vignetteCameraFov = sliderChange.value();
    }
    sliderChange = SliderLineComponent::render("Vignette center X",
                                               imageProcessing->vignetteCentreX,
                                               0.f, 1.f, 0.1f, "%.2f");
    if (sliderChange) {
      imageProcessing->vignetteCentreX = sliderChange.value();
    }
    sliderChange = SliderLineComponent::render("Vignette center Y",
                                               imageProcessing->vignetteCentreY,
                                               0.f, 1.f, 0.1f, "%.2f");
    if (sliderChange) {
      imageProcessing->vignetteCentreY = sliderChange.value();
    }
    Color3LineComponent::render("Vignette color",
                                imageProcessing->vignetteColor);
    optionChange = OptionsLineComponent::render(
      "Vignette blend mode", imageProcessing->vignetteBlendMode(),
      vignetteModeOptions);
    if (optionChange) {
      imageProcessing->vignetteBlendMode = optionChange.value();
    }
    imageprocessingContainerOpened = true;
  }
  else {
    imageprocessingContainerOpened = false;
  }
  // --- PHYSICS ---
  if (_dummy.has_value()) {
    static auto physicsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(physicsContainerOpened, ImGuiCond_Always);
    physicsContainerOpened = ImGui::CollapsingHeader("PHYSICS");
  }
  // --- COLLISIONS ---
  static auto collisionsContainerOpened = true;
  ImGui::SetNextTreeNodeOpen(collisionsContainerOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("COLLISIONS")) {
    Vector3LineComponent::render("Gravity", scene->gravity);
    collisionsContainerOpened = true;
  }
  else {
    collisionsContainerOpened = false;
  }
}

} // end of namespace BABYLON
