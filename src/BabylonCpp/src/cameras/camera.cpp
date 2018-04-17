#include <babylon/cameras/camera.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_follow_camera.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/culling/icullable.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/frustum.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/tools/serialization_helper.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

bool Camera::ForceAttachControlToAlwaysPreventDefault = false;
bool Camera::UseAlternateWebVRRendering               = false;

Camera::Camera(const string_t& iName, const Vector3& iPosition, Scene* scene)
    : Node(iName, scene)
    , position{iPosition}
    , upVector{Vector3::Up()}
    , orthoLeft{0.f}
    , orthoRight{0.f}
    , orthoBottom{0.f}
    , orthoTop{0.f}
    , fov{0.8f}
    , minZ{1.f}
    , maxZ{10000.f}
    , inertia{0.9f}
    , mode{Camera::PERSPECTIVE_CAMERA()}
    , isIntermediate{false}
    , viewport{Viewport(0, 0, 1, 1)}
    , layerMask{0x0FFFFFFF}
    , fovMode{Camera::FOVMODE_VERTICAL_FIXED()}
    , cameraRigMode{Camera::RIG_MODE_NONE()}
    , _skipRendering{false}
    , _alternateCamera{nullptr}
    , _projectionMatrix{Matrix()}
    , globalPosition{this, &Camera::get_globalPosition}
    , _webvrViewMatrix{Matrix::Identity()}
    , _computedViewMatrix{Matrix::Identity()}
    , _doNotComputeProjectionMatrix{false}
    , _transformMatrix{Matrix::Zero()}
    , _webvrProjectionMatrix{Matrix::Identity()}
    , _globalPosition{Vector3::Zero()}
    , _refreshFrustumPlanes{true}
{
  _initCache();
}

Camera::~Camera()
{
}

IReflect::Type Camera::type() const
{
  return IReflect::Type::CAMERA;
}

void Camera::addToScene(unique_ptr_t<Camera>&& newCamera)
{
  if (!getScene()->activeCamera) {
    getScene()->activeCamera = newCamera.get();
  }

  getScene()->addCamera(::std::move(newCamera));
}

Camera& Camera::storeState()
{
  _stateStored = true;
  _storedFov   = fov;

  return *this;
}

bool Camera::_restoreStateValues()
{
  if (!_stateStored) {
    return false;
  }

  fov = _storedFov;

  return true;
}

bool Camera::restoreState()
{
  if (_restoreStateValues()) {
    onRestoreStateObservable.notifyObservers(this);
    return true;
  }

  return false;
}

string_t Camera::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name;
  oss << ", type: " << getClassName();
  if (!animations.empty()) {
    for (size_t i = 0; i < animations.size(); ++i) {
      oss << ", animation[" << i << "]: ";
      oss << animations[i]->toString(fullDetails);
    }
  }
  if (fullDetails) {
  }
  return oss.str();
}

Vector3& Camera::get_globalPosition()
{
  return _globalPosition;
}

vector_t<AbstractMesh*>& Camera::getActiveMeshes()
{
  return _activeMeshes;
}

bool Camera::isActiveMesh(AbstractMesh* mesh)
{
  return ::std::find(_activeMeshes.begin(), _activeMeshes.end(), mesh)
         != _activeMeshes.end();
}

bool Camera::isReady(bool completeCheck) const
{
  if (completeCheck) {
    for (auto& pp : _postProcesses) {
      if (pp && !pp->isReady()) {
        return false;
      }
    }
  }
  return Node::isReady(completeCheck);
}

