#include <babylon/rendering/rendering_group.h>

#include <babylon/cameras/camera.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/sub_mesh.h>

namespace BABYLON {

RenderingGroup::RenderingGroup(
  unsigned int iIndex, Scene* scene,
  const std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn,
  const std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn,
  const std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn)
    : index{iIndex}
    , onBeforeTransparentRendering{nullptr}
    , _scene{scene}
{
  _opaqueSubMeshes.reserve(256);
  _transparentSubMeshes.reserve(256);
  _alphaTestSubMeshes.reserve(256);

  setOpaqueSortCompareFn(opaqueSortCompareFn);
  setAlphaTestSortCompareFn(alphaTestSortCompareFn);
  setTransparentSortCompareFn(transparentSortCompareFn);
}

RenderingGroup::~RenderingGroup()
{
}

void RenderingGroup::setOpaqueSortCompareFn(
  const std::function<int(SubMesh* a, SubMesh* b)>& value)
{
  _opaqueSortCompareFn = value;
  if (value) {
    _renderOpaque = [this](const std::vector<SubMesh*>& subMeshes) {
      renderOpaqueSorted(subMeshes);
    };
  }
  else {
    _renderOpaque = [this](const std::vector<SubMesh*>& subMeshes) {
      RenderingGroup::renderUnsorted(subMeshes);
    };
  }
}

void RenderingGroup::setAlphaTestSortCompareFn(
  const std::function<int(SubMesh* a, SubMesh* b)>& value)
{
  _alphaTestSortCompareFn = value;
  if (value) {
    _renderAlphaTest = [this](const std::vector<SubMesh*>& subMeshes) {
      renderAlphaTestSorted(subMeshes);
    };
  }
  else {
    _renderAlphaTest = [this](const std::vector<SubMesh*>& subMeshes) {
      RenderingGroup::renderUnsorted(subMeshes);
    };
  }
}

void RenderingGroup::setTransparentSortCompareFn(
  const std::function<int(SubMesh* a, SubMesh* b)>& value)
{
  if (value) {
    _transparentSortCompareFn = value;
  }
  else {
    _transparentSortCompareFn = [this](SubMesh* a, SubMesh* b) {
      return RenderingGroup::defaultTransparentSortCompare(a, b);
    };
  }
  _renderTransparent = [this](const std::vector<SubMesh*>& subMeshes) {
    renderTransparentSorted(subMeshes);
  };
}

bool RenderingGroup::render(
  std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                     const std::vector<SubMesh*>& transparentSubMeshes,
                     const std::vector<SubMesh*>& alphaTestSubMeshes)>&
    customRenderFunction)
{
  if (customRenderFunction) {
    customRenderFunction(_opaqueSubMeshes, _alphaTestSubMeshes,
                         _transparentSubMeshes);
    return true;
  }

  if (_opaqueSubMeshes.empty() && _alphaTestSubMeshes.empty()
      && _transparentSubMeshes.empty()) {
    if (onBeforeTransparentRendering) {
      onBeforeTransparentRendering();
    }
    return false;
  }
  auto engine = _scene->getEngine();

  // Opaque
  _renderOpaque(_opaqueSubMeshes);

  // Alpha test
  engine->setAlphaTesting(true);
  _renderAlphaTest(_alphaTestSubMeshes);
  engine->setAlphaTesting(false);

  if (onBeforeTransparentRendering) {
    onBeforeTransparentRendering();
  }

  // Transparent
  _renderTransparent(_transparentSubMeshes);
  engine->setAlphaMode(Engine::ALPHA_DISABLE);

  return true;
}

void RenderingGroup::renderOpaqueSorted(const std::vector<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _opaqueSortCompareFn,
                                      _scene->activeCamera->globalPosition(),
                                      false);
}

void RenderingGroup::renderAlphaTestSorted(
  const std::vector<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _alphaTestSortCompareFn,
                                      _scene->activeCamera->globalPosition(),
                                      false);
}

void RenderingGroup::renderTransparentSorted(
  const std::vector<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _transparentSortCompareFn,
                                      _scene->activeCamera->globalPosition(),
                                      true);
}

void RenderingGroup::renderSorted(
  const std::vector<SubMesh*>& subMeshes,
  const std::function<int(SubMesh* a, SubMesh* b)>& sortCompareFn,
  const Vector3& cameraPosition, bool transparent)
{
  for (auto& subMesh : subMeshes) {
    subMesh->_alphaIndex = subMesh->getMesh()->alphaIndex;
    subMesh->_distanceToCamera
      = subMesh->getBoundingInfo()
          ->boundingSphere.centerWorld.subtract(cameraPosition)
          .length();
  }

  auto sortedArray = subMeshes;
  std::sort(sortedArray.begin(), sortedArray.end(), sortCompareFn);

  for (auto& subMesh : sortedArray) {
    subMesh->render(transparent);
  }
}

void RenderingGroup::renderUnsorted(const std::vector<SubMesh*>& subMeshes)
{
  for (auto& subMesh : subMeshes) {
    subMesh->render(false);
  }
}

int RenderingGroup::defaultTransparentSortCompare(SubMesh* a, SubMesh* b)
{
  // Alpha index first
  if (a->_alphaIndex > b->_alphaIndex) {
    return 1;
  }
  if (a->_alphaIndex < b->_alphaIndex) {
    return -1;
  }

  // Then distance to camera
  return RenderingGroup::backToFrontSortCompare(a, b);
}

int RenderingGroup::backToFrontSortCompare(SubMesh* a, SubMesh* b)
{
  // Then distance to camera
  if (a->_distanceToCamera < b->_distanceToCamera) {
    return 1;
  }
  if (a->_distanceToCamera > b->_distanceToCamera) {
    return -1;
  }

  return 0;
}

int RenderingGroup::frontToBackSortCompare(SubMesh* a, SubMesh* b)
{
  // Then distance to camera
  if (a->_distanceToCamera < b->_distanceToCamera) {
    return -1;
  }
  if (a->_distanceToCamera > b->_distanceToCamera) {
    return 1;
  }

  return 0;
}

void RenderingGroup::prepare()
{
  _opaqueSubMeshes.clear();
  _transparentSubMeshes.clear();
  _alphaTestSubMeshes.clear();
}

void RenderingGroup::dispatch(SubMesh* subMesh)
{
  auto material = subMesh->getMaterial();
  auto mesh     = subMesh->getMesh();

  if (material->needAlphaBlending() || mesh->visibility < 1.f
      || mesh->hasVertexAlpha) { // Transparent
    _transparentSubMeshes.emplace_back(subMesh);
  }
  else if (material->needAlphaTesting()) { // Alpha test
    _alphaTestSubMeshes.emplace_back(subMesh);
  }
  else {
    _opaqueSubMeshes.emplace_back(subMesh); // Opaque
  }
}

} // end of namespace BABYLON
