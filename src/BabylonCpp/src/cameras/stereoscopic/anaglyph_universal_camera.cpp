#include <babylon/cameras/stereoscopic/anaglyph_universal_camera.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

bool AnaglyphUniversalCamera::NodeConstructorAdded = false;

void AnaglyphUniversalCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor("AnaglyphUniversalCamera", [](const std::string& iName, Scene* scene,
                                                         const std::optional<json>& options) {
    auto interaxialDistance = 0.f;
    if (options) {
      interaxialDistance = json_util::get_number<float>(*options, "interaxial_distance");
    }
    return AnaglyphUniversalCamera::New(iName, Vector3::Zero(), interaxialDistance, scene);
  });
  AnaglyphUniversalCamera::NodeConstructorAdded = true;
}

AnaglyphUniversalCamera::AnaglyphUniversalCamera(const std::string& iName, const Vector3& iPosition,
                                                 float iInteraxialDistance, Scene* scene)
    : UniversalCamera{iName, iPosition, scene}
{
  interaxialDistance = iInteraxialDistance;
  RigParamaters rigParams;
  rigParams.interaxialDistance = interaxialDistance;
  setCameraRigMode(Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH, rigParams);
}

AnaglyphUniversalCamera::~AnaglyphUniversalCamera() = default;

std::string AnaglyphUniversalCamera::getClassName() const
{
  return "AnaglyphUniversalCamera";
}

} // end of namespace BABYLON
