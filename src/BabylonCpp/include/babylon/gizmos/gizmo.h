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

protected:
  /**
   * @brief Gets the mesh that the gizmo will be attached to. (eg. on a drag
   * gizmo the mesh that will be dragged).
   */
  AbstractMesh*& get_attachedMesh();

  /**
   * @brief Sets the mesh that the gizmo will be attached to. (eg. on a drag
   * gizmo the mesh that will be dragged).
   */
  virtual void set_attachedMesh(AbstractMesh* const& value);

  virtual void _attachedMeshChanged(AbstractMesh* value);

public:
  /**
   * Mesh that the gizmo will be attached to. (eg. on a drag gizmo the mesh that
   * will be dragged)
   * * When set, interactions will be enabled
   */
  Property<Gizmo, AbstractMesh*> attachedMesh;

  /**
   * The utility layer the gizmo will be added to
   */
  UtilityLayerRenderer* gizmoLayer;

  /**
   * If set the gizmo's rotation will be updated to match the attached mesh each
   * frame (Default: true)
   */
  bool updateGizmoRotationToMatchAttachedMesh;

  /**
   * If set the gizmo's position will be updated to match the attached mesh each
   * frame (Default: true)
   */
  bool updateGizmoPositionToMatchAttachedMesh;

protected:
  /**
   * The root mesh of the gizmo
   */
  Mesh* _rootMesh;

  /**
   * When set, the gizmo will always appear the same size no matter where the
   * camera is (default: false)
   */
  bool _updateScale;
  bool _interactionsEnabled;

private:
  AbstractMesh* _attachedMesh;
  Observer<Scene>::Ptr _beforeRenderObserver;

}; // end of class Gizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_GIZMO_H
