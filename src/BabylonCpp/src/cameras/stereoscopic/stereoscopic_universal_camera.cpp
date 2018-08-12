#include <babylon/cameras/stereoscopic/stereoscopic_universal_camera.h>

#include <babylon/core/json.h>

namespace BABYLON {

bool StereoscopicUniversalCamera::NodeConstructorAdded = false;

void StereoscopicUniversalCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "StereoscopicUniversalCamera", [](const string_t& name, Scene* scene,
                                      const nullable_t<Json::value>& options) {
      float interaxialDistance      = 0.f;
      bool isStereoscopicSideBySide = false;
      if (options) {
        interaxialDistance
          = Json::GetNumber<float>(*options, "interaxial_distance", 0.f);
        isStereoscopicSideBySide
          = Json::GetBool(*options, "isStereoscopicSideBySide");
      }
      return StereoscopicUniversalCamera::New(name, Vector3::Zero(),
                                              interaxialDistance,
                                              isStereoscopicSideBySide, scene);
    });
  StereoscopicUniversalCamera::NodeConstructorAdded = true;
}

StereoscopicUniversalCamera::StereoscopicUniversalCamera(
  const string_t& name, const Vector3& position, float iInteraxialDistance,
  bool iIsStereoscopicSideBySide, Scene* scene)
    : UniversalCamera{name, position, scene}
{
  interaxialDistance       = iInteraxialDistance;
  isStereoscopicSideBySide = iIsStereoscopicSideBySide;
  Json::object rigParams;
  rigParams["interaxialDistance"]
    = picojson::value(static_cast<double>(interaxialDistance));
  setCameraRigMode(isStereoscopicSideBySide ?
                     Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL() :
                     Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER(),
                   Json::value(rigParams));
}

StereoscopicUniversalCamera::~StereoscopicUniversalCamera()
{
}

const string_t StereoscopicUniversalCamera::getClassName() const
{
  return "StereoscopicUniversalCamera";
}

} // end of namespace BABYLON
