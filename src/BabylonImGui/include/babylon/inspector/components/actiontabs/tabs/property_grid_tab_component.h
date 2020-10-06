#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H

#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/pane_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/animation_reserved_data_store.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/texture_reserved_data_store.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/mesh_reserved_data_store.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/skeleton_reserved_data_store.h>

namespace BABYLON {

#define FWD_CLASS_SPTR(className)                                                                  \
  class className;                                                                                 \
  using className##Ptr = std::shared_ptr<className>;

FWD_CLASS_SPTR(ArcRotateCamera)
FWD_CLASS_SPTR(ArcRotateCamera)
FWD_CLASS_SPTR(BackgroundMaterial)
FWD_CLASS_SPTR(BaseTexture)
FWD_CLASS_SPTR(Bone)
FWD_CLASS_SPTR(DirectionalLight)
FWD_CLASS_SPTR(FollowCamera)
FWD_CLASS_SPTR(FreeCamera)
FWD_CLASS_SPTR(HemisphericLight)
FWD_CLASS_SPTR(Material)
FWD_CLASS_SPTR(Mesh)
FWD_CLASS_SPTR(MultiMaterial)
FWD_CLASS_SPTR(PBRMaterial)
FWD_CLASS_SPTR(PBRMetallicRoughnessMaterial)
FWD_CLASS_SPTR(PBRSpecularGlossinessMaterial)
FWD_CLASS_SPTR(PointLight)
FWD_CLASS_SPTR(ScenePropertyGridComponent)
FWD_CLASS_SPTR(Skeleton)
FWD_CLASS_SPTR(SpotLight)
FWD_CLASS_SPTR(StandardMaterial)
FWD_CLASS_SPTR(TransformNode)

struct EntityCache {
  // Bones
  BonePtr bone         = nullptr;
  SkeletonPtr skeleton = nullptr;
  // Cameras
  ArcRotateCameraPtr arcRotateCamera = nullptr;
  FollowCameraPtr followCamera       = nullptr;
  FreeCameraPtr freeCamera           = nullptr;
  // Lights
  DirectionalLightPtr directionalLight = nullptr;
  HemisphericLightPtr hemisphericLight = nullptr;
  PointLightPtr pointLight             = nullptr;
  SpotLightPtr spotLight               = nullptr;
  // Materials
  BackgroundMaterialPtr backgroundMaterial                       = nullptr;
  MaterialPtr material                                           = nullptr;
  MultiMaterialPtr multiMaterial                                 = nullptr;
  PBRMaterialPtr pbrMaterial                                     = nullptr;
  PBRMetallicRoughnessMaterialPtr pbrMetallicRoughnessMaterial   = nullptr;
  PBRSpecularGlossinessMaterialPtr pbrSpecularGlossinessMaterial = nullptr;
  StandardMaterialPtr standardMaterial                           = nullptr;
  // Meshes
  MeshPtr mesh                   = nullptr;
  TransformNodePtr transformNode = nullptr;
  // Texture
  BaseTexturePtr texture = nullptr;
}; // end of struct EntityCache

struct ReservedDataStore {
  // Animations
  std::unordered_map<size_t, AnimationReservedDataStore> animation;
  // Bones
  std::unordered_map<size_t, SkeletonReservedDataStore> skeleton;
  // Meshes
  std::unordered_map<size_t, MeshReservedDataStore> mesh;
  // Texture
  std::unordered_map<std::string, TextureReservedDataStore> texture;
};

class BABYLON_SHARED_EXPORT PropertyGridTabComponent : public PaneComponent {

public:
  PropertyGridTabComponent(const IPaneComponentProps& props);
  ~PropertyGridTabComponent() override;

  void componentWillMount();
  void componentWillUnmount();
  void render() override;

private:
  std::unique_ptr<ScenePropertyGridComponent> _scenePropertyGridComponent;
  EntityCache _entityCache;
  ReservedDataStore _reservedDataStore;

}; // end of class StatisticsTabComponent

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H
