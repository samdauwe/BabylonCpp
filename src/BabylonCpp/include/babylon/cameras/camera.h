#ifndef BABYLON_CAMERAS_CAMERA_H
#define BABYLON_CAMERAS_CAMERA_H

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>
#include <babylon/engine/node.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>
#include <babylon/math/vector3.h>
#include <babylon/math/viewport.h>
#include <babylon/tools/observable.h>

using json = nlohmann::json;

namespace BABYLON {

class Camera;
struct ICullable;
class FreeCamera;
class PostProcess;
class Ray;
class RenderTargetTexture;
using CameraPtr              = std::shared_ptr<Camera>;
using FreeCameraPtr          = std::shared_ptr<FreeCamera>;
using PostProcessPtr         = std::shared_ptr<PostProcess>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief This is the base class of all the camera used in the application.
 * @see http://doc.babylonjs.com/features/cameras
 */
class BABYLON_SHARED_EXPORT Camera : public Node {

public:
  /**
   * This is the default projection mode used by the cameras.
   * It helps recreating a feeling of perspective and better appreciate depth.
   * This is the best way to simulate real life cameras.
   */
  static constexpr unsigned int PERSPECTIVE_CAMERA = 0;
  /**
   * This helps creating camera with an orthographic mode.
   * Orthographic is commonly used in engineering as a means to produce object
   * specifications that communicate dimensions unambiguously, each line of 1
   * unit length (cm, meter..whatever) will appear to have the same length
   * everywhere on the drawing. This allows the drafter to dimension only a
   * subset of lines and let the reader know that other lines of that length on
   * the drawing are also that length in reality. Every parallel line in the
   * drawing is also parallel in the object.
   */
  static constexpr unsigned int ORTHOGRAPHIC_CAMERA = 1;

  /**
   * This is the default FOV mode for perspective cameras.
   * This setting aligns the upper and lower bounds of the viewport to the upper
   * and lower bounds of the camera frustum.
   */
  static constexpr unsigned int FOVMODE_VERTICAL_FIXED = 0;
  /**
   * This setting aligns the left and right bounds of the viewport to the left
   * and right bounds of the camera frustum.
   */
  static constexpr unsigned int FOVMODE_HORIZONTAL_FIXED = 1;

  /**
   * This specifies ther is no need for a camera rig.
   * Basically only one eye is rendered corresponding to the camera.
   */
  static constexpr unsigned int RIG_MODE_NONE = 0;
  /**
   * Simulates a camera Rig with one blue eye and one red eye.
   * This can be use with 3d blue and red glasses.
   */
  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_ANAGLYPH = 10;
  /**
   * Defines that both eyes of the camera will be rendered side by side with a
   * parallel target.
   */
  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL = 11;
  /**
   * Defines that both eyes of the camera will be rendered side by side with a
   * none parallel target.
   */
  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED = 12;
  /**
   * Defines that both eyes of the camera will be rendered over under each
   * other.
   */
  static constexpr unsigned int RIG_MODE_STEREOSCOPIC_OVERUNDER = 13;
  /**
   * Defines that both eyes of the camera should be renderered in a VR mode
   * (carbox).
   */
  static constexpr unsigned int RIG_MODE_VR = 20;
  /**
   * Defines that both eyes of the camera should be renderered in a VR mode
   * (webVR).
   */
  static constexpr unsigned int RIG_MODE_WEBVR = 21;

  /**
   * Defines if by default attaching controls should prevent the default
   * javascript event to continue.
   */
  static bool ForceAttachControlToAlwaysPreventDefault;

  /**
   * Might be removed once multiview will be a thing
   */
  static bool UseAlternateWebVRRendering;

public:
  virtual ~Camera() override;

  virtual IReflect::Type type() const override;
  void addToScene(const CameraPtr& newCamera);

  /**
   * @brief Store current camera state (fov, position, etc..)
   * @returns the camera
   */
  virtual Camera& storeState();

