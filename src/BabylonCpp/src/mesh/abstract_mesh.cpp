#include <babylon/mesh/abstract_mesh.h>

#include <babylon/actions/action_manager.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/frustum.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/particles/particle_system.h>
#include <babylon/particles/solid_particle.h>
#include <babylon/physics/joint/physics_joint.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/rendering/bounding_box_renderer.h>
#include <babylon/rendering/edges_renderer.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Vector3 AbstractMesh::_lookAtVectorCache = Vector3(0.f, 0.f, 0.f);

AbstractMesh::AbstractMesh(const string_t& iName, Scene* scene)
    : TransformNode(iName, scene, false)
    , facetNb{this, &AbstractMesh::get_facetNb}
    , partitioningSubdivisions{this,
                               &AbstractMesh::get_partitioningSubdivisions,
                               &AbstractMesh::set_partitioningSubdivisions}
    , partitioningBBoxRatio{this, &AbstractMesh::get_partitioningBBoxRatio,
                            &AbstractMesh::set_partitioningBBoxRatio}
    , mustDepthSortFacets{this, &AbstractMesh::get_mustDepthSortFacets,
                          &AbstractMesh::set_mustDepthSortFacets}
    , facetDepthSortFrom{this, &AbstractMesh::get_facetDepthSortFrom,
                         &AbstractMesh::set_facetDepthSortFrom}
    , isFacetDataEnabled{this, &AbstractMesh::get_isFacetDataEnabled}
    , onCollide{this, &AbstractMesh::set_onCollide}
    , onCollisionPositionChange{this,
                                &AbstractMesh::set_onCollisionPositionChange}
    , definedFacingForward{true} // orientation for POV movement & rotation
    , occlusionQueryAlgorithmType{AbstractMesh::
                                    OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE}
    , occlusionType{AbstractMesh::OCCLUSION_TYPE_NONE}
    , occlusionRetryCount{-1}
    , _occlusionInternalRetryCounter{0}
    , _isOccluded{false}
    , _isOcclusionQueryInProgress{false}
    , _occlusionQuery{nullptr}
    , isOccluded{this, &AbstractMesh::get_isOccluded,
                 &AbstractMesh::set_isOccluded}
    , isOcclusionQueryInProgress{this,
                                 &AbstractMesh::get_isOcclusionQueryInProgress}
    , visibility{this, &AbstractMesh::get_visibility,
                 &AbstractMesh::set_visibility}
    , alphaIndex{numeric_limits_t<int>::max()}
    , isVisible{true}
    , isPickable{true}
    , showBoundingBox{this, &AbstractMesh::get_showBoundingBox,
                      &AbstractMesh::set_showBoundingBox}
    , showSubMeshesBoundingBox{false}
    , isBlocker{false}
    , enablePointerMoveEvents{false}
    , renderingGroupId{0}
    , material{this, &AbstractMesh::get_material, &AbstractMesh::set_material}
    , receiveShadows{this, &AbstractMesh::get_receiveShadows,
                     &AbstractMesh::set_receiveShadows}
    , renderOutline{false}
    , outlineColor{Color3::Red()}
    , outlineWidth{0.02f}
    , renderOverlay{false}
    , overlayColor{Color3::Red()}
    , overlayAlpha{0.5f}
    , hasVertexAlpha{this, &AbstractMesh::get_hasVertexAlpha,
                     &AbstractMesh::set_hasVertexAlpha}
    , useVertexColors{this, &AbstractMesh::get_useVertexColors,
                      &AbstractMesh::set_useVertexColors}
    , computeBonesUsingShaders{this,
                               &AbstractMesh::get_computeBonesUsingShaders,
                               &AbstractMesh::set_computeBonesUsingShaders}
    , numBoneInfluencers{this, &AbstractMesh::get_numBoneInfluencers,
                         &AbstractMesh::set_numBoneInfluencers}
    , applyFog{this, &AbstractMesh::get_applyFog, &AbstractMesh::set_applyFog}
    , useOctreeForRenderingSelection{true}
    , useOctreeForPicking{true}
    , useOctreeForCollisions{true}
    , layerMask{this, &AbstractMesh::get_layerMask,
                &AbstractMesh::set_layerMask}
    , alwaysSelectAsActiveMesh{false}
    , actionManager{nullptr}
    , physicsImpostor{nullptr}
    , ellipsoid{Vector3(0.5f, 1.f, 0.5f)}
    , ellipsoidOffset{Vector3(0, 0, 0)}
    , collisionMask{this, &AbstractMesh::get_collisionMask,
                    &AbstractMesh::set_collisionMask}
    , collisionGroup{this, &AbstractMesh::get_collisionGroup,
                     &AbstractMesh::set_collisionGroup}
    , edgesWidth{1.f}
    , edgesColor{Color4(1.f, 0.f, 0.f, 1.f)}
    , _edgesRenderer{nullptr}
    , _masterMesh{nullptr}
    , _materialDefines{nullptr}
    , _boundingInfo{nullptr}
    , _renderId{0}
    , _submeshesOctree{nullptr}
    , _unIndexed{false}
    , _waitingFreezeWorldMatrix{nullopt_t}
    , skeleton{this, &AbstractMesh::get_skeleton, &AbstractMesh::set_skeleton}
    , edgesRenderer{this, &AbstractMesh::get_edgesRenderer}
    , isBlocked{this, &AbstractMesh::get_isBlocked}
    , useBones{this, &AbstractMesh::get_useBones}
    , checkCollisions{this, &AbstractMesh::get_checkCollisions,
                      &AbstractMesh::set_checkCollisions}
    , collider{this, &AbstractMesh::get_collider}
    , _facetNb{0}
    , _partitioningSubdivisions{10}
    , _partitioningBBoxRatio{1.01f}
    , _facetDataEnabled{false}
    , _bbSize{Vector3::Zero()}
    , _subDiv{1, 1, 1, 1}
    , _facetDepthSort{false}
    , _facetDepthSortEnabled{false}
    , _facetDepthSortFunction{nullptr}
    , _facetDepthSortFrom{::std::make_unique<Vector3>(Vector3::Zero())}
    , _onCollideObserver{nullptr}
    , _onCollisionPositionChangeObserver{nullptr}
    , _visibility{1.f}
    , _material{nullptr}
    , _receiveShadows{false}
    , _hasVertexAlpha{false}
    , _useVertexColors{true}
    , _computeBonesUsingShaders{true}
    , _numBoneInfluencers{4}
    , _applyFog{true}
    , _layerMask{0x0FFFFFFF}
    , _checkCollisions{false}
    , _collisionMask{-1}
    , _collisionGroup{-1}
    , _collider{nullptr}
    , _oldPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _diffPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _collisionsTransformMatrix{Matrix::Zero()}
    , _collisionsScalingMatrix{Matrix::Zero()}
    , _skeleton{nullptr}
    , _showBoundingBox{false}
{
  _resyncLightSources();
}

AbstractMesh::~AbstractMesh()
{
}

IReflect::Type AbstractMesh::type() const
{
  return IReflect::Type::ABSTRACTMESH;
}

void AbstractMesh::addToScene(const AbstractMeshPtr& newMesh)
{
  getScene()->addMesh(newMesh);
}

