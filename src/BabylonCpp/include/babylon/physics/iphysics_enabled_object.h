#ifndef BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H
#define BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

/**
 * @brief Interface for a physics-enabled object.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT IPhysicsEnabledObject : public AbstractMesh {
  AbstractMesh* getParent() override        = 0;
  Scene* getScene() const override          = 0;
  virtual bool hasBoundingInfo()            = 0;
  std::string getClassName() const override = 0;
}; // end of struct IPhysicsEnabledObject

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H
