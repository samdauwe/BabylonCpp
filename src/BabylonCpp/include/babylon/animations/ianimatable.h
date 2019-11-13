#ifndef BABYLON_ANIMATIONS_IANIMATABLE_H
#define BABYLON_ANIMATIONS_IANIMATABLE_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>

namespace BABYLON {

class Animation;
struct AnimationPropertiesOverride;
class AnimationRange;
class IAnimatable;
class Node;
using AnimationPtr = std::shared_ptr<Animation>;
using AnimationPropertiesOverridePtr
  = std::shared_ptr<AnimationPropertiesOverride>;
using AnimationRangePtr = std::shared_ptr<AnimationRange>;
using IAnimatablePtr    = std::shared_ptr<IAnimatable>;

class BABYLON_SHARED_EXPORT IAnimatable {

public:
  IAnimatable();
  virtual ~IAnimatable(); // = default

  virtual Type type() const = 0;
  virtual std::vector<AnimationPtr> getAnimations();
  virtual std::vector<IAnimatablePtr> getAnimatables();
  virtual std::vector<AnimationRangePtr> getAnimationRanges();
  virtual bool markTargetAsDirty() const;
  virtual IAnimatable& markAsDirty(const std::string& property = "");
  virtual void markAsDirty(unsigned int /*flag*/);
  virtual Matrix& getWorldMatrix();
  virtual std::optional<Matrix>& getRestPose();

  virtual AnimationValue
  getProperty(const std::vector<std::string>& targetPropertyPath);
  virtual void setProperty(const std::vector<std::string>& targetPropertyPath,
                           const AnimationValue& value);

  static AnimationValue getProperty(const std::string& key,
                                    const Color3& color);
  static AnimationValue getProperty(const std::string& key,
                                    const Color4& color);
  static AnimationValue getProperty(const std::string& key,
                                    const Vector2& vector);
  static AnimationValue getProperty(const std::string& key,
                                    const Vector3& vector);
  static AnimationValue getProperty(const std::string& key,
                                    const Quaternion& quaternion);

  static void setProperty(const std::string& key, Color3& color, float value);
  static void setProperty(const std::string& key, Color4& color, float value);
  static void setProperty(const std::string& key, Vector2& vector, float value);
  static void setProperty(const std::string& key, Vector3& vector, float value);
  static void setProperty(const std::string& key, Quaternion& quaternion,
                          float value);

protected:
  virtual Node*& get_parent();
  virtual void set_parent(Node* const& parent);
  virtual AnimationPropertiesOverridePtr& get_animationPropertiesOverride();
  virtual void
  set_animationPropertiesOverride(const AnimationPropertiesOverridePtr& value);

public:
  /**
   * Parent of the node.
   */
  Property<IAnimatable, Node*> parent;

  /**
   * Gets the animation properties override
   */
  Property<IAnimatable, AnimationPropertiesOverridePtr>
    animationPropertiesOverride;

  /**
   * Gets or sets a string used to store user defined state for the node
   */
  std::string state;

private:
  Node* nullNode;
  Matrix _identityMatrix;
  std::optional<Matrix> _nullMatrix;
  AnimationPropertiesOverridePtr _nullAnimationPropertiesOverride;

}; // end of struct IAnimatable

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_IANIMATABLE_H