size_t AbstractMesh::get_facetNb() const
{
  return _facetNb;
}

unsigned int AbstractMesh::get_partitioningSubdivisions() const
{
  return _partitioningSubdivisions;
}

void AbstractMesh::set_partitioningSubdivisions(unsigned int nb)
{
  _partitioningSubdivisions = nb;
}

float AbstractMesh::get_partitioningBBoxRatio() const
{
  return _partitioningBBoxRatio;
}

void AbstractMesh::set_partitioningBBoxRatio(float ratio)
{
  _partitioningBBoxRatio = ratio;
}

bool AbstractMesh::get_mustDepthSortFacets() const
{
  return _facetDepthSort;
}

void AbstractMesh::set_mustDepthSortFacets(bool sort)
{
  _facetDepthSort = sort;
}

Vector3& AbstractMesh::get_facetDepthSortFrom()
{
  return *_facetDepthSortFrom.get();
}

void AbstractMesh::set_facetDepthSortFrom(const Vector3& location)
{
  _facetDepthSortFrom = ::std::make_unique<Vector3>(location);
}

bool AbstractMesh::get_isFacetDataEnabled() const
{
  return _facetDataEnabled;
}

bool AbstractMesh::_updateNonUniformScalingState(bool value)
{
  if (!TransformNode::_updateNonUniformScalingState(value)) {
    return false;
  }
  _markSubMeshesAsMiscDirty();
  return true;
}

void AbstractMesh::set_onCollide(
  const ::std::function<void(AbstractMesh*, EventState&)>& callback)
{
  if (_onCollideObserver) {
    onCollideObservable.remove(_onCollideObserver);
  }
  _onCollideObserver = onCollideObservable.add(callback);
}

void AbstractMesh::set_onCollisionPositionChange(
  const ::std::function<void(Vector3*, EventState&)>& callback)
{
  if (_onCollisionPositionChangeObserver) {
    onCollisionPositionChangeObservable.remove(
      _onCollisionPositionChangeObserver);
  }
  _onCollisionPositionChangeObserver
    = onCollisionPositionChangeObservable.add(callback);
}

MaterialPtr& AbstractMesh::get_material()
{
  return _material;
}

void AbstractMesh::set_material(const MaterialPtr& value)
{
  if (_material == value) {
    return;
  }

  _material = value;

  if (onMaterialChangedObservable.hasObservers()) {
    onMaterialChangedObservable.notifyObservers(this);
  }

  if (subMeshes.empty()) {
    return;
  }

  _unBindEffect();
}

bool AbstractMesh::get_receiveShadows() const
{
  return _receiveShadows;
}

void AbstractMesh::set_receiveShadows(bool value)
{
  if (_receiveShadows == value) {
    return;
  }

  _receiveShadows = value;
  _markSubMeshesAsLightDirty();
}

bool AbstractMesh::get_hasVertexAlpha() const
{
  return _hasVertexAlpha;
}

void AbstractMesh::set_hasVertexAlpha(bool value)
{
  if (_hasVertexAlpha == value) {
    return;
  }

  _hasVertexAlpha = value;
  _markSubMeshesAsAttributesDirty();
  _markSubMeshesAsMiscDirty();
}

bool AbstractMesh::get_useVertexColors() const
{
  return _useVertexColors;
}

void AbstractMesh::set_useVertexColors(bool value)
{
  if (_useVertexColors == value) {
    return;
  }

  _useVertexColors = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::get_computeBonesUsingShaders() const
{
  return _computeBonesUsingShaders;
}

void AbstractMesh::set_computeBonesUsingShaders(bool value)
{
  if (_computeBonesUsingShaders == value) {
    return;
  }

  _computeBonesUsingShaders = value;
  _markSubMeshesAsAttributesDirty();
}

unsigned int AbstractMesh::get_numBoneInfluencers() const
{
  return _numBoneInfluencers;
}

void AbstractMesh::set_numBoneInfluencers(unsigned int value)
{
  if (_numBoneInfluencers == value) {
    return;
  }

  _numBoneInfluencers = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::get_applyFog() const
{
  return _applyFog;
}

void AbstractMesh::set_applyFog(bool value)
{
  if (_applyFog == value) {
    return;
  }

  _applyFog = value;
  _markSubMeshesAsMiscDirty();
}

unsigned int AbstractMesh::get_layerMask() const
{
  return _layerMask;
}

void AbstractMesh::set_layerMask(unsigned int value)
{
  if (value == _layerMask) {
    return;
  }

  _layerMask = value;
  _resyncLightSources();
}

bool AbstractMesh::get_isOccluded() const
{
  return _isOccluded;
}

void AbstractMesh::set_isOccluded(bool value)
{
  _isOccluded = value;
}

bool AbstractMesh::get_isOcclusionQueryInProgress() const
{
  return _isOcclusionQueryInProgress;
}

float AbstractMesh::get_visibility() const
{
  return _visibility;
}

void AbstractMesh::set_visibility(float value)
{
  if (stl_util::almost_equal(_visibility, value)) {
    return;
  }

  _visibility = value;
  _markSubMeshesAsMiscDirty();
}

bool AbstractMesh::get_showBoundingBox() const
{
  return _showBoundingBox;
}

void AbstractMesh::set_showBoundingBox(bool value)
{
  _showBoundingBox = value;
  // Lazyly creates a BB renderer if needed.
  if (value) {
    getScene()->getBoundingBoxRenderer();
  }
}

int AbstractMesh::get_collisionMask() const
{
  return _collisionMask;
}

void AbstractMesh::set_collisionMask(int mask)
{
  _collisionMask = !isNan(mask) ? mask : -1;
}

int AbstractMesh::get_collisionGroup() const
{
  return _collisionGroup;
}

void AbstractMesh::set_collisionGroup(int mask)
{
  _collisionGroup = !isNan(mask) ? mask : -1;
}

const string_t AbstractMesh::getClassName() const
{
  return "AbstractMesh";
}

string_t AbstractMesh::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name << ", isInstance: ";
  oss << ((type() == IReflect::Type::INSTANCEDMESH) ? "YES" : "NO");
  oss << ", # of submeshes: " << (subMeshes.size());
  if (_skeleton) {
    oss << ", skeleton: " << _skeleton->name;
  }
  if (fullDetails) {
    oss << ", billboard mode: ";
    const vector_t<string_t> billboardModes{"NONE", "X",    "Y",    "null",
                                            "Z",    "null", "null", "ALL"};
    if (billboardMode < billboardModes.size()) {
      oss << billboardModes[billboardMode];
    }
    else {
      oss << "null";
    }
    oss << ", freeze wrld mat: "
        << (_isWorldMatrixFrozen
                || (_waitingFreezeWorldMatrix.has_value()
                    && *_waitingFreezeWorldMatrix) ?
              "YES" :
              "NO");
  }
  return oss.str();
}

void AbstractMesh::_resyncLightSources()
{
  _lightSources.clear();

  for (auto& light : getScene()->lights) {
    if (!light->isEnabled()) {
      continue;
    }

    if (light->canAffectMesh(this)) {
      _lightSources.emplace_back(light.get());
    }
  }

  _markSubMeshesAsLightDirty();
}

void AbstractMesh::_rebuild()
{
  if (_occlusionQuery) {
    _occlusionQuery = nullptr;
  }

  if (_edgesRenderer) {
    _edgesRenderer->_rebuild();
  }

  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    subMesh->_rebuild();
  }
}

