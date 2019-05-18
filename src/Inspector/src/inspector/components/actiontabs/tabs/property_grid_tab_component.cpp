#include <babylon/inspector/components/actiontabs/tabs/property_grid_tab_component.h>

#include <imgui.h>

#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/arc_rotate_camera_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/free_camera_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/directional_light_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/hemispheric_light_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/point_light_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/spot_light_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/background_material_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/material_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/pbr_material_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/pbr_metallic_roughness_material_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/pbr_specular_glossiness_material_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/standard_material_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/texture_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/bone_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/mesh_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/transform_node_property_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/scene_property_grid_component.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/background/background_material.h>
#include <babylon/materials/material.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

PropertyGridTabComponent::PropertyGridTabComponent(
  const IPaneComponentProps& iProps)
    : PaneComponent{iProps}, _scenePropertyGridComponent{nullptr}
{
  componentWillMount();
}

PropertyGridTabComponent::~PropertyGridTabComponent()
{
  componentWillUnmount();
}

void PropertyGridTabComponent::componentWillMount()
{
  IScenePropertyGridComponentProps sceneProps;
  sceneProps.scene = props.scene;
  _scenePropertyGridComponent
    = std::make_unique<ScenePropertyGridComponent>(sceneProps);
}

void PropertyGridTabComponent::componentWillUnmount()
{
}

