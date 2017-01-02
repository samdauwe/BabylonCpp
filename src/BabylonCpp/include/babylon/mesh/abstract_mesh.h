#ifndef BABYLON_MESH_ABSTRACT_MESH_H
#define BABYLON_MESH_ABSTRACT_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/collisions/collider.h>
#include <babylon/core/json.h>
#include <babylon/culling/icullable.h>
#include <babylon/culling/octrees/octree.h>
#include <babylon/engine/node.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/physics/iphysics_enabled_object.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT AbstractMesh : public Node,
                                           public IPhysicsEnabledObject,
                                           public ICullable {

public:
  static constexpr unsigned int BILLBOARDMODE_NONE = 0;
  static constexpr unsigned int BILLBOARDMODE_X    = 1;
  static constexpr unsigned int BILLBOARDMODE_Y    = 2;
  static constexpr unsigned int BILLBOARDMODE_Z    = 4;
  static constexpr unsigned int BILLBOARDMODE_ALL  = 7;

  static Quaternion _rotationAxisCache;
  static Vector3 _lookAtVectorCache;

  // Constructor
  ~AbstractMesh();

  virtual Node::Type type() const override;
  void addToScene(std::unique_ptr<AbstractMesh>&& newMesh);

  // Events
  void setOnCollide(const std::function<void()>& callback);
  void setOnCollisionPositionChange(const std::function<void()>& callback);

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  std::string toString(bool fullDetails = false) const;
  Scene* getScene() override;
  void setSkeleton(Skeleton* value);
  virtual Skeleton* skeleton();
  Vector3& position();
  void setPosition(const Vector3& newPosition);
  Vector3& rotation();
  void setRotation(const Vector3& newRotation);
  Vector3& scaling();
  void setScaling(const Vector3& newScaling);
  Quaternion& rotationQuaternion();
  bool rotationQuaternionSet() const;
  void setRotationQuaternion(const Quaternion& quaternion);
  void resetRotationQuaternion();
  virtual AbstractMesh* getParent() override;

  /** Methods **/
  virtual Material* getMaterial();
  std::vector<AbstractMesh*>
  getChildMeshes(bool directDecendantsOnly,
                 const std::function<bool(Node* node)>& predicate
                 = nullptr) override;
  void updatePoseMatrix(const Matrix& matrix);
  Matrix& getPoseMatrix();
  void disableEdgesRendering();
  void enableEdgesRendering(float epsilon                      = 0.95f,
                            bool checkVerticesInsteadOfIndices = false);
  bool isBlocked() const;
  virtual AbstractMesh* getLOD(Camera* camera,
                               BoundingSphere* boundingSphere = nullptr);
  virtual size_t getTotalVertices() const;
  virtual Uint32Array getIndices(bool copyWhenShared = false) override;
  virtual Float32Array getVerticesData(unsigned int kind,
                                       bool copyWhenShared = false) override;
  virtual bool isVerticesDataPresent(unsigned int kind);
  BoundingInfo* getBoundingInfo() override;
  void setBoundingInfo(const BoundingInfo& boundingInfo);
  bool useBones();
  virtual void _preActivate();
  virtual void _preActivateForIntermediateRendering(int /*renderId*/);
  virtual void _activate(int renderId);
  Matrix* getWorldMatrix() override;
  Matrix& worldMatrixFromCache();
  Vector3* absolutePosition();
  void freezeWorldMatrix();
  void unfreezeWorldMatrix();
  bool isWorldMatrixFrozen();
  void rotate(Vector3& axis, float amount, const Space& space = Space::LOCAL);
  void translate(Vector3& axis, float distance,
                 const Space& space = Space::LOCAL);
  Vector3* getAbsolutePosition();
  void setAbsolutePosition(const Vector3& absolutePosition);

  // ========================= Point of View Movement ==========================
  /**
   * Perform relative position change from the point of view of behind the front
   * of the mesh. This is performed taking into account the meshes current
   * rotation, so you do not have to care. Supports definition of mesh facing
   * forward or backward.
   * @param {number} amountRight
   * @param {number} amountUp
   * @param {number} amountForward
   */
  void movePOV(float amountRight, float amountUp, float amountForward);

  /**
   * Calculate relative position change from the point of view of behind the
   * front of the mesh. This is performed taking into account the meshes current
   * rotation, so you do not have to care. Supports definition of mesh facing
   * forward or backward.
   * @param {number} amountRight
   * @param {number} amountUp
   * @param {number} amountForward
   */
  Vector3 calcMovePOV(float amountRight, float amountUp, float amountForward);

  // ========================= Point of View Rotation ==========================
  /**
   * Perform relative rotation change from the point of view of behind the front
   * of the mesh. Supports definition of mesh facing forward or backward.
   * @param {number} flipBack
   * @param {number} twirlClockwise
   * @param {number} tiltRight
   */
  void rotatePOV(float flipBack, float twirlClockwise, float tiltRight);

  /**
   * Calculate relative rotation change from the point of view of behind the
   * front of the mesh. Supports definition of mesh facing forward or backward.
   * @param {number} flipBack
   * @param {number} twirlClockwise
   * @param {number} tiltRight
   */
  Vector3 calcRotatePOV(float flipBack, float twirlClockwise, float tiltRight);

  void setPivotMatrix(const Matrix& matrix);
  Matrix& getPivotMatrix();
  bool _isSynchronized() override;
  void _initCache() override;
  void markAsDirty(const std::string& property) override;
  void _updateBoundingInfo();
  void _updateSubMeshesBoundingInfo(Matrix& matrix);
  Matrix computeWorldMatrix(bool force = false) override;

  /**
   * If you'd like to be callbacked after the mesh position, rotation or scaling
   * has been updated
   * @param func: callback function to add
   */
  void registerAfterWorldMatrixUpdate(
    const std::function<void(AbstractMesh* mesh)>& func);

  void unregisterAfterWorldMatrixUpdate(
    const std::function<void(AbstractMesh* mesh)>& func);
  void setPositionWithLocalVector(const Vector3& vector3);
  Vector3 getPositionExpressedInLocalSpace();
  void locallyTranslate(const Vector3& vector3);
  void lookAt(const Vector3& targetPoint, float yawCor = 0.f,
              float pitchCor = 0.f, float rollCor = 0.f,
              Space space = Space::LOCAL);
  void attachToBone(Bone* bone, AbstractMesh* affectedMesh);
  void detachFromBone();
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes) override;
  bool isCompletelyInFrustum(
    const std::array<Plane, 6>& frustumPlanes) const override;
  bool intersectsMesh(AbstractMesh* mesh, bool precise);
  bool intersectsMesh(SolidParticle* sp, bool precise);
  bool intersectsPoint(const Vector3& point);

  /** Physics **/
  PhysicsImpostor* getPhysicsImpostor();
  Vector3 getPositionInCameraSpace(Camera* camera);
  float getDistanceToCamera(Camera* camera);
  void applyImpulse(const Vector3& force, const Vector3& contactPoint);
  void setPhysicsLinkWith(Mesh* otherMesh, const Vector3& pivot1,
                          const Vector3& pivot2, const PhysicsParams& options);

  /** Collisions **/
  bool checkCollisions() const;
  void setCheckCollisions(bool collisionEnabled);
  void moveWithCollisions(const Vector3& velocity);

  /** Submeshes octree **/
  /**
    * This function will create an octree to help select the right submeshes for
    * rendering, picking and collisions
    * Please note that you must have a decent number of submeshes to get
    * performance improvements when using octree
    */
  Octree<SubMesh*>* createOrUpdateSubmeshesOctree(size_t maxCapacity = 64,
                                                  size_t maxDepth    = 2);

  /** Collisions **/
  void _collideForSubMesh(SubMesh* subMesh, const Matrix& transformMatrix,
                          Collider* collider);
  void _processCollisionsForSubMeshes(Collider* collider,
                                      const Matrix& transformMatrix);
  void _checkCollision(Collider* collider);

  /** Picking **/
  virtual bool _generatePointsArray();
  virtual PickingInfo* intersects(const Ray& ray, bool fastCheck = true);
  AbstractMesh* clone(const std::string& name, Node* newParent,
                      bool doNotCloneChildren = true);
  void releaseSubMeshes();
  virtual void dispose(bool doNotRecurse = false) override;
  Vector3 getDirection(const Vector3& localAxis);
  void getDirectionToRef(const Vector3& localAxis, Vector3& result);
  void setPivotPoint(const Vector3& point, const Space& space = Space::LOCAL);
  Vector3 getPivotPoint() const;
  void getPivotPointToRef(Vector3& result) const;
  Vector3 getAbsolutePivotPoint();
  void getAbsolutePivotPointToRef(Vector3& result);

