#include <babylon/debug/skeleton_viewer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/engine/scene.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {

SkeletonViewer::SkeletonViewer(Skeleton* iSkeleton, AbstractMesh* iMesh,
                               Scene* iScene, bool iAutoUpdateBonesMatrices,
                               unsigned int iRenderingGroupId)
    : color{Color3::White()}
    , skeleton{iSkeleton}
    , mesh{iMesh}
    , autoUpdateBonesMatrices{iAutoUpdateBonesMatrices}
    , renderingGroupId{iRenderingGroupId}
    , _scene{iScene}
    , _debugMesh{nullptr}
    , _isEnabled{false}
{
  update();
  _renderFunction = [this](Scene*, const EventState&) { update(); };
}

SkeletonViewer::~SkeletonViewer()
{
}

void SkeletonViewer::setIsEnabled(bool value)
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

bool SkeletonViewer::isEnabled() const
{
  return _isEnabled;
}

void SkeletonViewer::_getBonePosition(Vector3& position, Bone* bone,
                                      const Matrix& meshMat, float x, float y,
                                      float z) const
{
  auto& tmat      = Tmp::MatrixArray[0];
  auto parentBone = bone->getParent();
  tmat.copyFrom(bone->getLocalMatrix());

  if (!stl_util::almost_equal(x, 0.f) || !stl_util::almost_equal(y, 0.f)
      || !stl_util::almost_equal(z, 0.f)) {
    auto& tmat2 = Tmp::MatrixArray[1];
    Matrix::IdentityToRef(tmat2);
    tmat2.m[12] = x;
    tmat2.m[13] = y;
    tmat2.m[14] = z;
    tmat2.multiplyToRef(tmat, tmat);
  }

  if (parentBone) {
    tmat.multiplyToRef(parentBone->getAbsoluteTransform(), tmat);
  }

  tmat.multiplyToRef(meshMat, tmat);

  position.x = tmat.m[12];
  position.y = tmat.m[13];
  position.z = tmat.m[14];
}

void SkeletonViewer::_getLinesForBonesWithLength(
  const vector_t<unique_ptr_t<Bone>>& bones, const Matrix& meshMat)
{
  _resizeDebugLines(bones.size());

  auto meshPos   = mesh->position();
  unsigned int i = 0;
  for (auto& bone : bones) {
    auto& points = _debugLines[i];
    _getBonePosition(points[0], bone.get(), meshMat);
    _getBonePosition(points[1], bone.get(), meshMat, 0, bones.size(), 0);
    points[0].subtractInPlace(meshPos);
    points[1].subtractInPlace(meshPos);
    ++i;
  }
}

void SkeletonViewer::_getLinesForBonesNoLength(
  const vector_t<unique_ptr_t<Bone>>& bones, const Matrix& /*meshMat*/)
{
  _resizeDebugLines(bones.size());

  auto meshPos         = mesh->position();
  unsigned int boneNum = 0;
  for (size_t i = bones.size(); i-- > 0;) {
    auto childBone  = bones[i].get();
    auto parentBone = childBone->getParent();
    if (!parentBone) {
      continue;
    }
    auto& points = _debugLines[i];
    childBone->getAbsolutePositionToRef(mesh, points[0]);
    parentBone->getAbsolutePositionToRef(mesh, points[1]);
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
    vector_t<Vector3> point{Vector3::Zero(), Vector3::Zero()};
    for (size_t i = 0; i < sizeDiff; ++i) {
      _debugLines.emplace_back(point);
    }
  }
}

void SkeletonViewer::update()
{
  if (autoUpdateBonesMatrices) {
    skeleton->computeAbsoluteTransforms();
  }

  if (!skeleton->bones.empty() && skeleton->bones[0]->length == -1) {
    _getLinesForBonesNoLength(skeleton->bones, *mesh->getWorldMatrix());
  }
  else {
    _getLinesForBonesWithLength(skeleton->bones, *mesh->getWorldMatrix());
  }

  LineSystemOptions options;
  options.lines     = _debugLines;
  options.updatable = true;

  if (!_debugMesh) {
    _debugMesh = MeshBuilder::CreateLineSystem("", options, _scene);
    _debugMesh->renderingGroupId = renderingGroupId;
  }
  else {
    options.instance = _debugMesh;
    MeshBuilder::CreateLineSystem("", options, _scene);
  }
  _debugMesh->position().copyFrom(mesh->position());
  _debugMesh->color = color;
}

void SkeletonViewer::dispose()
{
  if (_debugMesh) {
    setIsEnabled(false);
    _debugMesh->dispose();
    _debugMesh = nullptr;
  }
}

} // end of namespace BABYLON
