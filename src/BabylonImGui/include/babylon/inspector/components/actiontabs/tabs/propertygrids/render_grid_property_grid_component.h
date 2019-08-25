#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SCENE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SCENE_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/mesh_reserved_data_store.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/grid/grid_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

struct BABYLON_SHARED_EXPORT RenderGridPropertyGridComponent {

  static void addOrRemoveGrid(const MeshPtr& mesh,
                              MeshReservedDataStore& meshReservedDataStore)
  {
    const auto& scene
      = UtilityLayerRenderer::DefaultKeepDepthUtilityLayer()->utilityLayerScene;

    if (!meshReservedDataStore.gridMesh) {
      auto extend = mesh->getScene()->getWorldExtends();
      auto width  = (extend.max.x - extend.min.x) * 5.f;
      auto depth  = (extend.max.z - extend.min.z) * 5.f;

      meshReservedDataStore.gridMesh
        = Mesh::CreateGround("grid", 1.f, 1.f, 1, scene.get());
      meshReservedDataStore.gridMesh->scaling().x = std::max(width, depth);
      meshReservedDataStore.gridMesh->scaling().z
        = meshReservedDataStore.gridMesh->scaling().x;
      meshReservedDataStore.gridMesh->isPickable = false;

      auto groundMaterial
        = MaterialsLibrary::GridMaterial::New("GridMaterial", scene);
      groundMaterial->majorUnitFrequency  = 10.f;
      groundMaterial->minorUnitVisibility = 0.3f;
      groundMaterial->gridRatio           = 0.01f;
      groundMaterial->backFaceCulling     = false;
      groundMaterial->mainColor           = Color3(1.f, 1.f, 1.f);
      groundMaterial->lineColor           = Color3(1.f, 1.f, 1.f);
      groundMaterial->opacity             = 0.8f;
      groundMaterial->zOffset             = 1.f;
      // groundMaterial->opacityTexture =
      // Texture::New("textures/backgroundGround.png", scene);

      meshReservedDataStore.gridMesh->material = groundMaterial;

      meshReservedDataStore.renderGridEnabled = true;
      return;
    }

    meshReservedDataStore.renderGridEnabled
      = !meshReservedDataStore.renderGridEnabled;
    meshReservedDataStore.gridMesh->dispose(true, true);
    meshReservedDataStore.gridMesh = nullptr;
  }

  static void render(const MeshPtr& mesh,
                     MeshReservedDataStore& meshReservedDataStore)
  {
    if (CheckBoxLineComponent::render(
          "Render grid", meshReservedDataStore.renderGridEnabled)) {
      addOrRemoveGrid(mesh, meshReservedDataStore);
    }
  }

}; // end of class ScenePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SCENE_PROPERTY_GRID_COMPONENT_H
