#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct TreeItemSpecializedComponent;
using TreeItemSpecializedComponentPtr
  = std::shared_ptr<TreeItemSpecializedComponent>;

enum class EntityType {
  // Default option
  Unknown = 0,
  // Animations
  AnimationGroup,
  // Bones
  Bone,
  Skeleton,
  // Cameras
  ArcRotateCamera,
  FreeCamera,
  UniversalCamera,
  // Engine
  Scene,
  // Lights
  DirectionalLight,
  HemisphericLight,
  PointLight,
  SpotLight,
  // Materials
  BackgroundMaterial,
  Image,
  Material,
  PBRMaterial,
  PBRMetallicRoughnessMaterial,
  PBRSpecularGlossinessMaterial,
  StandardMaterial,
  // Meshes
  Mesh,
  TransformNode,
  // Post process
  PostProcess,
  // Rendering pipelines
  DefaultRenderingPipeline,
  RenderingPipeline,
  LensRenderingPipeline,
  SSAORenderingPipeline,
  SSAO2RenderingPipeline,
  // Texture
  Texture,
}; // end of enum class EntityType

struct BABYLON_SHARED_EXPORT TreeItemComponent {
  char label[128]                           = {"no label"};
  char key[128]                             = {"no key"};
  float offset                              = 0.f;
  bool isExpanded                           = false;
  bool mustExpand                           = false;
  bool isSelected                           = false;
  TreeItemSpecializedComponentPtr component = nullptr;

}; // end of struct TreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_COMPONENT_H
