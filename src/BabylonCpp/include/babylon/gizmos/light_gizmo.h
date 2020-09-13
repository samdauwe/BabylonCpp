#ifndef BABYLON_GIZMOS_LIGHT_GIZMO_H
#define BABYLON_GIZMOS_LIGHT_GIZMO_H

#include <babylon/babylon_api.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

class Light;
class StandardMaterial;
class TransformNode;
using LightPtr            = std::shared_ptr<Light>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;
using TransformNodePtr    = std::shared_ptr<TransformNode>;

/**
 * @brief Gizmo that enables viewing a light.
 */
class BABYLON_SHARED_EXPORT LightGizmo : public Gizmo {

public:
  /**
   * @brief Creates a LightGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  LightGizmo(const UtilityLayerRendererPtr& gizmoLayer
             = UtilityLayerRenderer::DefaultUtilityLayer());
  ~LightGizmo() override; // = default

  /**
   * @brief Disposes of the light gizmo.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Updates the gizmo to match the attached mesh's position/rotation.
   * Hidden
   */
  void _update();

private:
  // Static helper methods
  /**
   * Creates the lines for a light mesh
   */
  static MeshPtr _CreateLightLines(float levels, Scene* scene);

  static MeshPtr _CreateHemisphericLightMesh(Scene* scene);
  static MeshPtr _CreatePointLightMesh(Scene* scene);
  static MeshPtr _CreateSpotLightMesh(Scene* scene);
  static MeshPtr _CreateDirectionalLightMesh(Scene* scene);

protected:
  /**
   * @brief Gets the light that the gizmo is attached to.
   */
  LightPtr& get_light();

  /**
   * @brief Sets the light that the gizmo is attached to.
   */
  void set_light(const LightPtr& light);

  /**
   * @brief Gets the material used to render the light gizmo.
   */
  StandardMaterialPtr& get_material();

public:
  /**
   * Event that fires each time the gizmo is clicked
   */
  Observable<Light> onClickedObservable;

  /**
   * The light that the gizmo is attached to
   */
  Property<LightGizmo, LightPtr> light;

  /**
   * Gets the material used to render the light gizmo
   */
  ReadOnlyProperty<LightGizmo, StandardMaterialPtr> material;

private:
  static float _Scale;
  MeshPtr _lightMesh;
  StandardMaterialPtr _material;
  Vector3 _cachedPosition;
  Vector3 _cachedForward;
  TransformNodePtr _attachedMeshParent;
  Observer<PointerInfo>::Ptr _pointerObserver;
  LightPtr _light;

}; // end of class LightGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_LIGHT_GIZMO_H
