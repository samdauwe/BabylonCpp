#include <babylon/cameras/stereoscopic/anaglyph_gamepad_camera.h>

#include <babylon/core/json.h>

namespace BABYLON {

bool AnaglyphGamepadCamera::NodeConstructorAdded = false;

void AnaglyphGamepadCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "AnaglyphGamepadCamera", [](const std::string& name, Scene* scene,
                                const std::optional<Json::value>& options) {
      float interaxialDistance = 0.f;
      if (options) {
        interaxialDistance
          = Json::GetNumber<float>(*options, "interaxial_distance", 0.f);
      }
      return AnaglyphGamepadCamera::New(name, Vector3::Zero(),
                                        interaxialDistance, scene);
    });
  AnaglyphGamepadCamera::NodeConstructorAdded = true;
}

AnaglyphGamepadCamera::AnaglyphGamepadCamera(const std::string& name,
                                             const Vector3& position,
                                             float iInteraxialDistance,
                                             Scene* scene)
    : GamepadCamera{name, position, scene}
{
  interaxialDistance = iInteraxialDistance;
  Json::object rigParams;
  rigParams["interaxialDistance"]
    = picojson::value(static_cast<double>(interaxialDistance));
  setCameraRigMode(Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH(),
                   Json::value(rigParams));
}

AnaglyphGamepadCamera::~AnaglyphGamepadCamera()
{
}

const std::string AnaglyphGamepadCamera::getClassName() const
{
  return "AnaglyphGamepadCamera";
}

} // end of namespace BABYLON
