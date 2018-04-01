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

  /**
   * Gets or sets a string used to store user defined state for the node
   */
  string_t state;

  virtual Node* parent() const
  {
    return nullptr;
  }
  virtual Type animatableType() const
  {
    return Type::UNKNOWN;
  }
  virtual vector_t<Animation*> getAnimations()
  {
    return vector_t<Animation*>();
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
  virtual AnimationValue operator[](const string_t& /*key*/)
  {
    return AnimationValue(true);
  }
}; // end of struct IAnimatable

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_IANIMATABLE_H
