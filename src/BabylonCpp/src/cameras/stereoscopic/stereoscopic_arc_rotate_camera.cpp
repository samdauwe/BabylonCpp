#include <babylon/cameras/stereoscopic/stereoscopic_arc_rotate_camera.h>

#include <babylon/core/json.h>

namespace BABYLON {

bool StereoscopicArcRotateCamera::NodeConstructorAdded = false;

void StereoscopicArcRotateCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "StereoscopicArcRotateCamera",
    [](const std::string& name, Scene* scene,
       const std::optional<Json::value>& options) {
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
}

StereoscopicArcRotateCamera::StereoscopicArcRotateCamera(
  const std::string& name, float alpha, float beta, float radius,
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

const std::string StereoscopicArcRotateCamera::getClassName() const
{
  return "StereoscopicArcRotateCamera";
}

} // end of namespace BABYLON
