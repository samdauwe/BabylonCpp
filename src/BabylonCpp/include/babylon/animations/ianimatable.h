#ifndef BABYLON_ANIMATIONS_IANIMATABLE_H
#define BABYLON_ANIMATIONS_IANIMATABLE_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>

namespace BABYLON {

class Animation;
class Node;
using AnimationPtr = std::shared_ptr<Animation>;

class BABYLON_SHARED_EXPORT IAnimatable {

public:
  IAnimatable();
  virtual ~IAnimatable();

  virtual Type type() const = 0;
  virtual std::vector<AnimationPtr> getAnimations();
  virtual bool markTargetAsDirty() const;
  virtual IAnimatable& markAsDirty(const std::string& property = "");
  virtual void markAsDirty(unsigned int /*flag*/);
  virtual Matrix& getWorldMatrix();

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

public:
  /**
   * Parent of the node.
   */
  Property<IAnimatable, Node*> parent;

  /**
   * Gets or sets a string used to store user defined state for the node
   */
  std::string state;

private:
  Node* nullNode;
  Matrix _identityMatrix;

}; // end of struct IAnimatable

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_IANIMATABLE_H
