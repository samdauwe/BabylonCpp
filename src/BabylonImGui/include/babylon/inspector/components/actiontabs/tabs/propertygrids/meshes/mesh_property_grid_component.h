#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/quaternion_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/mesh_reserved_data_store.h>
#include <babylon/materials/material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materialslibrary/normal/normal_material.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT MeshPropertyGridComponent {

  static void renderWireframeOver(const MeshPtr& /*mesh*/,
                                  MeshReservedDataStore& /*meshReservedDataStore*/)
  {
  }

  static void renderNormalVectors(const MeshPtr& mesh, MeshReservedDataStore& meshReservedDataStore)
  {
    const auto& scene = mesh->getScene();

    if (meshReservedDataStore.normalLines) {
      meshReservedDataStore.normalLines->dispose();
      meshReservedDataStore.normalLines = nullptr;

      return;
    }

    const auto normals   = mesh->getVerticesData(VertexBuffer::NormalKind);
    const auto positions = mesh->getVerticesData(VertexBuffer::PositionKind);

    const auto color = Color3::White();
    const auto bbox  = mesh->getBoundingInfo();
    // TODO FIXME
    // const auto diag  = bbox->maximum().subtractToRef(bbox->minimum(),
    // TmpVectors::Vector3Array[0]); const auto size  = diag.length() * 0.05f;
    const auto size = 1.f;

    std::vector<std::vector<Vector3>> lines;
    for (auto i = 0u; i < normals.size(); i += 3) {
      const auto v1             = Vector3::FromArray(positions, i);
      const auto v2             = v1.add(Vector3::FromArray(normals, i).scaleInPlace(size));
      std::vector<Vector3> v1v2 = {v1, v2};
      lines.emplace_back(v1v2);
    }

    LineSystemOptions lineOptions;
    lineOptions.lines            = lines;
    auto normalLines             = MeshBuilder::CreateLineSystem("normalLines", lineOptions, scene);
    normalLines->color           = color;
    normalLines->parent          = mesh.get();
    meshReservedDataStore.hidden = true;

    meshReservedDataStore.normalLines = normalLines;
  }

  static void displayNormals(const MeshPtr& mesh, MeshReservedDataStore& meshReservedDataStore)
  {
    const auto& scene = mesh->getScene();
    if (!mesh->material()) {
      return;
    }

    if (mesh->material() && mesh->material()->getClassName() == "NormalMaterial") {
      mesh->material()->dispose();

      mesh->material                         = meshReservedDataStore.originalMaterial;
      meshReservedDataStore.originalMaterial = nullptr;
      meshReservedDataStore.displayNormals   = false;
    }
    else {
      meshReservedDataStore.originalMaterial = mesh->material;
      const auto normalMaterial = MaterialsLibrary::NormalMaterial::New("normalMaterial", scene);
      normalMaterial->disableLighting = true;
      if (mesh->material()) {
        normalMaterial->sideOrientation = mesh->material()->sideOrientation;
      }
      meshReservedDataStore.normalMaterialHidden = true;
      mesh->material                             = normalMaterial;
      meshReservedDataStore.displayNormals       = true;
    }
  }

  static void displayVertexColors(const MeshPtr& mesh, MeshReservedDataStore& meshReservedDataStore)
  {
    const auto scene = mesh->getScene();

    if (mesh->material() && meshReservedDataStore.isVertexColorMaterial) {
      mesh->material()->dispose();

      mesh->material                            = meshReservedDataStore.originalMaterial;
      meshReservedDataStore.originalMaterial    = nullptr;
      meshReservedDataStore.displayVertexColors = false;
    }
    else {
      if (!meshReservedDataStore.originalMaterial) {
        meshReservedDataStore.originalMaterial = mesh->material();
      }
      const auto vertexColorMaterial       = StandardMaterial::New("vertex colors", scene);
      vertexColorMaterial->disableLighting = true;
      vertexColorMaterial->emissiveColor   = Color3::White();
      if (mesh->material()) {
        vertexColorMaterial->sideOrientation = mesh->material()->sideOrientation;
      }
      meshReservedDataStore.hidden                = true;
      meshReservedDataStore.isVertexColorMaterial = true;
      mesh->useVertexColors                       = true;
      mesh->material                              = vertexColorMaterial;
      meshReservedDataStore.displayVertexColors   = true;
    }
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
        case PhysicsImpostor::ConvexHullImpostor:
          return "Convex hull";
        case PhysicsImpostor::RopeImpostor:
          return "Rope";
        case PhysicsImpostor::SoftbodyImpostor:
          return "Soft body";
      }
    }

    return "Unknown";
  }

  static void render(const MeshPtr& mesh, MeshReservedDataStore& meshReservedDataStore)
  {
    // --- GENERAL ---
    static auto generalOpened = true;
    ImGui::SetNextTreeNodeOpen(generalOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("ID", mesh->id);
      TextLineComponent::render("Unique ID", std::to_string(mesh->uniqueId));
      TextLineComponent::render("Class", mesh->getClassName());
      TextLineComponent::render("Vertices", std::to_string(mesh->getTotalVertices()));
      TextLineComponent::render("Faces", std::to_string(mesh->getTotalIndices() / 3));
      TextLineComponent::render(
        "Sub-meshes", !mesh->subMeshes.empty() ? std::to_string(mesh->subMeshes.size()) : "0");
      TextLineComponent::render("Has skeleton", mesh->skeleton() ? "Yes" : "No");
      if (CheckBoxLineComponent::render("IsEnabled", mesh->isEnabled())) {
        mesh->setEnabled(!mesh->isEnabled());
      }
      if (CheckBoxLineComponent::render("IsPickable", mesh->isPickable)) {
        mesh->isPickable = !mesh->isPickable;
      }
      if (mesh->material()) {
        TextLineComponent::render("Material", mesh->material()->name);
      }
      generalOpened = true;
    }
    else {
      generalOpened = false;
    }
    // --- TRANSFORMS ---
    static auto transformsOpened = true;
    ImGui::SetNextTreeNodeOpen(transformsOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORMS")) {
      Vector3LineComponent::render("Position", mesh->position());
      if (!mesh->rotationQuaternion()) {
        Vector3LineComponent::render("Rotation", mesh->rotation());
      }
      if (mesh->rotationQuaternion()) {
        QuaternionLineComponent::render("Rotation", *mesh->rotationQuaternion());
      }
      Vector3LineComponent::render("Scaling", mesh->scaling());
      transformsOpened = true;
    }
    else {
      transformsOpened = false;
    }
    // --- DISPLAY ---
    static auto displayOpened = false;
    ImGui::SetNextTreeNodeOpen(displayOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("DISPLAY")) {
      if (!mesh->isAnInstance()) {
        auto sliderChange
          = SliderLineComponent::render("Visibility", mesh->visibility(), 0.f, 1.f, 0.01f, "%.2f");
        if (sliderChange) {
          mesh->visibility = sliderChange.value();
        }
      }
      auto valueChange
        = FloatLineComponent::render("Alpha index", static_cast<float>(mesh->alphaIndex));
      if (valueChange) {
        mesh->alphaIndex = static_cast<int>(valueChange.value());
      }
      if (CheckBoxLineComponent::render("Receive shadows", mesh->receiveShadows())) {
        mesh->receiveShadows = !mesh->receiveShadows();
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
        if (CheckBoxLineComponent::render("Use vertex colors", mesh->useVertexColors())) {
          mesh->useVertexColors = !mesh->useVertexColors();
        }
      }
      auto scene = mesh->getScene();
      if (scene->fogMode != Scene::FOGMODE_NONE) {
        if (CheckBoxLineComponent::render("Apply fog", mesh->applyFog())) {
          mesh->applyFog = !mesh->applyFog();
        }
      }
      if (!mesh->parent) {
        if (CheckBoxLineComponent::render("Infinite distance", mesh->infiniteDistance)) {
          mesh->infiniteDistance = !mesh->infiniteDistance;
        }
      }
      displayOpened = true;
    }
    else {
      displayOpened = false;
    }
    // --- ADVANCED ---
    static auto advancedOpened = false;
    ImGui::SetNextTreeNodeOpen(advancedOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("ADVANCED")) {
      if (mesh->useBones) {
        if (CheckBoxLineComponent::render("Compute bones using shaders",
                                          mesh->computeBonesUsingShaders())) {
          mesh->computeBonesUsingShaders = !mesh->computeBonesUsingShaders();
        }
      }
      if (CheckBoxLineComponent::render("Collisions", mesh->checkCollisions())) {
        mesh->checkCollisions = !mesh->checkCollisions();
      }
      TextLineComponent::render(
        "Has normals", mesh->isVerticesDataPresent(VertexBuffer::NormalKind) ? "Yes" : "No");
      TextLineComponent::render(
        "Has vertex colors", mesh->isVerticesDataPresent(VertexBuffer::ColorKind) ? "Yes" : "No");
      TextLineComponent::render("has UV set 0",
                                mesh->isVerticesDataPresent(VertexBuffer::UVKind) ? "Yes" : "No");
      TextLineComponent::render("has UV set 1",
                                mesh->isVerticesDataPresent(VertexBuffer::UV2Kind) ? "Yes" : "No");
      TextLineComponent::render("has UV set 2",
                                mesh->isVerticesDataPresent(VertexBuffer::UV3Kind) ? "Yes" : "No");
      TextLineComponent::render("has UV set 3",
                                mesh->isVerticesDataPresent(VertexBuffer::UV4Kind) ? "Yes" : "No");
      TextLineComponent::render(
        "has tangents", mesh->isVerticesDataPresent(VertexBuffer::TangentKind) ? "Yes" : "No");
      TextLineComponent::render(
        "has matrix weights",
        mesh->isVerticesDataPresent(VertexBuffer::MatricesWeightsKind) ? "Yes" : "No");
      TextLineComponent::render(
        "has matrix indices",
        mesh->isVerticesDataPresent(VertexBuffer::MatricesIndicesKind) ? "Yes" : "No");
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
        if (mesh->physicsImpostor()) {
          auto valueChange = FloatLineComponent::render("Mass", mesh->physicsImpostor()->mass());
          if (valueChange) {
            mesh->physicsImpostor()->mass = valueChange.value();
          }
          valueChange = FloatLineComponent::render("Friction", mesh->physicsImpostor()->friction());
          if (valueChange) {
            mesh->physicsImpostor()->friction = valueChange.value();
          }
          valueChange
            = FloatLineComponent::render("Restitution", mesh->physicsImpostor()->restitution());
          if (valueChange) {
            mesh->physicsImpostor()->restitution = valueChange.value();
          }
          TextLineComponent::render("Type", convertPhysicsTypeToString(mesh));
        }
        physicsOpened = true;
      }
      else {
        physicsOpened = false;
      }
    }
    // --- EDGE RENDERING ---
    static auto edgeRenderingOpened = false;
    ImGui::SetNextTreeNodeOpen(edgeRenderingOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("EDGE RENDERING")) {
      const auto edgesRendererEnabled = (mesh->edgesRenderer() != nullptr);
      if (CheckBoxLineComponent::render("Enable", edgesRendererEnabled)) {
        if (edgesRendererEnabled) {
          mesh->disableEdgesRendering();
        }
        else {
          mesh->enableEdgesRendering();
        }
      }
      auto sliderChange
        = SliderLineComponent::render("Edge width", mesh->edgesWidth, 0.f, 10.f, 0.1f, "%.2f");
      if (sliderChange) {
        mesh->edgesWidth = sliderChange.value();
      }
      Color3LineComponent::render("Edge color", mesh->edgesColor);
      edgeRenderingOpened = true;
    }
    else {
      edgeRenderingOpened = false;
    }
    // --- OUTLINE & OVERLAY ---
    static auto outlineAndOverlayOpened = false;
    ImGui::SetNextTreeNodeOpen(outlineAndOverlayOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("OUTLINE & OVERLAY")) {
      if (CheckBoxLineComponent::render("Render overlay", mesh->renderOverlay())) {
        mesh->renderOverlay = !mesh->renderOverlay();
      }
      Color3LineComponent::render("Overlay color", mesh->overlayColor);
      if (CheckBoxLineComponent::render("Render outline", mesh->renderOutline())) {
        mesh->renderOutline = !mesh->renderOutline();
      }
      Color3LineComponent::render("Outline color", mesh->outlineColor);
      outlineAndOverlayOpened = true;
    }
    else {
      outlineAndOverlayOpened = false;
    }
    // --- DEBUG ---
    static auto debugOpened = false;
    ImGui::SetNextTreeNodeOpen(debugOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("DEBUG")) {
      if (mesh->material()) {
        if (CheckBoxLineComponent::render("Display normals",
                                          meshReservedDataStore.displayNormals)) {
          displayNormals(mesh, meshReservedDataStore);
        }
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
        if (CheckBoxLineComponent::render("Render vertex normals",
                                          meshReservedDataStore.renderNormalVectors)) {
          renderNormalVectors(mesh, meshReservedDataStore);
          meshReservedDataStore.renderNormalVectors = !meshReservedDataStore.renderNormalVectors;
        }
      }
      if (CheckBoxLineComponent::render("Render wireframe over mesh",
                                        meshReservedDataStore.renderWireframeOver)) {
        renderWireframeOver(mesh, meshReservedDataStore);
        meshReservedDataStore.renderWireframeOver = !meshReservedDataStore.renderWireframeOver;
      }
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
