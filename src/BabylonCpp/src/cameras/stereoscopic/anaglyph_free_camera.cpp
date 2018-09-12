#include <babylon/cameras/stereoscopic/anaglyph_free_camera.h>

#include <babylon/core/json.h>

namespace BABYLON {

bool AnaglyphFreeCamera::NodeConstructorAdded = false;

void AnaglyphFreeCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "AnaglyphFreeCamera", [](const std::string& name, Scene* scene,
                             const std::optional<Json::value>& options) {
      float interaxialDistance = 0.f;
      if (options) {
        interaxialDistance
          = Json::GetNumber<float>(*options, "interaxial_distance", 0.f);
      }
      return AnaglyphFreeCamera::New(name, Vector3::Zero(), interaxialDistance,
                                     scene);
    });
  AnaglyphFreeCamera::NodeConstructorAdded = true;
}

AnaglyphFreeCamera::AnaglyphFreeCamera(const std::string& name,
                                       const Vector3& position,
                                       float iInteraxialDistance, Scene* scene)
    : FreeCamera{name, position, scene}
{
  interaxialDistance = iInteraxialDistance;
  Json::object rigParams;
  rigParams["interaxialDistance"]
    = picojson::value(static_cast<double>(interaxialDistance));
  setCameraRigMode(Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH(),
                   Json::value(rigParams));
}

AnaglyphFreeCamera::~AnaglyphFreeCamera()
{
}

const std::string AnaglyphFreeCamera::getClassName() const
{
  return "AnaglyphFreeCamera";
}

} // end of namespace BABYLON