void AbstractMesh::_resyncLighSource(Light* light)
{
  bool isIn = light->isEnabled() && light->canAffectMesh(this);

  auto index = ::std::find_if(_lightSources.begin(), _lightSources.end(),
                              [light](const LightPtr& lightSource) {
                                return lightSource.get() == light;
                              });

  if (index != _lightSources.end()) {
    if (!isIn) {
      return;
    }
    _lightSources.emplace_back(light);
  }
  else {
    if (isIn) {
      return;
    }
    _lightSources.erase(index);
  }

  _markSubMeshesAsLightDirty();
}

void AbstractMesh::_unBindEffect()
{
  for (auto& subMesh : subMeshes) {
    subMesh->setEffect(nullptr);
  }
}

void AbstractMesh::_removeLightSource(const LightPtr& light)
{
  return _removeLightSource(light.get());
}

void AbstractMesh::_removeLightSource(Light* light)
{
  auto index = ::std::find_if(
    _lightSources.begin(), _lightSources.end(),
    [light](const LightPtr& _light) { return _light.get() == light; });

  if (index == _lightSources.end()) {
    return;
  }

  _lightSources.erase(index);

  _markSubMeshesAsLightDirty();
}

void AbstractMesh::_markSubMeshesAsDirty(
  const ::std::function<void(const MaterialDefines& defines)>& func)
{
  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    if (subMesh->_materialDefines) {
      func(*subMesh->_materialDefines);
    }
  }
}

void AbstractMesh::_markSubMeshesAsLightDirty()
{
}

void AbstractMesh::_markSubMeshesAsAttributesDirty()
{
}

void AbstractMesh::_markSubMeshesAsMiscDirty()
{
  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    auto material = subMesh->getMaterial();
    if (material) {
      material->markAsDirty(Material::MiscDirtyFlag());
    }
  }
}

Scene* AbstractMesh::getScene() const
{
  return Node::getScene();
}

void AbstractMesh::set_skeleton(const SkeletonPtr& value)
{
  if (_skeleton && _skeleton->needInitialSkinMatrix) {
    _skeleton->_unregisterMeshWithPoseMatrix(this);
  }

  if (value && value->needInitialSkinMatrix) {
    value->_registerMeshWithPoseMatrix(this);
  }

  _skeleton = value;

  if (!_skeleton) {
    _bonesTransformMatrices.clear();
  }

  _markSubMeshesAsAttributesDirty();
}

SkeletonPtr& AbstractMesh::get_skeleton()
{
  return _skeleton;
}

Vector3& AbstractMesh::get_scaling()
{
  return _scaling;
}

void AbstractMesh::set_scaling(const Vector3& newScaling)
{
  _scaling = newScaling;
  if (physicsImpostor) {
    physicsImpostor->forceUpdate();
  }
}

AbstractMesh* AbstractMesh::getParent()
{
  if (parent()->type() == IReflect::Type::ABSTRACTMESH) {
    return dynamic_cast<AbstractMesh*>(parent());
  }

  return nullptr;
}

MaterialPtr AbstractMesh::getMaterial()
{
  return material();
}

AbstractMesh& AbstractMesh::disableEdgesRendering()
{
  if (_edgesRenderer) {
    _edgesRenderer->dispose();
    _edgesRenderer.reset(nullptr);
  }
  return *this;
}

AbstractMesh&
AbstractMesh::enableEdgesRendering(float epsilon,
                                   bool checkVerticesInsteadOfIndices)
{
  disableEdgesRendering();

  _edgesRenderer = ::std::make_unique<EdgesRenderer>(
    this, epsilon, checkVerticesInsteadOfIndices);

  return *this;
}

unique_ptr_t<EdgesRenderer>& AbstractMesh::get_edgesRenderer()
{
  return _edgesRenderer;
}

bool AbstractMesh::get_isBlocked() const
{
  return false;
}

AbstractMesh* AbstractMesh::getLOD(const CameraPtr& /*camera*/,
                                   BoundingSphere* /*boundingSphere*/)
{
  return this;
}

size_t AbstractMesh::getTotalVertices() const
{
  return 0;
}

Uint32Array AbstractMesh::getIndices(bool /*copyWhenShared*/)
{
  return Uint32Array();
}

Float32Array AbstractMesh::getVerticesData(unsigned int /*kind*/,
                                           bool /*copyWhenShared*/,
                                           bool /*forceCopy*/)
{
  return Float32Array();
}

AbstractMesh*
AbstractMesh::setVerticesData(unsigned int /*kind*/,
                              const Float32Array& /*data*/, bool /*updatable*/,
                              const nullable_t<size_t>& /*stride*/)
{
  return this;
}

AbstractMesh* AbstractMesh::updateVerticesData(unsigned int /*kind*/,
                                               const Float32Array& /*data*/,
                                               bool /*updateExtends*/,
                                               bool /*makeItUnique*/)
{
  return this;
}

AbstractMesh* AbstractMesh::setIndices(const IndicesArray& /*indices*/,
                                       size_t /*totalVertices*/,
                                       bool /*updatable*/)
{
  return this;
}

bool AbstractMesh::isVerticesDataPresent(unsigned int /*kind*/) const
{
  return false;
}

BoundingInfo& AbstractMesh::getBoundingInfo()
{
  if (_masterMesh) {
    return _masterMesh->getBoundingInfo();
  }

  if (!_boundingInfo) {
    // _boundingInfo is being created here
    _updateBoundingInfo();
  }
  // cannot be null.
  return *_boundingInfo;
}

AbstractMesh& AbstractMesh::normalizeToUnitCube(bool includeDescendants)
{
  auto boundingVectors = getHierarchyBoundingVectors(includeDescendants);
  auto sizeVec         = boundingVectors.max.subtract(boundingVectors.min);
  auto maxDimension    = stl_util::max(sizeVec.x, sizeVec.y, sizeVec.z);

  if (maxDimension == 0.f) {
    return *this;
  }

  auto scale = 1.f / maxDimension;

  scaling().scaleInPlace(scale);

  return *this;
}

AbstractMesh& AbstractMesh::setBoundingInfo(const BoundingInfo& boundingInfo)
{
  _boundingInfo = ::std::make_unique<BoundingInfo>(boundingInfo);
  return *this;
}

bool AbstractMesh::get_useBones() const
{
  return _skeleton && getScene()->skeletonsEnabled()
         && isVerticesDataPresent(VertexBuffer::MatricesIndicesKind)
         && isVerticesDataPresent(VertexBuffer::MatricesWeightsKind);
}

void AbstractMesh::_preActivate()
{
}

void AbstractMesh::_preActivateForIntermediateRendering(int /*renderId*/)
{
}

void AbstractMesh::_activate(int renderId)
{
  _renderId = renderId;
}

