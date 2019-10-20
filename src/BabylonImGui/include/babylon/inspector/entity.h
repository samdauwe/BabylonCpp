#ifndef BABYLON_INSPECTOR_ENTITY_H
#define BABYLON_INSPECTOR_ENTITY_H

#include <optional>
#include <string>

namespace BABYLON {

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
  MultiMaterial,
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

struct EntityInfo {
  EntityType type                = EntityType::Unknown;
  std::optional<size_t> uniqueId = std::nullopt;
  std::string name               = "";
}; // end of struct SpecializedComponentInfo

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_ENTITY_H