// Cache
void Camera::_initCache()
{
  Node::_initCache();

  _cache.position
    = Vector3(numeric_limits_t<float>::max(), numeric_limits_t<float>::max(),
              numeric_limits_t<float>::max());
  _cache.upVector
    = Vector3(numeric_limits_t<float>::max(), numeric_limits_t<float>::max(),
              numeric_limits_t<float>::max());

  _cache.mode = 0;
  _cache.minZ = 0.f;
  _cache.maxZ = 0.f;

  _cache.fov         = 0.f;
  _cache.fovMode     = 0;
  _cache.aspectRatio = 0.f;

  _cache.orthoLeft    = 0.f;
  _cache.orthoRight   = 0.f;
  _cache.orthoBottom  = 0.f;
  _cache.orthoTop     = 0.f;
  _cache.renderWidth  = 0;
  _cache.renderHeight = 0;
}

void Camera::_updateCache(bool ignoreParentClass)
{
  if (!ignoreParentClass) {
    Node::_updateCache();
  }

  _cache.position.copyFrom(position);
  _cache.upVector.copyFrom(upVector);
}

// Synchronized
bool Camera::_isSynchronized()
{
  return _isSynchronizedViewMatrix() && _isSynchronizedProjectionMatrix();
}

bool Camera::_isSynchronizedViewMatrix()
{
  if (!Node::_isSynchronized()) {
    return false;
  }

  return _cache.position.equals(position) && _cache.upVector.equals(upVector)
         && isSynchronizedWithParent();
}

bool Camera::_isSynchronizedProjectionMatrix()
{
  bool check = _cache.mode == mode && stl_util::almost_equal(_cache.minZ, minZ)
               && stl_util::almost_equal(_cache.maxZ, maxZ);
  if (!check) {
    return false;
  }

  auto engine = getEngine();

  if (mode == Camera::PERSPECTIVE_CAMERA()) {
    check = stl_util::almost_equal(_cache.fov, fov) && _cache.fovMode == fovMode
            && stl_util::almost_equal(_cache.aspectRatio,
                                      engine->getAspectRatio(this));
  }
  else {
    check = stl_util::almost_equal(_cache.orthoLeft, orthoLeft)
            && stl_util::almost_equal(_cache.orthoRight, orthoRight)
            && stl_util::almost_equal(_cache.orthoBottom, orthoBottom)
            && stl_util::almost_equal(_cache.orthoTop, orthoTop)
            && _cache.renderWidth == engine->getRenderWidth()
            && _cache.renderHeight == engine->getRenderHeight();
  }

  return check;
}

// Controls
void Camera::attachControl(ICanvas* /*canvas*/, bool /*noPreventDefault*/,
                           bool /*useCtrlForPanning*/,
                           MouseButtonType /*panningMouseButton*/)
{
}

void Camera::detachControl(ICanvas* /*canvas*/)
{
}

void Camera::update()
{
  _checkInputs();
  if (cameraRigMode != Camera::RIG_MODE_NONE()) {
    _updateRigCameras();
  }
}

void Camera::_checkInputs()
{
  onAfterCheckInputsObservable.notifyObservers(this);
}

vector_t<Camera*>& Camera::rigCameras()
{
  return _rigCameras;
}

const vector_t<Camera*>& Camera::rigCameras() const
{
  return _rigCameras;
}

PostProcess* Camera::rigPostProcess()
{
  return _rigPostProcess;
}

PostProcess* Camera::_getFirstPostProcess() const
{
  for (const auto& pp : _postProcesses) {
    if (pp != nullptr) {
      return pp;
    }
  }

  return nullptr;
}

void Camera::_cascadePostProcessesToRigCams()
{
  // invalidate framebuffer
  auto firstPostProcess = _getFirstPostProcess();
  if (firstPostProcess) {
    firstPostProcess->markTextureDirty();
  }

  // glue the rigPostProcess to the end of the user postprocesses & assign to
  // each sub-camera
  for (auto& cam : _rigCameras) {
    auto rigPostProcess = cam->_rigPostProcess;

    // for VR rig, there does not have to be a post process
    if (rigPostProcess) {
      auto _passPostProcess = dynamic_cast<PassPostProcess*>(rigPostProcess);
      if (_passPostProcess) {
        // any rig which has a PassPostProcess for rig[0], cannot be
        // isIntermediate when there are also user postProcesses
        cam->isIntermediate = _postProcesses.empty();
      }
      cam->_postProcesses = stl_util::slice(_postProcesses, 0);
      cam->_postProcesses.emplace_back(rigPostProcess);
      rigPostProcess->markTextureDirty();
    }
    else {
      cam->_postProcesses = stl_util::slice(_postProcesses, 0);
    }
  }
}

