#include <babylon/lights/directional_light.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

DirectionalLight::DirectionalLight(const std::string& iName,
                                   const Vector3& iDirection, Scene* scene)
    : IShadowLight{iName, scene}
    , position{iDirection.scale(-1.f)}
    , direction{iDirection}
    , transformedPosition{nullptr}
    , shadowOrthoScale{0.5f}
    , autoUpdateExtends{true}
    , _transformedDirection{nullptr}
    , _worldMatrix{nullptr}
    , _orthoLeft{std::numeric_limits<float>::max()}
    , _orthoRight{std::numeric_limits<float>::min()}
    , _orthoTop{std::numeric_limits<float>::min()}
    , _orthoBottom{std::numeric_limits<float>::max()}
{
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::_buildUniformLayout()
{
  _uniformBuffer->addUniform("vLightData", 4);
  _uniformBuffer->addUniform("vLightDiffuse", 4);
  _uniformBuffer->addUniform("vLightSpecular", 3);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->create();
}

const char* DirectionalLight::getClassName() const
{
  return "DirectionalLight";
}

IReflect::Type DirectionalLight::type() const
{
  return IReflect::Type::DIRECTIONALLIGHT;
}

Scene* DirectionalLight::getScene()
{
  return Node::getScene();
}

Vector3 DirectionalLight::getAbsolutePosition()
{
  return transformedPosition ? *transformedPosition : position;
}

Vector3& DirectionalLight::setDirectionToTarget(Vector3& target)
{
  direction = Vector3::Normalize(target.subtract(position));
  return direction;
}

float DirectionalLight::getDepthScale() const
{
  return 30.f;
}

void DirectionalLight::setShadowProjectionMatrix(
  Matrix& matrix, const Matrix& viewMatrix,
  const std::vector<AbstractMesh*>& renderList)
{
  if (customProjectionMatrixBuilder) {
    customProjectionMatrixBuilder(viewMatrix, renderList, matrix);
  }
  else {
    auto activeCamera = getScene()->activeCamera;

    // Check extends
    if (autoUpdateExtends || stl_util::almost_equal(
                               _orthoLeft, std::numeric_limits<float>::max())) {
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

        auto boundingInfo = mesh->getBoundingInfo();
        if (!boundingInfo) {
          continue;
        }

        const auto& boundingBox = boundingInfo->boundingBox;

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
    }

    const float xOffset = _orthoRight - _orthoLeft;
    const float yOffset = _orthoTop - _orthoBottom;

    Matrix::OrthoOffCenterLHToRef(
      _orthoLeft - xOffset * shadowOrthoScale,
      _orthoRight + xOffset * shadowOrthoScale,
      _orthoBottom - yOffset * shadowOrthoScale,
      _orthoTop + yOffset * shadowOrthoScale,
      shadowMinZ.hasValue() ? shadowMinZ.value : activeCamera->minZ,
      shadowMaxZ.hasValue() ? shadowMaxZ.value : activeCamera->maxZ, matrix);
  }
}

bool DirectionalLight::needRefreshPerFrame() const
{
  return true;
}

bool DirectionalLight::needCube() const
{
  return false;
}

Vector3 DirectionalLight::getShadowDirection(unsigned int /*faceIndex*/)
{
  return direction;
}

bool DirectionalLight::computeTransformedPosition()
{
  if (parent() && parent()->getWorldMatrix()) {
    if (!transformedPosition) {
      transformedPosition = std::make_unique<Vector3>(Vector3::Zero());
    }

    Vector3::TransformCoordinatesToRef(position, *parent()->getWorldMatrix(),
                                       *transformedPosition);
    return true;
  }

  return false;
}

void DirectionalLight::transferToEffect(Effect* /*effect*/,
                                        const std::string& lightIndex)
{
  if (parent() && parent()->getWorldMatrix()) {
    if (!_transformedDirection) {
      _transformedDirection = std::make_unique<Vector3>(Vector3::Zero());
    }

    Vector3::TransformNormalToRef(direction, *parent()->getWorldMatrix(),
                                  *_transformedDirection);

    _uniformBuffer->updateFloat4("vLightData", _transformedDirection->x,
                                 _transformedDirection->y,
                                 _transformedDirection->z, 1, lightIndex);

    return;
  }

  _uniformBuffer->updateFloat4("vLightData", direction.x, direction.y,
                               direction.z, 1, lightIndex);
}

Matrix* DirectionalLight::_getWorldMatrix()
{
  if (!_worldMatrix) {
    _worldMatrix = std::make_unique<Matrix>(Matrix::Identity());
  }

  Matrix::TranslationToRef(position.x, position.y, position.z, *_worldMatrix);

  return _worldMatrix.get();
}

unsigned int DirectionalLight::getTypeID() const
{
  return 1;
}

} // end of namespace BABYLON
