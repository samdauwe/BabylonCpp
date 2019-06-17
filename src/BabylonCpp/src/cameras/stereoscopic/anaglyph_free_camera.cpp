#include <babylon/cameras/stereoscopic/anaglyph_free_camera.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

bool AnaglyphFreeCamera::NodeConstructorAdded = false;

void AnaglyphFreeCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "AnaglyphFreeCamera", [](const std::string& iName, Scene* scene,
                             const std::optional<json>& options) {
      float interaxialDistance = 0.f;
      if (options) {
        interaxialDistance
          = json_util::get_number<float>(*options, "interaxial_distance");
      }
      return AnaglyphFreeCamera::New(iName, Vector3::Zero(), interaxialDistance,
                                     scene);
    });
  AnaglyphFreeCamera::NodeConstructorAdded = true;
}

AnaglyphFreeCamera::AnaglyphFreeCamera(const std::string& iName,
                                       const Vector3& iPosition,
                                       float iInteraxialDistance, Scene* scene)
    : FreeCamera{iName, iPosition, scene}
{
  interaxialDistance = iInteraxialDistance;
  RigParamaters rigParams;
  rigParams.interaxialDistance = interaxialDistance;
  setCameraRigMode(Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH, rigParams);
}

AnaglyphFreeCamera::~AnaglyphFreeCamera()
{
}

const std::string AnaglyphFreeCamera::getClassName() const
{
  return "AnaglyphFreeCamera";
}

} // end of namespace BABYLON