int Camera::attachPostProcess(PostProcess* postProcess, int insertAt)
{
  if (!postProcess->isReusable()
      && stl_util::index_of(_postProcesses, postProcess) > -1) {
    BABYLON_LOG_WARN(
      "Camera",
      "You're trying to reuse a post process not defined as reusable.");
    return 0;
  }

  if (insertAt < 0) {
    _postProcesses.emplace_back(postProcess);
  }
  else if (static_cast<size_t>(insertAt) < _postProcesses.size()
           && _postProcesses[static_cast<size_t>(insertAt)] == nullptr) {
    _postProcesses[static_cast<size_t>(insertAt)] = postProcess;
  }
  else {
    stl_util::splice(_postProcesses, insertAt, 0, {postProcess});
  }
  _cascadePostProcessesToRigCams(); // also ensures framebuffer invalidated
  return stl_util::index_of(_postProcesses, postProcess);
}

void Camera::detachPostProcess(PostProcess* postProcess)
{
  auto idx = stl_util::index_of(_postProcesses, postProcess);
  if (idx != -1) {
    _postProcesses[static_cast<size_t>(idx)] = nullptr;
  }
  _cascadePostProcessesToRigCams(); // also ensures framebuffer invalidated
}

Matrix* Camera::getWorldMatrix()
{
  if (!_worldMatrix) {
    _worldMatrix = ::std::make_unique<Matrix>(Matrix::Identity());
  }

  auto viewMatrix = getViewMatrix();

  viewMatrix.invertToRef(*_worldMatrix);

  return _worldMatrix.get();
}

Matrix Camera::_getViewMatrix()
{
  return Matrix::Identity();
}

Matrix& Camera::getViewMatrix(bool force)
{
  if (!force && _isSynchronizedViewMatrix()) {
    return _computedViewMatrix;
  }

  _updateCache();
  _computedViewMatrix = _getViewMatrix();
  _currentRenderId    = getScene()->getRenderId();
  _childRenderId      = _currentRenderId;

  _refreshFrustumPlanes = true;

  if (!parent() || !parent()->getWorldMatrix()) {
    _globalPosition.copyFrom(position);
  }
  else {
    if (!_worldMatrix) {
      _worldMatrix = ::std::make_unique<Matrix>(Matrix::Identity());
    }

    _computedViewMatrix.invertToRef(*_worldMatrix);

    _worldMatrix->multiplyToRef(*parent()->getWorldMatrix(),
                                _computedViewMatrix);
    _globalPosition.copyFromFloats(_computedViewMatrix.m[12],
                                   _computedViewMatrix.m[13],
                                   _computedViewMatrix.m[14]);

    _computedViewMatrix.invert();

    _markSyncedWithParent();
  }

  if (_cameraRigParams.vrPreViewMatrixSet) {
    _computedViewMatrix.multiplyToRef(_cameraRigParams.vrPreViewMatrix,
                                      _computedViewMatrix);
  }

  onViewMatrixChangedObservable.notifyObservers(this);

  return _computedViewMatrix;
}

void Camera::freezeProjectionMatrix()
{
  _doNotComputeProjectionMatrix = true;
}

void Camera::freezeProjectionMatrix(const Matrix& projection)
{
  _doNotComputeProjectionMatrix = true;
  _projectionMatrix             = projection;
}

void Camera::unfreezeProjectionMatrix()
{
  _doNotComputeProjectionMatrix = false;
}