  /**
   * @brief Restored camera state. You must call storeState() first.
   * @returns true if restored and false otherwise
   */
  bool restoreState();

  /**
   * @brief Gets a string representation of the camera usefull for debug
   * purpose.
   * @param fullDetails Defines that a more verboe level of logging is required
   * @returns the string representation
   */
  std::string toString(bool fullDetails = false) const;

  /**
   * @brief Gets the list of active meshes this frame (meshes no culled or
   * excluded by lod s in the frame).
   * @returns the active meshe list
   */
  std::vector<AbstractMeshPtr>& getActiveMeshes();

  /**
   * @brief Check wether a mesh is part of the current active mesh list of the
   * camera.
   * @param mesh Defines the mesh to check
   * @returns true if active, false otherwise
   */
  bool isActiveMesh(const AbstractMeshPtr& mesh);

  /**
   * @brief Is this camera ready to be used/rendered.
   * @param completeCheck defines if a complete check (including post processes)
   * has to be done (false by default)
   * @return true if the camera is ready
   */
  bool isReady(bool completeCheck        = false,
               bool forceInstanceSupport = false) override;

  /**
   * @brief Hidden
   */
  virtual void _initCache() override;

  /**
   * @brief Hidden
   */
  void _updateCache(bool ignoreParentClass = false) override;

  /**
   * @brief Hidden
   */
  bool _isSynchronized() override;

  /**
   * @brief Hidden
   */
  virtual bool _isSynchronizedViewMatrix();

  /**
   * @brief Hidden
   */
  bool _isSynchronizedProjectionMatrix();

  /**
   * @brief Attach the input controls to a specific dom element to get the input
   * from.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  virtual void attachControl(ICanvas* canvas, bool noPreventDefault = false,
                             bool useCtrlForPanning = true,
                             MouseButtonType panningMouseButton
                             = MouseButtonType::RIGHT);

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param element Defines the element to stop listening the inputs from
   */
  virtual void detachControl(ICanvas* canvas);

  /**
   * @brief Update the camera state according to the different inputs gathered
   * during the frame.
   */
  void update();

  /**
   * @brief Hidden
   */
  virtual void _checkInputs();

  /**
   * @brief Hidden
   */
  std::vector<CameraPtr>& rigCameras();

  /**
   * @brief Hidden
   */
  const std::vector<CameraPtr>& rigCameras() const;

  /**
   * @brief Gets the post process used by the rig cameras.
   */
  PostProcess* rigPostProcess();

  /**
   * @brief Internal, gets the first post proces.
   * @returns the first post process to be run on this camera.
   */
  PostProcessPtr _getFirstPostProcess() const;

  /**
   * @brief Attach a post process to the camera.
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_postprocesses#attach-postprocess
   * @param postProcess The post process to attach to the camera
   * @param insertAt The position of the post process in case several of them
   * are in use in the scene
   * @returns the position the post process has been inserted at
   */
  int attachPostProcess(const PostProcessPtr& postProcess, int insertAt = -1);

  /**
   * @brief Detach a post process to the camera.
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_postprocesses#attach-postprocess
   * @param postProcess The post process to detach from the camera
   */
  void detachPostProcess(const PostProcessPtr& postProcess);

  /**
   * @brief Detach a post process to the camera.
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_postprocesses#attach-postprocess
   * @param postProcess The post process to detach from the camera
   */
  void detachPostProcess(PostProcess* postProcess);

  /**
   * @brief Gets the current world matrix of the camera.
   */
  Matrix& getWorldMatrix() override;

  /**
   * @brief Gets the current view matrix of the camera.
   * @param force forces the camera to recompute the matrix without looking at
   * the cached state
   * @returns the view matrix
   */
  Matrix& getViewMatrix(bool force = false);

  /**
   * @brief Freeze the projection matrix.
   * It will prevent the cache check of the camera projection compute and can
   * speed up perf if no parameter of the camera are meant to change
   * @param projection Defines manually a projection if necessary
   */
  void freezeProjectionMatrix();

