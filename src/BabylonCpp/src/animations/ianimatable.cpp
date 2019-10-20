#include <babylon/animations/ianimatable.h>

namespace BABYLON {

IAnimatable::IAnimatable()
    : parent{this, &IAnimatable::get_parent, &IAnimatable::set_parent}
    , animationPropertiesOverride{this,
                                  &IAnimatable::get_animationPropertiesOverride,
                                  &IAnimatable::set_animationPropertiesOverride}
    , nullNode{nullptr}
    , _identityMatrix{Matrix::Identity()}
    , _nullMatrix{std::nullopt}
    , _nullAnimationPropertiesOverride{nullptr}
{
}

IAnimatable::~IAnimatable()
{
}

Node*& IAnimatable::get_parent()
{
  return nullNode;
}

void IAnimatable::set_parent(Node* const& /*parent*/)
{
}

AnimationPropertiesOverridePtr& IAnimatable::get_animationPropertiesOverride()
{
  return _nullAnimationPropertiesOverride;
}

void IAnimatable::set_animationPropertiesOverride(
  const AnimationPropertiesOverridePtr& /*value*/)
{
}

std::vector<AnimationPtr> IAnimatable::getAnimations()
{
  return {};
}

std::vector<IAnimatablePtr> IAnimatable::getAnimatables()
{
  return {};
}

std::vector<AnimationRangePtr> IAnimatable::getAnimationRanges()
{
  return {};
}

bool IAnimatable::markTargetAsDirty() const
{
  return false;
}

IAnimatable& IAnimatable::markAsDirty(const std::string& /*property*/)
{
  return *this;
}

void IAnimatable::markAsDirty(unsigned int /*flag*/)
{
}

Matrix& IAnimatable::getWorldMatrix()
{
  return _identityMatrix;
}

std::optional<Matrix>& IAnimatable::getRestPose()
{
  return _nullMatrix;
}

AnimationValue
IAnimatable::getProperty(const std::vector<std::string>& /*targetPropertyPath*/)
{
  return AnimationValue();
}

void IAnimatable::setProperty(
  const std::vector<std::string>& /*targetPropertyPath*/,
  const AnimationValue& /*value*/)
{
}

AnimationValue IAnimatable::getProperty(const std::string& key,
                                        const Color3& color)
{
  if (key == "r") {
    return AnimationValue(color.r);
  }
  else if (key == "g") {
    return AnimationValue(color.g);
  }
  else if (key == "b") {
    return AnimationValue(color.b);
  }

  return AnimationValue();
}

AnimationValue IAnimatable::getProperty(const std::string& key,
                                        const Color4& color)
{
  if (key == "r") {
    return AnimationValue(color.r);
  }
  else if (key == "g") {
    return AnimationValue(color.g);
  }
  else if (key == "b") {
    return AnimationValue(color.b);
  }
  else if (key == "a") {
    return AnimationValue(color.a);
  }

  return AnimationValue();
}

AnimationValue IAnimatable::getProperty(const std::string& key,
                                        const Vector2& vector)
{
  if (key == "x") {
    return AnimationValue(vector.x);
  }
  else if (key == "y") {
    return AnimationValue(vector.y);
  }

  return AnimationValue();
}

AnimationValue IAnimatable::getProperty(const std::string& key,
                                        const Vector3& vector)
{
  if (key == "x") {
    return AnimationValue(vector.x);
  }
  else if (key == "y") {
    return AnimationValue(vector.y);
  }
  else if (key == "z") {
    return AnimationValue(vector.z);
  }

  return AnimationValue();
}

AnimationValue IAnimatable::getProperty(const std::string& key,
                                        const Quaternion& quaternion)
{
  if (key == "x") {
    return AnimationValue(quaternion.x);
  }
  else if (key == "y") {
    return AnimationValue(quaternion.y);
  }
  else if (key == "z") {
    return AnimationValue(quaternion.z);
  }
  else if (key == "w") {
    return AnimationValue(quaternion.w);
  }

  return AnimationValue();
}

void IAnimatable::setProperty(const std::string& key, Color3& color,
                              float value)
{
  if (key == "r") {
    color.r = value;
  }
  else if (key == "g") {
    color.g = value;
  }
  else if (key == "b") {
    color.b = value;
  }
}

void IAnimatable::setProperty(const std::string& key, Color4& color,
                              float value)
{
  if (key == "r") {
    color.r = value;
  }
  else if (key == "g") {
    color.g = value;
  }
  else if (key == "b") {
    color.b = value;
  }
  else if (key == "a") {
    color.a = value;
  }
}

void IAnimatable::setProperty(const std::string& key, Vector2& vector,
                              float value)
{
  if (key == "x") {
    vector.x = value;
  }
  else if (key == "y") {
    vector.y = value;
  }
}

void IAnimatable::setProperty(const std::string& key, Vector3& vector,
                              float value)
{
  if (key == "x") {
    vector.x = value;
  }
  else if (key == "y") {
    vector.y = value;
  }
  else if (key == "z") {
    vector.z = value;
  }
}

void IAnimatable::setProperty(const std::string& key, Quaternion& quaternion,
                              float value)
{
  if (key == "x") {
    quaternion.x = value;
  }
  else if (key == "y") {
    quaternion.y = value;
  }
  else if (key == "z") {
    quaternion.z = value;
  }
  else if (key == "w") {
    quaternion.w = value;
  }
}

} // end of namespace BABYLON