Matrix& Camera::getProjectionMatrix(bool force)
{
  if (_doNotComputeProjectionMatrix
      || (!force && _isSynchronizedProjectionMatrix())) {
    return _projectionMatrix;
  }

  // Cache
  _cache.mode = mode;
  _cache.minZ = minZ;
  _cache.maxZ = maxZ;

  // Matrix
  _refreshFrustumPlanes = true;

  auto engine = getEngine();
  auto scene  = getScene();
  if (mode == Camera::PERSPECTIVE_CAMERA()) {
    _cache.fov         = fov;
    _cache.fovMode     = fovMode;
    _cache.aspectRatio = engine->getAspectRatio(this);

    if (minZ <= 0.f) {
      minZ = 0.1f;
    }

    if (scene->useRightHandedSystem()) {
      Matrix::PerspectiveFovRHToRef(
        fov, engine->getAspectRatio(this), minZ, maxZ, _projectionMatrix,
        fovMode == Camera::FOVMODE_VERTICAL_FIXED());
    }
    else {
      Matrix::PerspectiveFovLHToRef(
        fov, engine->getAspectRatio(this), minZ, maxZ, _projectionMatrix,
        fovMode == Camera::FOVMODE_VERTICAL_FIXED());
    }
  }
  else {
    float halfWidth  = static_cast<float>(engine->getRenderWidth()) / 2.f;
    float halfHeight = static_cast<float>(engine->getRenderHeight()) / 2.f;
    if (scene->useRightHandedSystem()) {
      Matrix::OrthoOffCenterRHToRef(
        !stl_util::almost_equal(orthoLeft, 0.f) ? orthoLeft : -halfWidth,
        !stl_util::almost_equal(orthoRight, 0.f) ? orthoRight : halfWidth,
        !stl_util::almost_equal(orthoBottom, 0.f) ? orthoBottom : -halfHeight,
        !stl_util::almost_equal(orthoTop, 0.f) ? orthoTop : halfHeight, minZ,
        maxZ, _projectionMatrix);
    }
    else {
      Matrix::OrthoOffCenterLHToRef(
        !stl_util::almost_equal(orthoLeft, 0.f) ? orthoLeft : -halfWidth,
        !stl_util::almost_equal(orthoRight, 0.f) ? orthoRight : halfWidth,
        !stl_util::almost_equal(orthoBottom, 0.f) ? orthoBottom : -halfHeight,
        !stl_util::almost_equal(orthoTop, 0.f) ? orthoTop : halfHeight, minZ,
        maxZ, _projectionMatrix);
    }

    _cache.orthoLeft    = orthoLeft;
    _cache.orthoRight   = orthoRight;
    _cache.orthoBottom  = orthoBottom;
    _cache.orthoTop     = orthoTop;
    _cache.renderWidth  = engine->getRenderWidth();
    _cache.renderHeight = engine->getRenderHeight();
  }

  onProjectionMatrixChangedObservable.notifyObservers(this);

  return _projectionMatrix;
}

Matrix& Camera::getTranformationMatrix()
{
  _computedViewMatrix.multiplyToRef(_projectionMatrix, _transformMatrix);
  return _transformMatrix;
}

void Camera::updateFrustumPlanes()
{
  if (!_refreshFrustumPlanes) {
    return;
  }

  getTranformationMatrix();

  if (_frustumPlanes.empty()) {
    _frustumPlanes = Frustum::GetPlanes(_transformMatrix);
  }
  else {
    Frustum::GetPlanesToRef(_transformMatrix, _frustumPlanes);
  }

  _refreshFrustumPlanes = false;
}

bool Camera::isInFrustum(ICullable* target)
{
  updateFrustumPlanes();

  return target->isInFrustum(_frustumPlanes);
}

bool Camera::isCompletelyInFrustum(ICullable* target)
{
  updateFrustumPlanes();

  return target->isCompletelyInFrustum(_frustumPlanes);
}

Ray Camera::getForwardRay(float length)
{
  return getForwardRay(length, *getWorldMatrix(), position);
}

