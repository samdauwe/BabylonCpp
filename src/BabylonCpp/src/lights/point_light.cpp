#include <babylon/lights/point_light.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

PointLight::PointLight(const std::string& iName, const Vector3& iPosition,
                       Scene* scene)
    : Light{iName, scene}
    , transformedPosition{nullptr}
    , position{iPosition}
    , _worldMatrix{nullptr}
{
}

PointLight::~PointLight()
{
}

const char* PointLight::getClassName() const
{
  return "PointLight";
}

IReflect::Type PointLight::type() const
{
  return IReflect::Type::POINTLIGHT;
}

Scene* PointLight::getScene()
{
  return Node::getScene();
}

Vector3 PointLight::getAbsolutePosition()
{
  return transformedPosition ? *transformedPosition : position;
}

bool PointLight::computeTransformedPosition()
{
  if (parent() && parent()->getWorldMatrix()) {
    if (!transformedPosition) {
      transformedPosition = std_util::make_unique<Vector3>(Vector3::Zero());
    }

    Vector3::TransformCoordinatesToRef(position, *parent()->getWorldMatrix(),
                                       *transformedPosition);

    return true;
  }

  return false;
}

void PointLight::transferToEffect(Effect* effect,
                                  const std::string& positionUniformName)
{
  if (parent() && parent()->getWorldMatrix()) {
    computeTransformedPosition();

    effect->setFloat4(positionUniformName, transformedPosition->x,
                      transformedPosition->y, transformedPosition->z, 0.f);

    return;
  }

  effect->setFloat4(positionUniformName, position.x, position.y, position.z,
                    0.f);
}

bool PointLight::needCube() const
{
  return true;
}

bool PointLight::needRefreshPerFrame() const
{
  return false;
}

Vector3 PointLight::getShadowDirection(unsigned int faceIndex)
{
  switch (faceIndex) {
    case 0:
      return Vector3(1.f, 0.f, 0.f);
    case 1:
      return Vector3(-1.f, 0.f, 0.f);
    case 2:
      return Vector3(0.f, -1.f, 0.f);
    case 3:
      return Vector3(0.f, 1.f, 0.f);
    case 4:
      return Vector3(0.f, 0.f, 1.f);
    case 5:
      return Vector3(0.f, 0.f, -1.f);
    default:
      break;
  }

  return Vector3::Zero();
}

ShadowGenerator* PointLight::getShadowGenerator()
{
  return nullptr;
}

void PointLight::setShadowProjectionMatrix(
  Matrix& matrix, const Matrix& viewMatrix,
  const std::vector<AbstractMesh*>& renderList)
{
  if (customProjectionMatrixBuilder) {
    customProjectionMatrixBuilder(viewMatrix, renderList, matrix);
  }
  else {
    auto activeCamera = getScene()->activeCamera;
    Matrix::PerspectiveFovLHToRef(
      Math::PI_2, 1.f,
      shadowMinZ.hasValue() ? shadowMinZ.value : activeCamera->minZ,
      shadowMaxZ.hasValue() ? shadowMaxZ.value : activeCamera->maxZ, matrix);
  }
}

Matrix* PointLight::_getWorldMatrix()
{
  if (!_worldMatrix) {
    _worldMatrix = std_util::make_unique<Matrix>(Matrix::Identity());
  }

  Matrix::TranslationToRef(position.x, position.y, position.z, *_worldMatrix);

  return _worldMatrix.get();
}

unsigned int PointLight::getTypeID() const
{
  return 0;
}

} // end of namespace BABYLON
