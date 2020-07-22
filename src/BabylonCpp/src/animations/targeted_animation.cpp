#include <babylon/animations/targeted_animation.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

std::string TargetedAnimation::getClassName() const
{
  return "TargetedAnimation";
}

json TargetedAnimation::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
