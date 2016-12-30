#ifndef BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H
#define BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IPhysicsEnabledObject {
  //virtual Vector3 position(){return nullptr;};
  //virtual Quaternion* rotationQuaternion()                   = 0;
  //virtual void setRotationQuaternion(Quaternion* quaternion) = 0;
  //virtual Vector3* scaling()                                 = 0;
  //virtual Vector3* rotation()                                = 0;
  virtual AbstractMesh* getParent()                          = 0;
  virtual BoundingInfo* getBoundingInfo()                    = 0;
  virtual Matrix computeWorldMatrix(bool force = false) = 0;
  virtual std::vector<AbstractMesh*>
  getChildMeshes(bool directDecendantsOnly,
                 const std::function<bool(Node* node)>& predicate)
    = 0;
  virtual Float32Array getVerticesData(unsigned int kind,
                                       bool copyWhenShared = false)
    = 0;
  virtual Uint32Array getIndices(bool copyWhenShared = false) = 0;
  virtual Scene* getScene()                                   = 0;
}; // end of struct IPhysicsEnabledObject

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_ENABLED_OBJECT_H