Ray Camera::getForwardRay(float length, const Matrix& transform)
{
  return getForwardRay(length, transform, position);
}

Ray Camera::getForwardRay(float length, const Matrix& transform,
                          const Vector3& origin)
{
  Vector3 forward(0.f, 0.f, 1.f);
  auto forwardWorld = Vector3::TransformNormal(forward, transform);

  auto direction = Vector3::Normalize(forwardWorld);

  return Ray(origin, direction, length);
}

void Camera::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  // Observables
  onViewMatrixChangedObservable.clear();
  onProjectionMatrixChangedObservable.clear();
  onAfterCheckInputsObservable.clear();
  onRestoreStateObservable.clear();

  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  getScene()->removeCamera(this);
  for (auto& rigCamera : _rigCameras) {
    if (rigCamera) {
      rigCamera->dispose();
    }
  }
  _rigCameras.clear();

  // Postprocesses
  if (_rigPostProcess) {
    _rigPostProcess->dispose(this);
    _rigPostProcess = nullptr;
    _postProcesses.clear();
  }
  else if (cameraRigMode != Camera::RIG_MODE_NONE()) {
    _rigPostProcess = nullptr;
    _postProcesses.clear();
  }
  else {
    for (size_t i = _postProcesses.size(); i-- > 0;) {
      if (_postProcesses[i]) {
        _postProcesses[i]->dispose(this);
      }
    }
    _postProcesses.clear();
  }

  // Render targets
  auto i = customRenderTargets.size();
  while (i-- > 0) {
    customRenderTargets[i]->dispose();
  }
  customRenderTargets.clear();

  // Active Meshes
  _activeMeshes.clear();

  Node::dispose(doNotRecurse, disposeMaterialAndTextures);
}

// ---- Camera rigs section ----
FreeCamera* Camera::leftCamera()
{
  if (_rigCameras.size() < 1) {
    return nullptr;
  }
  return static_cast<FreeCamera*>(_rigCameras[0]);
}

FreeCamera* Camera::rightCamera()
{
  if (_rigCameras.size() < 2) {
    return nullptr;
  }
  return static_cast<FreeCamera*>(_rigCameras[1]);
}

Vector3* Camera::getLeftTarget()
{
  if (_rigCameras.size() < 1) {
    return nullptr;
  }
  return &static_cast<TargetCamera*>(_rigCameras[0])->getTarget();
}

Vector3* Camera::getRightTarget()
{
  if (_rigCameras.size() < 2) {
    return nullptr;
  }
  return &static_cast<TargetCamera*>(_rigCameras[1])->getTarget();
}

void Camera::setCameraRigMode(int /*iMode*/, const string_t& /*rigParams*/)
{
}

Matrix& Camera::_getVRProjectionMatrix()
{
  Matrix::PerspectiveFovLHToRef(_cameraRigParams.vrMetrics.aspectRatioFov,
                                _cameraRigParams.vrMetrics.aspectRatio, minZ,
                                maxZ, _cameraRigParams.vrWorkMatrix);
  _cameraRigParams.vrWorkMatrix.multiplyToRef(_cameraRigParams.vrHMatrix,
                                              _projectionMatrix);
  return _projectionMatrix;
}

void Camera::_updateCameraRotationMatrix()
{
  // Here for WebVR
}

void Camera::_updateWebVRCameraRotationMatrix()
{
  // Here for WebVR
}

/**
 * This function MUST be overwritten by the different WebVR cameras available.
 * The context in which it is running is the RIG camera. So 'this' is the
 * TargetCamera, left or right.
 */
Matrix& Camera::_getWebVRProjectionMatrix()
{
  return _webvrProjectionMatrix;
}

/**
 * This function MUST be overwritten by the different WebVR cameras available.
 * The context in which it is running is the RIG camera. So 'this' is the
 * TargetCamera, left or right.
 */
Matrix& Camera::_getWebVRViewMatrix()
{
  return _webvrViewMatrix;
}

