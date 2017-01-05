#include <babylon/cameras/camera.h>

#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/culling/icullable.h>
#include <babylon/math/frustum.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

bool Camera::ForceAttachControlToAlwaysPreventDefault = false;

Camera::Camera(const std::string& iName, const Vector3& iPosition, Scene* scene)
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
    , mode{Camera::PERSPECTIVE_CAMERA}
    , isIntermediate{false}
    , viewport{Viewport(0, 0, 1.f, 1.f)}
    , layerMask{0x0FFFFFFF}
    , fovMode{Camera::FOVMODE_VERTICAL_FIXED}
    , cameraRigMode{Camera::RIG_MODE_NONE}
    , _projectionMatrix{Matrix()}
    , _computedViewMatrix{Matrix::Identity()}
    , _doNotComputeProjectionMatrix{false}
    , _transformMatrix{Matrix::Zero()}
    , _webvrViewMatrix{Matrix::Identity()}
    , _globalPosition{Vector3::Zero()}
    , _refreshFrustumPlanes{true}
{
  _initCache();

  /*scene->addCamera(shared_from_this());

  if (!scene->activeCamera) {
    scene->activeCamera = this;
  }*/
}

Camera::~Camera()
{
}

IReflect::Type Camera::type() const
{
  return IReflect::Type::CAMERA;
}

void Camera::addToScene(std::unique_ptr<Camera>&& newCamera)
{
  if (!getScene()->activeCamera) {
    getScene()->activeCamera = newCamera.get();
  }

  getScene()->addCamera(std::move(newCamera));
}

std::string Camera::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name;
  oss << ", type: " << getTypeName();
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

Vector3& Camera::globalPosition()
{
  return _globalPosition;
}

std::vector<Mesh*>& Camera::getActiveMeshes()
{
  return _activeMeshes;
}

bool Camera::isActiveMesh(Mesh* mesh)
{
  return std::find(_activeMeshes.begin(), _activeMeshes.end(), mesh)
         != _activeMeshes.end();
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

  auto engine = getEngine();

  _cache.position.copyFrom(position);
  _cache.upVector.copyFrom(upVector);

  _cache.mode = mode;
  _cache.minZ = minZ;
  _cache.maxZ = maxZ;

  _cache.fov         = fov;
  _cache.fovMode     = fovMode;
  _cache.aspectRatio = engine->getAspectRatio(this);

  _cache.orthoLeft    = orthoLeft;
  _cache.orthoRight   = orthoRight;
  _cache.orthoBottom  = orthoBottom;
  _cache.orthoTop     = orthoTop;
  _cache.renderWidth  = engine->getRenderWidth();
  _cache.renderHeight = engine->getRenderHeight();
}