  /**
   * @brief Freeze the projection matrix.
   * It will prevent the cache check of the camera projection compute and can
   * speed up perf if no parameter of the camera are meant to change
   * @param projection Defines manually a projection if necessary
   */
  void freezeProjectionMatrix(const Matrix& projection);

  /**
   * @brief Unfreeze the projection matrix if it has previously been freezed by
   * freezeProjectionMatrix.
   */
  void unfreezeProjectionMatrix();

  /**
   * @brief Gets the current projection matrix of the camera.
   * @param force forces the camera to recompute the matrix without looking at
   * the cached state
   * @returns the projection matrix
   */
  Matrix& getProjectionMatrix(bool force = false);

  /**
   * @brief Gets the transformation matrix (ie. the multiplication of view by
   * projection matrices).
   * @returns a Matrix
   */
  Matrix& getTransformationMatrix();

  /**
   * @brief Checks if a cullable object (mesh...) is in the camera frustum.
   * This checks the bounding box center. See isCompletelyInFrustum for a full
   * bounding check
   * @param target The object to check
   * @returns true if the object is in frustum otherwise false
   */
  bool isInFrustum(ICullable* target);

  /**
   * @brief Checks if a cullable object (mesh...) is in the camera frustum.
   * Unlike isInFrustum this cheks the full bounding box
   * @param target The object to check
   * @returns true if the object is in frustum otherwise false
   */
  bool isCompletelyInFrustum(ICullable* target);

  /**
   * @brief Gets a ray in the forward direction from the camera.
   * @param length Defines the length of the ray to create
   * @returns the forward ray
   */
  Ray getForwardRay(float length = 100.f);

  /**
   * @brief Gets a ray in the forward direction from the camera.
   * @param length Defines the length of the ray to create
   * @param transform Defines the transform to apply to the ray, by default the
   * world matrx is used to create a workd space ray
   * @returns the forward ray
   */
  Ray getForwardRay(float length, const Matrix& transform);

  /**
   * @brief Gets a ray in the forward direction from the camera.
   * @param length Defines the length of the ray to create
   * @param transform Defines the transform to apply to the ray, by default the
   * world matrx is used to create a workd space ray
   * @param origin Defines the start point of the ray which defaults to the
   * camera position
   * @returns the forward ray
   */
  Ray getForwardRay(float length, const Matrix& transform,
                    const Vector3& origin);

  /**
   * @brief Releases resources associated with this node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Gets the left camera of a rig setup in case of Rigged Camera.
   */
  FreeCameraPtr leftCamera();

  /**
   * @brief Gets the right camera of a rig setup in case of Rigged Camera.
   */
  FreeCameraPtr rightCamera();

  /**
   * @brief Gets the left camera target of a rig setup in case of Rigged Camera.
   * @returns the target position
   */
  Vector3* getLeftTarget();

  /**
   * @brief Gets the right camera target of a rig setup in case of Rigged
   * Camera.
   * @returns the target position
   */
  Vector3* getRightTarget();

  /**
   * @brief Hidden
   */
  void setCameraRigMode(int mode, const json& rigParams);

  /**
   * @brief Hidden
   */
  void setCameraRigParameter(const std::string& name, float value);

  /**
   * @brief Hidden
   */
  virtual CameraPtr createRigCamera(const std::string& name, int cameraIndex);

  /**
   * @brief Hidden
   */
  virtual void _updateRigCameras();
  /**
   * @brief Hidden
   */
  virtual void _setupInputs();

  /**
   * @brief Serialiaze the camera setup to a json represention.
   * @returns the JSON representation
   */
  virtual json serialize() const;

  /**
   * @brief Gets the class name of the camera.
   * @returns the class name
   */
  virtual const std::string getClassName() const override;

  /**
   * @brief Clones the current camera.
   * @param name The cloned camera name
   * @returns the cloned camera
   */
  Camera* clone(const std::string& name);