protected:
  AbstractMesh(const std::string& name, Scene* scene);

private:
  void _onCollisionPositionChange(int collisionId, const Vector3& newPosition,
                                  AbstractMesh* collidedMesh = nullptr);

public:
  /**
   * An event triggered when this mesh collides with another one
   */
  Observable<AbstractMesh> onCollideObservable;
  /**
   * An event triggered when the collision's position changes
   */
  Observable<Vector3> onCollisionPositionChangeObservable;
  /**
   * An event triggered after the world matrix is updated
   */
  Observable<AbstractMesh> onAfterWorldMatrixUpdateObservable;
  // Properties
  bool definedFacingForward;
  bool _rotationQuaternionSet;
  Quaternion _rotationQuaternion;
  unsigned int billboardMode;
  float visibility;
  int alphaIndex;
  bool infiniteDistance;
  bool isVisible;
  bool isPickable;
  bool showBoundingBox;
  bool showSubMeshesBoundingBox;
  bool isBlocker;
  unsigned int renderingGroupId;
  Material* material;
  bool receiveShadows;
  bool renderOutline;
  Color3 outlineColor;
  float outlineWidth;
  bool renderOverlay;
  Color3 overlayColor;
  float overlayAlpha;
  bool hasVertexAlpha;
  bool useVertexColors;
  bool applyFog;
  bool computeBonesUsingShaders;
  float scalingDeterminant;
  unsigned int numBoneInfluencers;
  bool useOctreeForRenderingSelection;
  bool useOctreeForPicking;
  bool useOctreeForCollisions;
  unsigned int layerMask;
  bool alwaysSelectAsActiveMesh;
  // This scene's action manager
  ActionManager* actionManager;
  // Physics
  PhysicsImpostor* physicsImpostor;
  // Collisions
  Vector3 ellipsoid;
  Vector3 ellipsoidOffset;
  // Edges
  float edgesWidth;
  Color4 edgesColor;
  std::unique_ptr<EdgesRenderer> _edgesRenderer;
  // Cache
  std::unique_ptr<Matrix> _worldMatrix;
  std::vector<Vector3> _positions;
  AbstractMesh* _masterMesh;
  std::unique_ptr<MaterialDefines> _materialDefines;
  std::unique_ptr<BoundingInfo> _boundingInfo;
  bool _isDisposed;
  int _renderId;
  std::vector<std::unique_ptr<SubMesh>> subMeshes;
  Octree<SubMesh*>* _submeshesOctree;
  std::vector<AbstractMesh*> _intersectionsInProgress;
  bool _unIndexed;
  std::unique_ptr<Matrix> _poseMatrix;
  // Loading properties
  std::vector<Json::value> _waitingActions;
  bool _waitingFreezeWorldMatrix;
  // Skeleton
  Float32Array _bonesTransformMatrices;

private:
  // Events
  Observer<AbstractMesh>::Ptr _onCollideObserver;
  Observer<Vector3>::Ptr _onCollisionPositionChangeObserver;
  // Properties
  Vector3 _position;
  Vector3 _rotation;
  Vector3 _scaling;
  // Collisions
  bool _checkCollisions;
  // Collider* _collider;
  Vector3 _oldPositionForCollisions;
  Vector3 _diffPositionForCollisions;
  Vector3 _newPositionForCollisions;
  // Attach to bone
  AbstractMesh* _meshToBoneReferal;
  // Cache
  Matrix _localWorld;
  Matrix _rotateYByPI;
  std::unique_ptr<Vector3> _absolutePosition;
  Matrix _collisionsTransformMatrix;
  Matrix _collisionsScalingMatrix;
  bool _isDirty;
  Matrix _pivotMatrix;
  bool _isWorldMatrixFrozen;
  // Skeleton
  Skeleton* _skeleton;

}; // end of class AbstractMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_ABSTRACT_MESH_H
