#include <babylon/cameras/stereoscopic/anaglyph_arc_rotate_camera.h>

#include <babylon/core/json.h>

namespace BABYLON {

bool AnaglyphArcRotateCamera::NodeConstructorAdded = false;

void AnaglyphArcRotateCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "AnaglyphArcRotateCamera", [](const std::string& name, Scene* scene,
                                  const std::optional<Json::value>& options) {
      float interaxialDistance = 0.f;
      if (options) {
        interaxialDistance
          = Json::GetNumber<float>(*options, "interaxial_distance", 0.f);
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
  Json::object rigParams;
  rigParams["interaxialDistance"]
    = picojson::value(static_cast<double>(interaxialDistance));
  setCameraRigMode(Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH(),
                   Json::value(rigParams));
}

AnaglyphArcRotateCamera::~AnaglyphArcRotateCamera()
{
}

const std::string AnaglyphArcRotateCamera::getClassName() const
{
  return "AnaglyphArcRotateCamera";
}

} // end of namespace BABYLON
