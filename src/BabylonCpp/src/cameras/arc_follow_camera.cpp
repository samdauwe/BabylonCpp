#include <babylon/cameras/arc_follow_camera.h>

#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

ArcFollowCamera::ArcFollowCamera(const string_t& iName, float iAlpha,
                                 float iBeta, float iRadius,
                                 AbstractMesh* iTarget, Scene* scene)
    : TargetCamera{iName, Vector3::Zero(), scene}
    , alpha{iAlpha}
    , beta{iBeta}
    , radius{iRadius}
    , target{iTarget}
    , _cartesianCoordinates{Vector3::Zero()}
{
  follow();
}

ArcFollowCamera::~ArcFollowCamera()
{
}

IReflect::Type ArcFollowCamera::type() const
{
  return IReflect::Type::ARCFOLLOWCAMERA;
}

void ArcFollowCamera::follow()
{
  if (!target) {
    return;
  }

  _cartesianCoordinates.x = radius * ::std::cos(alpha) * ::std::cos(beta);
  _cartesianCoordinates.y = radius * ::std::sin(beta);
  _cartesianCoordinates.z = radius * ::std::sin(alpha) * ::std::cos(beta);

  auto targetPosition = target->getAbsolutePosition();
  position            = targetPosition.add(_cartesianCoordinates);
  setTarget(targetPosition);
}

void ArcFollowCamera::_checkInputs()
{
  TargetCamera::_checkInputs();
  follow();
}

const string_t ArcFollowCamera::getClassName() const
{
  return "ArcFollowCamera";
}

} // end of namespace BABYLON
