#include <babylon/inspector/components/actiontabs/tabs/propertygrids/scene_property_grid_component.h>

#include <babylon/engine/scene.h>
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
    : props{iProps}, _dummy{std::nullopt}
{
}

ScenePropertyGridComponent::~ScenePropertyGridComponent()
{
}

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

  // --- RENDERING MODE ---
  static auto renderingModeContainerOpened = true;
  ImGui::SetNextTreeNodeOpen(renderingModeContainerOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("RENDERING MODE")) {
    RadioButtonLineComponent::render(
      "Point", scene->forcePointsCloud,
      [this]() { setRenderingModes(true, false); });
    RadioButtonLineComponent::render(
      "Wireframe", scene->forceWireframe,
      [this]() { setRenderingModes(false, true); });
    RadioButtonLineComponent::render(
      "Solid", !scene->forcePointsCloud && !scene->forceWireframe,
      [this]() { setRenderingModes(false, false); });

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
    CheckBoxLineComponent::render("Environment texture (IBL)",
                                  scene->environmentTexture() != nullptr,
                                  [this](bool /*value*/) { switchIBL(); });
    FogPropertyGridComponent::render(scene);
    environmentContainerOpened = true;
  }
  else {
    environmentContainerOpened = false;
  }
  // --- IMAGE PROCESSING ---
  static auto imageprocessingContainerOpened = true;
  ImGui::SetNextTreeNodeOpen(imageprocessingContainerOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("IMAGE PROCESSING")) {
    SliderLineComponent::render(
      "Contrast", imageProcessing->contrast(), 0.f, 4.f, 0.1f,
      [&](float value) { imageProcessing->contrast = value; }, "%.2f");
    SliderLineComponent::render(
      "Exposure", imageProcessing->exposure(), 0.f, 4.f, 0.1f,
      [&](float value) { imageProcessing->exposure = value; }, "%.2f");
    CheckBoxLineComponent::render(
      "Tone mapping", imageProcessing->toneMappingEnabled(),
      [&](bool value) { imageProcessing->toneMappingEnabled = value; });
    OptionsLineComponent::render(
      "Tone mapping type", imageProcessing->toneMappingType(),
      toneMappingOptions,
      [&](unsigned int value) { imageProcessing->toneMappingType = value; });
    imageprocessingContainerOpened = true;
  }
  else {
    imageprocessingContainerOpened = false;
  }
  // --- PHYSICS ---
  if (_dummy.has_value()) {
    static auto physicsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(physicsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("PHYSICS")) {
      physicsContainerOpened = true;
    }
    else {
      physicsContainerOpened = false;
    }
  }
  // --- COLLISIONS ---
  static auto collisionsContainerOpened = false;
  ImGui::SetNextTreeNodeOpen(collisionsContainerOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("COLLISIONS")) {
    Vector3LineComponent::render("Gravity", scene->gravity);
    collisionsContainerOpened = true;
  }
  else {
    collisionsContainerOpened = false;
  }
} // namespace BABYLON

} // end of namespace BABYLON
