#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/mesh_property_grid_component.h>

#include <imgui.h>

#include <babylon/mesh/mesh.h>

namespace BABYLON {

MeshPropertyGridComponent::MeshPropertyGridComponent(
  const IMeshPropertyGridComponentProps& iProps)
    : props{iProps}
{
}

MeshPropertyGridComponent::~MeshPropertyGridComponent()
{
}

void MeshPropertyGridComponent::renderNormalVectors()
{
}

void MeshPropertyGridComponent::displayNormals()
{
}

void MeshPropertyGridComponent::onMaterialLink()
{
}

std::string MeshPropertyGridComponent::convertPhysicsTypeToString() const
{
  const auto& mesh = props.mesh;
  if (mesh->physicsImpostor()) {
    switch (mesh->physicsImpostor()->physicsImposterType) {
      case PhysicsImpostor::NoImpostor:
        return "No impostor";
      case PhysicsImpostor::SphereImpostor:
        return "Sphere";
      case PhysicsImpostor::BoxImpostor:
        return "Box";
      case PhysicsImpostor::PlaneImpostor:
        return "Plane";
      case PhysicsImpostor::MeshImpostor:
        return "Mesh";
      case PhysicsImpostor::CylinderImpostor:
        return "Cylinder";
      case PhysicsImpostor::ParticleImpostor:
        return "Particle";
      case PhysicsImpostor::HeightmapImpostor:
        return "Heightmap";
    }
  }

  return "Unknown";
}

void MeshPropertyGridComponent::render()
{
  const auto& mesh = props.mesh;

  // --- GENERAL ---
  static auto generalOpened = true;
  ImGui::SetNextTreeNodeOpen(generalOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("GENERAL")) {
    generalOpened = true;
  }
  else {
    generalOpened = false;
  }
  // --- TRANSFORMS ---
  static auto transformsOpened = true;
  ImGui::SetNextTreeNodeOpen(transformsOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("TRANSFORMS")) {
    transformsOpened = true;
  }
  else {
    transformsOpened = false;
  }
  // --- DISPLAY ---
  static auto displayOpened = false;
  ImGui::SetNextTreeNodeOpen(displayOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("DISPLAY")) {
    displayOpened = true;
  }
  else {
    displayOpened = false;
  }
  // --- ADVANCED ---
  static auto advancedOpened = false;
  ImGui::SetNextTreeNodeOpen(advancedOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("ADVANCED")) {
    advancedOpened = true;
  }
  else {
    advancedOpened = false;
  }
  // --- PHYSICS ---
  if (mesh->physicsImpostor()) {
    static auto physicsOpened = false;
    ImGui::SetNextTreeNodeOpen(physicsOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("PHYSICS")) {
      physicsOpened = true;
    }
    else {
      physicsOpened = false;
    }
  }
  // --- DEBUG ---
  static auto debugOpened = false;
  ImGui::SetNextTreeNodeOpen(debugOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("DEBUG")) {
    debugOpened = true;
  }
  else {
    debugOpened = false;
  }
}

} // end of namespace BABYLON
