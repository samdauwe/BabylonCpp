#include <babylon/cameras/stereoscopic/stereoscopic_gamepad_camera.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

bool StereoscopicGamepadCamera::NodeConstructorAdded = false;

void StereoscopicGamepadCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "StereoscopicGamepadCamera", [](const std::string& iName, Scene* scene,
                                    const std::optional<json>& options) {
      float interaxialDistance      = 0.f;
      bool isStereoscopicSideBySide = false;
      if (options) {
        interaxialDistance
          = json_util::get_number<float>(*options, "interaxial_distance");
        isStereoscopicSideBySide
          = json_util::get_bool(*options, "isStereoscopicSideBySide");
      }
      return StereoscopicGamepadCamera::New(iName, Vector3::Zero(),
                                            interaxialDistance,
                                            isStereoscopicSideBySide, scene);
    });
  StereoscopicGamepadCamera::NodeConstructorAdded = true;
}

StereoscopicGamepadCamera::StereoscopicGamepadCamera(
  const std::string& name, const Vector3& position, float iInteraxialDistance,
  bool iIsStereoscopicSideBySide, Scene* scene)
    : GamepadCamera{name, position, scene}
{
  interaxialDistance       = iInteraxialDistance;
  isStereoscopicSideBySide = iIsStereoscopicSideBySide;
  json rigParams;
  rigParams["interaxialDistance"] = interaxialDistance;
  setCameraRigMode(isStereoscopicSideBySide ?
                     Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL :
                     Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER,
                   rigParams);
}

StereoscopicGamepadCamera::~StereoscopicGamepadCamera()
{
}

const std::string StereoscopicGamepadCamera::getClassName() const
{
  return "StereoscopicGamepadCamera";
}

} // end of namespace BABYLON
