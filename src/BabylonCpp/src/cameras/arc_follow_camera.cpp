#include <babylon/cameras/arc_follow_camera.h>

#include <nlohmann/json.hpp>

#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

bool ArcFollowCamera::NodeConstructorAdded = false;

void ArcFollowCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor("ArcFollowCamera", [](const std::string& iName, Scene* scene,
                                                 const std::optional<json>& /*options*/) {
    return ArcFollowCamera::New(iName, 0.f, 0.f, 1.f, nullptr, scene);
  });
  ArcFollowCamera::NodeConstructorAdded = true;
}

ArcFollowCamera::ArcFollowCamera(const std::string& iName, float iAlpha, float iBeta, float iRadius,
                                 AbstractMesh* iTarget, Scene* scene)
    : TargetCamera{iName, Vector3::Zero(), scene}
    , alpha{iAlpha}
    , beta{iBeta}
    , radius{iRadius}
    , _cartesianCoordinates{Vector3::Zero()}
{
  setMeshTarget(iTarget);
}

ArcFollowCamera::~ArcFollowCamera() = default;

Type ArcFollowCamera::type() const
{
  return Type::ARCFOLLOWCAMERA;
}

void ArcFollowCamera::setMeshTarget(AbstractMesh* iTarget)
{
  _meshTarget = iTarget;
  _follow();
}

void ArcFollowCamera::_follow()
{
  if (!_meshTarget) {
    return;
  }

  _cartesianCoordinates.x = radius * std::cos(alpha) * std::cos(beta);
  _cartesianCoordinates.y = radius * std::sin(beta);
  _cartesianCoordinates.z = radius * std::sin(alpha) * std::cos(beta);

  auto targetPosition = _meshTarget->getAbsolutePosition();
  position            = targetPosition.add(_cartesianCoordinates);
  setTarget(targetPosition);
}

void ArcFollowCamera::_checkInputs()
{
  TargetCamera::_checkInputs();
  _follow();
}

std::string ArcFollowCamera::getClassName() const
{
  return "ArcFollowCamera";
}

} // end of namespace BABYLON
