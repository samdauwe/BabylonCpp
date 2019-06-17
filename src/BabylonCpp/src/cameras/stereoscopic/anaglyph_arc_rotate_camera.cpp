#include <babylon/cameras/stereoscopic/anaglyph_arc_rotate_camera.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

bool AnaglyphArcRotateCamera::NodeConstructorAdded = false;

void AnaglyphArcRotateCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "AnaglyphArcRotateCamera", [](const std::string& iName, Scene* scene,
                                  const std::optional<json>& options) {
      float interaxialDistance = 0.f;
      if (options) {
        interaxialDistance
          = json_util::get_number<float>(*options, "interaxial_distance");
      }
      return AnaglyphArcRotateCamera::New(iName, 0.f, 0.f, 1.f, Vector3::Zero(),
                                          interaxialDistance, scene);
    });
  AnaglyphArcRotateCamera::NodeConstructorAdded = true;
}

AnaglyphArcRotateCamera::AnaglyphArcRotateCamera(
  const std::string& iName, float iAlpha, float iBeta, float iRadius,
  const Vector3& iTarget, float iInteraxialDistance, Scene* scene)
    : ArcRotateCamera{iName, iAlpha, iBeta, iRadius, iTarget, scene}
{
  interaxialDistance = iInteraxialDistance;
  RigParamaters rigParams;
  rigParams.interaxialDistance = interaxialDistance;
  setCameraRigMode(Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH, rigParams);
}

AnaglyphArcRotateCamera::~AnaglyphArcRotateCamera()
{
}

const std::string AnaglyphArcRotateCamera::getClassName() const
{
  return "AnaglyphArcRotateCamera";
}

} // end of namespace BABYLON
