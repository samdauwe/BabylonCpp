#ifndef BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H
#define BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IPhysicsEnabledObject : public AbstractMesh {
  virtual AbstractMesh* getParent()        = 0;
  virtual Scene* getScene()                = 0;
  virtual bool hasBoundingInfo()           = 0;
  virtual const char* getClassName() const = 0;
}; // end of struct IPhysicsEnabledObject

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H
