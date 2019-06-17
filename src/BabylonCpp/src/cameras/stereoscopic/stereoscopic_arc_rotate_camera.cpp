#include <babylon/cameras/stereoscopic/stereoscopic_arc_rotate_camera.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

bool StereoscopicArcRotateCamera::NodeConstructorAdded = false;

void StereoscopicArcRotateCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "StereoscopicArcRotateCamera", [](const std::string& iName, Scene* scene,
                                      const std::optional<json>& options) {
      float interaxialDistance      = 0.f;
      bool isStereoscopicSideBySide = false;
      if (options) {
        interaxialDistance
          = json_util::get_number<float>(*options, "interaxial_distance");
        isStereoscopicSideBySide
          = json_util::get_bool(*options, "isStereoscopicSideBySide");
      }
      return StereoscopicArcRotateCamera::New(
        iName, 0.f, 0.f, 1.f, Vector3::Zero(), interaxialDistance,
        isStereoscopicSideBySide, scene);
    });
  StereoscopicArcRotateCamera::NodeConstructorAdded = true;
}

StereoscopicArcRotateCamera::StereoscopicArcRotateCamera(
  const std::string& iName, float iAlpha, float iBeta, float iRadius,
  const Vector3& iTarget, float iInteraxialDistance,
  bool iIsStereoscopicSideBySide, Scene* scene)
    : ArcRotateCamera{iName, iAlpha, iBeta, iRadius, iTarget, scene}
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

StereoscopicArcRotateCamera::~StereoscopicArcRotateCamera()
{
}

const std::string StereoscopicArcRotateCamera::getClassName() const
{
  return "StereoscopicArcRotateCamera";
}

} // end of namespace BABYLON
