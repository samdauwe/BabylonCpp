#include <babylon/cameras/stereoscopic/anaglyph_gamepad_camera.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

bool AnaglyphGamepadCamera::NodeConstructorAdded = false;

void AnaglyphGamepadCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "AnaglyphGamepadCamera", [](const std::string& iName, Scene* scene,
                                const std::optional<json>& options) {
      float interaxialDistance = 0.f;
      if (options) {
        interaxialDistance
          = json_util::get_number<float>(*options, "interaxial_distance");
      }
      return AnaglyphGamepadCamera::New(iName, Vector3::Zero(),
                                        interaxialDistance, scene);
    });
  AnaglyphGamepadCamera::NodeConstructorAdded = true;
}

AnaglyphGamepadCamera::AnaglyphGamepadCamera(const std::string& iName,
                                             const Vector3& iPosition,
                                             float iInteraxialDistance,
                                             Scene* scene)
    : GamepadCamera{iName, iPosition, scene}
{
  interaxialDistance = iInteraxialDistance;
  RigParamaters rigParams;
  rigParams.interaxialDistance = interaxialDistance;
  setCameraRigMode(Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH, rigParams);
}

AnaglyphGamepadCamera::~AnaglyphGamepadCamera() = default;

const std::string AnaglyphGamepadCamera::getClassName() const
{
  return "AnaglyphGamepadCamera";
}

} // end of namespace BABYLON
