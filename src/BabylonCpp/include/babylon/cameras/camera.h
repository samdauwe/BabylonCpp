#ifndef BABYLON_CAMERAS_CAMERA_H
#define BABYLON_CAMERAS_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>
#include <babylon/math/viewport.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Camera : public Node {

public:
  /** Statics **/
  static constexpr unsigned int PERSPECTIVE_CAMERA  = 0;
  static constexpr unsigned int ORTHOGRAPHIC_CAMERA = 1;

  static constexpr unsigned int FOVMODE_VERTICAL_FIXED   = 0;
  static constexpr unsigned int FOVMODE_HORIZONTAL_FIXED = 1;

  static constexpr unsigned int RIG_MODE_NONE                              = 0;
  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_ANAGLYPH             = 10;
  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL  = 11;
  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED = 12;
  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_OVERUNDER            = 13;
  static constexpr unsigned int RIG_MODE_VR                                = 20;
  static constexpr unsigned int RIG_MODE_WEBVR                             = 21;

  static bool ForceAttachControlToAlwaysPreventDefault;

public:
  virtual ~Camera();

  virtual IReflect::Type type() const override;
  void addToScene(unique_ptr_t<Camera>&& newCamera);

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  string_t toString(bool fullDetails = false) const;

  Vector3& globalPosition();
  vector_t<Mesh*>& getActiveMeshes();
  bool isActiveMesh(Mesh* mesh);

  /** Cache **/
  virtual void _initCache() override;
  void _updateCache(bool ignoreParentClass) override;
  void _updateFromScene();

  /** Synchronized **/
  bool _isSynchronized() override;
  bool _isSynchronizedViewMatrix();
  bool _isSynchronizedProjectionMatrix();

  /** Controls **/
  virtual void attachControl(ICanvas* canvas, bool noPreventDefault = false,
                             bool useCtrlForPanning = true,
                             MouseButtonType panningMouseButton
                             = MouseButtonType::RIGHT);
  virtual void detachControl(ICanvas* canvas);
  void _update();
  virtual void _checkInputs();
  vector_t<Camera*>& rigCameras();
  const vector_t<Camera*>& rigCameras() const;
  PostProcess* rigPostProcess();
  int attachPostProcess(PostProcess* postProcess, int insertAt = -1);
  Int32Array detachPostProcess(PostProcess* postProcess,
                               const Uint32Array& atIndices = Uint32Array());
  Matrix* getWorldMatrix() override;
  virtual Matrix _getViewMatrix();
  Matrix& getViewMatrix(bool force = false);
  void freezeProjectionMatrix();
  void freezeProjectionMatrix(const Matrix& projection);
  void unfreezeProjectionMatrix();
  Matrix& getProjectionMatrix(bool force = false);
  Matrix& getTranformationMatrix();
  bool isInFrustum(ICullable* target);
  bool isCompletelyInFrustum(ICullable* target);
  Ray getForwardRay(float length = 100.f);
  Ray getForwardRay(float length, const Matrix& transform);
  Ray getForwardRay(float length, const Matrix& transform,
                    const Vector3& origin);
  virtual void dispose(bool doNotRecurse = false) override;

  /** Camera rigs section **/
  FreeCamera* leftCamera();
  FreeCamera* rightCamera();
  Vector3* getLeftTarget();
  Vector3* getRightTarget();
  void setCameraRigMode(int mode, const string_t& rigParams);
  void setCameraRigParameter(const string_t& name, float value);
  virtual Camera* createRigCamera(const string_t& name, int cameraIndex);
  virtual void _updateRigCameras();
  virtual void _setupInputs();
  virtual Json::object serialize() const;
  virtual const char* getClassName() const override;
  Camera* clone(const string_t& name);
  Vector3 getDirection(const Vector3& localAxis);
  void getDirectionToRef(const Vector3& localAxis, Vector3& result);

  // Statics
  static Camera* GetConstructorFromName(const string_t& type,
                                        const string_t& name, Scene* scene,
                                        float interaxial_distance     = 0.f,
                                        bool isStereoscopicSideBySide = true);
  static Camera* Parse(const Json::value& parsedCamera, Scene* scene);

protected:
  Camera(const string_t& name, const Vector3& position, Scene* scene);

private:
  void _cascadePostProcessesToRigCams();
  void updateFrustumPlanes();
  Matrix& _getVRProjectionMatrix();
  virtual void _updateCameraRotationMatrix();
  virtual void _updateWebVRCameraRotationMatrix();
  virtual Matrix& _getWebVRProjectionMatrix();
  virtual Matrix& _getWebVRViewMatrix();

public:
  /** Members **/
  Vector3 position;
  Vector3 upVector;
  float orthoLeft;
  float orthoRight;
  float orthoBottom;
  float orthoTop;
  float fov;
  float minZ;
  float maxZ;
  float inertia;
  unsigned int mode;
  bool isIntermediate;
  Viewport viewport;
  unsigned int layerMask;
  unsigned int fovMode;

  /** Camera rig members **/
  unsigned int cameraRigMode;
  float interaxialDistance;
  bool isStereoscopicSideBySide;
  CameraRigParams _cameraRigParams;
  vector_t<Camera*> _rigCameras;
  PostProcess* _rigPostProcess;
  vector_t<RenderTargetTexture*> customRenderTargets;

  /** Observables **/
  Observable<Camera> onViewMatrixChangedObservable;
  Observable<Camera> onProjectionMatrixChangedObservable;
  Observable<Camera> onAfterCheckInputsObservable;

  /** Cache **/
  Matrix _projectionMatrix;
  vector_t<PostProcess*> _postProcesses;
  Uint32Array _postProcessesTakenIndices;
  vector_t<Mesh*> _activeMeshes;

private:
  Matrix _computedViewMatrix;
  bool _doNotComputeProjectionMatrix;
  unique_ptr_t<Matrix> _worldMatrix;
  Matrix _transformMatrix;
  Matrix _webvrProjectionMatrix;
  Matrix _webvrViewMatrix;
  Vector3 _globalPosition;
  array_t<Plane, 6> _frustumPlanes;
  bool _refreshFrustumPlanes;

}; // end of class Camera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_CAMERA_H