void Camera::_updateFromScene()
{
  updateCache();
  _update();
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
  bool check = _cache.mode == mode && std_util::almost_equal(_cache.minZ, minZ)
               && std_util::almost_equal(_cache.maxZ, maxZ);
  if (!check) {
    return false;
  }

  auto engine = getEngine();

  if (mode == Camera::PERSPECTIVE_CAMERA) {
    check = std_util::almost_equal(_cache.fov, fov) && _cache.fovMode == fovMode
            && std_util::almost_equal(_cache.aspectRatio,
                                      engine->getAspectRatio(this));
  }
  else {
    check = std_util::almost_equal(_cache.orthoLeft, orthoLeft)
            && std_util::almost_equal(_cache.orthoRight, orthoRight)
            && std_util::almost_equal(_cache.orthoBottom, orthoBottom)
            && std_util::almost_equal(_cache.orthoTop, orthoTop)
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

void Camera::_update()
{
  if (cameraRigMode != Camera::RIG_MODE_NONE) {
    _updateRigCameras();
  }
  _checkInputs();
}

void Camera::_checkInputs()
{
}

void Camera::_cascadePostProcessesToRigCams()
{
  // invalidate framebuffer
  if (!_postProcesses.empty()) {
    _postProcesses[0]->markTextureDirty();
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
      cam->_postProcesses = std_util::slice(_postProcesses, 0);
      cam->_postProcesses.emplace_back(rigPostProcess);
      rigPostProcess->markTextureDirty();
    }
    else {
      cam->_postProcesses = std_util::slice(_postProcesses, 0);
    }
  }
}

int Camera::attachPostProcess(PostProcess* postProcess, int insertAt)
{
  if (!postProcess->isReusable()
      && std_util::index_of(_postProcesses, postProcess) > -1) {
    BABYLON_LOG_WARN(
      "Camera",
      "You're trying to reuse a post process not defined as reusable.");
    return 0;
  }

  if (insertAt < 0) {
    _postProcesses.emplace_back(postProcess);
  }
  else {
    std_util::splice(_postProcesses, insertAt, 0, {postProcess});
  }
  _cascadePostProcessesToRigCams(); // also ensures framebuffer invalidated
  return std_util::index_of(_postProcesses, postProcess);
}

Int32Array Camera::detachPostProcess(PostProcess* postProcess,
                                     const Uint32Array& atIndices)
{
  Int32Array result;
  size_t i;
  if (!atIndices.empty()) {
    _postProcesses.erase(
      std::remove(_postProcesses.begin(), _postProcesses.end(), postProcess),
      _postProcesses.end());
  }
  else {
    // iterate descending, so can just splice as we go
    for (i = atIndices.size(); i > 0; --i) {
      if (_postProcesses[atIndices[i - 1]] != postProcess) {
        result.emplace_back(i - 1);
        continue;
      }
      std_util::splice(_postProcesses, static_cast<int>(i), 1);
    }
  }
  _cascadePostProcessesToRigCams(); // also ensures framebuffer invalidated
  return result;
}

Matrix* Camera::getWorldMatrix()
{
  if (!_worldMatrix) {
    _worldMatrix = std_util::make_unique<Matrix>(Matrix::Identity());
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
  _computedViewMatrix = _computeViewMatrix(force);

  if (!force && _isSynchronizedViewMatrix()) {
    return _computedViewMatrix;
  }

  _refreshFrustumPlanes = true;

  if (!parent() || !parent()->getWorldMatrix()) {
    _globalPosition.copyFrom(position);
  }
  else {
    if (!_worldMatrix) {
      _worldMatrix = std_util::make_unique<Matrix>(Matrix::Identity());
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

  _currentRenderId = getScene()->getRenderId();

  return _computedViewMatrix;
}

Matrix& Camera::_computeViewMatrix(bool force)
{
  if (!force && _isSynchronizedViewMatrix()) {
    return _computedViewMatrix;
  }

  _computedViewMatrix = _getViewMatrix();
  _currentRenderId    = getScene()->getRenderId();

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

  _refreshFrustumPlanes = true;

  auto engine = getEngine();
  auto scene  = getScene();
  if (mode == Camera::PERSPECTIVE_CAMERA) {
    if (minZ <= 0.f) {
      minZ = 0.1f;
    }

    if (scene->useRightHandedSystem) {
      Matrix::PerspectiveFovRHToRef(fov, engine->getAspectRatio(this), minZ,
                                    maxZ, _projectionMatrix,
                                    fovMode == Camera::FOVMODE_VERTICAL_FIXED);
    }
    else {
      Matrix::PerspectiveFovLHToRef(fov, engine->getAspectRatio(this), minZ,
                                    maxZ, _projectionMatrix,
                                    fovMode == Camera::FOVMODE_VERTICAL_FIXED);
    }
    return _projectionMatrix;
  }

  float halfWidth  = static_cast<float>(engine->getRenderWidth()) / 2.f;
  float halfHeight = static_cast<float>(engine->getRenderHeight()) / 2.f;
  if (scene->useRightHandedSystem) {
    Matrix::OrthoOffCenterRHToRef(
      !std_util::almost_equal(orthoLeft, 0.f) ? orthoLeft : -halfWidth,
      !std_util::almost_equal(orthoRight, 0.f) ? orthoRight : halfWidth,
      !std_util::almost_equal(orthoBottom, 0.f) ? orthoBottom : -halfHeight,
      !std_util::almost_equal(orthoTop, 0.f) ? orthoTop : halfHeight, minZ,
      maxZ, _projectionMatrix);
  }
  else {
    Matrix::OrthoOffCenterLHToRef(
      !std_util::almost_equal(orthoLeft, 0.f) ? orthoLeft : -halfWidth,
      !std_util::almost_equal(orthoRight, 0.f) ? orthoRight : halfWidth,
      !std_util::almost_equal(orthoBottom, 0.f) ? orthoBottom : -halfHeight,
      !std_util::almost_equal(orthoTop, 0.f) ? orthoTop : halfHeight, minZ,
      maxZ, _projectionMatrix);
  }
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

void Camera::dispose(bool /*doNotRecurse*/)
{
  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  getScene()->removeCamera(this);
  for (auto& rigCamera : _rigCameras) {
    rigCamera->dispose();
  }
  _rigCameras.clear();

  // Postprocesses
  for (auto& postProcess : _postProcesses) {
    postProcess->dispose(this);
  }

  _postProcesses.clear();

  Node::dispose();
}

// ---- Camera rigs section ----
void Camera::setCameraRigMode(int /*iMode*/, const std::string& /*rigParams*/)
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

Matrix& Camera::_getWebVRProjectionMatrix()
{
  return _projectionMatrix;
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

/**
 * May needs to be overridden by children so sub has required properties
 * to be copied
 */
Camera* Camera::createRigCamera(const std::string& /*name*/,
                                int /*cameraIndex*/)
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
  if (cameraRigMode == Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH) {
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

std::string Camera::getTypeName() const
{
  return "Camera";
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
  Vector3::TransformNormalToRef(localAxis, *getWorldMatrix(), result);
}

Camera* Camera::Parse(const Json::value& /*parsedCamera*/, Scene* /*scene*/)
{
  return nullptr;
}

} // end of namespace BABYLON