  /**
   * @brief Gets the direction of the camera relative to a given local axis.
   * @param localAxis Defines the reference axis to provide a relative
   * direction.
   * @return the direction
   */
  Vector3 getDirection(const Vector3& localAxis);

  /**
   * @brief Gets the direction of the camera relative to a given local axis into
   * a passed vector.
   * @param localAxis Defines the reference axis to provide a relative
   * direction.
   * @param result Defines the vector to store the result in
   */
  void getDirectionToRef(const Vector3& localAxis, Vector3& result);

  /**
   * @brief Compute the world  matrix of the camera.
   * @returns the camera world matrix
   */
  Matrix& computeWorldMatrix(bool force             = false,
                             bool useWasUpdatedFlag = false) override;

  /**
   * @brief Gets a camera constructor for a given camera type
   * @param type The type of the camera to construct (should be equal to one of
   * the camera class name)
   * @param name The name of the camera the result will be able to instantiate
   * @param scene The scene the result will construct the camera in
   * @param interaxial_distance In case of stereoscopic setup, the distance
   * between both eyes
   * @param isStereoscopicSideBySide In case of stereoscopic setup, should the
   * sereo be side b side
   * @returns a factory method to construc the camera
   */
  static CameraPtr GetConstructorFromName(const std::string& type,
                                          const std::string& name, Scene* scene,
                                          float interaxial_distance     = 0.f,
                                          bool isStereoscopicSideBySide = true);

  /**
   * @brief Parse a JSON and creates the camera from the parsed information
   * @param parsedCamera The JSON to parse
   * @param scene The scene to instantiate the camera in
   * @returns the newly constructed camera
   */
  static CameraPtr Parse(const json& parsedCamera, Scene* scene);

protected:
  /**
   * @brief Instantiates a new camera object.
   * This should not be used directly but through the inherited cameras:
   * ArcRotate, Free...
   * @see http://doc.babylonjs.com/features/cameras
   * @param name Defines the name of the camera in the scene
   * @param position Defines the position of the camera
   * @param scene Defines the scene the camera belongs too
   * @param setActiveOnSceneIfNoneActive Defines if the camera should be set as
   * active after creation if no other camera have been defined in the scene
   */
  Camera(const std::string& name, const Vector3& position, Scene* scene,
         bool setActiveOnSceneIfNoneActive = true);

  /**
   * @brief Hidden
   */
  virtual Matrix _getViewMatrix();

  virtual bool _restoreStateValues();

private:
  /**
   * @brief Gets the current world space position of the camera.
   */
  Vector3& get_globalPosition();
  void _cascadePostProcessesToRigCams();
  void _updateFrustumPlanes();
  Matrix& _getVRProjectionMatrix();
  virtual void _updateCameraRotationMatrix();
  virtual void _updateWebVRCameraRotationMatrix();
  virtual Matrix& _getWebVRProjectionMatrix();
  virtual Matrix& _getWebVRViewMatrix();

public:
  /**
   * Define the input manager associated with the camera.
   */
  CameraInputsManager<Camera> inputs;

  /**
   * Define the current local position of the camera in the scene
   */
  Vector3 position;

  /**
   * The vector the camera should consider as up.
   * (default is Vector3(0, 1, 0) aka Vector3.Up())
   */
  Vector3 upVector;

  /**
   * Define the current limit on the left side for an orthographic camera
   * In scene unit
   */
  float orthoLeft;

  /**
   * Define the current limit on the right side for an orthographic camera
   * In scene unit
   */
  float orthoRight;

  /**
   * Define the current limit on the bottom side for an orthographic camera
   * In scene unit
   */
  float orthoBottom;

  /**
   * Define the current limit on the top side for an orthographic camera
   * In scene unit
   */
  float orthoTop;

  /**
   * Field Of View is set in Radians. (default is 0.8)
   */
  float fov;

