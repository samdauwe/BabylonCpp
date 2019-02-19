#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

struct BABYLON_SHARED_EXPORT MeshPropertyGridComponent {

  static void renderNormalVectors()
  {
  }

  static void displayNormals()
  {
  }

  static void onMaterialLink()
  {
  }

  static std::string convertPhysicsTypeToString(const MeshPtr& mesh)
  {
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

  static void render(const MeshPtr& mesh)
  {
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

}; // end of struct MeshPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_PROPERTY_GRID_COMPONENT_H