Matrix* AbstractMesh::getWorldMatrix()
{
  if (_masterMesh) {
    return _masterMesh->getWorldMatrix();
  }

  return TransformNode::getWorldMatrix();
}

float AbstractMesh::_getWorldMatrixDeterminant() const
{
  if (_masterMesh) {
    return _masterMesh->_getWorldMatrixDeterminant();
  }

  return TransformNode::_getWorldMatrixDeterminant();
}

AbstractMesh& AbstractMesh::movePOV(float amountRight, float amountUp,
                                    float amountForward)
{
  position().addInPlace(calcMovePOV(amountRight, amountUp, amountForward));
  return *this;
}

Vector3 AbstractMesh::calcMovePOV(float amountRight, float amountUp,
                                  float amountForward)
{
  Matrix rotMatrix;
  auto rotQuaternion = rotationQuaternion() ?
                         *rotationQuaternion() :
                         Quaternion::RotationYawPitchRoll(
                           rotation().y, rotation().x, rotation().z);
  rotQuaternion.toRotationMatrix(rotMatrix);

  auto translationDelta = Vector3::Zero();
  auto defForwardMult   = definedFacingForward ? 1.f : -1.f;
  Vector3::TransformCoordinatesFromFloatsToRef(
    amountRight * defForwardMult, amountUp, amountForward * defForwardMult,
    rotMatrix, translationDelta);
  return translationDelta;
}

AbstractMesh& AbstractMesh::rotatePOV(float flipBack, float twirlClockwise,
                                      float tiltRight)
{
  rotation().addInPlace(calcRotatePOV(flipBack, twirlClockwise, tiltRight));
  return *this;
}

Vector3 AbstractMesh::calcRotatePOV(float flipBack, float twirlClockwise,
                                    float tiltRight)
{
  auto defForwardMult = definedFacingForward ? 1.f : -1.f;
  return Vector3(flipBack * defForwardMult, twirlClockwise,
                 tiltRight * defForwardMult);
}

MinMax AbstractMesh::getHierarchyBoundingVectors(
  bool includeDescendants,
  const ::std::function<bool(AbstractMesh* abstractMesh)>& predicate)
{
  // Ensures that all world matrix will be recomputed.
  getScene()->incrementRenderId();

  computeWorldMatrix(true);

  Vector3 min;
  Vector3 max;
  auto boundingInfo = getBoundingInfo();

  if (subMeshes.empty()) {
    min
      = Vector3(numeric_limits_t<float>::max(), numeric_limits_t<float>::max(),
                numeric_limits_t<float>::max());
    max = Vector3(numeric_limits_t<float>::lowest(),
                  numeric_limits_t<float>::lowest(),
                  numeric_limits_t<float>::lowest());
  }
  else {
    min = boundingInfo.boundingBox.minimumWorld;
    max = boundingInfo.boundingBox.maximumWorld;
  }

  if (includeDescendants) {
    auto descendants = getDescendants(false);

    for (auto& descendant : descendants) {
      auto childMesh
        = ::std::static_pointer_cast<class AbstractMesh>(descendant);

      childMesh->computeWorldMatrix(true);

      // Filters meshes based on custom predicate function.
      if (predicate && !predicate(childMesh.get())) {
        continue;
      }

      // make sure we have the needed params to get mix and max
      if (childMesh->getTotalVertices() == 0) {
        continue;
      }

      auto childBoundingInfo  = childMesh->getBoundingInfo();
      const auto& boundingBox = childBoundingInfo.boundingBox;

      auto minBox = boundingBox.minimumWorld;
      auto maxBox = boundingBox.maximumWorld;

      Tools::CheckExtends(minBox, min, max);
      Tools::CheckExtends(maxBox, min, max);
    }
  }

  return {min, max};
}

AbstractMesh& AbstractMesh::_updateBoundingInfo()
{
  if (!_boundingInfo) {
    _boundingInfo = ::std::make_unique<BoundingInfo>(absolutePosition(),
                                                     absolutePosition());
  }

  _boundingInfo->update(worldMatrixFromCache());
  _updateSubMeshesBoundingInfo(worldMatrixFromCache());

  return *this;
}

AbstractMesh& AbstractMesh::_updateSubMeshesBoundingInfo(const Matrix& matrix)
{
  if (subMeshes.empty()) {
    return *this;
  }

  for (auto& subMesh : subMeshes) {
    if (!subMesh->isGlobal()) {
      subMesh->updateBoundingInfo(matrix);
    }
  }

  return *this;
}

void AbstractMesh::_afterComputeWorldMatrix()
{
  // Bounding info
  _updateBoundingInfo();
}

bool AbstractMesh::isInFrustum(const array_t<Plane, 6>& frustumPlanes)
{
  return _boundingInfo != nullptr && _boundingInfo->isInFrustum(frustumPlanes);
}

bool AbstractMesh::isCompletelyInFrustum(
  const array_t<Plane, 6>& frustumPlanes) const
{
  return _boundingInfo != nullptr
         && _boundingInfo->isCompletelyInFrustum(frustumPlanes);
}

bool AbstractMesh::intersectsMesh(class AbstractMesh* mesh, bool precise,
                                  bool includeDescendants)
{
  if (!_boundingInfo || !mesh->_boundingInfo) {
    return false;
  }

  if (_boundingInfo->intersects(*mesh->_boundingInfo, precise)) {
    return true;
  }

  if (includeDescendants) {
    for (auto& child : getChildMeshes()) {
      if (child->intersectsMesh(mesh, precise, true)) {
        return true;
      }
    }
  }

  return false;
}

bool AbstractMesh::intersectsMesh(SolidParticle* sp, bool precise,
                                  bool includeDescendants)
{
  if (!_boundingInfo || !sp->_boundingInfo) {
    return false;
  }

  if (_boundingInfo->intersects(*sp->_boundingInfo, precise)) {
    return true;
  }

  if (includeDescendants) {
    for (auto& child : getChildMeshes()) {
      if (child->intersectsMesh(sp, precise, true)) {
        return true;
      }
    }
  }

  return false;
}

bool AbstractMesh::intersectsPoint(const Vector3& point)
{
  if (!_boundingInfo) {
    return false;
  }

  return _boundingInfo->intersectsPoint(point);
}

PhysicsImpostor* AbstractMesh::getPhysicsImpostor()
{
  return physicsImpostor ? physicsImpostor.get() : nullptr;
}

Vector3 AbstractMesh::getPositionInCameraSpace(CameraPtr camera)
{
  if (!camera) {
    camera = getScene()->activeCamera;
  }

  return Vector3::TransformCoordinates(absolutePosition(),
                                       camera->getViewMatrix());
}

float AbstractMesh::getDistanceToCamera(CameraPtr camera)
{
  if (!camera) {
    camera = getScene()->activeCamera;
  }

  return absolutePosition().subtract(camera->position).length();
}

AbstractMesh& AbstractMesh::applyImpulse(const Vector3& force,
                                         const Vector3& contactPoint)
{
  if (!physicsImpostor) {
    return *this;
  }

  physicsImpostor->applyImpulse(force, contactPoint);

  return *this;
}