  /**
   * Define the minimum distance the camera can see from.
   * This is important to note that the depth buffer are not infinite and the
   * closer it starts the more your scene might encounter depth fighting issue.
   */
  float minZ;

  /**
   * Define the maximum distance the camera can see to.
   * This is important to note that the depth buffer are not infinite and the
   * further it end the more your scene might encounter depth fighting issue.
   */
  float maxZ;

  /**
   * Define the default inertia of the camera.
   * This helps giving a smooth feeling to the camera movement.
   */
  float inertia;

  /**
   * Define the mode of the camera (Camera.PERSPECTIVE_CAMERA or
   * Camera.PERSPECTIVE_ORTHOGRAPHIC)
   */
  unsigned int mode;

  /**
   * Define wether the camera is intermediate.
   * This is usefull to not present the output directly to the screen in case of
   * rig without post process for instance
   */
  bool isIntermediate;

  /**
   * Define the viewport of the camera.
   * This correspond to the portion of the screen the camera will render to in
   * normalized 0 to 1 unit.
   */
  Viewport viewport;

  /**
   * Restricts the camera to viewing objects with the same layerMask.
   * A camera with a layerMask of 1 will render mesh.layerMask &
   * camera.layerMask!== 0
   */
  unsigned int layerMask;

  /**
   * fovMode sets the camera frustum bounds to the viewport bounds. (default is
   * FOVMODE_VERTICAL_FIXED)
   */
  unsigned int fovMode;

  /**
   * Rig mode of the camera.
   * This is usefull to create the camera with two "eyes" instead of one to
   * create VR or stereoscopic scenes. This is normally controlled byt the
   * camera themselves as internal use.
   */
  unsigned int cameraRigMode;

  /**
   * Defines the distance between both "eyes" in case of a RIG
   */
  float interaxialDistance;

  /**
   * Defines if stereoscopic rendering is done side by side or over under.
   */
  bool isStereoscopicSideBySide;

  /**
   * Defines the list of custom render target the camera should render to.
   * This is pretty helpfull if you wish to make a camera render to a texture
   * you could reuse somewhere else in the scene.
   */
  std::vector<RenderTargetTexturePtr> customRenderTargets;

  /**
   * Observable triggered when the camera view matrix has changed.
   */
  Observable<Camera> onViewMatrixChangedObservable;
  /**
   * Observable triggered when the camera Projection matrix has changed.
   */
  Observable<Camera> onProjectionMatrixChangedObservable;
  /**
   * Observable triggered when the inputs have been processed.
   */
  Observable<Camera> onAfterCheckInputsObservable;
  /**
   * Observable triggered when reset has been called and applied to the camera.
   */
  Observable<Camera> onRestoreStateObservable;

  /** Hidden */
  CameraRigParams _cameraRigParams;
  /** Hidden */
  std::vector<CameraPtr> _rigCameras;
  /** Hidden */
  PostProcess* _rigPostProcess;
  /** Hidden */
  bool _skipRendering;
  /** Hidden */
  Camera* _alternateCamera;

  /** Cache **/
  Matrix _projectionMatrix;

  /** Hidden */
  std::vector<PostProcessPtr> _postProcesses;

  /** Hidden */
  Uint32Array _postProcessesTakenIndices;

  /** Hidden */
  std::vector<AbstractMeshPtr> _activeMeshes;

  /** Hidden */
  ReadOnlyProperty<Camera, Vector3> globalPosition;

protected:
  Matrix _webvrViewMatrix;
  Vector3 _globalPosition;

private:
  Matrix _computedViewMatrix;
  bool _doNotComputeProjectionMatrix;
  Matrix _transformMatrix;
  Matrix _webvrProjectionMatrix;
  std::array<Plane, 6> _frustumPlanes;
  bool _refreshFrustumPlanes;
  float _storedFov;
  bool _stateStored;
  bool _setActiveOnSceneIfNoneActive;

}; // end of class Camera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_CAMERA_H
