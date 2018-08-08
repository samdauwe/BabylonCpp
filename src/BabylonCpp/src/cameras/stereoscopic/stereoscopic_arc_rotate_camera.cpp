#include <babylon/cameras/stereoscopic/stereoscopic_arc_rotate_camera.h>

#include <babylon/core/json.h>

namespace BABYLON {

bool StereoscopicArcRotateCamera::NodeConstructorAdded = false;

::std::function<void()> StereoscopicArcRotateCamera::AddNodeConstructor = []() {
  Node::AddNodeConstructor(
    "StereoscopicArcRotateCamera", [](const string_t& name, Scene* scene,
                                      const nullable_t<Json::value>& options) {
      float interaxialDistance      = 0.f;
      bool isStereoscopicSideBySide = false;
      if (options) {
        interaxialDistance
          = Json::GetNumber<float>(*options, "interaxial_distance", 0.f);
        isStereoscopicSideBySide
          = Json::GetBool(*options, "isStereoscopicSideBySide");
      }
      return StereoscopicArcRotateCamera::New(
        name, 0.f, 0.f, 1.f, Vector3::Zero(), interaxialDistance,
        isStereoscopicSideBySide, scene);
    });
  StereoscopicArcRotateCamera::NodeConstructorAdded = true;
};

StereoscopicArcRotateCamera::StereoscopicArcRotateCamera(
  const string_t& name, float alpha, float beta, float radius,
  const Vector3& target, float iInteraxialDistance,
  bool iIsStereoscopicSideBySide, Scene* scene)
    : ArcRotateCamera{name, alpha, beta, radius, target, scene}
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

StereoscopicArcRotateCamera::~StereoscopicArcRotateCamera()
{
}

const string_t StereoscopicArcRotateCamera::getClassName() const
{
  return "StereoscopicArcRotateCamera";
}

} // end of namespace BABYLON