AbstractMesh& AbstractMesh::setPhysicsLinkWith(Mesh* otherMesh,
                                               const Vector3& /*pivot1*/,
                                               const Vector3& /*pivot2*/,
                                               const PhysicsParams& /*options*/)
{
  if (!physicsImpostor || !otherMesh->physicsImpostor) {
    return *this;
  }

  return *this;
}

bool AbstractMesh::get_checkCollisions() const
{
  return _checkCollisions;
}

void AbstractMesh::set_checkCollisions(bool collisionEnabled)
{
  _checkCollisions = collisionEnabled;
  if (getScene()->workerCollisions()) {
    getScene()->collisionCoordinator->onMeshUpdated(this);
  }
}

unique_ptr_t<Collider>& AbstractMesh::get_collider()
{
  return _collider;
}

AbstractMesh& AbstractMesh::moveWithCollisions(Vector3& displacement)
{
  auto globalPosition = getAbsolutePosition();

  globalPosition.addToRef(ellipsoidOffset, _oldPositionForCollisions);

  if (!_collider) {
    _collider = ::std::make_unique<Collider>();
  }

  _collider->_radius = ellipsoid;

  getScene()->collisionCoordinator->getNewPosition(
    _oldPositionForCollisions, displacement, _collider.get(), 3, this,
    [this](int collisionId, Vector3& newPosition, AbstractMesh* collidedMesh) {
      _onCollisionPositionChange(collisionId, newPosition, collidedMesh);
    },
    static_cast<unsigned int>(uniqueId));

  return *this;
}

void AbstractMesh::_onCollisionPositionChange(int /*collisionId*/,
                                              Vector3& newPosition,
                                              AbstractMesh* collidedMesh)
{
  if (getScene()->workerCollisions()) {
    newPosition.multiplyInPlace(_collider->_radius);
  }

  newPosition.subtractToRef(_oldPositionForCollisions,
                            _diffPositionForCollisions);

  if (_diffPositionForCollisions.length() > Engine::CollisionsEpsilon) {
    position().addInPlace(_diffPositionForCollisions);
  }

  if (collidedMesh) {
    onCollideObservable.notifyObservers(collidedMesh);
  }

  onCollisionPositionChangeObservable.notifyObservers(&position());
}

Octree<SubMesh*>*
AbstractMesh::createOrUpdateSubmeshesOctree(size_t maxCapacity, size_t maxDepth)
{
  if (!_submeshesOctree) {
    _submeshesOctree = new Octree<SubMesh*>(
      [](SubMesh* entry, OctreeBlock<SubMesh*>& block) {
        Octree<AbstractMesh*>::CreationFuncForSubMeshes(entry, block);
      },
      maxCapacity, maxDepth);
  }

  computeWorldMatrix(true);

  auto boundingInfo = getBoundingInfo();

  // Update octree
  vector_t<SubMesh*> subMeshPtrs;
  subMeshPtrs.reserve(subMeshes.size());
  for (auto& subMesh : subMeshes) {
    subMeshPtrs.emplace_back(subMesh.get());
  }

  BoundingBox& bbox = boundingInfo.boundingBox;
  _submeshesOctree->update(bbox.minimumWorld, bbox.maximumWorld, subMeshPtrs);

  return _submeshesOctree;
}

AbstractMesh&
AbstractMesh::_collideForSubMesh(SubMesh* /*subMesh*/,
                                 const Matrix& /*transformMatrix*/,
                                 Collider* /*collider*/)
{
  /*_generatePointsArray();
  // Transformation
  if (!subMesh._lastColliderWorldVertices
      || !subMesh._lastColliderTransformMatrix.equals(transformMatrix)) {
    subMesh._lastColliderTransformMatrix = transformMatrix.clone();
    subMesh._lastColliderWorldVertices   = [];
    subMesh._trianglePlanes              = [];
    var start                            = subMesh.verticesStart;
    var end = (subMesh.verticesStart + subMesh.verticesCount);
    for (var i = start; i < end; i++) {
      subMesh._lastColliderWorldVertices.push(
        Vector3.TransformCoordinates(_positions[i], transformMatrix));
    }
  }
  // Collide
  collider._collide(subMesh._trianglePlanes, subMesh._lastColliderWorldVertices,
                    getIndices(), subMesh.indexStart,
                    subMesh.indexStart + subMesh.indexCount,
                    subMesh.verticesStart, !!subMesh.getMaterial());
  if (collider.collisionFound) {
    collider.collidedMesh = this;
  }*/
  return *this;
}

AbstractMesh&
AbstractMesh::_processCollisionsForSubMeshes(Collider* /*collider*/,
                                             const Matrix& /*transformMatrix*/)
{
  /*var subMeshes : SubMesh[];
  var len : number;

  // Octrees
  if (_submeshesOctree && useOctreeForCollisions) {
    var radius
      = collider.velocityWorldLength
        + Math.max(collider.radius.x, collider.radius.y, collider.radius.z);
    var intersections
      = _submeshesOctree.intersects(collider.basePointWorld, radius);

    len       = intersections.length;
    subMeshes = intersections.data;
  }
  else {
    subMeshes = subMeshes;
    len       = subMeshes.length;
  }

  for (var index = 0; index < len; index++) {
    var subMesh = subMeshes[index];

    // Bounding test
    if (len > 1 && !subMesh._checkCollision(collider))
      continue;

    _collideForSubMesh(subMesh, transformMatrix, collider);
  }*/
  return *this;
}

AbstractMesh& AbstractMesh::_checkCollision(Collider* /*collider*/)
{
  // Bounding box test
  /*if (!_boundingInfo->_checkCollision(collider))
    return;

  // Transformation matrix
  Matrix.ScalingToRef(1.0 / collider.radius.x, 1.0 / collider.radius.y,
                      1.0 / collider.radius.z, _collisionsScalingMatrix);
  worldMatrixFromCache.multiplyToRef(_collisionsScalingMatrix,
                                     _collisionsTransformMatrix);

  _processCollisionsForSubMeshes(collider, _collisionsTransformMatrix);*/
  return *this;
}

bool AbstractMesh::_generatePointsArray()
{
  return false;
}

