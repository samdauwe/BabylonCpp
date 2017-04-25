#ifndef BABYLON_ANIMATIONS_IANIMATABLE_H
#define BABYLON_ANIMATIONS_IANIMATABLE_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_global.h>
#include <babylon/interfaces/ireflect.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IAnimatable : public IReflect {
  enum class Type {
    UNKNOWN = 0,
    CAMERA  = 1,
    LIGHT   = 2,
    MESH    = 3,
    SCENE   = 4,
  }; // end of enum class Type

  std::string state;
  virtual Node* parent() const
  {
    return nullptr;
  }
  virtual Type animatableType() const
  {
    return Type::UNKNOWN;
  }
  virtual std::vector<Animation*> getAnimations()
  {
    return std::vector<Animation*>();
  }
  virtual bool markTargetAsDirty() const
  {
    return false;
  }
  virtual void markAsDirty(unsigned int /*flag*/)
  {
  }
  virtual Matrix* getWorldMatrix()
  {
    return nullptr;
  }
  virtual AnimationValue operator[](const std::string& /*key*/)
  {
    return AnimationValue(true);
  }
}; // end of struct IAnimatable

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_IANIMATABLE_H
