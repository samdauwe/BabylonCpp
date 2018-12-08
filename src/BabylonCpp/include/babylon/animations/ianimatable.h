#ifndef BABYLON_ANIMATIONS_IANIMATABLE_H
#define BABYLON_ANIMATIONS_IANIMATABLE_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_api.h>
#include <babylon/interfaces/ireflect.h>

namespace BABYLON {

class Animation;
class Node;
using AnimationPtr = std::shared_ptr<Animation>;

class BABYLON_SHARED_EXPORT IAnimatable : public IReflect {

public:
  enum class Type {
    UNKNOWN = 0,
    CAMERA  = 1,
    LIGHT   = 2,
    MESH    = 3,
    SCENE   = 4,
  }; // end of enum class Type

public:
  IAnimatable();
  ~IAnimatable();

  /**
   * Gets or sets a string used to store user defined state for the node
   */
  std::string state;

  virtual Type animatableType() const
  {
    return Type::UNKNOWN;
  }
  virtual std::vector<AnimationPtr> getAnimations()
  {
    return std::vector<AnimationPtr>();
  }
  virtual bool markTargetAsDirty() const
  {
    return false;
  }
  virtual IAnimatable& markAsDirty(const std::string& /*property*/ = "")
  {
    return *this;
  }
  virtual void markAsDirty(unsigned int /*flag*/)
  {
  }
  virtual Matrix& getWorldMatrix()
  {
    return _identityMatrix;
  }
  virtual AnimationValue operator[](const std::string& /*key*/)
  {
    return AnimationValue(true);
  }

protected:
  virtual Node*& get_parent();
  virtual void set_parent(Node* const& parent);

public:
  /**
   * Parent of the node.
   */
  Property<IAnimatable, Node*> parent;

private:
  Node* nullNode;
  Matrix _identityMatrix;

}; // end of struct IAnimatable

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_IANIMATABLE_H
