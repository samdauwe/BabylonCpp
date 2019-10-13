#include <babylon/cameras/camera.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_follow_camera.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/camera_inputs_manager.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/universal_camera.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/culling/icullable.h>
#include <babylon/culling/ray.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/multiview_render_target.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/frustum.h>
#include <babylon/misc/serialization_helper.h>
#include <babylon/misc/tools.h>
#include <babylon/postprocesses/anaglyph_post_process.h>
#include <babylon/postprocesses/pass_post_process.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/stereoscopic_interlace_post_process.h>

namespace BABYLON {

bool Camera::ForceAttachControlToAlwaysPreventDefault = false;

CameraPtr Camera::_createDefaultParsedCamera(const std::string& iName,
                                             Scene* scene)
{
  return UniversalCamera::New(iName, Vector3::Zero(), scene);
}

Camera::Camera(const std::string& iName, const Vector3& iPosition, Scene* scene,
               bool setActiveOnSceneIfNoneActive)
    : Node{iName, scene}
    , _position{Vector3::Zero()}
    , position{this, &Camera::get_position, &Camera::set_position}
    , upVector{Vector3::Up()}
    , orthoLeft{0.f}
    , orthoRight{0.f}
    , orthoBottom{0.f}
    , orthoTop{0.f}
    , fov{0.8f}
    , minZ{1.f}
    , maxZ{10000.f}
    , inertia{0.9f}
    , mode{Camera::PERSPECTIVE_CAMERA}
    , isIntermediate{false}
    , viewport{Viewport(0.f, 0.f, 1.f, 1.f)}
    , layerMask{0x0FFFFFFF}
    , fovMode{Camera::FOVMODE_VERTICAL_FIXED}
    , cameraRigMode{Camera::RIG_MODE_NONE}
    , outputRenderTarget{nullptr}
    , _skipRendering{false}
    , _projectionMatrix{Matrix()}
    , _computedViewMatrix{Matrix::Identity()}
    , globalPosition{this, &Camera::get_globalPosition}
    , _isCamera{true}
    , _isLeftCamera{false}
    , isLeftCamera{this, &Camera::get_isLeftCamera}
    , _isRightCamera{false}
    , isRightCamera{this, &Camera::get_isRightCamera}
    , _useMultiviewToSingleView{false}
    , _multiviewTexture{nullptr}
    , _webvrViewMatrix{Matrix::Identity()}
    , _globalPosition{Vector3::Zero()}
    , _doNotComputeProjectionMatrix{false}
    , _transformMatrix{Matrix::Zero()}
    , _webvrProjectionMatrix{Matrix::Identity()}
    , _refreshFrustumPlanes{true}
    , _setActiveOnSceneIfNoneActive{setActiveOnSceneIfNoneActive}
{
  position = iPosition;
  _initCache();
}

Camera::~Camera()
{
}

Type Camera::type() const
{
  return Type::CAMERA;
}

void Camera::addToScene(const CameraPtr& newCamera)
{
  if (_setActiveOnSceneIfNoneActive && !getScene()->activeCamera()) {
    getScene()->activeCamera = newCamera;
  }

  newCamera->addToRootNodes();
  getScene()->addCamera(newCamera);
}

Vector3& Camera::get_position()
{
  return _position;
}

void Camera::set_position(const Vector3& newPosition)
{
  _position = newPosition;
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

const std::string Camera::getClassName() const
{
  return "Camera";
}

std::string Camera::toString(bool fullDetails) const
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

std::vector<AbstractMesh*>& Camera::getActiveMeshes()
{
  return _activeMeshes;
}

bool Camera::isActiveMesh(const AbstractMeshPtr& mesh)
{
  return std::find(_activeMeshes.begin(), _activeMeshes.end(), mesh.get())
         != _activeMeshes.end();
}

bool Camera::isReady(bool completeCheck, bool /*forceInstanceSupport*/)
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

  _cache.position = Vector3(std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max());
  _cache.upVector = Vector3(std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max());

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

  if (mode == Camera::PERSPECTIVE_CAMERA) {
    check = stl_util::almost_equal(_cache.fov, fov) && _cache.fovMode == fovMode
            && stl_util::almost_equal(_cache.aspectRatio,
                                      engine->getAspectRatio(*this));
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
  if (cameraRigMode != Camera::RIG_MODE_NONE) {
    _updateRigCameras();
  }
}

void Camera::_checkInputs()
{
  onAfterCheckInputsObservable.notifyObservers(this);
}

std::vector<CameraPtr>& Camera::rigCameras()
{
  return _rigCameras;
}

const std::vector<CameraPtr>& Camera::rigCameras() const
{
  return _rigCameras;
}

PostProcessPtr& Camera::rigPostProcess()
{
  return _rigPostProcess;
}

PostProcessPtr Camera::_getFirstPostProcess() const
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
      auto isPass = rigPostProcess->getEffectName() == "pass";
      if (isPass) {
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

int Camera::attachPostProcess(const PostProcessPtr& postProcess, int insertAt)
{
  if (!postProcess->isReusable()
      && stl_util::index_of(_postProcesses, postProcess) > -1) {
    BABYLON_LOG_WARN(
      "Camera",
      "You're trying to reuse a post process not defined as reusable.")
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

void Camera::detachPostProcess(const PostProcessPtr& postProcess)
{
  auto idx = stl_util::index_of(_postProcesses, postProcess);
  if (idx != -1) {
    _postProcesses[static_cast<size_t>(idx)] = nullptr;
  }
  _cascadePostProcessesToRigCams(); // also ensures framebuffer invalidated
}

void Camera::detachPostProcess(PostProcess* postProcess)
{
  auto idx = stl_util::index_of_raw_ptr(_postProcesses, postProcess);
  if (idx != -1) {
    _postProcesses[static_cast<size_t>(idx)] = nullptr;
  }
  _cascadePostProcessesToRigCams(); // also ensures framebuffer invalidated
}

Matrix& Camera::getWorldMatrix()
{
  if (_isSynchronizedViewMatrix()) {
    return _worldMatrix;
  }

  // Getting the the view matrix will also compute the world matrix.
  getViewMatrix();

  return _worldMatrix;
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

  updateCache();
  _computedViewMatrix = _getViewMatrix();
  _currentRenderId    = getScene()->getRenderId();
  ++_childUpdateId;

  _refreshFrustumPlanes = true;

  if (_cameraRigParams.vrPreViewMatrixSet) {
    _computedViewMatrix.multiplyToRef(_cameraRigParams.vrPreViewMatrix,
                                      _computedViewMatrix);
  }

  // Notify parent camera if rig camera is changed
  if (parent()) {
    auto parentCamera = static_cast<Camera*>(parent());
    if (parentCamera) {
      parentCamera->onViewMatrixChangedObservable.notifyObservers(parentCamera);
    }
  }

  onViewMatrixChangedObservable.notifyObservers(this);

  _computedViewMatrix.invertToRef(_worldMatrix);

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
  if (mode == Camera::PERSPECTIVE_CAMERA) {
    _cache.fov         = fov;
    _cache.fovMode     = fovMode;
    _cache.aspectRatio = engine->getAspectRatio(*this);

    if (minZ <= 0.f) {
      minZ = 0.1f;
    }

    if (scene->useRightHandedSystem()) {
      Matrix::PerspectiveFovRHToRef(fov, engine->getAspectRatio(*this), minZ,
                                    maxZ, _projectionMatrix,
                                    fovMode == Camera::FOVMODE_VERTICAL_FIXED);
    }
    else {
      Matrix::PerspectiveFovLHToRef(fov, engine->getAspectRatio(*this), minZ,
                                    maxZ, _projectionMatrix,
                                    fovMode == Camera::FOVMODE_VERTICAL_FIXED);
    }
  }
  else {
    auto halfWidth  = static_cast<float>(engine->getRenderWidth()) / 2.f;
    auto halfHeight = static_cast<float>(engine->getRenderHeight()) / 2.f;
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

Matrix& Camera::getTransformationMatrix()
{
  _computedViewMatrix.multiplyToRef(_projectionMatrix, _transformMatrix);
  return _transformMatrix;
}

void Camera::_updateFrustumPlanes()
{
  if (!_refreshFrustumPlanes) {
    return;
  }

  getTransformationMatrix();

  if (_frustumPlanes.empty()) {
    _frustumPlanes = Frustum::GetPlanes(_transformMatrix);
  }
  else {
    Frustum::GetPlanesToRef(_transformMatrix, _frustumPlanes);
  }

  _refreshFrustumPlanes = false;
}

bool Camera::isInFrustum(ICullable* target, bool checkRigCameras)
{
  _updateFrustumPlanes();

  if (checkRigCameras && !rigCameras().empty()) {
    bool result = false;
    for (const auto& cam : rigCameras()) {
      cam->_updateFrustumPlanes();
      result = result || target->isInFrustum(cam->_frustumPlanes);
    }
    return result;
  }
  else {
    return target->isInFrustum(_frustumPlanes);
  }
}

bool Camera::isCompletelyInFrustum(ICullable* target)
{
  _updateFrustumPlanes();

  return target->isCompletelyInFrustum(_frustumPlanes);
}

Ray Camera::getForwardRay(float length, const std::optional<Matrix>& iTransform,
                          const std::optional<Vector3>& iOrigin)
{
  const auto transform
    = iTransform.has_value() ? *iTransform : getWorldMatrix();

  const auto origin = iOrigin.has_value() ? *iOrigin : position();

  const auto forward = _scene->useRightHandedSystem ? Vector3(0.f, 0.f, -1.f) :
                                                      Vector3(0.f, 0.f, 1.f);
  const auto forwardWorld = Vector3::TransformNormal(forward, transform);

  const auto direction = Vector3::Normalize(forwardWorld);

  return Ray(origin, direction, length);
}

void Camera::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  // Observables
  onViewMatrixChangedObservable.clear();
  onProjectionMatrixChangedObservable.clear();
  onAfterCheckInputsObservable.clear();
  onRestoreStateObservable.clear();

  // Inputs
  inputs.clear();

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
  else if (cameraRigMode != Camera::RIG_MODE_NONE) {
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

bool Camera::get_isLeftCamera() const
{
  return _isLeftCamera;
}

bool Camera::get_isRightCamera() const
{
  return _isRightCamera;
}

FreeCameraPtr Camera::leftCamera()
{
  if (_rigCameras.size() < 1) {
    return nullptr;
  }
  return std::static_pointer_cast<FreeCamera>(_rigCameras[0]);
}

FreeCameraPtr Camera::rightCamera()
{
  if (_rigCameras.size() < 2) {
    return nullptr;
  }
  return std::static_pointer_cast<FreeCamera>(_rigCameras[1]);
}

Vector3* Camera::getLeftTarget()
{
  if (_rigCameras.size() < 1) {
    return nullptr;
  }
  return &std::static_pointer_cast<TargetCamera>(_rigCameras[0])->getTarget();
}

Vector3* Camera::getRightTarget()
{
  if (_rigCameras.size() < 2) {
    return nullptr;
  }
  return &std::static_pointer_cast<TargetCamera>(_rigCameras[1])->getTarget();
}

void Camera::setCameraRigMode(unsigned int iMode,
                              const RigParamaters& rigParams)
{
  if (cameraRigMode == iMode) {
    return;
  }

  for (const auto& camera : _rigCameras) {
    if (camera) {
      camera->dispose();
    }
  }
  _rigCameras.clear();
  cameraRigMode    = iMode;
  _cameraRigParams = {};
  // we have to implement stereo camera calcultating left and right viewpoints
  // from interaxialDistance and target, not from a given angle as it is now,
  // but until that complete code rewriting provisional stereoHalfAngle value is
  // introduced
  _cameraRigParams.interaxialDistance
    = rigParams.interaxialDistance.value_or(0.0637f);
  _cameraRigParams.stereoHalfAngle
    = Tools::ToRadians(_cameraRigParams.interaxialDistance / 0.0637f);

  // create the rig cameras, unless none
  if (cameraRigMode != Camera::RIG_MODE_NONE) {
    auto leftCamera = createRigCamera(name + "_L", 0);
    if (leftCamera) {
      leftCamera->_isLeftCamera = true;
    }
    auto rightCamera = createRigCamera(name + "_R", 1);
    if (rightCamera) {
      rightCamera->_isRightCamera = true;
    }
    if (leftCamera && rightCamera) {
      _rigCameras.emplace_back(leftCamera);
      _rigCameras.emplace_back(rightCamera);
    }
  }

  switch (cameraRigMode) {
    case Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH:
      Camera::_setStereoscopicAnaglyphRigMode(*this);
      break;
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL:
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED:
    case Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER:
      Camera::_setStereoscopicRigMode(*this);
      break;
    case Camera::RIG_MODE_VR:
      Camera::_setVRRigMode(*this, rigParams);
      break;
    case Camera::RIG_MODE_WEBVR:
      Camera::_setWebVRRigMode(*this, rigParams);
      break;
  }

  _cascadePostProcessesToRigCams();
  update();
}

void Camera::_setStereoscopicRigMode(Camera& camera)
{
  const auto isStereoscopicHoriz
    = camera.cameraRigMode == Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL
      || camera.cameraRigMode
           == Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED;

  camera._rigCameras[0]->_rigPostProcess = PassPostProcess::New(
    camera.name + "_passthru", 1.f, camera._rigCameras[0]);
  camera._rigCameras[1]->_rigPostProcess
    = StereoscopicInterlacePostProcess::New(camera.name + "_stereoInterlace",
                                            camera._rigCameras,
                                            isStereoscopicHoriz);
}

void Camera::_setStereoscopicAnaglyphRigMode(Camera& camera)
{
  camera._rigCameras[0]->_rigPostProcess = PassPostProcess::New(
    camera.name + "_passthru", 1.f, camera._rigCameras[0]);
  camera._rigCameras[1]->_rigPostProcess = AnaglyphPostProcess::New(
    camera.name + "_anaglyph", 1.f, camera._rigCameras);
}

void Camera::_setVRRigMode(Camera& /*camera*/,
                           const RigParamaters& /*rigParams*/)
{
}

void Camera::_setWebVRRigMode(Camera& /*camera*/,
                              const RigParamaters& /*rigParams*/)
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

Matrix& Camera::_getWebVRProjectionMatrix()
{
  return _webvrProjectionMatrix;
}

Matrix& Camera::_getWebVRViewMatrix()
{
  return _webvrViewMatrix;
}

void Camera::setCameraRigParameter(const std::string& _name, float value)
{
  _cameraRigParams.params[_name] = value;
  // provisionnally:
  if (name == "interaxialDistance") {
    _cameraRigParams.stereoHalfAngle = Tools::ToRadians(value / 0.0637f);
  }
}

CameraPtr Camera::createRigCamera(const std::string& /*name*/,
                                  int /*cameraIndex*/)
{
  return nullptr;
}

void Camera::_updateRigCameras()
{
  for (const auto& rigCamera : _rigCameras) {
    rigCamera->minZ = minZ;
    rigCamera->maxZ = maxZ;
    rigCamera->fov  = fov;
    rigCamera->upVector.copyFrom(upVector);
  }

  // only update viewport when ANAGLYPH
  if (cameraRigMode == Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH) {
    _rigCameras[0]->viewport = _rigCameras[1]->viewport = viewport;
  }
}

void Camera::_setupInputs()
{
}

json Camera::serialize() const
{
  return nullptr;
}

Camera* Camera::clone(const std::string& /*name*/)
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
  Vector3::TransformNormalToRef(localAxis, getWorldMatrix(), result);
}

Matrix& Camera::computeWorldMatrix(bool /*force*/, bool /*useWasUpdatedFlag*/)
{
  return getWorldMatrix();
}

void Camera::_resizeOrCreateMultiviewTexture(int width, int height)
{
  if (!_multiviewTexture) {
    _multiviewTexture
      = MultiviewRenderTarget::New(getScene(), ISize{width, height});
  }
  else if (_multiviewTexture->getRenderWidth() != width
           || _multiviewTexture->getRenderHeight() != height) {
    _multiviewTexture->dispose();
    _multiviewTexture
      = MultiviewRenderTarget::New(getScene(), ISize{width, height});
  }
}

std::function<CameraPtr()> Camera::GetConstructorFromName(
  const std::string& type, const std::string& iName, Scene* scene,
  float interaxial_distance, bool isStereoscopicSideBySide)
{
  auto constructorFunc = Node::Construct(
    type, iName, scene,
    json{{"interaxial_distance", interaxial_distance},
         {"isStereoscopicSideBySide", isStereoscopicSideBySide}});

  if (constructorFunc) {
    return [constructorFunc]() {
      return std::static_pointer_cast<Camera>(constructorFunc());
    };
  }

  // Default to universal camera
  return [iName, scene]() {
    return UniversalCamera::New(iName, Vector3::Zero(), scene);
  };
}

CameraPtr Camera::Parse(const json& parsedCamera, Scene* scene)
{
  auto type      = json_util::get_string(parsedCamera, "type");
  auto construct = Camera::GetConstructorFromName(
    type, json_util::get_string(parsedCamera, "name"), scene,
    json_util::get_number<float>(parsedCamera, "interaxial_distance"),
    json_util::get_bool(parsedCamera, "isStereoscopicSideBySide"));

  auto camera = SerializationHelper::Parse(construct, parsedCamera, scene);

  // Parent
  if (json_util::has_valid_key_value(parsedCamera, "parentId")) {
    camera->_waitingParentId = json_util::get_string(parsedCamera, "parentId");
  }

  // If camera has an input manager, let it parse inputs settings
  // if (camera.inputs) {
  //  camera->inputs.parse(parsedCamera);
  //  camera->_setupInputs();
  // }

  // Need to force position
  if (camera->type() == Type::ARCROTATECAMERA) {
    camera->position().copyFromFloats(0.f, 0.f, 0.f);
    if (auto arcRotateCamera
        = std::static_pointer_cast<ArcRotateCamera>(camera)) {
      arcRotateCamera->setPosition(Vector3::FromArray(
        json_util::get_array<float>(parsedCamera, "position")));
    }
  }

  // Target
  if (json_util::has_valid_key_value(parsedCamera, "target")) {
    if (auto targetCamera = std::static_pointer_cast<TargetCamera>(camera)) {
      targetCamera->setTarget(Vector3::FromArray(
        json_util::get_array<float>(parsedCamera, "target")));
    }
  }

  // Apply 3d rig, when found
  if (json_util::has_valid_key_value(parsedCamera, "cameraRigMode")) {
    RigParamaters rigParams{};
    if (json_util::has_key(parsedCamera, "interaxial_distance")) {
      rigParams.interaxialDistance
        = json_util::get_number<float>(parsedCamera, "interaxial_distance");
    }
    camera->setCameraRigMode(
      json_util::get_number<unsigned int>(parsedCamera, "cameraRigMode"),
      rigParams);
  }

  // Animations
  if (json_util::has_key(parsedCamera, "animations")) {
    for (const auto& parsedAnimation :
         json_util::get_array<json>(parsedCamera, "animations")) {
      camera->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
    Node::ParseAnimationRanges(*camera, parsedCamera, scene);
  }

  if (json_util::has_key(parsedCamera, "autoAnimate")) {
    scene->beginAnimation(
      camera, json_util::get_number(parsedCamera, "autoAnimateFrom", 0.f),
      json_util::get_number(parsedCamera, "autoAnimateTo", 0.f),
      json_util::get_bool(parsedCamera, "autoAnimateLoop"),
      json_util::get_number(parsedCamera, "autoAnimateSpeed", 1.f));
  }

  return camera;
}

} // end of namespace BABYLON
