#include <babylon/cameras/stereoscopic/anaglyph_arc_rotate_camera.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

bool AnaglyphArcRotateCamera::NodeConstructorAdded = false;

void AnaglyphArcRotateCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "AnaglyphArcRotateCamera", [](const std::string& name, Scene* scene,
                                  const std::optional<json>& options) {
      float interaxialDistance = 0.f;
      if (options) {
        interaxialDistance
          = json_util::get_number<float>(*options, "interaxial_distance");
      }
      return AnaglyphArcRotateCamera::New(name, 0.f, 0.f, 1.f, Vector3::Zero(),
                                          interaxialDistance, scene);
    });
  AnaglyphArcRotateCamera::NodeConstructorAdded = true;
}

AnaglyphArcRotateCamera::AnaglyphArcRotateCamera(
  const std::string& name, float alpha, float beta, float radius,
  const Vector3& target, float iInteraxialDistance, Scene* scene)
    : ArcRotateCamera{name, alpha, beta, radius, target, scene}
{
  interaxialDistance = iInteraxialDistance;
  json rigParams;
  rigParams["interaxialDistance"] = interaxialDistance;
  setCameraRigMode(Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH(), rigParams);
}

AnaglyphArcRotateCamera::~AnaglyphArcRotateCamera()
{
}

const std::string AnaglyphArcRotateCamera::getClassName() const
{
  return "AnaglyphArcRotateCamera";
}

} // end of namespace BABYLON
