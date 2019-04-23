#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H

#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/pane_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/mesh_reserved_data_store.h>

namespace BABYLON {

class ArcRotateCamera;
class BackgroundMaterial;
class Bone;
class DirectionalLight;
class FreeCamera;
class HemisphericLight;
class Material;
class Mesh;
class PBRMaterial;
class PBRMetallicRoughnessMaterial;
class PBRSpecularGlossinessMaterial;
class PointLight;
class ScenePropertyGridComponent;
class SpotLight;
class StandardMaterial;
class TransformNode;
using ArcRotateCameraPtr    = std::shared_ptr<ArcRotateCamera>;
using BackgroundMaterialPtr = std::shared_ptr<BackgroundMaterial>;
using BonePtr               = std::shared_ptr<Bone>;
using DirectionalLightPtr   = std::shared_ptr<DirectionalLight>;
using FreeCameraPtr         = std::shared_ptr<FreeCamera>;
using HemisphericLightPtr   = std::shared_ptr<HemisphericLight>;
using MaterialPtr           = std::shared_ptr<Material>;
using MeshPtr               = std::shared_ptr<Mesh>;
using PBRMaterialPtr        = std::shared_ptr<PBRMaterial>;
using PBRMetallicRoughnessMaterialPtr
  = std::shared_ptr<PBRMetallicRoughnessMaterial>;
using PBRSpecularGlossinessMaterialPtr
  = std::shared_ptr<PBRSpecularGlossinessMaterial>;
using PointLightPtr       = std::shared_ptr<PointLight>;
using SpotLightPtr        = std::shared_ptr<SpotLight>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;
using TransformNodePtr    = std::shared_ptr<TransformNode>;

struct EntityCache {
  // Bones
  BonePtr bone = nullptr;
  // Cameras
  ArcRotateCameraPtr arcRotateCamera = nullptr;
  FreeCameraPtr freeCamera           = nullptr;
  // Lights
  DirectionalLightPtr directionalLight = nullptr;
  HemisphericLightPtr hemisphericLight = nullptr;
  PointLightPtr pointLight             = nullptr;
  SpotLightPtr spotLight               = nullptr;
  // Materials
  BackgroundMaterialPtr backgroundMaterial                       = nullptr;
  MaterialPtr material                                           = nullptr;
  PBRMaterialPtr pbrMaterial                                     = nullptr;
  PBRMetallicRoughnessMaterialPtr pbrMetallicRoughnessMaterial   = nullptr;
  PBRSpecularGlossinessMaterialPtr pbrSpecularGlossinessMaterial = nullptr;
  StandardMaterialPtr standardMaterial                           = nullptr;
  // Meshes
  MeshPtr mesh                   = nullptr;
  TransformNodePtr transformNode = nullptr;
}; // end of struct EntityCache

struct ReservedDataStore {
  std::unordered_map<size_t, MeshReservedDataStore> mesh;
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

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRID_TAB_COMPONENT_H
