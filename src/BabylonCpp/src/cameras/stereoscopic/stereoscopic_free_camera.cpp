#include <babylon/cameras/stereoscopic/stereoscopic_free_camera.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

bool StereoscopicFreeCamera::NodeConstructorAdded = false;

void StereoscopicFreeCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "StereoscopicFreeCamera", [](const std::string& iName, Scene* scene,
                                 const std::optional<json>& options) {
      float interaxialDistance      = 0.f;
      bool isStereoscopicSideBySide = false;
      if (options) {
        interaxialDistance
          = json_util::get_number<float>(*options, "interaxial_distance");
        isStereoscopicSideBySide
          = json_util::get_bool(*options, "isStereoscopicSideBySide");
      }
      return StereoscopicFreeCamera::New(iName, Vector3::Zero(),
                                         interaxialDistance,
                                         isStereoscopicSideBySide, scene);
    });
  StereoscopicFreeCamera::NodeConstructorAdded = true;
}

StereoscopicFreeCamera::StereoscopicFreeCamera(const std::string& iName,
                                               const Vector3& iPosition,
                                               float iInteraxialDistance,
                                               bool iIsStereoscopicSideBySide,
                                               Scene* scene)
    : FreeCamera{iName, iPosition, scene}
{
  interaxialDistance       = iInteraxialDistance;
  isStereoscopicSideBySide = iIsStereoscopicSideBySide;
  RigParamaters rigParams;
  rigParams.interaxialDistance = interaxialDistance;
  setCameraRigMode(isStereoscopicSideBySide ?
                     Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL :
                     Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER,
                   rigParams);
}

StereoscopicFreeCamera::~StereoscopicFreeCamera() = default;

std::string StereoscopicFreeCamera::getClassName() const
{
  return "StereoscopicFreeCamera";
}

} // end of namespace BABYLON
