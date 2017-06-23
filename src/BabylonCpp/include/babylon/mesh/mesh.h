#ifndef BABYLON_MESH_MESH_H
#define BABYLON_MESH_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/math/isize.h>
#include <babylon/math/path3d.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/iget_set_vertices_data.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Mesh : public AbstractMesh {

  friend class MeshBuilder;

public:
  /** Consts **/

  /**
   * Mesh side orientation : usually the external or front surface
   */
  static constexpr unsigned int FRONTSIDE = 0;

  /**
   * Mesh side orientation : usually the internal or back surface
   */
  static constexpr unsigned int BACKSIDE = 1;

  /**
   * Mesh side orientation : both internal and external or front and back
   * surfaces
   */
  static constexpr unsigned int DOUBLESIDE = 2;

  /**
   * Mesh side orientation : by default, `FRONTSIDE`
   */
  static constexpr unsigned int DEFAULTSIDE = 0;

  /**
   * Mesh cap setting : no cap
   */
  static constexpr unsigned int NO_CAP = 0;

  /**
   * Mesh cap setting : one cap at the beginning of the mesh
   */
  static constexpr unsigned int CAP_START = 1;

  /**
   * Mesh cap setting : one cap at the end of the mesh
   */
  static constexpr unsigned int CAP_END = 2;

  /**
   * Mesh cap setting : two caps, one at the beginning  and one at the end of
   * the mesh
   */
  static constexpr unsigned int CAP_ALL = 3;

  /** Events **/

  /**
   * An event triggered before rendering the mesh
   */
  Observable<Mesh> onBeforeRenderObservable;

  /**
   * An event triggered after rendering the mesh
   */
  Observable<Mesh> onAfterRenderObservable;

  /**
   * An event triggered before drawing the mesh
   */
  Observable<Mesh> onBeforeDrawObservable;

  template <typename... Ts>
  static Mesh* New(Ts&&... args)
  {
    auto mesh = new Mesh(std::forward<Ts>(args)...);
    mesh->addToScene(static_cast<std::unique_ptr<AbstractMesh>>(mesh));

    return mesh;
  }
  ~Mesh();

  MorphTargetManager* morphTargetManager();
  void setMorphTargetManager(MorphTargetManager* value);

  Mesh* source();

  /**
   * @brief Returns the string "Mesh".
   */
  const char* getClassName() const override;

  virtual IReflect::Type type() const override;

  /** Methods **/
  void setOnBeforeDraw(const std::function<void()>& callback);

  /**
   * @brief Returns a string.
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  std::string toString(bool fullDetails = false);

  void addSubMesh(SubMesh* subMesh);

  /**
   * @brief Returns if the mesh has some Levels Of Details (LOD).
   */
  bool hasLODLevels() const;

  /**
   * @brief Add a mesh as LOD level triggered at the given distance.
   * tuto : http://doc.babylonjs.com/tutorials/How_to_use_LOD
   * @param {number} distance The distance from the center of the object to show
   * this level
   * @param {Mesh} mesh The mesh to be added as LOD level
   * @returns {Mesh} This mesh (for chaining)
   */
  Mesh& addLODLevel(float distance, Mesh* mesh);

  /**
   * @brief Returns the LOD level mesh at the passed distance or null if not
   * found.
   * It is related to the method `addLODLevel(distance, mesh)`.
   * tuto : http://doc.babylonjs.com/tutorials/How_to_use_LOD
   * @returns An object Mesh or `null`.
   */
  Mesh* getLODLevelAtDistance(float distance);

  /**
   * @brief Remove a mesh from the LOD array
   * tuto : http://doc.babylonjs.com/tutorials/How_to_use_LOD
   * @param {Mesh} mesh The mesh to be removed.
   * @returns {Mesh} This mesh (for chaining)
   */
  Mesh& removeLODLevel(Mesh* mesh);

  /**
   * @brief Returns the registered LOD mesh distant from the parameter `camera`
   * position if any, else returns the current mesh.
   * tuto : http://doc.babylonjs.com/tutorials/How_to_use_LOD
   */
  AbstractMesh* getLOD(Camera* camera,
                       BoundingSphere* boundingSphere = nullptr) override;

  /**
   * @brief Returns the mesh internal Geometry object.
   */
  Geometry* geometry() const;

  void setGeometry(Geometry* geometry);

  /**
   * @brief Returns a positive integer : the total number of vertices within the
   * mesh geometry or zero if the mesh has no geometry.
   */
  size_t getTotalVertices() const override;

  /**
   * @brief Returns an array of integers or floats, or a Float32Array, depending
   * on the requested `kind` (positions, indices, normals, etc).
   * If `copywhenShared` is true (default false) and if the mesh geometry is
   * shared among some other meshes, the returned array is a copy of the
   * internal one.
   * You can force the copy with forceCopy == true
   * Possible `kind` values :
   * - BABYLON.VertexBuffer.PositionKind
   * - BABYLON.VertexBuffer.UVKind
   * - BABYLON.VertexBuffer.UV2Kind
   * - BABYLON.VertexBuffer.UV3Kind
   * - BABYLON.VertexBuffer.UV4Kind
   * - BABYLON.VertexBuffer.UV5Kind
   * - BABYLON.VertexBuffer.UV6Kind
   * - BABYLON.VertexBuffer.ColorKind
   * - BABYLON.VertexBuffer.MatricesIndicesKind
   * - BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * - BABYLON.VertexBuffer.MatricesWeightsKind
   * - BABYLON.VertexBuffer.MatricesWeightsExtraKind
   */
  Float32Array getVerticesData(unsigned int kind, bool copyWhenShared = false,
                               bool forceCopy = false) override;

  /**
   * @brief Returns the mesh VertexBuffer object from the requested `kind` :
   * positions, indices, normals, etc.
   * Possible `kind` values :
   * - BABYLON.VertexBuffer.PositionKind
   * - BABYLON.VertexBuffer.UVKind
   * - BABYLON.VertexBuffer.UV2Kind
   * - BABYLON.VertexBuffer.UV3Kind
   * - BABYLON.VertexBuffer.UV4Kind
   * - BABYLON.VertexBuffer.UV5Kind
   * - BABYLON.VertexBuffer.UV6Kind
   * - BABYLON.VertexBuffer.ColorKind
   * - BABYLON.VertexBuffer.MatricesIndicesKind
   * - BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * - BABYLON.VertexBuffer.MatricesWeightsKind
   * - BABYLON.VertexBuffer.MatricesWeightsExtraKind
   */
  VertexBuffer* getVertexBuffer(unsigned int kind);

  /**
   * @brief Returns a boolean depending on the existence of the Vertex Data for
   * the requested `kind`.
   * Possible `kind` values :
   * - BABYLON.VertexBuffer.PositionKind
   * - BABYLON.VertexBuffer.UVKind
   * - BABYLON.VertexBuffer.UV2Kind
   * - BABYLON.VertexBuffer.UV3Kind
   * - BABYLON.VertexBuffer.UV4Kind
   * - BABYLON.VertexBuffer.UV5Kind
   * - BABYLON.VertexBuffer.UV6Kind
   * - BABYLON.VertexBuffer.ColorKind
   * - BABYLON.VertexBuffer.MatricesIndicesKind
   * - BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * - BABYLON.VertexBuffer.MatricesWeightsKind
   * - BABYLON.VertexBuffer.MatricesWeightsExtraKind
   */
  bool isVerticesDataPresent(unsigned int kind) override;

  /**
   * @brief Returns a string : the list of existing `kinds` of Vertex Data for
   * this mesh.
   * Possible `kind` values :
   * - BABYLON.VertexBuffer.PositionKind
   * - BABYLON.VertexBuffer.UVKind
   * - BABYLON.VertexBuffer.UV2Kind
   * - BABYLON.VertexBuffer.UV3Kind
   * - BABYLON.VertexBuffer.UV4Kind
   * - BABYLON.VertexBuffer.UV5Kind
   * - BABYLON.VertexBuffer.UV6Kind
   * - BABYLON.VertexBuffer.ColorKind
   * - BABYLON.VertexBuffer.MatricesIndicesKind
   * - BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * - BABYLON.VertexBuffer.MatricesWeightsKind
   * - BABYLON.VertexBuffer.MatricesWeightsExtraKind
   */
  Uint32Array getVerticesDataKinds();

  /**
   * @brief Returns a positive integer : the total number of indices in this
   * mesh geometry.
   * @returns Zero if the mesh has no geometry.
   */
  size_t getTotalIndices();

  /**
   * @brief Returns an IndicesArray populated with the mesh indices.
   * If the parameter `copyWhenShared` is true (default false) and and if the
   * mesh geometry is shared among some other meshes, the returned array is a
   * copy of the internal one.
   * @returns An empty array if the mesh has no geometry.
   */
  IndicesArray getIndices(bool copyWhenShared = false) override;

  bool isBlocked();

  /**
   * @brief Returns true once the mesh is ready after all the delayed process
   * (loading, etc) are complete.
   */
  bool isReady() const;

  /**
   * @brief Returns true if the mesh has been disposed.
   */
  bool isDisposed() const;

  unsigned int sideOrientation() const;

  /**
   * @brief Sets the mesh side orientation : BABYLON.Mesh.FRONTSIDE,
   * BABYLON.Mesh.BACKSIDE, BABYLON.Mesh.DOUBLESIDE or BABYLON.Mesh.DEFAULTSIDE
   * tuto :
   * http://doc.babylonjs.com/tutorials/Discover_Basic_Elements#side-orientation
   */
  void setSideOrientation(unsigned int sideO);

  /**
   * @brief Returns true if the normals aren't to be recomputed on next mesh
   * `positions` array update.
   * This property is pertinent only for updatable parametric shapes.
   */
  bool areNormalsFrozen() const;

  /**
   * @brief This function affects parametric shapes on vertex position update
   * only : ribbons, tubes, etc.
   * It has no effect at all on other shapes.
   * It prevents the mesh normals from being recomputed on next `positions`
   * array update.
   * @returns the Mesh.
   */
  Mesh& freezeNormals();

  /**
   * @brief This function affects parametric shapes on vertex position update
   * only : ribbons, tubes, etc.
   * It has no effect at all on other shapes.
   * It reactivates the mesh normals computation if it was previously frozen.
   * @returns the Mesh.
   */
  Mesh& unfreezeNormals();

  /**
   * @brief Overrides instance count. Only applicable when custom instanced
   * InterleavedVertexBuffer are used rather than InstancedMeshs
   */
  void setOverridenInstanceCount(size_t count);

  /** Methods **/
  void _preActivate() override;
  void _preActivateForIntermediateRendering(int renderId) override;
  Mesh& _registerInstanceForRenderId(InstancedMesh* instance, int renderId);

  /**
   * @brief This method recomputes and sets a new BoundingInfo to the mesh
   * unless it is locked.
   * This means the mesh underlying bounding box and sphere are recomputed.
   * @returns The Mesh.
   */
  Mesh& refreshBoundingInfo();

  SubMesh* _createGlobalSubMesh();
  void subdivide(size_t count);

  /**
   * @brief Sets the vertex data of the mesh geometry for the requested `kind`.
   * If the mesh has no geometry, a new Geometry object is set to the mesh and
   * then passed this vertex data.
   * The `data` are either a numeric array either a Float32Array.
   * The parameter `updatable` is passed as is to the underlying Geometry object
   * constructor (if initianilly none) or updater.
   * The parameter `stride` is an optional positive integer, it is usually
   * automatically deducted from the `kind` (3 for positions or normals, 2 for
   * UV, etc).
   * Note that a new underlying VertexBuffer object is created each call.
   * If the `kind` is the `PositionKind`, the mesh BoundingInfo is renewed, so
   * the bounding box and sphere, and the mesh World Matrix is recomputed.
   *
   * Possible `kind` values :
   * - BABYLON.VertexBuffer.PositionKind
   * - BABYLON.VertexBuffer.UVKind
   * - BABYLON.VertexBuffer.UV2Kind
   * - BABYLON.VertexBuffer.UV3Kind
   * - BABYLON.VertexBuffer.UV4Kind
   * - BABYLON.VertexBuffer.UV5Kind
   * - BABYLON.VertexBuffer.UV6Kind
   * - BABYLON.VertexBuffer.ColorKind
   * - BABYLON.VertexBuffer.MatricesIndicesKind
   * - BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * - BABYLON.VertexBuffer.MatricesWeightsKind
   * - BABYLON.VertexBuffer.MatricesWeightsExtraKind
   */
  Mesh* setVerticesData(unsigned int kind, const Float32Array& data,
                        bool updatable = false, int stride = -1) override;

  void markVerticesDataAsUpdatable(unsigned int kind, bool updatable = true);

  /**
   * @brief Sets the mesh VertexBuffer.
   * @returns The Mesh.
   */
  Mesh& setVerticesBuffer(std::unique_ptr<VertexBuffer>&& buffer);

  /**
   * @brief Updates the existing vertex data of the mesh geometry for the
   * requested `kind`.
   * If the mesh has no geometry, it is simply returned as it is.
   * The `data` are either a numeric array either a Float32Array.
   * No new underlying VertexBuffer object is created.
   * If the `kind` is the `PositionKind` and if `updateExtends` is true, the
   * mesh BoundingInfo is renewed, so the bounding box and sphere, and the mesh
   * World Matrix is recomputed.
   * If the parameter `makeItUnique` is true, a new global geometry is created
   * from this positions and is set to the mesh.
   *
   * Possible `kind` values :
   * - BABYLON.VertexBuffer.PositionKind
   * - BABYLON.VertexBuffer.UVKind
   * - BABYLON.VertexBuffer.UV2Kind
   * - BABYLON.VertexBuffer.UV3Kind
   * - BABYLON.VertexBuffer.UV4Kind
   * - BABYLON.VertexBuffer.UV5Kind
   * - BABYLON.VertexBuffer.UV6Kind
   * - BABYLON.VertexBuffer.ColorKind
   * - BABYLON.VertexBuffer.MatricesIndicesKind
   * - BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * - BABYLON.VertexBuffer.MatricesWeightsKind
   * - BABYLON.VertexBuffer.MatricesWeightsExtraKind
   */
  Mesh* updateVerticesData(unsigned int kind, const Float32Array& data,
                           bool updateExtends = false,
                           bool makeItUnique  = false) override;

  /**
   * @brief This method updates the vertex positions of an updatable mesh
   * according to
   * the `positionFunction` returned values.
   * tuto :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#other-shapes-updatemeshpositions
   * The parameter `positionFunction` is a simple JS function what is passed the
   * mesh `positions` array. It doesn't need to return anything.
   * The parameter `computeNormals` is a boolean (default true) to
   * enable/disable the mesh normal recomputation after the vertex position
   * update.
   * @returns The Mesh.
   */
  Mesh& updateMeshPositions(
    std::function<void(Float32Array& positions)> positionFunction,
    bool computeNormals = true);

  /**
   * @brief This method will force the computation of normals for the mesh.
   * Please note that the mesh must have normals vertex data already.
   * @returns The Mesh.
   */
  Mesh* recomputeNormals();

  /**
   * @brief Creates a un-shared specific occurence of the geometry for the mesh.
   * @returns The Mesh.
   */
  Mesh& makeGeometryUnique();

  /**
   * @brief Sets the mesh indices.
   * Expects an IndicesArray.
   * If the mesh has no geometry, a new Geometry object is created and set to
   * the mesh.
   * This method creates a new index buffer each call.
   */
  Mesh* setIndices(const IndicesArray& indices,
                   size_t totalVertices = 0) override;

  /**
   * @brief Invert the geometry to move from a right handed system to a left
   * handed one.
   * @returns The Mesh.
   */
  Mesh& toLeftHanded();

  virtual void _bind(SubMesh* subMesh, Effect* effect, unsigned int fillMode);
  virtual void _draw(SubMesh* subMesh, int fillMode, size_t instancesCount = 0);

  /**
   * @brief Registers for this mesh a javascript function called just before the
   * rendering process.
   * This function is passed the current mesh.
   * @returns The Mesh.
   */
  Mesh&
  registerBeforeRender(const std::function<void(AbstractMesh* mesh)>& func);

  /**
   * @brief Disposes a previously registered javascript function called before
   * the rendering.
   * This function is passed the current mesh.
   * @returns The Mesh.
   */
  Mesh&
  unregisterBeforeRender(const std::function<void(AbstractMesh* mesh)>& func);

  /**
   * @brief Registers for this mesh a javascript function called just after the
   * rendering is complete.
   * This function is passed the current mesh.
   * @returns The Mesh.
   */
  Mesh&
  registerAfterRender(const std::function<void(AbstractMesh* mesh)>& func);

  /**
   * @brief Disposes a previously registered javascript function called after
   * the rendering.
   * This function is passed the current mesh.
   * @returns The Mesh.
   */
  Mesh&
  unregisterAfterRender(const std::function<void(AbstractMesh* mesh)>& func);

  _InstancesBatch* _getInstancesRenderList(size_t subMeshId);
  Mesh& _renderWithInstances(SubMesh* subMesh, int fillMode,
                             _InstancesBatch* batch, Effect* effect,
                             Engine* engine);
  Mesh& _processRendering(SubMesh* subMesh, Effect* effect, int fillMode,
                          _InstancesBatch* batch,
                          bool hardwareInstancedRendering,
                          std::function<void(bool isInstance, Matrix world,
                                             Material* effectiveMaterial)>
                            onBeforeDraw,
                          Material* effectiveMaterial = nullptr);

  /**
   * @brief Triggers the draw call for the mesh.
   * Usually, you don't need to call this method by your own because the mesh
   * rendering is handled by the scene rendering manager.
   * @returns Returns the Mesh.
   */
  Mesh& render(SubMesh* subMesh, bool enableAlphaMode);

  /**
   * @brief Returns an array populated with ParticleSystem objects whose the
   * mesh is the emitter.
   */
  std::vector<ParticleSystem*> getEmittedParticleSystems();

  /**
   * @brief Returns an array populated with ParticleSystem objects whose the
   * mesh or its children are the emitter.
   */
  std::vector<ParticleSystem*> getHierarchyEmittedParticleSystems();

  std::vector<Node*> getChildren();
  Mesh& _checkDelayState();

  /**
   * @brief Returns true if the mesh in the frustum defined by the Plane objects
   * from the `frustumPlanes` array parameter.
   */
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes) override;

  /**
   * @brief Sets the mesh material by the material or multiMaterial `id`
   * property.
   * The material `id` is a string identifying the material or the
   * multiMaterial.
   * @returns the Mesh.
   */
  Mesh& setMaterialByID(const std::string& id);

  /**
   * @brief Returns as a new array populated with the mesh material and/or
   * skeleton, if any.
   */
  std::vector<IAnimatable*> getAnimatables();

  /**
   * @brief Modifies the mesh geometry according to the passed transformation
   * matrix.
   * This method returns nothing but it really modifies the mesh even if it's
   * originally not set as updatable.
   * The mesh normals are modified accordingly the same transformation.
   * tuto :
   * http://doc.babylonjs.com/tutorials/How_Rotations_and_Translations_Work#baking-transform
   * Note that, under the hood, this method sets a new VertexBuffer each call.
   * @returns the Mesh.
   */
  Mesh& bakeTransformIntoVertices(const Matrix& transform);

  /**
   * @brief Modifies the mesh geometry according to its own current World
   * Matrix. The mesh World Matrix is then reset.
   * This method returns nothing but really modifies the mesh even if it's
   * originally not set as updatable.
   * tuto : tuto :
   * http://doc.babylonjs.com/tutorials/How_Rotations_and_Translations_Work#baking-transform
   * Note that, under the hood, this method sets a new VertexBuffer each call.
   * @returns the Mesh.
   */
  Mesh& bakeCurrentTransformIntoVertices();

  /** Cache **/
  std::vector<Vector3>& _positions() override;
  Mesh& _resetPointsArrayCache();
  bool _generatePointsArray() override;

  /** Clone **/

  /**
   * @brief Returns a new Mesh object generated from the current mesh
   * properties. This method must not get confused with createInstance().
   * The parameter `name` is a string, the name given to the new mesh.
   * The optional parameter `newParent` can be any `Node` object (default
   * `null`).
   * The optional parameter `newParent` can be any Node object (default `null`).
   * The optional parameter `doNotCloneChildren` (default `false`) allows/denies
   * the recursive cloning of the original mesh children if any.
   * The parameter `clonePhysicsImpostor` (default `true`)  allows/denies the
   * cloning in the same time of the original mesh `body` used by the physics
   * engine, if any.
   */
  Mesh* clone(const std::string& name, Node* newParent = nullptr,
              bool doNotCloneChildren   = false,
              bool clonePhysicsImpostor = true);

  /** Dispose **/

  /**
   * @brief Disposes the mesh.
   * This also frees the memory allocated under the hood to all the buffers used
   * by WebGL.
   */
  virtual void dispose(bool doNotRecurse = false) override;

  /** Geometric tools **/

  /**
   * @brief Modifies the mesh geometry according to a displacement map.
   * A displacement map is a colored image. Each pixel color value (actually a
   * gradient computed from red, green, blue values) will give the displacement
   * to apply to each mesh vertex.
   * The mesh must be set as updatable. Its internal geometry is directly
   * modified, no new buffer are allocated.
   * This method returns nothing.
   * The parameter `url` is a string, the URL from the image file is to be
   * downloaded.
   * The parameters `minHeight` and `maxHeight` are the lower and upper limits
   * of the displacement.
   * The parameter `onSuccess` is an optional Javascript function to be called
   * just after the mesh is modified. It is passed the modified mesh and must
   * return nothing.
   */
  void applyDisplacementMap(const std::string& url, int minHeight,
                            int maxHeight,
                            const std::function<void(Mesh* mesh)> onSuccess);

  /**
   * @brief Modifies the mesh geometry according to a displacementMap buffer.
   * A displacement map is a colored image. Each pixel color value (actually a
   * gradient computed from red, green, blue values) will give the displacement
   * to apply to each mesh vertex.
   * The mesh must be set as updatable. Its internal geometry is directly
   * modified, no new buffer are allocated.
   * This method returns nothing.
   * The parameter `buffer` is a `Uint8Array` buffer containing series of
   * `Uint8` lower than 255, the red, green, blue and alpha values of each
   * successive pixel.
   * The parameters `heightMapWidth` and `heightMapHeight` are positive integers
   * to set the width and height of the buffer image.
   * The parameters `minHeight` and `maxHeight` are the lower and upper limits
   * of the displacement.
   */
  void applyDisplacementMapFromBuffer(const Uint8Array& buffer,
                                      unsigned int heightMapWidth,
                                      unsigned int heightMapHeight,
                                      int minHeight, int maxHeight);

  /**
   * @brief Modify the mesh to get a flat shading rendering.
   * This means each mesh facet will then have its own normals. Usually new
   * vertices are added in the mesh geometry to get this result.
   * This method returns nothing.
   * Warning : the mesh is really modified even if not set originally as
   * updatable and, under the hood, a new VertexBuffer is allocated.
   * @returns the Mesh.
   */
  Mesh& convertToFlatShadedMesh();

  /**
   * @brief This method removes all the mesh indices and add new vertices
   * (duplication) in order to unfold facets into buffers.
   * In other words, more vertices, no more indices and a single bigger VBO.
   * This method returns nothing.
   * The mesh is really modified even if not set originally as updatable. Under
   * the hood, a new VertexBuffer is allocated.
   * @returns the Mesh.
   */
  Mesh& convertToUnIndexedMesh();

  /**
   * @brief Inverses facet orientations and inverts also the normals with
   * `flipNormals` (default `false`) if true.
   * @returns the Mesh.
   * Warning : the mesh is really modified even if not set originally as
   * updatable. A new VertexBuffer is created under the hood each call.
   */
  Mesh& flipFaces(bool flipNormals = false);

  /** Instances **/

  /**
   * @brief Creates a new InstancedMesh object from the mesh model.
   * An instance shares the same properties and the same material than its
   * model.
   * Only these properties of each instance can then be set individually :
   * - position
   * - rotation
   * - rotationQuaternion
   * - setPivotMatrix
   * - scaling
   * tuto : http://doc.babylonjs.com/tutorials/How_to_use_Instances
   * Warning : this method is not supported for Line mesh and LineSystem
   */
  InstancedMesh* createInstance(const std::string& name);

  /**
   * @brief Synchronises all the mesh instance submeshes to the current mesh
   * submeshes, if any.
   * After this call, all the mesh instances have the same submeshes than the
   * current mesh.
   * @returns the Mesh.
   */
  Mesh& synchronizeInstances();

  /**
   * @brief Simplify the mesh according to the given array of settings.
   * Function will return immediately and will simplify async.
   * @param settings a collection of simplification settings.
   * @param parallelProcessing should all levels calculate parallel or one after
   * the other.
   * @param type the type of simplification to run.
   * @param successCallback optional success callback to be called after the
   * simplification finished processing all settings.
   */
  /*void
  simplify(const std::vector<ISimplificationSettings*>& settings,
           std::function<void(Mesh* mesh, int submeshIndex)>& successCallback,
           bool parallelProcessing = true, SimplificationType simplificationType
                                           = SimplificationType::QUADRATIC);*/

  /**
   * @brief Optimization of the mesh's indices, in case a mesh has duplicated
   * vertices. The function will only reorder the indices and will not remove
   * unused vertices to avoid problems with submeshes.
   * This should be used together with the simplification to avoid disappearing
   * triangles.
   * @param successCallback an optional success callback to be called after the
   * optimization finished.
   */
  void optimizeIndices(const std::function<void(Mesh* mesh)>& successCallback);

  void _syncGeometryWithMorphTargetManager();

  /** Statics **/

  /**
   * @brief Returns a new Mesh object what is a deep copy of the passed mesh.
   * The parameter `parsedMesh` is the mesh to be copied.
   * The parameter `rootUrl` is a string, it's the root URL to prefix the
   * `delayLoadingFile` property with
   */
  static Mesh* Parse(const Json::value& parsedMesh, Scene* scene,
                     const std::string& rootUrl);

  /**
   * @brief Creates a ribbon mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The ribbon is a parametric shape :
   * http://doc.babylonjs.com/tutorials/Parametric_Shapes.  It has no predefined
   * shape. Its final shape will depend on the input parameters.
   *
   * Please read this full tutorial to understand how to design a ribbon :
   * http://doc.babylonjs.com/tutorials/Ribbon_Tutorial
   * The parameter `pathArray` is a required array of paths, what are each an
   * array of successive Vector3. The pathArray parameter depicts the ribbon
   * geometry.
   * The parameter `closeArray` (boolean, default false) creates a seam between
   * the first and the last paths of the path array.
   * The parameter `closePath` (boolean, default false) creates a seam between
   * the first and the last points of each path of the path array.
   * The parameter `offset` (positive integer, default : rounded half size of
   * the pathArray length), is taken in account only if the `pathArray` is
   * containing a single path.
   * It's the offset to join together the points from the same path. Ex : offset
   * = 10 means the point 1 is joined to the point 11.
   * The optional parameter `instance` is an instance of an existing Ribbon
   * object to be updated with the passed `pathArray` parameter :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#ribbon
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateRibbon(const std::string& name,
                            const std::vector<std::vector<Vector3>>& pathArray,
                            bool closeArray = false, bool closePath = false,
                            int offset = -1, Scene* = nullptr,
                            bool updatable               = false,
                            unsigned int sideOrientation = Mesh::DEFAULTSIDE,
                            Mesh* ribbonInstance         = nullptr);

  /**
   * @brief Creates a plane polygonal mesh.  By default, this is a disc.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `radius` sets the radius size (float) of the polygon (default
   * 0.5).
   * The parameter `tessellation` sets the number of polygon sides (positive
   * integer, default 64). So a tessellation valued to 3 will build a triangle,
   * to 4 a square, etc.
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateDisc(const std::string& name, float radius = 0.5f,
                          unsigned int tessellation = 64, Scene* = nullptr,
                          bool updatable               = false,
                          unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates a box mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `size` sets the size (float) of each box side (default 1).
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateBox(const std::string& name, float size = 1.f,
                         Scene* = nullptr, bool updatable = false,
                         unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates a sphere mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `diameter` sets the diameter size (float) of the sphere
   * (default 1).
   * The parameter `segments` sets the sphere number of horizontal stripes
   * (positive integer, default 32).
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateSphere(const std::string& name, unsigned int segments = 32,
                            float diameter = 1.f, Scene* = nullptr,
                            bool updatable               = false,
                            unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates a cylinder or a cone mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `height` sets the height size (float) of the cylinder/cone
   * (float, default 2).
   * The parameter `diameter` sets the diameter of the top and bottom cap at
   * once (float, default 1).
   * The parameters `diameterTop` and `diameterBottom` overwrite the parameter
   * `diameter` and set respectively the top cap and bottom cap diameter
   * (floats, default 1). The parameter "diameterBottom" can't be zero.
   * The parameter `tessellation` sets the number of cylinder sides (positive
   * integer, default 24). Set it to 3 to get a prism for instance.
   * The parameter `subdivisions` sets the number of rings along the cylinder
   * height (positive integer, default 1).
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateCylinder(const std::string& name, float height = 2.f,
                              float diameterTop         = 1.f,
                              float diameterBottom      = 1.f,
                              unsigned int tessellation = 24,
                              unsigned int subdivisions = 1, Scene* = nullptr,
                              bool updatable               = false,
                              unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  // Torus  (Code from SharpDX.org)
  /**
   * @brief Creates a torus mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `diameter` sets the diameter size (float) of the torus
   * (default 1).
   * The parameter `thickness` sets the diameter size of the tube of the torus
   * (float, default 0.5).
   * The parameter `tessellation` sets the number of torus sides (postive
   * integer, default 16).
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateTorus(const std::string& name, float diameter = 1.f,
                           float thickness           = 0.5f,
                           unsigned int tessellation = 16, Scene* = nullptr,
                           bool updatable               = false,
                           unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates a torus knot mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `radius` sets the global radius size (float) of the torus
   * knot (default 2).
   * The parameter `radialSegments` sets the number of sides on each tube
   * segments (positive integer, default 32).
   * The parameter `tubularSegments` sets the number of tubes to decompose the
   * knot into (positive integer, default 32).
   * The parameters `p` and `q` are the number of windings on each axis
   * (positive integers, default 2 and 3).
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh*
  CreateTorusKnot(const std::string& name, float radius = 2.f,
                  float tube = 0.5f, unsigned int radialSegments = 32,
                  unsigned int tubularSegments = 32, float p = 2.f,
                  float q = 3.f, Scene* = nullptr, bool updatable = false,
                  unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates a line mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * A line mesh is considered as a parametric shape since it has no predefined
   * original shape. Its shape is determined by the passed array of points as an
   * input parameter.
   * Like every other parametric shape, it is dynamically updatable by passing
   * an existing instance of LineMesh to this static function.
   * The parameter `points` is an array successive Vector3.
   * The optional parameter `instance` is an instance of an existing LineMesh
   * object to be updated with the passed `points` parameter :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#lines-and-dashedlines
   * When updating an instance, remember that only point positions can change,
   * not the number of points.
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static LinesMesh* CreateLines(const std::string& name,
                                const std::vector<Vector3>& points, Scene*,
                                bool updatable           = false,
                                LinesMesh* linesInstance = nullptr);

  /**
   * @brief Creates a dashed line mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * A dashed line mesh is considered as a parametric shape since it has no
   * predefined original shape. Its shape is determined by the passed array of
   * points as an input parameter.
   * Like every other parametric shape, it is dynamically updatable by passing
   * an existing instance of LineMesh to this static function.
   * The parameter `points` is an array successive Vector3.
   * The parameter `dashNb` is the intended total number of dashes (positive
   * integer, default 200).
   * The parameter `dashSize` is the size of the dashes relatively the dash
   * number (positive float, default 3).
   * The parameter `gapSize` is the size of the gap between two successive
   * dashes relatively the dash number (positive float, default 1).
   * The optional parameter `instance` is an instance of an existing LineMesh
   * object to be updated with the passed `points` parameter :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#lines-and-dashedlines
   * When updating an instance, remember that only point positions can change,
   * not the number of points.
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static LinesMesh* CreateDashedLines(const std::string& name,
                                      std::vector<Vector3>& points,
                                      float dashSize = 3.f, float gapSize = 1.f,
                                      unsigned int dashNb = 200,
                                      Scene* = nullptr, bool updatable = false,
                                      LinesMesh* linesInstance = nullptr);
  /**
   * @brief Creates a polygon mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The polygon's shape will depend on the input parameters and is constructed
   * parallel to a ground mesh.
   * The parameter `shape` is a required array of successive Vector3
   * representing the corners of the polygon in th XoZ plane, that is y = 0 for
   * all vectors.
   * You can set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   * Remember you can only change the shape positions, not their number when
   * updating a polygon.
   */
  static Mesh* CreatePolygon(const std::string& name,
                             const std::vector<Vector3>& shape, Scene* scene,
                             const std::vector<std::vector<Vector3>>& holes,
                             bool updatable               = false,
                             unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * Creates an extruded polygon mesh, with depth in the Y direction.
   * Please consider using the same method from the MeshBuilder class instead.
   */
  static Mesh* ExtrudePolygon(const std::string& name,
                              const std::vector<Vector3>& shape, float depth,
                              Scene* scene,
                              const std::vector<std::vector<Vector3>>& holes,
                              bool updatable               = false,
                              unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates an extruded shape mesh.
   * The extrusion is a parametric shape :
   * http://doc.babylonjs.com/tutorials/Parametric_Shapes.  It has no predefined
   * shape. Its final shape will depend on the input parameters.
   * Please consider using the same method from the MeshBuilder class instead.
   *
   * Please read this full tutorial to understand how to design an extruded
   * shape : http://doc.babylonjs.com/tutorials/Parametric_Shapes#extrusion
   * The parameter `shape` is a required array of successive Vector3. This array
   * depicts the shape to be extruded in its local space : the shape must be
   * designed in the xOy plane and will be
   * extruded along the Z axis.
   * The parameter `path` is a required array of successive Vector3. This is the
   * axis curve the shape is extruded along.
   * The parameter `rotation` (float, default 0 radians) is the angle value to
   * rotate the shape each step (each path point), from the former step (so
   * rotation added each step) along the curve.
   * The parameter `scale` (float, default 1) is the value to scale the shape.
   * The parameter `cap` sets the way the extruded shape is capped. Possible
   * values : BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START,
   * BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL
   * The optional parameter `instance` is an instance of an existing
   * ExtrudedShape object to be updated with the passed `shape`, `path`, `scale`
   * or `rotation` parameters :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#extruded-shape
   * Remember you can only change the shape or path point positions, not their
   * number when updating an extruded shape.
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* ExtrudeShape(const std::string& name,
                            const std::vector<Vector3>& shape,
                            const std::vector<Vector3>& path, float scale,
                            float rotation, unsigned int cap, Scene*,
                            bool updatable               = false,
                            unsigned int sideOrientation = Mesh::DEFAULTSIDE,
                            Mesh* instance               = nullptr);

  /**
   * @brief Creates an custom extruded shape mesh.
   * The custom extrusion is a parametric shape :
   * http://doc.babylonjs.com/tutorials/Parametric_Shapes.  It has no predefined
   * shape. Its final shape will depend on the input parameters.
   * Please consider using the same method from the MeshBuilder class instead.
   *
   * Please read this full tutorial to understand how to design a custom
   * extruded shape :
   * http://doc.babylonjs.com/tutorials/Parametric_Shapes#extrusion
   * The parameter `shape` is a required array of successive Vector3. This array
   * depicts the shape to be extruded in its local space : the shape must be
   * designed in the xOy plane and will be
   * extruded along the Z axis.
   * The parameter `path` is a required array of successive Vector3. This is the
   * axis curve the shape is extruded along.
   * The parameter `rotationFunction` (JS function) is a custom Javascript
   * function called on each path point. This function is passed the position i
   * of the point in the path
   * and the distance of this point from the begining of the path :
   * ```javascript
   * var rotationFunction = function(i, distance) {
   *     // do things
   *     return rotationValue; }
   * ```
   * It must returns a float value that will be the rotation in radians applied
   * to the shape on each path point.
   * The parameter `scaleFunction` (JS function) is a custom Javascript function
   * called on each path point. This function is passed the position i of the
   * point in the path
   * and the distance of this point from the begining of the path :
   * ```javascript
   * var scaleFunction = function(i, distance) {
   *     // do things
   *    return scaleValue;}
   * ```
   * It must returns a float value that will be the scale value applied to the
   * shape on each path point.
   * The parameter `ribbonClosePath` (boolean, default false) forces the
   * extrusion underlying ribbon to close all the paths in its `pathArray`.
   * The parameter `ribbonCloseArray` (boolean, default false) forces the
   * extrusion underlying ribbon to close its `pathArray`.
   * The parameter `cap` sets the way the extruded shape is capped. Possible
   * values : BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START,
   * BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL
   * The optional parameter `instance` is an instance of an existing
   * ExtrudedShape object to be updated with the passed `shape`, `path`, `scale`
   * or `rotation` parameters :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#extruded-shape
   * Remember you can only change the shape or path point positions, not their
   * number when updating an extruded shape.
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* ExtrudeShapeCustom(
    const std::string& name, const std::vector<Vector3>& shape,
    const std::vector<Vector3>& path,
    const std::function<float(float i, float distance)>& scaleFunction,
    const std::function<float(float i, float distance)>& rotationFunction,
    bool ribbonCloseArray, bool ribbonClosePath, unsigned int cap, Scene*,
    bool updatable = false, unsigned int sideOrientation = Mesh::DEFAULTSIDE,
    Mesh* instance = nullptr);

  /**
   * @brief Creates lathe mesh.
   * The lathe is a shape with a symetry axis : a 2D model shape is rotated
   * around this axis to design the lathe.
   * Please consider using the same method from the MeshBuilder class instead.
   *
   * The parameter `shape` is a required array of successive Vector3. This array
   * depicts the shape to be rotated in its local space : the shape must be
   * designed in the xOy plane and will be
   * rotated around the Y axis. It's usually a 2D shape, so the Vector3 z
   * coordinates are often set to zero.
   * The parameter `radius` (positive float, default 1) is the radius value of
   * the lathe.
   * The parameter `tessellation` (positive integer, default 64) is the side
   * number of the lathe.
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateLathe(const std::string& name,
                           const std::vector<Vector3>& shape, float radius,
                           unsigned int tessellation, Scene* = nullptr,
                           bool updatable               = false,
                           unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates a plane mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `size` sets the size (float) of both sides of the plane at
   * once (default 1).
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreatePlane(const std::string& name, float size,
                           Scene* = nullptr, bool updatable = false,
                           unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates a ground mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameters `width` and `height` (floats, default 1) set the width and
   * height sizes of the ground.
   * The parameter `subdivisions` (positive integer) sets the number of
   * subdivisions per side.
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateGround(const std::string& name, unsigned int width = 1,
                            unsigned int height       = 1,
                            unsigned int subdivisions = 1, Scene* = nullptr,
                            bool updatable = false);

  /**
   * @brief Creates a tiled ground mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameters `xmin` and `xmax` (floats, default -1 and 1) set the ground
   * minimum and maximum X coordinates.
   * The parameters `zmin` and `zmax` (floats, default -1 and 1) set the ground
   * minimum and maximum Z coordinates.
   * The parameter `subdivisions` is a javascript object `{w: positive integer,
   * h: positive integer}` (default `{w: 6, h: 6}`). `w` and `h` are the
   * numbers of subdivisions on the ground width and height. Each subdivision is
   * called a tile.
   * The parameter `precision` is a javascript object `{w: positive integer, h:
   * positive integer}` (default `{w: 2, h: 2}`). `w` and `h` are the
   * numbers of subdivisions on the ground width and height of each tile.
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateTiledGround(const std::string& name, float xmin,
                                 float zmin, float xmax, float zmax,
                                 const ISize& subdivisions = ISize(1, 1),
                                 const ISize& precision    = ISize(1, 1),
                                 Scene* = nullptr, bool updatable = false);

  /**
   * @brief Creates a ground mesh from a height map.
   * tuto : http://doc.babylonjs.com/tutorials/14._Height_Map
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `url` sets the URL of the height map image resource.
   * The parameters `width` and `height` (positive floats, default 10) set the
   * ground width and height sizes.
   * The parameter `subdivisions` (positive integer, default 1) sets the number
   * of subdivision per side.
   * The parameter `minHeight` (float, default 0) is the minimum altitude on the
   * ground.
   * The parameter `maxHeight` (float, default 1) is the maximum altitude on the
   * ground.
   * The parameter `onReady` is a javascript callback function that will be
   * called  once the mesh is just built (the height map download can last some
   * time).
   * This function is passed the newly built mesh :
   * ```javascript
   * function(mesh) { // do things
   *     return; }
   * ```
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static GroundMesh* CreateGroundFromHeightMap(
    const std::string& name, const std::string& url, unsigned int width,
    unsigned int height, unsigned int subdivisions, unsigned int minHeight,
    unsigned int maxHeight, Scene*, bool updatable = false,
    const std::function<void(GroundMesh* mesh)>& onReady = nullptr);

  /**
   * @brief Creates a tube mesh.
   * The tube is a parametric shape :
   * http://doc.babylonjs.com/tutorials/Parametric_Shapes.  It has no predefined
   * shape. Its final shape will depend on the input parameters.
   *
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `path` is a required array of successive Vector3. It is the
   * curve used as the axis of the tube.
   * The parameter `radius` (positive float, default 1) sets the tube radius
   * size.
   * The parameter `tessellation` (positive float, default 64) is the number of
   * sides on the tubular surface.
   * The parameter `radiusFunction` (javascript function, default null) is a
   * vanilla javascript function. If it is not null, it overwrittes the
   * parameter `radius`.
   * This function is called on each point of the tube path and is passed the
   * index `i` of the i-th point and the distance of this point from the first
   * point of the path.
   * It must return a radius value (positive float) :
   * ```javascript
   * var radiusFunction = function(i, distance) {
   *     // do things
   *     return radius; }
   * ```
   * The parameter `cap` sets the way the extruded shape is capped. Possible
   * values : BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START,
   * BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL
   * The optional parameter `instance` is an instance of an existing Tube object
   * to be updated with the passed `pathArray` parameter :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#tube
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateTube(
    const std::string& name, const std::vector<Vector3>& path,
    float radius = 1.f, unsigned int tessellation = 64,
    const std::function<float(unsigned int i, unsigned int distance)>&
      radiusFunction
    = nullptr,
    unsigned int cap = Mesh::NO_CAP, Scene* = nullptr, bool updatable = false,
    unsigned int sideOrientation = Mesh::DEFAULTSIDE, Mesh* instance = nullptr);

  /**
   * @brief Creates a polyhedron mesh.
   *
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `type` (positive integer, max 14, default 0) sets the
   * polyhedron type to build among the 15 embbeded types. Please refer to the
   * type sheet in the tutorial
   *  to choose the wanted type.
   * The parameter `size` (positive float, default 1) sets the polygon size.
   * You can overwrite the `size` on each dimension bu using the parameters
   * `sizeX`, `sizeY` or `sizeZ` (positive floats, default to `size` value).
   * You can build other polyhedron types than the 15 embbeded ones by setting
   * the parameter `custom` (`polyhedronObject`, default null). If you set the
   * parameter `custom`, this overwrittes the parameter `type`.
   * A `polyhedronObject` is a formatted javascript object. You'll find a full
   * file with pre-set polyhedra here :
   * https://github.com/BabylonJS/Extensions/tree/master/Polyhedron
   * You can set the color and the UV of each side of the polyhedron with the
   * parameters `faceColors` (Color4, default `(1, 1, 1, 1)`) and faceUV
   * (Vector4, default `(0, 0, 1, 1)`).
   * To understand how to set `faceUV` or `faceColors`, please read this by
   * considering the right number of faces of your polyhedron, instead of only 6
   * for the box :
   * http://doc.babylonjs.com/tutorials/CreateBox_Per_Face_Textures_And_Colors
   * The parameter `flat` (boolean, default true). If set to false, it gives the
   * polyhedron a single global face, so less vertices and shared normals. In
   * this case, `faceColors` and `faceUV` are ignored.
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreatePolyhedron(const std::string& name,
                                PolyhedronOptions& options, Scene*);

  /**
   * @brief Creates a sphere based upon an icosahedron with 20 triangular faces
   * which can be subdivided.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `radius` sets the radius size (float) of the icosphere
   * (default 1).
   * You can set some different icosphere dimensions, for instance to build an
   * ellipsoid, by using the parameters `radiusX`, `radiusY` and `radiusZ` (all
   * by default have the same value than `radius`).
   * The parameter `subdivisions` sets the number of subdivisions (postive
   * integer, default 4). The more subdivisions, the more faces on the icosphere
   * whatever its size.
   * The parameter `flat` (boolean, default true) gives each side its own
   * normals. Set it to false to get a smooth continuous light reflection on the
   * surface.
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * Detail here :
   * http://doc.babylonjs.com/tutorials/02._Discover_Basic_Elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static Mesh* CreateIcoSphere(const std::string& name,
                               IcoSphereOptions& options, Scene*);

  /**
   * @brief Creates a decal mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * A decal is a mesh usually applied as a model onto the surface of another
   * mesh. So don't forget the parameter `sourceMesh` depicting the decal.
   * The parameter `position` (Vector3, default `(0, 0, 0)`) sets the position
   * of the decal in World coordinates.
   * The parameter `normal` (Vector3, default `Vector3.Up`) sets the normal of
   * the mesh where the decal is applied onto in World coordinates.
   * The parameter `size` (Vector3, default `(1, 1, 1)`) sets the decal scaling.
   * The parameter `angle` (float in radian, default 0) sets the angle to rotate
   * the decal.
   */
  static Mesh* CreateDecal(const std::string& name, AbstractMesh* sourceMesh,
                           const Vector3& position, const Vector3& normal,
                           const Vector3& size, float angle = 0.f);

  /** Skeletons **/

  /**
   * @brief Returns the original positions used for CPU skinning.  Useful for
   * integrating Morphing with skeletons in same mesh.
   */
  Float32Array& setPositionsForCPUSkinning();

  /**
   * @brief Returns original normals used for CPU skinning.  Useful for
   * integrating Morphing with skeletons in same mesh.
   */
  Float32Array& setNormalsForCPUSkinning();

  /**
   * @brief Update the vertex buffers by applying transformation from the bones.
   * @param {skeleton} skeleton to apply
   */
  Mesh* applySkeleton(Skeleton* skeleton);

  /** Tools **/

  /**
   * @brief Returns an object `{min:` Vector3`, max:` Vector3`}`
   * This min and max Vector3 are the minimum and maximum vectors of each mesh
   * bounding box from the passed array, in the World system
   */
  static MinMax GetMinMax(const std::vector<AbstractMesh*>& meshes);

  /**
   * @brief Returns a Vector3, the center of the `{min:` Vector3`, max:`
   * Vector3`}` or the center of MinMax vector3 computed from a mesh array.
   */
  static Vector3 Center(const MinMaxDistance& MinMaxVectorAndDistance);
  static Vector3 Center(const MinMax& minMaxVector);
  static Vector3 Center(const std::vector<AbstractMesh*>& meshes);

  /**
   * @brief Merge the array of meshes into a single mesh for performance
   * reasons.
   * @param {Array<Mesh>} meshes - The vertices source.  They should all be of
   * the same material.  Entries can empty
   * @param {boolean} disposeSource - When true (default), dispose of the
   * vertices from the source meshes
   * @param {boolean} allow32BitsIndices - When the sum of the vertices > 64k,
   * this must be set to true.
   * @param {Mesh} meshSubclass - When set, vertices inserted into this Mesh.
   * Meshes can then be merged into a Mesh sub-class.
   * @param {boolean} subdivideWithSubMeshes - When true (false default),
   * subdivide mesh to his subMesh array with meshes source.
   */
  static Mesh* MergeMeshes(std::vector<Mesh*>& meshes,
                           bool disposeSource          = true,
                           bool allow32BitsIndices     = true,
                           Mesh* meshSubclass          = nullptr,
                           bool subdivideWithSubMeshes = false);

protected:
  /**
   * @brief Constructor
   *
   * @param {string} name The value used by scene.getMeshByName() to do a
   *                 lookup.
   * @param {Scene} scene The scene to add this mesh to.
   * @param {Node} parent The parent of this mesh, if it has one
   * @param {Mesh} source An optional Mesh from which geometry is shared,
   *               cloned.
   * @param {bool} doNotCloneChildren When cloning, skip cloning child meshes of
   *               source, default False. When false, achieved by calling a
   *               clone(), also passing False. This will make creation of
   *               children, recursive.
   */
  Mesh(const std::string& name, Scene* scene, Node* parent = nullptr,
       Mesh* source = nullptr, bool doNotCloneChildren = true,
       bool clonePhysicsImpostor = true);

private:
  void _sortLODLevels();
  Mesh& _onBeforeDraw(bool isInstance, Matrix& world,
                      Material* effectiveMaterial);
  Mesh& _queueLoad(Mesh* mesh, Scene* scene);

public:
  int delayLoadState;
  std::vector<InstancedMesh*> instances;
  std::string delayLoadingFile;
  std::string _binaryInfo;
  std::function<void(float distance, Mesh* mesh, Mesh* selectedLevel)>
    onLODLevelSelection;
  Geometry* _geometry;
  Uint32Array _delayInfoKinds;
  std::function<void(const Json::value& parsedGeometry, Mesh* mesh)>
    _delayLoadingFunction;
  std::unique_ptr<_VisibleInstances> _visibleInstances;
  bool _shouldGenerateFlatShading;

private:
  Observer<Mesh>::Ptr _onBeforeDrawObserver;
  std::vector<std::unique_ptr<MeshLODLevel>> _LODLevels;
  std::vector<Vector3> _emptyPositions;
  // Morph
  MorphTargetManager* _morphTargetManager;
  std::vector<VertexBuffer*> _delayInfo;
  Int32Array _renderIdForInstances;
  std::unique_ptr<_InstancesBatch> _batchCache;
  unsigned int _instancesBufferSize;
  std::unique_ptr<Buffer> _instancesBuffer;
  Float32Array _instancesData;
  size_t _overridenInstanceCount;
  int _preActivateId;
  unsigned int _sideOrientation;
  // Will be used by ribbons mainly
  bool _areNormalsFrozen;
  // Will be used to save original positions when using software skinning
  Float32Array _sourcePositions;
  // Will be used to save original normals when using software skinning
  Float32Array _sourceNormals;
  // Will be used to save a source mesh reference, If any
  Mesh* _source;
  // For extrusion and tube
  Path3D _path3D;
  std::vector<std::vector<Vector3>> _pathArray;
  unsigned int _tessellation;
  unsigned int _cap;
  float _arc;
  // For ribbon
  bool _closePath;
  bool _closeArray;
  Uint32Array _idx;

}; // end of class Mesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_MESH_H
