#include <babylon/debug/ray_helper.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

unique_ptr_t<RayHelper> RayHelper::CreateAndShow(const Ray& ray, Scene* scene,
                                                 const Color3& color)
{
  auto helper = ::std::make_unique<RayHelper>(ray);

  helper->show(scene, color);

  return helper;
}

RayHelper::RayHelper(const Ray& iRay) : ray{iRay}
{
}

RayHelper::~RayHelper()
{
}

void RayHelper::show(Scene* scene, const Color3& color)
{
  if (!_renderFunction) {
    _renderFunction = [this]() { _render(); };
    _scene          = scene;
    _renderPoints
      = {ray.origin, ray.origin.add(ray.direction.scale(ray.length))};
    _renderLine = Mesh::CreateLines("ray", _renderPoints, scene, true);

    _scene->registerBeforeRender(_renderFunction);
  }

  {
    _renderLine->color.copyFrom(color);
  }
}

void RayHelper::hide()
{
  if (_renderFunction) {
    _scene->unregisterBeforeRender(_renderFunction);
    _scene          = nullptr;
    _renderFunction = nullptr;
    _renderLine->dispose();
    _renderLine = nullptr;
    _renderPoints.clear();
  }
}

void RayHelper::_render()
{
  auto& point = _renderPoints[1];
  float len   = ::std::min(ray.length, 1000000.f);

  point.copyFrom(ray.direction);
  point.scaleInPlace(len);
  point.addInPlace(ray.origin);

  Mesh::CreateLines("ray", _renderPoints, _scene, true, _renderLine);
}

void RayHelper::attachToMesh(AbstractMesh* mesh,
                             const Vector3& meshSpaceDirection,
                             const Vector3& meshSpaceOrigin, float length)
{
  _attachedToMesh = mesh;

  if (length > 0) {
    ray.length = length;
  }

  _meshSpaceDirection = meshSpaceDirection;
  _meshSpaceOrigin    = meshSpaceOrigin;

  if (!_updateToMeshFunction) {
    _updateToMeshFunction = [this]() { _updateToMesh(); };
    _attachedToMesh->getScene()->registerBeforeRender(_updateToMeshFunction);
  }

  _updateToMesh();
}

void RayHelper::detachFromMesh()
{
  if (_attachedToMesh) {
    _attachedToMesh->getScene()->unregisterBeforeRender(_updateToMeshFunction);
    _attachedToMesh       = nullptr;
    _updateToMeshFunction = nullptr;
  }
}

void RayHelper::_updateToMesh()
{
  if (_attachedToMesh->_isDisposed) {
    detachFromMesh();
    return;
  }

  _attachedToMesh->getDirectionToRef(_meshSpaceDirection, ray.direction);
  Vector3::TransformCoordinatesToRef(
    _meshSpaceOrigin, *_attachedToMesh->getWorldMatrix(), ray.origin);
}

void RayHelper::dispose()
{
  hide();
  detachFromMesh();
}

} // end of namespace BABYLON
