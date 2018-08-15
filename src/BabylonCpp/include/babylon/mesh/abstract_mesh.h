#ifndef BABYLON_MESH_ABSTRACT_MESH_H
#define BABYLON_MESH_ABSTRACT_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/collisions/collider.h>
#include <babylon/core/json.h>
#include <babylon/culling/icullable.h>
#include <babylon/culling/octrees/octree.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/axis.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/mesh/facet_parameters.h>
#include <babylon/mesh/iget_set_vertices_data.h>
#include <babylon/mesh/transform_node.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

namespace GL {
class IGLQuery;
} // end of namespace GL

/**
 * @brief Class used to store all common mesh properties.
 */
class BABYLON_SHARED_EXPORT AbstractMesh : public TransformNode,
                                           public ICullable,
                                           public IGetSetVerticesData {

public:
  // Statics

  /** No occlusion */
  static constexpr unsigned int OCCLUSION_TYPE_NONE = 0;
  /** Occlusion set to optimisitic */
  static constexpr unsigned int OCCLUSION_TYPE_OPTIMISTIC = 1;
  /** Occlusion set to strict */
  static constexpr unsigned int OCCLUSION_TYPE_STRICT = 2;
  /** Use an accurante occlusion algorithm */
  static constexpr unsigned int OCCLUSION_ALGORITHM_TYPE_ACCURATE = 0;
  /** Use a conservative occlusion algorithm */
  static constexpr unsigned int OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE = 1;

  static Vector3 _lookAtVectorCache;

  template <typename... Ts>
  static AbstractMesh* New(Ts&&... args)
  {
    auto mesh = new AbstractMesh(::std::forward<Ts>(args)...);
    mesh->addToScene(static_cast<unique_ptr_t<AbstractMesh>>(mesh));

    return mesh;
  }
  ~AbstractMesh() override;

  virtual IReflect::Type type() const override;
  void addToScene(unique_ptr_t<AbstractMesh>&& newMesh);

  /**
   * @brief Hidden
   */
  bool _updateNonUniformScalingState(bool value) override;

  /**
   * @brief Returns the string "AbstractMesh".
   * @returns "AbstractMesh"
   */
  const string_t getClassName() const override;

  /**
   * @brief Gets a string representation of the current mesh.
   * @param fullDetails defines a boolean indicating if full details must be
   * included
   * @returns a string representation of the current mesh
   */
  string_t toString(bool fullDetails = false) const;

  /**
   * @brief Hidden
   */
  void _rebuild();

  /**
   * @brief Hidden
   */
  void _resyncLightSources();

  /**
   * @brief Hidden
   */
  void _resyncLighSource(Light* light);

  /**
   * @brief Hidden
   */
  virtual void _unBindEffect();

  /**
   * @brief Hidden
   */
  void _removeLightSource(Light* light);

  /**
   * @brief Hidden
   */
  void _markSubMeshesAsLightDirty();

  /**
   * @brief Hidden
   */
  void _markSubMeshesAsAttributesDirty();

  /**
   * @brief Hidden
   */
  void _markSubMeshesAsMiscDirty();

  /**
   * @brief Hidden
   */
  Scene* getScene() override;

  /**
   * @brief Gets a Vector3 depicting the mesh scaling along each local axis X,
   * Y, Z.  Default is (1.0, 1.0, 1.0).
   */
  Vector3& get_scaling() override;

  /**
   * @brief Sets a Vector3 depicting the mesh scaling along each local axis X,
   * Y, Z.  Default is (1.0, 1.0, 1.0).
   */
  void set_scaling(const Vector3& newScaling) override;

  void resetRotationQuaternion();
  virtual AbstractMesh* getParent();

  /** Methods **/
  virtual Material* getMaterial();

  /**
   * @brief Disables the mesh edge rendering mode.
   * @returns the currentAbstractMesh
   */
  AbstractMesh& disableEdgesRendering();

  /**
   * @brief Enables the edge rendering mode on the mesh.
   * This mode makes the mesh edges visible
   * @param epsilon defines the maximal distance between two angles to detect a
   * face
   * @param checkVerticesInsteadOfIndices indicates that we should check vertex
   * list directly instead of faces
   * @returns the currentAbstractMesh
   * @see https://www.babylonjs-playground.com/#19O9TU#0
   */
  AbstractMesh& enableEdgesRendering(float epsilon = 0.95f,
                                     bool checkVerticesInsteadOfIndices
                                     = false);

  /**
   * @brief Gets the edgesRenderer associated with the mesh.
   */
  unique_ptr_t<EdgesRenderer>& edgesRenderer();

  /**
   * @brief Returns true if the mesh is blocked. Implemented by child classes.
   */
  bool isBlocked() const;

  /**
   * @brief Returns the mesh itself by default. Implemented by child classes.
   * @param camera defines the camera to use to pick the right LOD level
   * @returns the currentAbstractMesh
   */
  virtual AbstractMesh* getLOD(Camera* camera,
                               BoundingSphere* boundingSphere = nullptr);

  /**
   * @brief Returns 0 by default. Implemented by child classes.
   * @returns an integer
   */
  virtual size_t getTotalVertices() const;

  /**
   * @brief Returns null by default. Implemented by child classes.
   * @returns null
   */
  virtual Uint32Array getIndices(bool copyWhenShared = false) override;

  /**
   * @brief Returns the array of the requested vertex data kind. Implemented by
   * child classes.
   * @param kind defines the vertex data kind to use
   * @returns null
   */
  virtual Float32Array getVerticesData(unsigned int kind,
                                       bool copyWhenShared = false,
                                       bool forceCopy      = false) override;

  /**
   * @brief Sets the vertex data of the mesh geometry for the requested `kind`.
   * If the mesh has no geometry, a new Geometry object is set to the mesh and
   * then passed this vertex data. Note that a new underlying VertexBuffer
   * object is created each call. If the `kind` is the `PositionKind`, the mesh
   * BoundingInfo is renewed, so the bounding box and sphere, and the mesh World
   * Matrix is recomputed.
   * @param kind defines vertex data kind:
   * * BABYLON.VertexBuffer.PositionKind
   * * BABYLON.VertexBuffer.UVKind
   * * BABYLON.VertexBuffer.UV2Kind
   * * BABYLON.VertexBuffer.UV3Kind
   * * BABYLON.VertexBuffer.UV4Kind
   * * BABYLON.VertexBuffer.UV5Kind
   * * BABYLON.VertexBuffer.UV6Kind
   * * BABYLON.VertexBuffer.ColorKind
   * * BABYLON.VertexBuffer.MatricesIndicesKind
   * * BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * * BABYLON.VertexBuffer.MatricesWeightsKind
   * * BABYLON.VertexBuffer.MatricesWeightsExtraKind
   * @param data defines the data source
   * @param updatable defines if the data must be flagged as updatable (or
   * static)
   * @param stride defines the vertex stride (size of an entire vertex). Can be
   * null and in this case will be deduced from vertex data kind
   * @returns the current mesh
   */
  virtual AbstractMesh*
  setVerticesData(unsigned int kind, const Float32Array& data,
                  bool updatable                 = false,
                  const Nullable<size_t>& stride = nullptr) override;

  /**
   * @brief Updates the existing vertex data of the mesh geometry for the
   * requested `kind`. If the mesh has no geometry, it is simply returned as it
   * is.
   * @param kind defines vertex data kind:
   * * BABYLON.VertexBuffer.PositionKind
   * * BABYLON.VertexBuffer.UVKind
   * * BABYLON.VertexBuffer.UV2Kind
   * * BABYLON.VertexBuffer.UV3Kind
   * * BABYLON.VertexBuffer.UV4Kind
   * * BABYLON.VertexBuffer.UV5Kind
   * * BABYLON.VertexBuffer.UV6Kind
   * * BABYLON.VertexBuffer.ColorKind
   * * BABYLON.VertexBuffer.MatricesIndicesKind
   * * BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * * BABYLON.VertexBuffer.MatricesWeightsKind
   * * BABYLON.VertexBuffer.MatricesWeightsExtraKind
   * @param data defines the data source
   * @param updateExtends If `kind` is `PositionKind` and if `updateExtends` is
   * true, the mesh BoundingInfo is renewed, so the bounding box and sphere, and
   * the mesh World Matrix is recomputed
   * @param makeItUnique If true, a new global geometry is created from this
   * data and is set to the mesh
   * @returns the current mesh
   */
  virtual AbstractMesh* updateVerticesData(unsigned int kind,
                                           const Float32Array& data,
                                           bool updateExtends = false,
                                           bool makeItUnique  = false) override;

  /**
   * @brief Sets the mesh indices,
   * If the mesh has no geometry, a new Geometry object is created and set to
   * the mesh.
   * @param indices Expects an array populated with integers or a typed array
   * (Int32Array, Uint32Array, Uint16Array)
   * @param totalVertices Defines the total number of vertices
   * @returns the current mesh
   */
  virtual AbstractMesh* setIndices(const IndicesArray& indices,
                                   size_t totalVertices = 0,
                                   bool updatable       = false) override;

  /**
   * @brief Gets a boolean indicating if specific vertex data is present.
   * @param kind defines the vertex data kind to use
   * @returns true is data kind is present
   */
  virtual bool isVerticesDataPresent(unsigned int kind) override;

  /**
   * @brief Returns the mesh BoundingInfo object or creates a new one and
   * returns if it was undefined.
   * @returns a BoundingInfo
   */
  BoundingInfo& getBoundingInfo();

  /**
   * @brief Uniformly scales the mesh to fit inside of a unit cube (1 X 1 X 1
   * units).
   * @param includeDescendants Use the hierarchy's bounding box instead of the
   * mesh's bounding box
   * @returns the current mesh
   */
  AbstractMesh& normalizeToUnitCube(bool includeDescendants = true);

  /**
   * @brief Overwrite the current bounding info.
   * @param boundingInfo defines the new bounding info
   * @returns the current mesh
   */
  AbstractMesh& setBoundingInfo(const BoundingInfo& boundingInfo);

  /**
   * @brief Gets a boolean indicating if this mesh has skinning data and an
   * attached skeleton.
   */
  bool useBones();

  /**
   * @brief Hidden
   */
  virtual void _preActivate();

  /**
   * @brief Hidden
   */
  virtual void _preActivateForIntermediateRendering(int renderId);

  /**
   * @brief Hidden
   */
  virtual void _activate(int renderId);

  /**
   * @brief Gets the current world matrix.
   * @returns a Matrix
   */
  Matrix* getWorldMatrix() override;

  /**
   * @brief Hidden
   */
  float _getWorldMatrixDeterminant() const override;

  // ========================= Point of View Movement ==========================

  /**
   * @brief Perform relative position change from the point of view of behind
   * the front of the mesh. This is performed taking into account the meshes
   * current rotation, so you do not have to care. Supports definition of mesh
   * facing forward or backward
   * @param amountRight defines the distance on the right axis
   * @param amountUp defines the distance on the up axis
   * @param amountForward defines the distance on the forward axis
   * @returns the current mesh
   */
  AbstractMesh& movePOV(float amountRight, float amountUp, float amountForward);

  /**
   * @brief Calculate relative position change from the point of view of behind
   * the front of the mesh. This is performed taking into account the meshes
   * current rotation, so you do not have to care. Supports definition of mesh
   * facing forward or backward
   * @param amountRight defines the distance on the right axis
   * @param amountUp defines the distance on the up axis
   * @param amountForward defines the distance on the forward axis
   * @returns the new displacement vector
   */
  Vector3 calcMovePOV(float amountRight, float amountUp, float amountForward);

  // ========================= Point of View Rotation ==========================

  /**
   * @brief Perform relative rotation change from the point of view of behind
   * the front of the mesh. Supports definition of mesh facing forward or
   * backward
   * @param flipBack defines the flip
   * @param twirlClockwise defines the twirl
   * @param tiltRight defines the tilt
   * @returns the current mesh
   */
  AbstractMesh& rotatePOV(float flipBack, float twirlClockwise,
                          float tiltRight);

  /**
   * @brief Calculate relative rotation change from the point of view of behind
   * the front of the mesh. Supports definition of mesh facing forward or
   * backward.
   * @param flipBack defines the flip
   * @param twirlClockwise defines the twirl
   * @param tiltRight defines the tilt
   * @returns the new rotation vector
   */
  Vector3 calcRotatePOV(float flipBack, float twirlClockwise, float tiltRight);

  /**
   * @brief Return the minimum and maximum world vectors of the entire hierarchy
   * under current mesh
   * @param includeDescendants Include bounding info from descendants as well
   * (true by default)
   * @param predicate defines a callback function that can be customize to
   * filter what meshes should be included in the list used to compute the
   * bounding vectors
   * @returns the new bounding vectors
   */
  MinMax getHierarchyBoundingVectors(
    bool includeDescendants = true,
    const ::std::function<bool(AbstractMesh* abstractMesh)>& predicate
    = nullptr);

  /**
   * @brief Hidden
   */
  AbstractMesh& _updateBoundingInfo();

  /**
   * @brief Hidden
   */
  AbstractMesh& _updateSubMeshesBoundingInfo(const Matrix& matrix);

  /**
   * @brief Returns `true` if the mesh is within the frustum defined by the
   * passed array of planes. A mesh is in the frustum if its bounding box
   * intersects the frustum
   * @param frustumPlanes defines the frustum to test
   * @returns true if the mesh is in the frustum planes
   */
  bool isInFrustum(const array_t<Plane, 6>& frustumPlanes) override;

  /**
   * @brief Returns `true` if the mesh is completely in the frustum defined be
   * the passed array of planes. A mesh is completely in the frustum if its
   * bounding box it completely inside the frustum.
   * @param frustumPlanes defines the frustum to test
   * @returns true if the mesh is completely in the frustum planes
   */
  bool
  isCompletelyInFrustum(const array_t<Plane, 6>& frustumPlanes) const override;

  /**
   * @brief True if the mesh intersects another mesh or a SolidParticle object.
   * @param mesh defines a target mesh to test
   * @param precise Unless the parameter `precise` is set to `true` the
   * intersection is computed according to Axis Aligned Bounding Boxes (AABB),
   * else according to OBB (Oriented BBoxes)
   * @param includeDescendants Can be set to true to test if the mesh defined in
   * parameters intersects with the current mesh or any child meshes
   * @returns true if there is an intersection
   */
  bool intersectsMesh(class AbstractMesh* mesh, bool precise = false,
                      bool includeDescendants = false);

  /**
   * @brief True if the mesh intersects another mesh or a SolidParticle object.
   * @param mesh defines a SolidParticle to test
   * @param precise Unless the parameter `precise` is set to `true` the
   * intersection is computed according to Axis Aligned Bounding Boxes (AABB),
   * else according to OBB (Oriented BBoxes)
   * @param includeDescendants Can be set to true to test if the mesh defined in
   * parameters intersects with the current mesh or any child meshes
   * @returns true if there is an intersection
   */
  bool intersectsMesh(SolidParticle* sp, bool precise = false,
                      bool includeDescendants = false);

  /**
   * @brief Returns true if the passed point (Vector3) is inside the mesh
   * bounding box.
   * @param point defines the point to test
   * @returns true if there is an intersection
   */
  bool intersectsPoint(const Vector3& point);

  /** Physics **/

  /**
   * @brief Gets the current physics impostor.
   * @see http://doc.babylonjs.com/features/physics_engine
   * @returns a physics impostor or null
   */
  PhysicsImpostor* getPhysicsImpostor();

  /**
   * @brief Gets the position of the current mesh in camera space.
   * @param camera defines the camera to use
   * @returns a position
   */
  Vector3 getPositionInCameraSpace(Camera* camera = nullptr);

  /**
   * @brief Returns the distance from the mesh to the active camera.
   * @param camera defines the camera to use
   * @returns the distance
   */
  float getDistanceToCamera(Camera* camera = nullptr);

  /**
   * @brief Apply a physic impulse to the mesh.
   * @param force defines the force to apply
   * @param contactPoint defines where to apply the force
   * @returns the current mesh
   * @see http://doc.babylonjs.com/how_to/using_the_physics_engine
   */
  AbstractMesh& applyImpulse(const Vector3& force, const Vector3& contactPoint);

  /**
   * @brief Creates a physic joint between two meshes.
   * @param otherMesh defines the other mesh to use
   * @param pivot1 defines the pivot to use on this mesh
   * @param pivot2 defines the pivot to use on the other mesh
   * @param options defines additional options (can be plugin dependent)
   * @returns the current mesh
   * @see https://www.babylonjs-playground.com/#0BS5U0#0
   */
  AbstractMesh& setPhysicsLinkWith(Mesh* otherMesh, const Vector3& pivot1,
                                   const Vector3& pivot2,
                                   const PhysicsParams& options);

  /** Collisions **/

  /**
   * @brief Gets a boolean indicating that this mesh can be used in the
   * collision engine.
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   */
  bool checkCollisions() const;

  /**
   * @brief Sets a boolean indicating that this mesh can be used in the
   * collision engine.
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   */
  void setCheckCollisions(bool collisionEnabled);

  /**
   * @brief Gets Collider object used to compute collisions (not physics).
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   */
  Collider* collider() const;

  /**
   * @brief Move the mesh using collision engine.
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   * @param displacement defines the requested displacement vector
   * @returns the current mesh
   */
  AbstractMesh& moveWithCollisions(Vector3& displacement);

  /** Submeshes octree **/

  /**
   * @brief his function will create an octree to help to select the right
   * submeshes for rendering, picking and collision computations. Please note
   * that you must have a decent number of submeshes to get performance
   * improvements when using an octree
   * @param maxCapacity defines the maximum size of each block (64 by default)
   * @param maxDepth defines the maximum depth to use (no more than 2 levels by
   * default)
   * @returns the new octree
   * @see https://www.babylonjs-playground.com/#NA4OQ#12
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene_with_octrees
   */
  Octree<SubMesh*>* createOrUpdateSubmeshesOctree(size_t maxCapacity = 64,
                                                  size_t maxDepth    = 2);

  /** Collisions **/

  /**
   * @brief Hidden
   */
  AbstractMesh& _collideForSubMesh(SubMesh* subMesh,
                                   const Matrix& transformMatrix,
                                   Collider* collider);

  /**
   * @brief Hidden
   */
  AbstractMesh& _processCollisionsForSubMeshes(Collider* collider,
                                               const Matrix& transformMatrix);

  /**
   * @brief Hidden
   */
  AbstractMesh& _checkCollision(Collider* collider);

  /** Picking **/

  /**
   * @brief Hidden
   */
  virtual bool _generatePointsArray();

  /**
   * @brief Checks if the passed Ray intersects with the mesh.
   * @param ray defines the ray to use
   * @param fastCheck defines if fast mode (but less precise) must be used
   * (false by default)
   * @returns the picking info
   * @see http://doc.babylonjs.com/babylon101/intersect_collisions_-_mesh
   */
  virtual PickingInfo intersects(const Ray& ray, bool fastCheck = true);

  /**
   * @brief Clones the current mesh.
   * @param name defines the mesh name
   * @param newParent defines the new mesh parent
   * @param doNotCloneChildren defines a boolean indicating that children must
   * not be cloned (false by default)
   * @returns the new mesh
   */
  AbstractMesh* clone(const string_t& name, Node* newParent,
                      bool doNotCloneChildren = true);

  /**
   * @brief Disposes all the submeshes of the current meshes.
   * @returns the current mesh
   */
  AbstractMesh& releaseSubMeshes();

  /**
   * @brief Releases resources associated with this abstract mesh.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Adds the passed mesh as a child to the current mesh.
   * @param mesh defines the child mesh
   * @returns the current mesh
   */
  AbstractMesh& addChild(AbstractMesh* mesh);

  /**
   * @brief Removes the passed mesh from the current mesh children list.
   * @param mesh defines the child mesh
   * @returns the current mesh
   */
  AbstractMesh& removeChild(AbstractMesh* mesh);

  // Facet data

  /**
   * @brief Updates the mesh facetData arrays and the internal partitioning when
   * the mesh is morphed or updated. This method can be called within the render
   * loop. You don't need to call this method by yourself in the render loop
   * when you update/morph a mesh with the methods CreateXXX() as they
   * automatically manage this computation
   * @returns the current mesh
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  AbstractMesh& updateFacetData();

  /**
   * @brief Returns the facetLocalNormals array.
   * The normals are expressed in the mesh local spac
   * @returns an array of Vector3
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  vector_t<Vector3>& getFacetLocalNormals();

  /**
   * @brief Returns the facetLocalPositions array.
   * The facet positions are expressed in the mesh local space
   * @returns an array of Vector3
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  vector_t<Vector3>& getFacetLocalPositions();

  /**
   * @brief Returns the facetLocalPartioning array.
   * @returns an array of array of numbers
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  vector_t<Uint32Array>& getFacetLocalPartitioning();

  /**
   * @brief Returns the i-th facet position in the world system.
   * This method allocates a new Vector3 per call
   * @param i defines the facet index
   * @returns a new Vector3
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  Vector3 getFacetPosition(unsigned int i);

  /**
   * @brief Sets the reference Vector3 with the i-th facet position in the world
   * system.
   * @param i defines the facet index
   * @param ref defines the target vector
   * @returns the current mesh
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  AbstractMesh& getFacetPositionToRef(unsigned int i, Vector3& ref);

  /**
   * @brief Returns the i-th facet normal in the world system.
   * This method allocates a new Vector3 per call
   * @param i defines the facet index
   * @returns a new Vector3
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  Vector3 getFacetNormal(unsigned int i);

  /**
   * @brief Sets the reference Vector3 with the i-th facet normal in the world
   * system.
   * @param i defines the facet index
   * @param ref defines the target vector
   * @returns the current mesh
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  AbstractMesh& getFacetNormalToRef(unsigned int i, Vector3& ref);

  /**
   * @brief Returns the facets (in an array) in the same partitioning block than
   * the one the passed coordinates are located (expressed in the mesh local
   * system).
   * @param x defines x coordinate
   * @param y defines y coordinate
   * @param z defines z coordinate
   * @returns the array of facet indexes
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  Uint32Array getFacetsAtLocalCoordinates(float x, float y, float z);

  /**
   * @brief Returns the closest mesh facet index at (x,y,z) World coordinates,
   * null if not found.
   * @param projected sets as the (x,y,z) world projection on the facet
   * @param checkFace if true (default false), only the facet "facing" to
   * (x,y,z) or only the ones "turning their backs", according to the parameter
   * "facing" are returned
   * @param facing if facing and checkFace are true, only the facet "facing" to
   * (x, y, z) are returned : positive dot (x, y, z) * facet position. If facing
   * si false and checkFace is true, only the facet "turning their backs" to (x,
   * y, z) are returned : negative dot (x, y, z) * facet position
   * @param x defines x coordinate
   * @param y defines y coordinate
   * @param z defines z coordinate
   * @returns the face index if found (or null instead)
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  int getClosestFacetAtCoordinates(float x, float y, float z,
                                   Vector3& projected, bool projectedSet = true,
                                   bool checkFace = false, bool facing = true);

  /**
   * @brief Returns the closest mesh facet index at (x,y,z) local coordinates,
   * null if not found.
   * @param projected sets as the (x,y,z) local projection on the facet
   * @param checkFace if true (default false), only the facet "facing" to
   * (x,y,z) or only the ones "turning their backs", according to the parameter
   * "facing" are returned
   * @param facing if facing and checkFace are true, only the facet "facing" to
   * (x, y, z) are returned : positive dot (x, y, z) * facet position. If facing
   * si false and checkFace is true, only the facet "turning their backs" to (x,
   * y, z) are returned : negative dot (x, y, z) * facet position
   * @param x defines x coordinate
   * @param y defines y coordinate
   * @param z defines z coordinate
   * @returns the face index if found (or null instead)
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  int getClosestFacetAtLocalCoordinates(float x, float y, float z,
                                        Vector3& projected,
                                        bool projectedSet = true,
                                        bool checkFace    = false,
                                        bool facing       = true);

  /**
   * @brief Returns the object "parameter" set with all the expected parameters
   * for facetData computation by ComputeNormals().
   * @returns the parameters
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  FacetParameters& getFacetDataParameters();

  /**
   * @brief Disables the feature FacetData and frees the related memory.
   * @returns the current mesh
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata
   */
  AbstractMesh& disableFacetData();

  /**
   * @brief Updates the AbstractMesh indices array.
   * @param indices defines the data source
   * @returns the current mesh
   */
  AbstractMesh& updateIndices(const IndicesArray& indices);

  /**
   * @brief Creates new normals data for the mesh.
   * @param updatable defines if the normal vertex buffer must be flagged as
   * updatable
   * @returns the current mesh
   */
  AbstractMesh& createNormals(bool updatable);

  /**
   * @brief Align the mesh with a normal.
   * @param normal defines the normal to use
   * @param upDirection can be used to redefined the up vector to use (will use
   * the (0, 1, 0) by default)
   * @returns the current mesh
   */
  AbstractMesh& alignWithNormal(Vector3& normal,
                                const Vector3& upDirection = Axis::Y());

protected:
  // Constructor

  /**
   * @brief Creates a new AbstractMesh.
   * @param name defines the name of the mesh
   * @param scene defines the hosting scene
   */
  AbstractMesh(const string_t& name, Scene* scene);

  /**
   * @brief Hidden
   */
  void _afterComputeWorldMatrix() override;

  /**
   * @brief Hidden
   */
  void _checkOcclusionQuery();

protected:
  /**
   * @brief Gets the number of facets in the mesh.
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_facetdata#what-is-a-mesh-facet
   */
  size_t get_facetNb() const;

  /**
   * @brief Gets the number (integer) of subdivisions per axis in the partioning
   * space.
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_facetdata#tweaking-the-partitioning
   */
  unsigned int get_partitioningSubdivisions() const;

  /**
   * @brief Set the number (integer) of subdivisions per axis in the partioning
   * space.
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_facetdata#tweaking-the-partitioning
   */
  void set_partitioningSubdivisions(unsigned int nb);

  /**
   * @brief Gets the ratio (float) to apply to the bouding box size to set to
   * the partioning space. Ex : 1.01 (default) the partioning space is 1% bigger
   * than the bounding box
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_facetdata#tweaking-the-partitioning
   */
  float get_partitioningBBoxRatio() const;

  /**
   * @brief Set the ratio (float) to apply to the bouding box size to set to the
   * partioning space. Ex : 1.01 (default) the partioning space is 1% bigger
   * than the bounding box
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_facetdata#tweaking-the-partitioning
   */
  void set_partitioningBBoxRatio(float ratio);

  /**
   * @brief Gets a boolean indicating that the facets must be depth sorted on
   * next call to `updateFacetData()`. Works only for updatable meshes. Doesn't
   * work with multi-materials
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata#facet-depth-sort
   */
  bool get_mustDepthSortFacets() const;

  /**
   * @brief Sets a boolean indicating that the facets must be depth sorted on
   * next call to `updateFacetData()`. Works only for updatable meshes. Doesn't
   * work with multi-materials
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata#facet-depth-sort
   */
  void set_mustDepthSortFacets(bool sort);

  /**
   * @brief Gets the location (Vector3) where the facet depth sort must be
   * computed from. By default, the active camera position. Used only when facet
   * depth sort is enabled
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata#facet-depth-sort
   */
  Vector3& get_facetDepthSortFrom();

  /**
   * @brief Sets the location (Vector3) where the facet depth sort must be
   * computed from. By default, the active camera position. Used only when facet
   * depth sort is enabled
   * @see http://doc.babylonjs.com/how_to/how_to_use_facetdata#facet-depth-sort
   */
  void set_facetDepthSortFrom(const Vector3& location);

  /**
   * @brief Gets a boolean indicating if facetData is enabled.
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_facetdata#what-is-a-mesh-facet
   */
  bool get_isFacetDataEnabled() const;

  /**
   * @brief Set a function to call when this mesh collides with another one.
   */
  void set_onCollide(
    const ::std::function<void(AbstractMesh*, EventState&)>& callback);

  /**
   * @brief An event triggered when the collision's position changes.
   */
  void set_onCollisionPositionChange(
    const ::std::function<void(Vector3*, EventState&)>& callback);

  /**
   * @brief Gets whether the mesh is occluded or not, it is used also to set the
   * intial state of the mesh to be occluded or not.
   * @see http://doc.babylonjs.com/features/occlusionquery
   */
  bool get_isOccluded() const;

  /**
   * @brief Sets whether the mesh is occluded or not, it is used also to set the
   * intial state of the mesh to be occluded or not.
   * @see http://doc.babylonjs.com/features/occlusionquery
   */
  void set_isOccluded(bool value);

  /**
   * @brief Flag to check the progress status of the query.
   * @see http://doc.babylonjs.com/features/occlusionquery
   */
  bool get_isOcclusionQueryInProgress() const;

  /**
   * @brief Gets mesh visibility between 0 and 1 (default is 1).
   */
  float get_visibility() const;

  /**
   * @brief Sets mesh visibility between 0 and 1 (default is 1).
   */
  void set_visibility(float value);

  /**
   * @brief Gets current material.
   */
  Material*& get_material();

  /**
   * @brief Sets current material.
   */
  void set_material(Material* const& value);

  /**
   * @brief Gets a boolean indicating that this mesh can receive realtime
   * shadows.
   * @see http://doc.babylonjs.com/babylon101/shadows
   */
  bool get_receiveShadows() const;

  /**
   * @brief Sets a boolean indicating that this mesh can receive realtime
   * shadows.
   * @see http://doc.babylonjs.com/babylon101/shadows
   */
  void set_receiveShadows(bool value);

  /**
   * @brief Gets a boolean indicating that this mesh contains vertex color data
   * with alpha values.
   */
  bool get_hasVertexAlpha() const;

  /**
   * @brief Sets a boolean indicating that this mesh contains vertex color data
   * with alpha values.
   */
  void set_hasVertexAlpha(bool value);

  /**
   * @brief Gets a boolean indicating that this mesh needs to use vertex color
   * data to render (if this kind of vertex data is available in the geometry).
   */
  bool get_useVertexColors() const;

  /**
   * @brief Sets a boolean indicating that this mesh needs to use vertex color
   * data to render (if this kind of vertex data is available in the geometry).
   */
  void set_useVertexColors(bool value);

  /**
   * @brief Gets a boolean indicating that bone animations must be computed by
   * the CPU (false by default).
   */
  bool get_computeBonesUsingShaders() const;

  /**
   * @brief Sets a boolean indicating that bone animations must be computed by
   * the CPU (false by default).
   */
  void set_computeBonesUsingShaders(bool value);

  /**
   * @brief Gets the number of allowed bone influences per vertex (4 by
   * default).
   */
  unsigned int get_numBoneInfluencers() const;

  /**
   * @brief Sets the number of allowed bone influences per vertex (4 by
   * default).
   */
  void set_numBoneInfluencers(unsigned int value);

  /**
   * @brief Gets a boolean indicating that this mesh will allow fog to be
   * rendered on it (true by default).
   */
  bool get_applyFog() const;

  /**
   * @brief Sets a boolean indicating that this mesh will allow fog to be
   * rendered on it (true by default).
   */
  void set_applyFog(bool value);

  /**
   * @brief Gets the current layer mask (default is 0x0FFFFFFF).
   * @see http://doc.babylonjs.com/how_to/layermasks_and_multi-cam_textures
   */
  unsigned int get_layerMask() const;

  /**
   * @brief Sets the current layer mask (default is 0x0FFFFFFF).
   * @see http://doc.babylonjs.com/how_to/layermasks_and_multi-cam_textures
   */
  void set_layerMask(unsigned int value);

  /**
   * @brief Gets a collision mask used to mask collisions (default is -1). A
   * collision between A and B will happen if A.collisionGroup & b.collisionMask
   * !== 0
   */
  int get_collisionMask() const;

  /**
   * @brief Sets a collision mask used to mask collisions (default is -1). A
   * collision between A and B will happen if A.collisionGroup & b.collisionMask
   * !== 0
   */
  void set_collisionMask(int mask);

  /**
   * @brief Gets the current collision group mask (-1 by default). A collision
   * between A and B will happen if A.collisionGroup & b.collisionMask !== 0
   */
  int get_collisionGroup() const;

  /**
   * @brief Sets the current collision group mask (-1 by default). A collision
   * between A and B will happen if A.collisionGroup & b.collisionMask !== 0
   */
  void set_collisionGroup(int mask);

  /**
   * @brief Sets a skeleton to apply skining transformations.
   * @see http://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons
   */
  void set_skeleton(Skeleton* const& value);

  /**
   * @brief Gets a skeleton to apply skining transformations.
   * @see http://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons
   */
  virtual Skeleton*& get_skeleton();

private:
  /**
   * @brief Hidden
   */
  void _markSubMeshesAsDirty(
    const ::std::function<void(const MaterialDefines& defines)>& func);

  /**
   * @brief Hidden
   */
  void _onCollisionPositionChange(int collisionId, Vector3& newPosition,
                                  AbstractMesh* collidedMesh = nullptr);
  // Facet data

  /**
   * @brief Hidden
   */
  AbstractMesh& _initFacetData();

public:
  /**
   * The number of facets in the mesh
   */
  ReadOnlyProperty<AbstractMesh, size_t> facetNb;

  /**
   * The number (integer) of subdivisions per axis in the partioning space
   */
  Property<AbstractMesh, unsigned int> partitioningSubdivisions;

  /**
   * The ratio (float) to apply to the bouding box size to set to the partioning
   * space.
   */
  Property<AbstractMesh, float> partitioningBBoxRatio;

  /**
   * A boolean indicating that the facets must be depth sorted on next call to
   * `updateFacetData()`. Works only for updatable meshes. Doesn't work with
   * multi-materials
   */
  Property<AbstractMesh, bool> mustDepthSortFacets;

  /**
   * The location (Vector3) where the facet depth sort must be computed from. By
   * default, the active camera position. Used only when facet depth sort is
   * enabled
   */
  Property<AbstractMesh, Vector3> facetDepthSortFrom;

  /**
   * A boolean indicating if facetData is enabled
   */
  ReadOnlyProperty<AbstractMesh, bool> isFacetDataEnabled;

  /**
   * An event triggered when this mesh collides with another one
   */
  Observable<AbstractMesh> onCollideObservable;

  /**
   * The function to call when this mesh collides with another one
   */
  WriteOnlyProperty<AbstractMesh,
                    ::std::function<void(AbstractMesh*, EventState&)>>
    onCollide;

  /**
   * An event triggered when the collision's position changes
   */
  Observable<Vector3> onCollisionPositionChangeObservable;

  /**
   * @brief An event triggered when the collision's position changes
   */
  WriteOnlyProperty<AbstractMesh, ::std::function<void(Vector3*, EventState&)>>
    onCollisionPositionChange;

  /**
   * An event triggered when material is changed
   */
  Observable<AbstractMesh> onMaterialChangedObservable;

  // Properties

  /**
   * Gets or sets the orientation for POV movement & rotation
   */
  bool definedFacingForward;

  /**
   * This property determines the type of occlusion query algorithm to run in
   * WebGl, you can use:
   * * AbstractMesh.OCCLUSION_ALGORITHM_TYPE_ACCURATE which is mapped to
   * GL_ANY_SAMPLES_PASSED.
   * * AbstractMesh.OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE (Default Value) which
   * is mapped to GL_ANY_SAMPLES_PASSED_CONSERVATIVE which is a false positive
   * algorithm that is faster than GL_ANY_SAMPLES_PASSED but less accurate.
   * @see http://doc.babylonjs.com/features/occlusionquery
   */
  unsigned int occlusionQueryAlgorithmType;

  /**
   * This property is responsible for starting the occlusion query within the
   * Mesh or not, this property is also used to determine what should happen
   * when the occlusionRetryCount is reached. It has supports 3 values:
   * * OCCLUSION_TYPE_NONE (Default Value): this option means no occlusion query
   * whith the Mesh.
   * * OCCLUSION_TYPE_OPTIMISTIC: this option is means use occlusion query and
   * if occlusionRetryCount is reached and the query is broken show the mesh.
   * * OCCLUSION_TYPE_STRICT: this option is means use occlusion query and if
   * occlusionRetryCount is reached and the query is broken restore the last
   * state of the mesh occlusion if the mesh was visible then show the mesh if
   * was hidden then hide don't show.
   * @see http://doc.babylonjs.com/features/occlusionquery
   */
  unsigned int occlusionType;

  /**
   * This number indicates the number of allowed retries before stop the
   * occlusion query, this is useful if the occlusion query is taking long time
   * before to the query result is retireved, the query result indicates if the
   * object is visible within the scene or not and based on that Babylon.Js
   * engine decideds to show or hide the object. The default value is -1 which
   * means don't break the query and wait till the result
   * @see http://doc.babylonjs.com/features/occlusionquery
   */
  int occlusionRetryCount;

  /**
   * Whether the mesh is occluded or not, it is used also to set the intial
   * state of the mesh to be occluded or not
   */
  Property<AbstractMesh, bool> isOccluded;

  /**
   * Flag to check the progress status of the query
   */
  ReadOnlyProperty<AbstractMesh, bool> isOcclusionQueryInProgress;

  /**
   * The mesh visibility between 0 and 1 (default is 1)
   */
  Property<AbstractMesh, float> visibility;

  /**
   * Gets or sets the alpha index used to sort transparent meshes
   * @see
   * http://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered#alpha-index
   */
  int alphaIndex;

  /**
   * Gets or sets a boolean indicating if the mesh is visible (renderable).
   * Default is true
   */
  bool isVisible;

  /**
   * Gets or sets a boolean indicating if the mesh can be picked (by scene.pick
   * for instance or through actions). Default is true
   */
  bool isPickable;

  /**
   * Gets or sets a boolean indicating if the bounding box must be rendered as
   * well (false by default)
   */
  bool showBoundingBox;

  /**
   * Gets or sets a boolean indicating that bounding boxes of subMeshes must be
   * rendered as well (false by default)
   */
  bool showSubMeshesBoundingBox;

  /**
   * Gets or sets a boolean indicating if the mesh must be considered as a ray
   * blocker for lens flares (false by default)
   * @see http://doc.babylonjs.com/how_to/how_to_use_lens_flares
   */
  bool isBlocker;

  /**
   * Gets or sets a boolean indicating that pointer move events must be
   * supported on this mesh (false by default)
   */
  bool enablePointerMoveEvents;

  /**
   * Specifies the rendering group id for this mesh (0 by default)
   * @see
   * http://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered#rendering-groups
   */
  unsigned int renderingGroupId;

  /**
   * The current material
   */
  Property<AbstractMesh, Material*> material;

  /**
   * A boolean indicating that this mesh can receive realtime shadows.
   */
  Property<AbstractMesh, bool> receiveShadows;

  /**
   * Gets or sets a boolean indicating if the outline must be rendered as well
   * @see https://www.babylonjs-playground.com/#10WJ5S#3
   */
  bool renderOutline;

  /**
   * Defines color to use when rendering outline
   */
  Color3 outlineColor;

  /**
   * Define width to use when rendering outline
   */
  float outlineWidth;

  /**
   * Gets or sets a boolean indicating if the overlay must be rendered as well
   * @see https://www.babylonjs-playground.com/#10WJ5S#2
   */
  bool renderOverlay;

  /**
   * Defines color to use when rendering overlay
   */
  Color3 overlayColor;

  /**
   * Defines alpha to use when rendering overlay
   */
  float overlayAlpha;

  /**
   * A boolean indicating that this mesh contains vertex color data with alpha
   * values
   */
  Property<AbstractMesh, bool> hasVertexAlpha;

  /**
   * A boolean indicating that this mesh needs to use vertex color data to
   * render (if this kind of vertex data is available in the geometry)
   */
  Property<AbstractMesh, bool> useVertexColors;

  /**
   * A boolean indicating that bone animations must be computed by the CPU
   * (false by default)
   */
  Property<AbstractMesh, bool> computeBonesUsingShaders;

  /**
   * The number of allowed bone influences per vertex (4 by default)
   */
  Property<AbstractMesh, unsigned int> numBoneInfluencers;

  /**
   * A boolean indicating that this mesh will allow fog to be rendered on it
   * (true by default)
   */
  Property<AbstractMesh, bool> applyFog;

  /**
   * Gets or sets a boolean indicating that internal octree (if available) can
   * be used to boost submeshes selection (true by default)
   */
  bool useOctreeForRenderingSelection;

  /**
   * Gets or sets a boolean indicating that internal octree (if available) can
   * be used to boost submeshes picking (true by default)
   */
  bool useOctreeForPicking;

  /**
   * Gets or sets a boolean indicating that internal octree (if available) can
   * be used to boost submeshes collision (true by default)
   */
  bool useOctreeForCollisions;

  /**
   * The current layer mask (default is 0x0FFFFFFF)
   */
  Property<AbstractMesh, unsigned int> layerMask;

  /**
   * True if the mesh must be rendered in any case (this will shortcut the
   * frustum clipping phase)
   */
  bool alwaysSelectAsActiveMesh;

  /**
   * Gets or sets the current action manager
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions
   */
  ActionManager* actionManager;

  /**
   * Gets or sets impostor used for physic simulation
   * @see http://doc.babylonjs.com/features/physics_engine
   */
  unique_ptr_t<PhysicsImpostor> physicsImpostor;

  // Collisions

  /**
   * Gets or sets the ellipsoid used to impersonate this mesh when using
   * collision engine (default is (0.5, 1, 0.5))
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   */
  Vector3 ellipsoid;

  /**
   * Gets or sets the ellipsoid offset used to impersonate this mesh when using
   * collision engine (default is (0, 0, 0))
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   */
  Vector3 ellipsoidOffset;

  /**
   * A collision mask used to mask collisions (default is -1). A collision
   * between A and B will happen if A.collisionGroup & b.collisionMask !== 0
   */
  Property<AbstractMesh, int> collisionMask;

  /**
   * the current collision group mask (-1 by default). A collision between A and
   * B will happen if A.collisionGroup & b.collisionMask !== 0
   */
  Property<AbstractMesh, int> collisionGroup;

  // Edges

  /**
   * Defines edge width used when edgesRenderer is enabled
   * @see https://www.babylonjs-playground.com/#10OJSG#13
   */
  float edgesWidth;

  /**
   * Defines edge color used when edgesRenderer is enabled
   * @see https://www.babylonjs-playground.com/#10OJSG#13
   */
  Color4 edgesColor;

  /** Hidden */
  unique_ptr_t<EdgesRenderer> _edgesRenderer;

  // Cache

  /** Hidden */
  AbstractMesh* _masterMesh;

  /** Hidden */
  unique_ptr_t<MaterialDefines> _materialDefines;

  /** Hidden */
  unique_ptr_t<BoundingInfo> _boundingInfo;

  /** Hidden */
  int _renderId;

  /**
   * Gets or sets the list of subMeshes
   * @see http://doc.babylonjs.com/how_to/multi_materials
   */
  vector_t<shared_ptr_t<SubMesh>> subMeshes;

  /** Hidden */
  Octree<SubMesh*>* _submeshesOctree;

  /** Hidden */
  vector_t<AbstractMesh*> _intersectionsInProgress;

  /** Hidden */
  bool _unIndexed;

  /** Hidden */
  vector_t<Light*> _lightSources;

  // Loading properties

  /** Hidden */
  vector_t<Json::value> _waitingActions;

  /** Hidden */
  Nullable<bool> _waitingFreezeWorldMatrix;

  // Skeleton

  /** Hidden */
  Float32Array _bonesTransformMatrices;

  /**
   * A skeleton to apply skining transformations
   */
  Property<AbstractMesh, Skeleton*> skeleton;

protected:
  bool _isOccluded;

private:
  // FacetData private properties
  // Facet local positions
  vector_t<Vector3> _facetPositions;
  // Facet local normals
  vector_t<Vector3> _facetNormals;
  // Partitioning array of facet index arrays
  vector_t<Uint32Array> _facetPartitioning;
  // facet number
  size_t _facetNb;
  // Number of subdivisions per axis in the partioning space
  unsigned int _partitioningSubdivisions;
  // The partioning array space is by default 1% bigger than the bounding box
  float _partitioningBBoxRatio;
  // Is the facet data feature enabled on this mesh ?
  bool _facetDataEnabled;
  // Keep a reference to the object parameters to avoid memory re-allocation
  FacetParameters _facetParameters;
  // bbox size approximated for facet data
  Vector3 _bbSize;
  // Actual number of subdivisions per axis for ComputeNormals()
  SubdivisionsPerAxis _subDiv;
  // is the facet depth sort to be computed
  bool _facetDepthSort;
  // is the facet depth sort initialized
  bool _facetDepthSortEnabled;
  // copy of the indices array to store them once sorted
  IndicesArray _depthSortedIndices;
  // array of depth sorted facets
  vector_t<DepthSortedFacet> _depthSortedFacets;
  // facet depth sort function
  ::std::function<int(const DepthSortedFacet& f1, const DepthSortedFacet& f2)>
    _facetDepthSortFunction;
  // location where to depth sort from
  unique_ptr_t<Vector3> _facetDepthSortFrom;
  // same as facetDepthSortFrom but expressed in the mesh local space
  Vector3 _facetDepthSortOrigin;
  Matrix _invertedMatrix;
  // Events
  Observer<AbstractMesh>::Ptr _onCollideObserver;
  Observer<Vector3>::Ptr _onCollisionPositionChangeObserver;
  // Properties
  int _occlusionInternalRetryCounter;
  bool _isOcclusionQueryInProgress;
  unique_ptr_t<GL::IGLQuery> _occlusionQuery;
  float _visibility;
  Material* _material;
  bool _receiveShadows;
  bool _hasVertexAlpha;
  bool _useVertexColors;
  bool _computeBonesUsingShaders;
  unsigned int _numBoneInfluencers;
  bool _applyFog;
  unsigned int _layerMask;
  // Collisions
  bool _checkCollisions;
  int _collisionMask;
  int _collisionGroup;
  unique_ptr_t<Collider> _collider;
  Vector3 _oldPositionForCollisions;
  Vector3 _diffPositionForCollisions;
  // Cache
  Matrix _collisionsTransformMatrix;
  Matrix _collisionsScalingMatrix;
  // Skeleton
  Skeleton* _skeleton;

}; // end of class AbstractMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_ABSTRACT_MESH_H
