#include <babylon/debug/skeleton_viewer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/builders/lines_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/shape_builder.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {
namespace Debug {

SkeletonViewer::SkeletonViewer(const SkeletonPtr& iSkeleton, const AbstractMeshPtr& iMesh,
                               Scene* iScene, bool iAutoUpdateBonesMatrices, int iRenderingGroupId,
                               const ISkeletonViewerOptions& iOptions)
    : color{Color3::White()}
    , scene{this, &SkeletonViewer::get_scene}
    , utilityLayer{this, &SkeletonViewer::get_utilityLayer}
    , isReady{this, &SkeletonViewer::get_isReady}
    , ready{this, &SkeletonViewer::set_ready}
    , debugMesh{this, &SkeletonViewer::get_debugMesh, &SkeletonViewer::set_debugMesh}
    , material{this, &SkeletonViewer::get_material, &SkeletonViewer::set_material}
    , displayMode{this, &SkeletonViewer::get_displayMode, &SkeletonViewer::set_displayMode}
    , skeleton{iSkeleton}
    , mesh{iMesh}
    , autoUpdateBonesMatrices{iAutoUpdateBonesMatrices}
    , renderingGroupId{iRenderingGroupId}
    , isEnabled{this, &SkeletonViewer::get_isEnabled, &SkeletonViewer::set_isEnabled}
    , _debugMesh{nullptr}
    , _isEnabled{false}
    , _obs{nullptr}
    , _utilityLayer{nullptr}
{
  _scene = iScene;
  _ready = false;

  // Defaults
  options.pauseAnimations        = iOptions.pauseAnimations.value_or(true);
  options.returnToRest           = iOptions.returnToRest.value_or(true);
  options.displayMode            = iOptions.displayMode.value_or(SkeletonViewer::DISPLAY_LINES);
  options.displayOptions.midStep = iOptions.displayOptions.midStep.value_or(0.235f);
  options.displayOptions.midStepFactor   = iOptions.displayOptions.midStepFactor.value_or(0.155f);
  options.displayOptions.sphereBaseSize  = iOptions.displayOptions.sphereBaseSize.value_or(0.15f);
  options.displayOptions.sphereScaleUnit = iOptions.displayOptions.sphereScaleUnit.value_or(2.f);
  options.displayOptions.sphereFactor    = iOptions.displayOptions.sphereFactor.value_or(0.865f);
  options.computeBonesUsingShaders       = iOptions.computeBonesUsingShaders.value_or(true);

  const auto boneIndices = mesh->getVerticesData(VertexBuffer::MatricesIndicesKind);
  const auto boneWeights = mesh->getVerticesData(VertexBuffer::MatricesWeightsKind);

  if (!boneIndices.empty() && !boneWeights.empty()) {
    for (size_t i = 0; i < boneIndices.size(); ++i) {
      const auto index = boneIndices[i], weight = boneWeights[i];

      if (weight != 0.f) {
        _boneIndices.insert(static_cast<int>(index));
      }
    }
  }

  /* Create Utility Layer */
  _utilityLayer                        = UtilityLayerRenderer::New(_scene, false);
  _utilityLayer->pickUtilitySceneFirst = false;
  _utilityLayer->utilityLayerScene->autoClearDepthAndStencil = true;

  auto iDisplayMode = options.displayMode.value_or(0u);
  if (iDisplayMode > SkeletonViewer::DISPLAY_SPHERE_AND_SPURS) {
    iDisplayMode = SkeletonViewer::DISPLAY_LINES;
  }
  displayMode = iDisplayMode;
  // Prep the Systems
  update();
  _bindObs();
}

SkeletonViewer::~SkeletonViewer() = default;

Scene*& SkeletonViewer::get_scene()
{
  return _scene;
}

UtilityLayerRendererPtr& SkeletonViewer::get_utilityLayer()
{
  return _utilityLayer;
}

bool SkeletonViewer::get_isReady() const
{
  return _ready;
}

void SkeletonViewer::set_ready(bool value)
{
  _ready = value;
}

AbstractMeshPtr& SkeletonViewer::get_debugMesh()
{
  return _debugMesh;
}

void SkeletonViewer::set_debugMesh(const AbstractMeshPtr& value)
{
  _debugMesh = value;
}

StandardMaterialPtr& SkeletonViewer::get_material()
{
  return material;
}

void SkeletonViewer::set_material(const StandardMaterialPtr& value)
{
  material = value;
}

unsigned int SkeletonViewer::get_displayMode() const
{
  return options.displayMode.value_or(SkeletonViewer::DISPLAY_LINES);
}

void SkeletonViewer::set_displayMode(unsigned int value)
{
  if (value > SkeletonViewer::DISPLAY_SPHERE_AND_SPURS) {
    value = SkeletonViewer::DISPLAY_LINES;
  }
  options.displayMode = value;
}

void SkeletonViewer::_bindObs()
{
  switch (displayMode) {
    case SkeletonViewer::DISPLAY_LINES: {
      _obs = scene()->onBeforeRenderObservable.add(
        [this](Scene* /*scene*/, EventState & /*es*/) -> void { _displayLinesUpdate(); });
      break;
    }
  }
}

void SkeletonViewer::update()
{
  switch (displayMode) {
    case SkeletonViewer::DISPLAY_LINES: {
      _displayLinesUpdate();
      break;
    }
    case SkeletonViewer::DISPLAY_SPHERES: {
      _buildSpheresAndSpurs(true);
      break;
    }
    case SkeletonViewer::DISPLAY_SPHERE_AND_SPURS: {
      _buildSpheresAndSpurs(false);
      break;
    }
  }
}

void SkeletonViewer::set_isEnabled(bool value)
{
  if (isEnabled == value) {
    return;
  }

  _isEnabled = value;

  if (debugMesh()) {
    debugMesh()->setEnabled(value);
  }

  if (value && !_obs) {
    _bindObs();
  }
  else if (!value && _obs) {
    scene()->onBeforeRenderObservable.remove(_obs);
    _obs = nullptr;
  }
}

bool SkeletonViewer::get_isEnabled() const
{
  return _isEnabled;
}

void SkeletonViewer::_getBonePosition(Vector3& position, const Bone& bone, const Matrix& meshMat,
                                      float x, float y, float z) const
{
  auto& tmat             = TmpVectors::MatrixArray[0];
  const auto& parentBone = bone.getParent();
  tmat.copyFrom(bone.getLocalMatrix());

  if (!stl_util::almost_equal(x, 0.f) || !stl_util::almost_equal(y, 0.f)
      || !stl_util::almost_equal(z, 0.f)) {
    auto& tmat2 = TmpVectors::MatrixArray[1];
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

void SkeletonViewer::_getLinesForBonesWithLength(const std::vector<BonePtr>& bones,
                                                 const Matrix& meshMat)
{
  _resizeDebugLines(bones.size());

  auto _mesh   = mesh->_effectiveMesh();
  auto meshPos = _mesh->position();
  auto idx     = 0u;
  for (const auto& bone : bones) {
    auto& points = _debugLines[idx];
    if (bone->_index == -1 || !stl_util::contains(_boneIndices, bone->getIndex())) {
      continue;
    }
    if (points.size() < 2) {
      points           = {Vector3::Zero(), Vector3::Zero()};
      _debugLines[idx] = points;
    }
    _getBonePosition(points[0], *bone, meshMat);
    _getBonePosition(points[1], *bone, meshMat, 0.f, static_cast<float>(bones.size()), 0.f);
    points[0].subtractInPlace(meshPos);
    points[1].subtractInPlace(meshPos);
    ++idx;
  }
}

void SkeletonViewer::_getLinesForBonesNoLength(const std::vector<BonePtr>& bones)
{
  _resizeDebugLines(bones.size());

  auto _mesh   = mesh->_effectiveMesh();
  auto meshPos = _mesh->position();
  auto boneNum = 0u;
  for (size_t i = bones.size(); i-- > 0;) {
    auto& childBone = bones[i];
    auto parentBone = childBone->getParent();
    if (!parentBone || !stl_util::contains(_boneIndices, childBone->getIndex())) {
      continue;
    }
    auto& points = _debugLines[i];
    if (points.size() < 2) {
      points               = {Vector3::Zero(), Vector3::Zero()};
      _debugLines[boneNum] = points;
    }
    childBone->getAbsolutePositionToRef(_mesh, points[0]);
    parentBone->getAbsolutePositionToRef(_mesh, points[1]);
    points[0].subtractInPlace(meshPos);
    points[1].subtractInPlace(meshPos);
    ++boneNum;
  }
}

void SkeletonViewer::_revert(bool animationState)
{
  if (options.pauseAnimations) {
    scene()->animationsEnabled = animationState;
  }
}

void SkeletonViewer::_buildSpheresAndSpurs(bool spheresOnly)
{
  if (_debugMesh) {
    _debugMesh->dispose();
    _debugMesh = nullptr;
    ready      = false;
  }

  struct MeshBone {
    MeshPtr mesh = nullptr;
    BonePtr bone = nullptr;
  }; // end of struct MeshBone

  _ready      = false;
  auto iScene = scene();
  auto bones  = skeleton->bones;
  std::vector<MeshBone> spheres;
  std::vector<MeshPtr> spurs;

  const auto animationState = iScene->animationsEnabled;

  try {
    if (*options.pauseAnimations) {
      iScene->animationsEnabled = false;
    }

    if (*options.returnToRest) {
      skeleton->returnToRest();
    }

    if (autoUpdateBonesMatrices) {
      skeleton->computeAbsoluteTransforms();
    }

    auto longestBoneLength = std::numeric_limits<float>::lowest();
    std::function<void(Bone * bone, Matrix & matrix)> getAbsoluteRestPose = nullptr;
    getAbsoluteRestPose = [&getAbsoluteRestPose](Bone* bone, Matrix& matrix) -> void {
      if (bone == nullptr || bone->_index == -1) {
        matrix.copyFrom(Matrix::Identity());
        return;
      }
      getAbsoluteRestPose(bone->getParent(), matrix);
      bone->getBindPose().multiplyToRef(matrix, matrix);
      return;
    };

    const auto& displayOptions = options.displayOptions;

    for (const auto& bone : bones) {
      if (bone->_index == -1 || !stl_util::contains(_boneIndices, bone->getIndex())) {
        continue;
      }

      Matrix boneAbsoluteRestTransform;
      getAbsoluteRestPose(bone.get(), boneAbsoluteRestTransform);

      std::optional<Vector3> scale       = std::nullopt;
      std::optional<Quaternion> rotation = std::nullopt;
      std::optional<Vector3> anchorPoint = Vector3();
      boneAbsoluteRestTransform.decompose(scale, rotation, anchorPoint);

      for (const auto& bc : bone->children) {
        Matrix childAbsoluteRestTransform;
        bc->getRestPose()->multiplyToRef(boneAbsoluteRestTransform, childAbsoluteRestTransform);
        std::optional<Vector3> bcScale       = std::nullopt;
        std::optional<Quaternion> bcRotation = std::nullopt;
        std::optional<Vector3> childPoint    = Vector3();
        childAbsoluteRestTransform.decompose(bcScale, bcRotation, childPoint);

        auto distanceFromParent = Vector3::Distance(*anchorPoint, *childPoint);

        if (distanceFromParent > longestBoneLength) {
          longestBoneLength = distanceFromParent;
        }
        if (spheresOnly) {
          return;
        }

        auto dir = childPoint->copy().subtract(anchorPoint->copy());
        auto h   = dir.length();
        auto up  = dir.normalize().scale(h);

        auto midStep       = displayOptions.midStep.value_or(0.165f);
        auto midStepFactor = displayOptions.midStepFactor.value_or(0.215f);

        auto up0 = up.scale(midStep);

        ExtrudeShapeCustomOptions spurOptions;
        spurOptions.shape = {
          Vector3(1.f, -1.f, 0.f),  //
          Vector3(1.f, 1.f, 0.f),   //
          Vector3(-1.f, 1.f, 0.f),  //
          Vector3(-1.f, -1.f, 0.f), //
          Vector3(1.f, -1.f, 0.f)   //
        };
        spurOptions.path          = {Vector3::Zero(), up0, up};
        spurOptions.scaleFunction = [h, midStepFactor](float i, float /*distance*/) -> float {
          if (i == 0.f || i == 2.f) {
            return 0.f;
          }
          else if (i == 1.f) {
            return h * midStepFactor;
          }
          return 0.f;
        };
        spurOptions.sideOrientation = Mesh::DEFAULTSIDE;
        spurOptions.updatable       = false;
        auto spur = ShapeBuilder::ExtrudeShapeCustom(bc->name + ":spur", spurOptions, scene);

        spur->convertToFlatShadedMesh();

        auto numVertices = spur->getTotalVertices();
        Float32Array mwk, mik;

        for (size_t i = 0; i < numVertices; i++) {
          stl_util::concat(mwk, {1.f, 0.f, 0.f, 0.f});
          stl_util::concat(mik, {static_cast<float>(bone->getIndex()), 0.f, 0.f, 0.f});
        }
        spur->position = *anchorPoint;

        spur->setVerticesData(VertexBuffer::MatricesWeightsKind, mwk, false);
        spur->setVerticesData(VertexBuffer::MatricesIndicesKind, mik, false);

        spurs.emplace_back(spur);
      }

      const auto sphereBaseSize = displayOptions.sphereBaseSize.value_or(0.2f);

      SphereOptions sphereOptions;
      sphereOptions.segments  = 6u;
      sphereOptions.diameter  = sphereBaseSize;
      sphereOptions.updatable = false;
      auto sphere = SphereBuilder::CreateSphere(bone->name + ":sphere", sphereOptions, scene);

      const auto numVertices = sphere->getTotalVertices();

      Float32Array mwk;
      Float32Array mik;

      for (size_t i = 0; i < numVertices; i++) {
        stl_util::concat(mwk, {1.f, 0.f, 0.f, 0.f});
        stl_util::concat(mik, {static_cast<float>(bone->getIndex()), 0.f, 0.f, 0.f});
      }

      sphere->setVerticesData(VertexBuffer::MatricesWeightsKind, mwk, false);
      sphere->setVerticesData(VertexBuffer::MatricesIndicesKind, mik, false);

      sphere->position = *anchorPoint;
      spheres.emplace_back(MeshBone{sphere, bone});
    }

    const auto sphereScaleUnit = displayOptions.sphereScaleUnit.value_or(2.f);
    const auto sphereFactor    = displayOptions.sphereFactor.value_or(0.85f);

    std::vector<MeshPtr> meshes;
    for (const auto& meshBone : spheres) {
      const auto& sphere = meshBone.mesh;
      const auto& bone   = meshBone.bone;
      auto scale         = 1.f / (sphereScaleUnit / longestBoneLength);

      auto _stepsOut = 0;
      auto _b        = bone.get();

      while ((_b->getParent()) && _b->getParent()->getIndex() != -1) {
        ++_stepsOut;
        _b = _b->getParent();
      }
      sphere->scaling().scaleInPlace(scale * std::pow(sphereFactor, _stepsOut));
      meshes.emplace_back(sphere);
    }

    debugMesh              = Mesh::MergeMeshes(stl_util::concat(meshes, spurs), true, true);
    const auto& iDebugMesh = debugMesh();
    if (iDebugMesh) {
      iDebugMesh->renderingGroupId         = renderingGroupId;
      iDebugMesh->skeleton                 = skeleton;
      iDebugMesh->parent                   = mesh.get();
      iDebugMesh->computeBonesUsingShaders = options.computeBonesUsingShaders.value_or(true);
      iDebugMesh->alwaysSelectAsActiveMesh = true;
    }

    _revert(animationState);
    ready = true;
  }
  catch (const std::exception& err) {
    BABYLON_LOG_ERROR("SkeletonViewer", err.what());
    _revert(animationState);
    dispose();
  }
}

void SkeletonViewer::_displayLinesUpdate()
{
  if (!_utilityLayer) {
    return;
  }

  if (autoUpdateBonesMatrices) {
    skeleton->computeAbsoluteTransforms();
  }

  auto _mesh = mesh->_effectiveMesh();

  if (!skeleton->bones.empty() && stl_util::almost_equal(skeleton->bones[0]->length, -1.f)) {
    _getLinesForBonesNoLength(skeleton->bones);
  }
  else {
    _getLinesForBonesWithLength(skeleton->bones, _mesh->getWorldMatrix());
  }

  auto targetScene = _utilityLayer->utilityLayerScene.get();

  if (targetScene) {
    LineSystemOptions debugMeshOptions;
    debugMeshOptions.lines     = _debugLines;
    debugMeshOptions.updatable = true;
    debugMeshOptions.instance  = nullptr;

    if (!_debugMesh) {
      _debugMesh = LinesBuilder::CreateLineSystem("", debugMeshOptions, targetScene);
      _debugMesh->renderingGroupId = renderingGroupId;
    }
    else {
      auto debugLineMesh        = std::static_pointer_cast<LinesMesh>(_debugMesh);
      debugMeshOptions.instance = debugLineMesh;
      LinesBuilder::CreateLineSystem("", debugMeshOptions, targetScene);
    }
    auto debugLineMesh = std::static_pointer_cast<LinesMesh>(_debugMesh);
    if (debugLineMesh) {
      debugLineMesh->position().copyFrom(mesh->position());
      debugLineMesh->color = color;
    }
  }
}

void SkeletonViewer::changeDisplayMode(unsigned int mode)
{
  const auto wasEnabled = isEnabled() ? true : false;
  if (displayMode != mode) {
    isEnabled = false;
    if (_debugMesh) {
      _debugMesh->dispose();
      _debugMesh = nullptr;
      ready      = false;
    }
    displayMode = mode;

    update();
    _bindObs();
    isEnabled = wasEnabled;
  }
}

void SkeletonViewer::changeDisplayOptions(const std::string& option, float value)
{
  const auto wasEnabled = isEnabled() ? true : false;
  if (option == "midStep") {
    options.displayOptions.midStep = value;
  }
  else if (option == "midStepFactor") {
    options.displayOptions.midStepFactor = value;
  }
  else if (option == "sphereBaseSize") {
    options.displayOptions.sphereBaseSize = value;
  }
  else if (option == "sphereScaleUnit") {
    options.displayOptions.sphereScaleUnit = value;
  }
  else if (option == "sphereFactor") {
    options.displayOptions.sphereFactor = value;
  }
  isEnabled = false;
  if (_debugMesh) {
    _debugMesh->dispose();
    _debugMesh = nullptr;
    ready      = false;
  }
  update();
  _bindObs();
  isEnabled = wasEnabled;
}

void SkeletonViewer::dispose()
{
  isEnabled = false;

  if (_debugMesh) {
    _debugMesh->dispose();
    _debugMesh = nullptr;
  }

  if (_utilityLayer) {
    _utilityLayer->dispose();
    _utilityLayer = nullptr;
  }

  ready = false;
}

} // end of namespace Debug
} // end of namespace BABYLON
