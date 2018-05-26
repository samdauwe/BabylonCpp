#ifndef BABYLON_GIZMOS_GIZMO_H
#define BABYLON_GIZMOS_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief Renders gizmos on top of an existing scene which provide controls for
 * position, rotation, etc.
 */
class BABYLON_SHARED_EXPORT Gizmo : public IDisposable {

public:
  /**
   * @brief Creates a gizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   */
  Gizmo(UtilityLayerRenderer* gizmoLayer);
  virtual ~Gizmo();

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

public:
  /**
   * Mesh that the gizmo will be attached to. (eg. on a drag gizmo the mesh that
   * will be dragged)
   */
  Mesh* attachedMesh;

  /**
   * The utility layer the gizmo will be added to
   */
  UtilityLayerRenderer* gizmoLayer;

protected:
  /**
   * The root mesh of the gizmo
   */
  Mesh* _rootMesh;

private:
  Observer<Scene>::Ptr _beforeRenderObserver;

}; // end of class Gizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_GIZMO_H
