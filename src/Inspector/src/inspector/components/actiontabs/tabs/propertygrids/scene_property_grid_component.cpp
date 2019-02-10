#include <babylon/inspector/components/actiontabs/tabs/propertygrids/scene_property_grid_component.h>

#include <babylon/engine/scene.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/physics/physics_engine.h>

namespace BABYLON {

ScenePropertyGridComponent::ScenePropertyGridComponent(
  const IScenePropertyGridComponentProps& iProps)
    : props{iProps}
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
  // --- RENDERING MODE ---
  static auto renderingModeOpened = true;
  ImGui::SetNextTreeNodeOpen(renderingModeOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("RENDERING MODE")) {

    renderingModeOpened = true;
  }
  else {
    renderingModeOpened = false;
  }
  // --- ENVIRONMENT ---
  static auto environmentOpened = true;
  ImGui::SetNextTreeNodeOpen(environmentOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("ENVIRONMENT")) {

    environmentOpened = true;
  }
  else {
    environmentOpened = false;
  }
  // --- IMAGE PROCESSING ---
  static auto imageprocessingOpened = true;
  ImGui::SetNextTreeNodeOpen(imageprocessingOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("IMAGE PROCESSING")) {

    imageprocessingOpened = true;
  }
  else {
    imageprocessingOpened = false;
  }
  // --- PHYSICS ---
  static auto physicsOpened = false;
  ImGui::SetNextTreeNodeOpen(physicsOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("PHYSICS")) {

    physicsOpened = true;
  }
  else {
    physicsOpened = false;
  }
  // --- COLLISIONS ---
  static auto collisionsOpened = false;
  ImGui::SetNextTreeNodeOpen(collisionsOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("COLLISIONS")) {

    collisionsOpened = true;
  }
  else {
    collisionsOpened = false;
  }
}

} // end of namespace BABYLON
