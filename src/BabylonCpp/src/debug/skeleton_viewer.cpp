#include <babylon/debug/skeleton_viewer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/engines/scene.h>
#include <babylon/math/tmp.h>
#include <babylon/meshes/builders/lines_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {
namespace Debug {

SkeletonViewer::SkeletonViewer(Skeleton* iSkeleton, AbstractMesh* iMesh,
                               Scene* iScene, bool iAutoUpdateBonesMatrices,
                               int iRenderingGroupId)
    : color{Color3::White()}
    , debugMesh{this, &SkeletonViewer::get_debugMesh}
    , skeleton{iSkeleton}
    , mesh{iMesh}
    , autoUpdateBonesMatrices{iAutoUpdateBonesMatrices}
    , renderingGroupId{iRenderingGroupId}
    , isEnabled{this, &SkeletonViewer::get_isEnabled,
                &SkeletonViewer::set_isEnabled}
    , _debugMesh{nullptr}
    , _isEnabled{false}
    , _utilityLayer{nullptr}
{
  _scene = iScene;

  _utilityLayer = UtilityLayerRenderer::New(_scene, false);
  _utilityLayer->pickUtilitySceneFirst                       = false;
  _utilityLayer->utilityLayerScene->autoClearDepthAndStencil = true;

  update();

  _renderFunction = [this](Scene*, EventState&) { update(); };
}

SkeletonViewer::~SkeletonViewer()
{
}

LinesMeshPtr& SkeletonViewer::get_debugMesh()
{
  return _debugMesh;
}

void SkeletonViewer::set_isEnabled(bool value)
{
  if (_isEnabled == value) {
    return;
  }

  _isEnabled = value;

  if (value) {
    _scene->registerBeforeRender(_renderFunction);
  }
  else {
    _scene->unregisterBeforeRender(_renderFunction);
  }
}

bool SkeletonViewer::get_isEnabled() const
{
  return _isEnabled;
}

void SkeletonViewer::_getBonePosition(Vector3& position, const Bone& bone,
                                      const Matrix& meshMat, float x, float y,
                                      float z) const
{
  auto& tmat             = Tmp::MatrixArray[0];
  const auto& parentBone = bone.getParent();
  tmat.copyFrom(bone.getLocalMatrix());

  if (!stl_util::almost_equal(x, 0.f) || !stl_util::almost_equal(y, 0.f)
      || !stl_util::almost_equal(z, 0.f)) {
    auto& tmat2 = Tmp::MatrixArray[1];
    Matrix::IdentityToRef(tmat2);
    tmat2.setTranslationFromFloats(x, y, z);
    tmat2.multiplyToRef(tmat, tmat);
  }

  if (parentBone) {
    tmat.multiplyToRef(parentBone->getAbsoluteTransform(), tmat);
  }

  tmat.multiplyToRef(meshMat, tmat);

  const auto& tmatM = tmat.m();
  position.x        = tmatM[12];
  position.y        = tmatM[13];
  position.z        = tmatM[14];
}

void SkeletonViewer::_getLinesForBonesWithLength(
  const std::vector<BonePtr>& bones, const Matrix& meshMat)
{
  _resizeDebugLines(bones.size());

  auto _mesh   = mesh->_effectiveMesh();
  auto meshPos = _mesh->position();
  auto i       = 0u;
  for (const auto& bone : bones) {
    auto& points = _debugLines[i];
    if (points.size() < 2) {
      points = {Vector3::Zero(), Vector3::Zero()};
    }
    _getBonePosition(points[0], *bone, meshMat);
    _getBonePosition(points[1], *bone, meshMat, 0.f,
                     static_cast<float>(bones.size()), 0.f);
    points[0].subtractInPlace(meshPos);
    points[1].subtractInPlace(meshPos);
    ++i;
  }
}

void SkeletonViewer::_getLinesForBonesNoLength(
  const std::vector<BonePtr>& bones, const Matrix& /*meshMat*/)
{
  _resizeDebugLines(bones.size());

  auto _mesh   = mesh->_effectiveMesh();
  auto meshPos = _mesh->position();
  auto boneNum = 0u;
  for (size_t i = bones.size(); i-- > 0;) {
    auto& childBone = bones[i];
    auto parentBone = childBone->getParent();
    if (!parentBone) {
      continue;
    }
    auto& points = _debugLines[i];
    if (points.size() < 2) {
      points = {Vector3::Zero(), Vector3::Zero()};
    }
    childBone->getAbsolutePositionToRef(_mesh, points[0]);
    parentBone->getAbsolutePositionToRef(_mesh, points[1]);
    points[0].subtractInPlace(meshPos);
    points[1].subtractInPlace(meshPos);
    ++boneNum;
  }
}

void SkeletonViewer::_resizeDebugLines(size_t bonesSize)
{
  if (bonesSize > _debugLines.size()) {
    size_t sizeDiff = bonesSize - _debugLines.size();
    _debugLines.resize(bonesSize);
    std::vector<Vector3> point{Vector3::Zero(), Vector3::Zero()};
    for (size_t i = 0; i < sizeDiff; ++i) {
      _debugLines.emplace_back(point);
    }
  }
}

void SkeletonViewer::update()
{
  if (!_utilityLayer) {
    return;
  }

  if (autoUpdateBonesMatrices) {
    skeleton->computeAbsoluteTransforms();
  }

  auto _mesh = mesh->_effectiveMesh();

  if (!skeleton->bones.empty()
      && stl_util::almost_equal(skeleton->bones[0]->length, -1.f)) {
    _getLinesForBonesNoLength(skeleton->bones, _mesh->getWorldMatrix());
  }
  else {
    _getLinesForBonesWithLength(skeleton->bones, _mesh->getWorldMatrix());
  }
  auto targetScene = _utilityLayer->utilityLayerScene.get();

  LineSystemOptions options;
  options.lines     = _debugLines;
  options.updatable = true;
  options.instance  = nullptr;

  if (!_debugMesh) {
    _debugMesh = LinesBuilder::CreateLineSystem("", options, targetScene);
    _debugMesh->renderingGroupId = renderingGroupId;
  }
  else {
    options.instance = _debugMesh;
    LinesBuilder::CreateLineSystem("", options, targetScene);
  }
  _debugMesh->position().copyFrom(mesh->position());
  _debugMesh->color = color;
}

void SkeletonViewer::dispose()
{
  isEnabled = false;

  if (_debugMesh) {
    isEnabled = false;
    _debugMesh->dispose();
    _debugMesh = nullptr;
  }

  if (_utilityLayer) {
    _utilityLayer->dispose();
    _utilityLayer = nullptr;
  }
}

} // end of namespace Debug
} // end of namespace BABYLON