void PropertyGridTabComponent::render()
{
  const auto& entity = props.selectedEntity;

  if (!entity && (!entity->uniqueId || entity->name.empty())) {
    // Info message
    ImGui::TextWrapped("%s", "Please select an entity in the scene explorer.");
    return;
  }

  auto entityId    = entity->uniqueId ? *entity->uniqueId : 0ull;
  auto& entityName = entity->name;

  if (entity->type == EntityType::Scene) {
    if (_scenePropertyGridComponent) {
      _scenePropertyGridComponent->render();
    }
    return;
  }

  if (entity->type == EntityType::Mesh) {
    auto& mesh = _entityCache.mesh;
    if (!mesh || mesh->uniqueId != entityId) {
      mesh = std::dynamic_pointer_cast<Mesh>(
        props.scene->getMeshByUniqueID(entityId));
      _reservedDataStore.mesh[mesh->uniqueId] = MeshReservedDataStore{};
    }
    if (mesh && mesh->getTotalVertices() > 0) {
      MeshPropertyGridComponent::render(
        mesh, _reservedDataStore.mesh[mesh->uniqueId]);
    }
    return;
  }

  if (entity->type == EntityType::FreeCamera) {
    auto& freeCamera = _entityCache.freeCamera;
    if (!freeCamera || freeCamera->uniqueId != entityId) {
      freeCamera = std::dynamic_pointer_cast<FreeCamera>(
        props.scene->getCameraByUniqueID(entityId));
    }
    if (freeCamera) {
      FreeCameraPropertyGridComponent::render(freeCamera);
    }
    return;
  }

  if (entity->type == EntityType::ArcRotateCamera) {
    auto& arcRotateCamera = _entityCache.arcRotateCamera;
    if (!arcRotateCamera || arcRotateCamera->uniqueId != entityId) {
      arcRotateCamera = std::dynamic_pointer_cast<ArcRotateCamera>(
        props.scene->getCameraByUniqueID(entityId));
    }
    if (arcRotateCamera) {
      ArcRotateCameraPropertyGridComponent::render(arcRotateCamera);
    }
    return;
  }

  if (entity->type == EntityType::HemisphericLight) {
    auto& hemisphericLight = _entityCache.hemisphericLight;
    if (!hemisphericLight || hemisphericLight->uniqueId != entityId) {
      hemisphericLight = std::dynamic_pointer_cast<HemisphericLight>(
        props.scene->getLightByUniqueID(entityId));
    }
    if (hemisphericLight) {
      HemisphericLightPropertyGridComponent::render(hemisphericLight);
    }
    return;
  }

  if (entity->type == EntityType::PointLight) {
    auto& pointLight = _entityCache.pointLight;
    if (!pointLight || pointLight->uniqueId != entityId) {
      pointLight = std::dynamic_pointer_cast<PointLight>(
        props.scene->getLightByUniqueID(entityId));
    }
    if (pointLight) {
      PointLightPropertyGridComponent::render(pointLight);
    }
    return;
  }

  if (entity->type == EntityType::DirectionalLight) {
    auto& directionalLight = _entityCache.directionalLight;
    if (!directionalLight || directionalLight->uniqueId != entityId) {
      directionalLight = std::dynamic_pointer_cast<DirectionalLight>(
        props.scene->getLightByUniqueID(entityId));
    }
    if (directionalLight) {
      DirectionalLightPropertyGridComponent::render(directionalLight);
    }
    return;
  }

  if (entity->type == EntityType::SpotLight) {
    auto& spotLight = _entityCache.spotLight;
    if (!spotLight || spotLight->uniqueId != entityId) {
      spotLight = std::dynamic_pointer_cast<SpotLight>(
        props.scene->getLightByUniqueID(entityId));
    }
    if (spotLight) {
      SpotLightPropertyGridComponent::render(spotLight);
    }
    return;
  }

  if (entity->type == EntityType::TransformNode) {
    auto& transformNode = _entityCache.transformNode;
    if (!transformNode || transformNode->uniqueId != entityId) {
      transformNode = std::dynamic_pointer_cast<TransformNode>(
        props.scene->getTransformNodeByUniqueID(entityId));
    }
    if (transformNode) {
      TransformNodePropertyGridComponent::render(transformNode);
    }
    return;
  }

  if (entity->type == EntityType::StandardMaterial) {
    auto& standardMaterial = _entityCache.standardMaterial;
    if (!standardMaterial || standardMaterial->uniqueId != entityId) {
      standardMaterial = std::dynamic_pointer_cast<StandardMaterial>(
        props.scene->getMaterialByUniqueID(entityId));
    }
    if (standardMaterial) {
      StandardMaterialPropertyGridComponent::render(standardMaterial);
    }
    return;
  }

  if (entity->type == EntityType::PBRMaterial) {
    auto& pbrMaterial = _entityCache.pbrMaterial;
    if (!pbrMaterial || pbrMaterial->uniqueId != entityId) {
      pbrMaterial = std::dynamic_pointer_cast<PBRMaterial>(
        props.scene->getMaterialByUniqueID(entityId));
    }
    if (pbrMaterial) {
      PBRMaterialPropertyGridComponent::render(pbrMaterial);
    }
    return;
  }

  if (entity->type == EntityType::PBRMetallicRoughnessMaterial) {
    auto& pbrMetallicRoughnessMaterial
      = _entityCache.pbrMetallicRoughnessMaterial;
    if (!pbrMetallicRoughnessMaterial
        || pbrMetallicRoughnessMaterial->uniqueId != entityId) {
      pbrMetallicRoughnessMaterial
        = std::dynamic_pointer_cast<PBRMetallicRoughnessMaterial>(
          props.scene->getMaterialByUniqueID(entityId));
    }
    if (pbrMetallicRoughnessMaterial) {
      PBRMetallicRoughnessMaterialPropertyGridComponent::render(
        pbrMetallicRoughnessMaterial);
    }
    return;
  }

  if (entity->type == EntityType::PBRSpecularGlossinessMaterial) {
    auto& pbrSpecularGlossinessMaterial
      = _entityCache.pbrSpecularGlossinessMaterial;
    if (!pbrSpecularGlossinessMaterial
        || pbrSpecularGlossinessMaterial->uniqueId != entityId) {
      pbrSpecularGlossinessMaterial
        = std::dynamic_pointer_cast<PBRSpecularGlossinessMaterial>(
          props.scene->getMaterialByUniqueID(entityId));
    }
    if (pbrSpecularGlossinessMaterial) {
      PBRSpecularGlossinessMaterialPropertyGridComponent::render(
        pbrSpecularGlossinessMaterial);
    }
    return;
  }

  if (entity->type == EntityType::BackgroundMaterial) {
    auto& backgroundMaterial = _entityCache.backgroundMaterial;
    if (!backgroundMaterial || backgroundMaterial->uniqueId != entityId) {
      backgroundMaterial = std::dynamic_pointer_cast<BackgroundMaterial>(
        props.scene->getMaterialByUniqueID(entityId));
    }
    if (backgroundMaterial) {
      BackgroundMaterialPropertyGridComponent::render(backgroundMaterial);
    }
    return;
  }

  if (entity->type == EntityType::Material) {
    auto& material = _entityCache.material;
    if (!material || material->uniqueId != entityId) {
      material = std::dynamic_pointer_cast<Material>(
        props.scene->getMaterialByUniqueID(entityId));
    }
    if (material) {
      MaterialPropertyGridComponent::render(material);
    }
    return;
  }

  if (entity->type == EntityType::Texture) {
    auto& texture = _entityCache.texture;
    if (!texture || texture->name != entityName) {
      auto it = std::find_if(props.scene->textures.begin(),
                             props.scene->textures.end(),
                             [&entityName](const BaseTexturePtr& texture) {
                               return texture->name == entityName;
                             });
      texture = (it == props.scene->textures.end()) ?
                  nullptr :
                  std::static_pointer_cast<Texture>(*it);
      _reservedDataStore.texture[texture->name] = TextureReservedDataStore{};
    }
    if (texture) {
      TexturePropertyGridComponent::render(
        texture, _reservedDataStore.texture[texture->name]);
    }
    return;
  }

  if (entity->type == EntityType::Bone) {
    auto& bone = _entityCache.bone;
    if (!bone || bone->uniqueId != entityId) {
      bone = std::dynamic_pointer_cast<Bone>(
        props.scene->getBoneByUniqueID(entityId));
    }
    if (bone) {
      BonePropertyGridComponent::render(bone);
    }
    return;
  }
}

} // end of namespace BABYLON
