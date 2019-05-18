#include <babylon/lights/directional_light.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

bool DirectionalLight::NodeConstructorAdded = false;

void DirectionalLight::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "Light_Type_1", [](const std::string& iName, Scene* scene,
                       const std::optional<json>& /*options*/) {
      return DirectionalLight::New(iName, Vector3::Zero(), scene);
    });
  DirectionalLight::NodeConstructorAdded = true;
}

DirectionalLight::DirectionalLight(const std::string& iName,
                                   const Vector3& iDirection, Scene* scene)
    : ShadowLight{iName, scene}
    , shadowFrustumSize{this, &DirectionalLight::get_shadowFrustumSize,
                        &DirectionalLight::set_shadowFrustumSize}
    , shadowOrthoScale{this, &DirectionalLight::get_shadowOrthoScale,
                       &DirectionalLight::set_shadowOrthoScale}
    , autoUpdateExtends{true}
    , _shadowFrustumSize{0.f}
    , _shadowOrthoScale{0.1f}
    , _orthoLeft{std::numeric_limits<float>::max()}
    , _orthoRight{std::numeric_limits<float>::min()}
    , _orthoTop{std::numeric_limits<float>::min()}
    , _orthoBottom{std::numeric_limits<float>::max()}
{
  position  = iDirection.scale(-1.f);
  direction = iDirection;
}

DirectionalLight::~DirectionalLight()
{
}

Type DirectionalLight::type() const
{
  return Type::DIRECTIONALLIGHT;
}

const std::string DirectionalLight::getClassName() const
{
  return "DirectionalLight";
}

unsigned int DirectionalLight::getTypeID() const
{
  return Light::LIGHTTYPEID_DIRECTIONALLIGHT;
}

float DirectionalLight::get_shadowFrustumSize() const
{
  return _shadowFrustumSize;
}

void DirectionalLight::set_shadowFrustumSize(float value)
{
  _shadowFrustumSize = value;
  forceProjectionMatrixCompute();
}

float DirectionalLight::get_shadowOrthoScale() const
{
  return _shadowOrthoScale;
}

void DirectionalLight::set_shadowOrthoScale(float value)
{
  _shadowOrthoScale = value;
  forceProjectionMatrixCompute();
}

void DirectionalLight::_setDefaultShadowProjectionMatrix(
  Matrix& matrix, const Matrix& viewMatrix,
  const std::vector<AbstractMesh*>& renderList)
{
  if (shadowFrustumSize() > 0.f) {
    _setDefaultFixedFrustumShadowProjectionMatrix(matrix, viewMatrix);
  }
  else {
    _setDefaultAutoExtendShadowProjectionMatrix(matrix, viewMatrix, renderList);
  }
}

void DirectionalLight::_setDefaultFixedFrustumShadowProjectionMatrix(
  Matrix& matrix, const Matrix& /*viewMatrix*/)
{
  auto& activeCamera = getScene()->activeCamera;

  if (!activeCamera) {
    return;
  }

  Matrix::OrthoLHToRef(shadowFrustumSize(), shadowFrustumSize(),
                       shadowMinZ() ? *shadowMinZ() : activeCamera->minZ,
                       shadowMaxZ() ? *shadowMaxZ() : activeCamera->maxZ,
                       matrix);
}

void DirectionalLight::_setDefaultAutoExtendShadowProjectionMatrix(
  Matrix& matrix, const Matrix& viewMatrix,
  const std::vector<AbstractMesh*>& renderList)
{
  auto& activeCamera = getScene()->activeCamera;

  if (!activeCamera) {
    return;
  }

  // Check extends
  if (autoUpdateExtends
      || stl_util::almost_equal(_orthoLeft,
                                std::numeric_limits<float>::max())) {
    auto tempVector3 = Vector3::Zero();

    _orthoLeft   = std::numeric_limits<float>::max();
    _orthoRight  = std::numeric_limits<float>::min();
    _orthoTop    = std::numeric_limits<float>::min();
    _orthoBottom = std::numeric_limits<float>::max();

    // Check extends
    for (const auto& mesh : renderList) {
      if (!mesh) {
        continue;
      }

      const auto& boundingInfo = mesh->getBoundingInfo();
      const auto& boundingBox  = boundingInfo.boundingBox;

      for (const auto& vector : boundingBox.vectorsWorld) {
        Vector3::TransformCoordinatesToRef(vector, viewMatrix, tempVector3);

        if (tempVector3.x < _orthoLeft) {
          _orthoLeft = tempVector3.x;
        }
        if (tempVector3.y < _orthoBottom) {
          _orthoBottom = tempVector3.y;
        }

        if (tempVector3.x > _orthoRight) {
          _orthoRight = tempVector3.x;
        }
        if (tempVector3.y > _orthoTop) {
          _orthoTop = tempVector3.y;
        }
      }
    }

    const float xOffset = _orthoRight - _orthoLeft;
    const float yOffset = _orthoTop - _orthoBottom;

    Matrix::OrthoOffCenterLHToRef(
      _orthoLeft - xOffset * shadowOrthoScale(),
      _orthoRight + xOffset * shadowOrthoScale(),
      _orthoBottom - yOffset * shadowOrthoScale(),
      _orthoTop + yOffset * shadowOrthoScale(),
      shadowMinZ() ? *shadowMinZ() : activeCamera->minZ,
      shadowMaxZ() ? *shadowMaxZ() : activeCamera->maxZ, matrix);
  }
}

void DirectionalLight::_buildUniformLayout()
{
  _uniformBuffer->addUniform("vLightData", 4);
  _uniformBuffer->addUniform("vLightDiffuse", 4);
  _uniformBuffer->addUniform("vLightSpecular", 3);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->addUniform("depthValues", 2);
  _uniformBuffer->create();
}

void DirectionalLight::transferToEffect(const EffectPtr& /*effect*/,
                                        const std::string& lightIndex)
{
  if (computeTransformedInformation()) {
    _uniformBuffer->updateFloat4("vLightData", transformedDirection().x,
                                 transformedDirection().y,
                                 transformedDirection().z, 1, lightIndex);
    return;
  }

  const auto& iDirection = direction();
  _uniformBuffer->updateFloat4("vLightData", iDirection.x, iDirection.y,
                               iDirection.z, 1, lightIndex);
}

float DirectionalLight::getDepthMinZ(const Camera& /*activeCamera*/) const
{
  return 1.f;
}

float DirectionalLight::getDepthMaxZ(const Camera& /*activeCamera*/) const
{
  return 1.f;
}

void DirectionalLight::prepareLightSpecificDefines(MaterialDefines& defines,
                                                   unsigned int lightIndex)
{
  defines.boolDef["DIRLIGHT" + std::to_string(lightIndex)] = true;
}

} // end of namespace BABYLON