void Camera::setCameraRigParameter(const string_t& _name, float value)
{
  _cameraRigParams.params[_name] = value;
  // provisionnally:
  if (name == "interaxialDistance") {
    _cameraRigParams.stereoHalfAngle = Tools::ToRadians(value / 0.0637f);
  }
}

/**
 * May needs to be overridden by children so sub has required properties
 * to be copied
 */
Camera* Camera::createRigCamera(const string_t& /*name*/, int /*cameraIndex*/)
{
  return nullptr;
}

/**
 * May needs to be overridden by children
 */
void Camera::_updateRigCameras()
{
  for (auto& rigCamera : _rigCameras) {
    rigCamera->minZ = minZ;
    rigCamera->maxZ = maxZ;
    rigCamera->fov  = fov;
  }

  // only update viewport when ANAGLYPH
  if (cameraRigMode == Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH()) {
    _rigCameras[0]->viewport = _rigCameras[1]->viewport = viewport;
  }
}

void Camera::_setupInputs()
{
}

Json::object Camera::serialize() const
{
  return Json::object();
}

const string_t Camera::getClassName() const
{
  return "Camera";
}

Camera* Camera::clone(const string_t& /*name*/)
{
  return nullptr;
}

Vector3 Camera::getDirection(const Vector3& localAxis)
{
  auto result = Vector3::Zero();

  getDirectionToRef(localAxis, result);

  return result;
}

void Camera::getDirectionToRef(const Vector3& localAxis, Vector3& result)
{
  Vector3::TransformNormalToRef(localAxis, *getWorldMatrix(), result);
}

Matrix& Camera::computeWorldMatrix(bool /*force*/)
{
  return *getWorldMatrix();
}

Camera* Camera::GetConstructorFromName(const string_t& type,
                                       const string_t& name, Scene* scene,
                                       float /*interaxial_distance*/,
                                       bool /*isStereoscopicSideBySide*/)
{
  if (type == "ArcRotateCamera") {
    return ArcRotateCamera::New(name, 0.f, 0.f, 1.f, Vector3::Zero(), scene);
  }
  else if (type == "FollowCamera") {
    return FollowCamera::New(name, Vector3::Zero(), scene);
  }
  else if (type == "ArcFollowCamera") {
    return ArcFollowCamera::New(name, 0.f, 0.f, 1.f, nullptr, scene);
  }
  else if (type == "FreeCamera") {
    return FreeCamera::New(name, Vector3::Zero(), scene);
  }

  return nullptr;
}

Camera* Camera::Parse(const Json::value& parsedCamera, Scene* scene)
{
  auto type      = Json::GetString(parsedCamera, "type");
  auto construct = Camera::GetConstructorFromName(
    type, Json::GetString(parsedCamera, "name"), scene,
    Json::GetNumber<float>(parsedCamera, "interaxial_distance", 0.f),
    Json::GetBool(parsedCamera, "isStereoscopicSideBySide", true));
  if (!construct) {
    return nullptr;
  }

  auto camera = SerializationHelper::Parse(construct, parsedCamera, scene);

  // Parent
  if (parsedCamera.contains("parentId")) {
    camera->_waitingParentId = Json::GetString(parsedCamera, "parentId");
  }

  // Animations
  if (parsedCamera.contains("animations")) {
    for (auto parsedAnimation : Json::GetArray(parsedCamera, "animations")) {
      camera->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
    Node::ParseAnimationRanges(camera, parsedCamera, scene);
  }

  if (parsedCamera.contains("autoAnimate")) {
    scene->beginAnimation(
      camera, Json::GetNumber(parsedCamera, "autoAnimateFrom", 0),
      Json::GetNumber(parsedCamera, "autoAnimateTo", 0),
      Json::GetBool(parsedCamera, "autoAnimateLoop"),
      Json::GetNumber(parsedCamera, "autoAnimateSpeed", 1.f));
  }

  return camera;
}

} // end of namespace BABYLON
