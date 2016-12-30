#include <babylon/cameras/arc_follow_camera.h>

#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

ArcFollowCamera::ArcFollowCamera(const std::string& iName, float iAlpha,
                                 float iBeta, float iRadius,
                                 AbstractMesh* iTarget, Scene* scene)
    : TargetCamera(iName, Vector3::Zero(), scene)
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

Node::Type ArcFollowCamera::type() const
{
  return Node::Type::ARCFOLLOWCAMERA;
}

void ArcFollowCamera::follow()
{
  _cartesianCoordinates.x = radius * std::cos(alpha) * std::cos(beta);
  _cartesianCoordinates.y = radius * std::sin(beta);
  _cartesianCoordinates.z = radius * std::sin(alpha) * std::cos(beta);

  position = target->position.add(_cartesianCoordinates);
  setTarget(target->position);
}

void ArcFollowCamera::_checkInputs()
{
  TargetCamera::_checkInputs();
  follow();
}

std::string ArcFollowCamera::getTypeName() const
{
  return "ArcFollowCamera";
}

} // end of namespace BABYLON
