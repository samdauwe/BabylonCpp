#include <babylon/cameras/stereoscopic/stereoscopic_free_camera.h>

#include <babylon/core/json.h>

namespace BABYLON {

bool StereoscopicFreeCamera::NodeConstructorAdded = false;

::std::function<void()> StereoscopicFreeCamera::AddNodeConstructor = []() {
  Node::AddNodeConstructor(
    "StereoscopicFreeCamera", [](const string_t& name, Scene* scene,
                                 const nullable_t<Json::value>& options) {
      float interaxialDistance      = 0.f;
      bool isStereoscopicSideBySide = false;
      if (options) {
        interaxialDistance
          = Json::GetNumber<float>(*options, "interaxial_distance", 0.f);
        isStereoscopicSideBySide
          = Json::GetBool(*options, "isStereoscopicSideBySide");
      }
      return StereoscopicFreeCamera::New(name, Vector3::Zero(),
                                         interaxialDistance,
                                         isStereoscopicSideBySide, scene);
    });
  StereoscopicFreeCamera::NodeConstructorAdded = true;
};

StereoscopicFreeCamera::StereoscopicFreeCamera(const string_t& name,
                                               const Vector3& position,
                                               float iInteraxialDistance,
                                               bool iIsStereoscopicSideBySide,
                                               Scene* scene)
    : FreeCamera{name, position, scene}
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

StereoscopicFreeCamera::~StereoscopicFreeCamera()
{
}

const string_t StereoscopicFreeCamera::getClassName() const
{
  return "StereoscopicFreeCamera";
}

} // end of namespace BABYLON