PickingInfo AbstractMesh::intersects(const Ray& /*ray*/, bool /*fastCheck*/)
{
  /*var pickingInfo = new PickingInfo();

  if (!subMeshes || !_boundingInfo
      || !ray.intersectsSphere(_boundingInfo.boundingSphere)
      || !ray.intersectsBox(_boundingInfo.boundingBox)) {
    return pickingInfo;
  }

  if (!_generatePointsArray()) {
    return pickingInfo;
  }

  var intersectInfo : IntersectionInfo = null;

  // Octrees
  var subMeshes : SubMesh[];
  var len : number;

  if (_submeshesOctree && useOctreeForPicking) {
    var worldRay      = Ray.Transform(ray, getWorldMatrix());
    var intersections = _submeshesOctree.intersectsRay(worldRay);

    len       = intersections.length;
    subMeshes = intersections.data;
  }
  else {
    subMeshes = subMeshes;
    len       = subMeshes.length;
  }

  for (var index = 0; index < len; index++) {
    var subMesh = subMeshes[index];

    // Bounding test
    if (len > 1 && !subMesh.canIntersects(ray))
      continue;

    var currentIntersectInfo
      = subMesh.intersects(ray, _positions, getIndices(), fastCheck);

    if (currentIntersectInfo) {
      if (fastCheck || !intersectInfo
          || currentIntersectInfo.distance < intersectInfo.distance) {
        intersectInfo           = currentIntersectInfo;
        intersectInfo.subMeshId = index;

        if (fastCheck) {
          break;
        }
      }
    }
  }

  if (intersectInfo) {
    // Get picked point
    var world          = getWorldMatrix();
    var worldOrigin    = Vector3.TransformCoordinates(ray.origin, world);
    var direction      = ray.direction.clone();
    direction          = direction.scale(intersectInfo.distance);
    var worldDirection = Vector3.TransformNormal(direction, world);

    var pickedPoint = worldOrigin.add(worldDirection);

    // Return result
    pickingInfo.hit         = true;
    pickingInfo.distance    = Vector3.Distance(worldOrigin, pickedPoint);
    pickingInfo.pickedPoint = pickedPoint;
    pickingInfo.pickedMesh  = this;
    pickingInfo.bu          = intersectInfo.bu;
    pickingInfo.bv          = intersectInfo.bv;
    pickingInfo.faceId      = intersectInfo.faceId;
    pickingInfo.subMeshId   = intersectInfo.subMeshId;
    return pickingInfo;
  }

  return pickingInfo;*/

  return PickingInfo();
}

AbstractMesh* AbstractMesh::clone(const string_t& /*name*/, Node* /*newParent*/,
                                  bool /*doNotCloneChildren*/)
{
  return nullptr;
}

AbstractMesh& AbstractMesh::releaseSubMeshes()
{
  if (!subMeshes.empty()) {
    for (auto& subMesh : subMeshes) {
      subMesh->dispose();
    }
  }

  subMeshes.clear();

  return *this;
}

void AbstractMesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  // Smart Array Retainers.
  getScene()->freeActiveMeshes();
  getScene()->freeRenderingGroups();

  // Action manager
  if (actionManager) {
    actionManager->dispose();
    actionManager = nullptr;
  }

  // Skeleton
  _skeleton = nullptr;

  // Physics
  if (physicsImpostor) {
    physicsImpostor->dispose(/*!doNotRecurse*/);
  }

  // Intersections in progress
  for (auto& other : _intersectionsInProgress) {
    ::std::remove(other->_intersectionsInProgress.begin(),
                  other->_intersectionsInProgress.end(), this);
  }

  _intersectionsInProgress.clear();

  // Lights
  for (auto& light : getScene()->lights) {
    // Included meshes
    ::std::remove(light->includedOnlyMeshes().begin(),
                  light->includedOnlyMeshes().end(), this);

    // Excluded meshes
    ::std::remove(light->excludedMeshes().begin(),
                  light->excludedMeshes().end(), this);

    // Shadow generators
    auto generator = light->getShadowGenerator();
    if (generator) {
      auto shadowMap = generator->getShadowMap();
      if (shadowMap && !shadowMap->renderList().empty()) {
        ::std::remove_if(
          shadowMap->renderList().begin(), shadowMap->renderList().end(),
          [this](const AbstractMeshPtr& mesh) { return mesh.get() == this; });
      }
    }
  }

  // Edges
  if (_edgesRenderer) {
    _edgesRenderer->dispose();
    _edgesRenderer.reset(nullptr);
  }

  // SubMeshes
  if (type() != IReflect::Type::INSTANCEDMESH) {
    releaseSubMeshes();
  }

  // Octree
  auto sceneOctree = getScene()->selectionOctree();
  if (sceneOctree) {
#if 0
    sceneOctree->dynamicContent.erase(
      ::std::remove_if(
        sceneOctree->dynamicContent.begin(), sceneOctree->dynamicContent.end(),
        [this](const AbstractMeshPtr& mesh) { return mesh.get() == this; }),
      sceneOctree->dynamicContent.end());
#endif
  }

  // Query
  auto engine = getScene()->getEngine();
  if (_occlusionQuery) {
    _isOcclusionQueryInProgress = false;
    engine->deleteQuery(_occlusionQuery);
    _occlusionQuery = nullptr;
  }

  // Engine
  engine->wipeCaches();

  if (disposeMaterialAndTextures) {
    if (material()) {
      material()->dispose(false, true);
    }
  }

  if (!doNotRecurse) {
    // Particles
    for (size_t index = 0; index < getScene()->particleSystems.size();
         ++index) {
      auto& emitter = getScene()->particleSystems[index]->emitter;
      if (emitter.is<AbstractMeshPtr>()
          && (emitter.get<AbstractMeshPtr>().get() == this)) {
        getScene()->particleSystems[index]->dispose();
        --index;
      }
    }
  }

  // facet data
  if (_facetDataEnabled) {
    disableFacetData();
  }

  onAfterWorldMatrixUpdateObservable.clear();
  onCollideObservable.clear();
  onCollisionPositionChangeObservable.clear();

  _isDisposed = true;

  TransformNode::dispose(doNotRecurse, disposeMaterialAndTextures);

  // Remove from scene
  getScene()->removeMesh(this);
}

AbstractMesh& AbstractMesh::addChild(AbstractMesh& mesh)
{
  mesh.setParent(this);
  return *this;
}

AbstractMesh& AbstractMesh::removeChild(AbstractMesh& mesh)
{
  mesh.setParent(nullptr);
  return *this;
}

AbstractMesh& AbstractMesh::_initFacetData()
{
  _facetNb = getIndices().size() / 3;

  // default nb of partitioning subdivisions = 10
  _partitioningSubdivisions
    = (_partitioningSubdivisions) ? _partitioningSubdivisions : 10;
  // default ratio 1.01 = the partitioning is 1% bigger than the bounding box
  _partitioningBBoxRatio
    = (_partitioningBBoxRatio != 0.f) ? _partitioningBBoxRatio : 1.01f;

  _facetNormals.resize(_facetNb);
  ::std::fill(_facetNormals.begin(), _facetNormals.end(), Vector3::Zero());

  _facetPositions.resize(_facetNb);
  ::std::fill(_facetPositions.begin(), _facetPositions.end(), Vector3::Zero());

  _facetDataEnabled = true;

  return *this;
}

AbstractMesh& AbstractMesh::updateFacetData()
{
  if (!_facetDataEnabled) {
    _initFacetData();
  }
  auto positions = getVerticesData(VertexBuffer::PositionKind);
  auto indices   = getIndices();
  auto normals   = getVerticesData(VertexBuffer::NormalKind);
  auto bInfo     = getBoundingInfo();

  if (_facetDepthSort && !_facetDepthSortEnabled) {
    // init arrays, matrix and sort function on first call
    _facetDepthSortEnabled = true;
    // indices instanceof Uint32Array
    {
      _depthSortedIndices = Uint32Array(indices);
    }

    _facetDepthSortFunction
      = [](const DepthSortedFacet& f1, const DepthSortedFacet& f2) {
          const auto diff = f2.sqDistance - f1.sqDistance;
          return (diff < 0.f) ? -1 : (diff > 0.f) ? 1 : 0;
        };
    if (!_facetDepthSortFrom) {
      auto& camera        = getScene()->activeCamera;
      _facetDepthSortFrom = (camera) ?
                              ::std::make_unique<Vector3>(camera->position) :
                              ::std::make_unique<Vector3>(Vector3::Zero());
    }
    _depthSortedFacets.clear();
    for (unsigned int f = 0; f < _facetNb; f++) {
      DepthSortedFacet depthSortedFacet;
      depthSortedFacet.ind        = f * 3;
      depthSortedFacet.sqDistance = 0.f;
      _depthSortedFacets.emplace_back(depthSortedFacet);
    }
    _invertedMatrix       = Matrix::Identity();
    _facetDepthSortOrigin = Vector3::Zero();
  }

  _bbSize.x = (bInfo.maximum.x - bInfo.minimum.x > Math::Epsilon) ?
                bInfo.maximum.x - bInfo.minimum.x :
                Math::Epsilon;
  _bbSize.y = (bInfo.maximum.y - bInfo.minimum.y > Math::Epsilon) ?
                bInfo.maximum.y - bInfo.minimum.y :
                Math::Epsilon;
  _bbSize.z = (bInfo.maximum.z - bInfo.minimum.z > Math::Epsilon) ?
                bInfo.maximum.z - bInfo.minimum.z :
                Math::Epsilon;
  auto bbSizeMax = (_bbSize.x > _bbSize.y) ? _bbSize.x : _bbSize.y;
  bbSizeMax      = (bbSizeMax > _bbSize.z) ? bbSizeMax : _bbSize.z;
  _subDiv.max    = _partitioningSubdivisions;
  // adjust the number of subdivisions per axis
  _subDiv.X
    = static_cast<unsigned>(::std::floor(_subDiv.max * _bbSize.x / bbSizeMax));
  // according to each bbox size per axis
  _subDiv.Y
    = static_cast<unsigned>(::std::floor(_subDiv.max * _bbSize.y / bbSizeMax));
  // at least one subdivision
  _subDiv.Z
    = static_cast<unsigned>(::std::floor(_subDiv.max * _bbSize.z / bbSizeMax));
  _subDiv.X = _subDiv.X < 1 ? 1 : _subDiv.X;
  _subDiv.Y = _subDiv.Y < 1 ? 1 : _subDiv.Y;
  _subDiv.Z = _subDiv.Z < 1 ? 1 : _subDiv.Z;
  // set the parameters for ComputeNormals()
  _facetParameters.facetNormals      = getFacetLocalNormals();
  _facetParameters.facetPositions    = getFacetLocalPositions();
  _facetParameters.facetPartitioning = getFacetLocalPartitioning();
  _facetParameters.bInfo             = bInfo;
  _facetParameters.bbSize            = _bbSize;
  _facetParameters.subDiv            = _subDiv;
  _facetParameters.ratio             = partitioningBBoxRatio();
  _facetParameters.depthSort         = _facetDepthSort;
  if (_facetDepthSort && _facetDepthSortEnabled) {
    computeWorldMatrix(true);
    _worldMatrix->invertToRef(_invertedMatrix);
    Vector3::TransformCoordinatesToRef(*_facetDepthSortFrom, _invertedMatrix,
                                       _facetDepthSortOrigin);
    _facetParameters.distanceTo = _facetDepthSortOrigin;
  }
  _facetParameters.depthSortedFacets = _depthSortedFacets;
  VertexData::ComputeNormals(positions, indices, normals, _facetParameters);

  if (_facetDepthSort && _facetDepthSortEnabled) {
    ::std::sort(_depthSortedFacets.begin(), _depthSortedFacets.end(),
                _facetDepthSortFunction);
    auto l = (_depthSortedIndices.size() / 3);
    for (size_t f = 0; f < l; f++) {
      const auto& sind               = _depthSortedFacets[f].ind;
      _depthSortedIndices[f * 3]     = indices[sind];
      _depthSortedIndices[f * 3 + 1] = indices[sind + 1];
      _depthSortedIndices[f * 3 + 2] = indices[sind + 2];
    }
    updateIndices(_depthSortedIndices);
  }

  return *this;
}

vector_t<Vector3>& AbstractMesh::getFacetLocalNormals()
{
  if (_facetNormals.empty()) {
    updateFacetData();
  }
  return _facetNormals;
}

vector_t<Vector3>& AbstractMesh::getFacetLocalPositions()
{
  if (_facetPositions.empty()) {
    updateFacetData();
  }
  return _facetPositions;
}

vector_t<Uint32Array>& AbstractMesh::getFacetLocalPartitioning()
{
#if 0
  if (_facetPartitioning.empty()) {
    updateFacetData();
  }
#endif
  return _facetPartitioning;
}

Vector3 AbstractMesh::getFacetPosition(unsigned int i)
{
  auto pos = Vector3::Zero();
  getFacetPositionToRef(i, pos);
  return pos;
}

AbstractMesh& AbstractMesh::getFacetPositionToRef(unsigned int i, Vector3& ref)
{
  auto localPos = (getFacetLocalPositions())[i];
  auto world    = getWorldMatrix();
  Vector3::TransformCoordinatesToRef(localPos, *world, ref);
  return *this;
}

Vector3 AbstractMesh::getFacetNormal(unsigned int i)
{
  auto norm = Vector3::Zero();
  getFacetNormalToRef(i, norm);
  return norm;
}

AbstractMesh& AbstractMesh::getFacetNormalToRef(unsigned int i, Vector3& ref)
{
  auto localNorm = (getFacetLocalNormals())[i];
  Vector3::TransformNormalToRef(localNorm, *getWorldMatrix(), ref);
  return *this;
}

Uint32Array AbstractMesh::getFacetsAtLocalCoordinates(float x, float y, float z)
{
  auto bInfo = getBoundingInfo();

  int ox = static_cast<int>(
    ::std::floor((x - bInfo.minimum.x * _partitioningBBoxRatio) * _subDiv.X
                 * _partitioningBBoxRatio / _bbSize.x));
  int oy = static_cast<int>(
    ::std::floor((y - bInfo.minimum.y * _partitioningBBoxRatio) * _subDiv.Y
                 * _partitioningBBoxRatio / _bbSize.y));
  int oz = static_cast<int>(
    ::std::floor((z - bInfo.minimum.z * _partitioningBBoxRatio) * _subDiv.Z
                 * _partitioningBBoxRatio / _bbSize.z));

  if (ox < 0 || oy < 0 || oz < 0) {
    return Uint32Array();
  }

  unsigned int _ox = static_cast<unsigned>(ox);
  unsigned int _oy = static_cast<unsigned>(oy);
  unsigned int _oz = static_cast<unsigned>(oz);
  if (_ox > _subDiv.max || _oy > _subDiv.max || _oz > _subDiv.max) {
    return Uint32Array();
  }

  return _facetPartitioning[_ox + _subDiv.max * _oy
                            + _subDiv.max * _subDiv.max * _oz];
}

int AbstractMesh::getClosestFacetAtCoordinates(float x, float y, float z,
                                               Vector3& projected,
                                               bool projectedSet,
                                               bool checkFace, bool facing)
{
  auto world   = getWorldMatrix();
  auto& invMat = Tmp::MatrixArray[5];
  world->invertToRef(invMat);
  auto& invVect = Tmp::Vector3Array[8];
  int closest   = -1;
  // transform (x,y,z) to coordinates in the mesh local space
  Vector3::TransformCoordinatesFromFloatsToRef(x, y, z, invMat, invVect);
  closest = getClosestFacetAtLocalCoordinates(invVect.x, invVect.y, invVect.z,
                                              projected, checkFace, facing);
  if (projectedSet) {
    // tranform the local computed projected vector to world coordinates
    Vector3::TransformCoordinatesFromFloatsToRef(
      projected.x, projected.y, projected.z, *world, projected);
  }
  return closest;
}

int AbstractMesh::getClosestFacetAtLocalCoordinates(float x, float y, float z,
                                                    Vector3& projected,
                                                    bool projectedSet,
                                                    bool checkFace, bool facing)
{
  int closest = -1;
  float tmpx  = 0.f;
  float tmpy  = 0.f;
  float tmpz  = 0.f;
  float d     = 0.f; // tmp dot facet normal * facet position
  float t0    = 0.f;
  float projx = 0.f;
  float projy = 0.f;
  float projz = 0.f;
  // Get all the facets in the same partitioning block than (x, y, z)
  auto facetPositions = getFacetLocalPositions();
  auto facetNormals   = getFacetLocalNormals();
  auto facetsInBlock  = getFacetsAtLocalCoordinates(x, y, z);
  if (facetsInBlock.empty()) {
    return closest;
  }
  // Get the closest facet to (x, y, z)
  float shortest    = numeric_limits_t<float>::max(); // init distance vars
  float tmpDistance = shortest;
  size_t fib        = 0; // current facet in the block
  Vector3 norm;          // current facet normal
  Vector3 p0;            // current facet barycenter position
  // loop on all the facets in the current partitioning block
  for (size_t idx = 0; idx < facetsInBlock.size(); ++idx) {
    fib  = facetsInBlock[idx];
    norm = facetNormals[fib];
    p0   = facetPositions[fib];

    d = (x - p0.x) * norm.x + (y - p0.y) * norm.y + (z - p0.z) * norm.z;
    if (!checkFace || (checkFace && facing && d >= 0.f)
        || (checkFace && !facing && d <= 0.f)) {
      // compute (x,y,z) projection on the facet = (projx, projy, projz)
      d  = norm.x * p0.x + norm.y * p0.y + norm.z * p0.z;
      t0 = -(norm.x * x + norm.y * y + norm.z * z - d)
           / (norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
      projx = x + norm.x * t0;
      projy = y + norm.y * t0;
      projz = z + norm.z * t0;

      tmpx = projx - x;
      tmpy = projy - y;
      tmpz = projz - z;
      // compute length between (x, y, z) and its projection on the facet
      tmpDistance = tmpx * tmpx + tmpy * tmpy + tmpz * tmpz;
      // just keep the closest facet to (x, y, z)
      if (tmpDistance < shortest) {
        shortest = tmpDistance;
        closest  = static_cast<int>(fib);
        if (projectedSet) {
          projected.x = projx;
          projected.y = projy;
          projected.z = projz;
        }
      }
    }
  }
  return closest;
}

FacetParameters& AbstractMesh::getFacetDataParameters()
{
  return _facetParameters;
}

AbstractMesh& AbstractMesh::disableFacetData()
{
  if (_facetDataEnabled) {
    _facetDataEnabled = false;
    _facetPositions.clear();
    _facetNormals.clear();
    _facetPartitioning.clear();
    _facetParameters = FacetParameters();
    _depthSortedIndices.clear();
  }
  return *this;
}

AbstractMesh& AbstractMesh::updateIndices(const IndicesArray& /*indices*/)
{
  return *this;
}

AbstractMesh& AbstractMesh::createNormals(bool updatable)
{
  auto positions = getVerticesData(VertexBuffer::PositionKind);
  auto indices   = getIndices();
  Float32Array normals;

  if (isVerticesDataPresent(VertexBuffer::NormalKind)) {
    normals = getVerticesData(VertexBuffer::NormalKind);
  }

  FacetParameters options;
  options.useRightHandedSystem = getScene()->useRightHandedSystem();
  VertexData::ComputeNormals(positions, indices, normals, options);
  setVerticesData(VertexBuffer::NormalKind, normals, updatable);

  return *this;
}

AbstractMesh& AbstractMesh::alignWithNormal(Vector3& normal,
                                            const Vector3& upDirection)
{
  auto& axisX = Tmp::Vector3Array[0];
  auto& axisZ = Tmp::Vector3Array[1];
  Vector3::CrossToRef(upDirection, normal, axisZ);
  Vector3::CrossToRef(normal, axisZ, axisX);

  if (rotationQuaternion()) {
    auto rotationQuat = *rotationQuaternion();
    Quaternion::RotationQuaternionFromAxisToRef(axisX, normal, axisZ,
                                                rotationQuat);
    rotationQuaternion = rotationQuat;
  }
  else {
    Vector3::RotationFromAxisToRef(axisX, normal, axisZ, rotation());
  }
  return *this;
}

void AbstractMesh::_checkOcclusionQuery()
{
  auto engine = getEngine();

  if (engine->webGLVersion() < 2.f
      || occlusionType == AbstractMesh::OCCLUSION_TYPE_NONE) {
    _isOccluded = false;
    return;
  }

  if (isOcclusionQueryInProgress() && _occlusionQuery) {
    auto isOcclusionQueryAvailable
      = engine->isQueryResultAvailable(_occlusionQuery);
    if (isOcclusionQueryAvailable) {
      auto occlusionQueryResult = engine->getQueryResult(_occlusionQuery);

      _isOcclusionQueryInProgress    = false;
      _occlusionInternalRetryCounter = 0;
      _isOccluded                    = occlusionQueryResult == 1 ? false : true;
    }
    else {
      ++_occlusionInternalRetryCounter;

      if (occlusionRetryCount != -1
          && _occlusionInternalRetryCounter > occlusionRetryCount) {
        _isOcclusionQueryInProgress    = false;
        _occlusionInternalRetryCounter = 0;

        // if optimistic set isOccluded to false regardless of the status of
        // isOccluded. (Render in the current render loop)
        // if strict continue the last state of the object.
        _isOccluded = occlusionType == AbstractMesh::OCCLUSION_TYPE_OPTIMISTIC ?
                        false :
                        _isOccluded;
      }
      else {
        return;
      }
    }
  }

  auto scene                        = getScene();
  auto occlusionBoundingBoxRenderer = scene->getBoundingBoxRenderer();

  if (!_occlusionQuery) {
    _occlusionQuery = engine->createQuery();
  }

  engine->beginOcclusionQuery(occlusionQueryAlgorithmType, _occlusionQuery);
  occlusionBoundingBoxRenderer->renderOcclusionBoundingBox(this);
  engine->endOcclusionQuery(occlusionQueryAlgorithmType);
  _isOcclusionQueryInProgress = true;
}

} // end of namespace BABYLON
