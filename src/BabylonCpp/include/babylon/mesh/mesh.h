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

private:
  // Consts
  static constexpr unsigned int _FRONTSIDE   = 0;
  static constexpr unsigned int _BACKSIDE    = 1;
  static constexpr unsigned int _DOUBLESIDE  = 2;
  static constexpr unsigned int _DEFAULTSIDE = 0;
  static constexpr unsigned int _NO_CAP      = 0;
  static constexpr unsigned int _CAP_START   = 1;
  static constexpr unsigned int _CAP_END     = 2;
  static constexpr unsigned int _CAP_ALL     = 3;

public:
  /** Consts **/

  /**
   * @brief Mesh side orientation : usually the external or front surface
   */
  static constexpr unsigned int FRONTSIDE()
  {
    return Mesh::_FRONTSIDE;
  }
  /**
   * @brief Mesh side orientation : usually the internal or back surface
   */
  static constexpr unsigned int BACKSIDE()
  {
    return Mesh::_BACKSIDE;
  }
  /**
   * @brief Mesh side orientation : both internal and external or front and back
   * surfaces
   */
  static constexpr unsigned int DOUBLESIDE()
  {
    return Mesh::_DOUBLESIDE;
  }
  /**
   * @brief Mesh side orientation : by default, `FRONTSIDE`
   */
  static constexpr unsigned int DEFAULTSIDE()
  {
    return Mesh::_DEFAULTSIDE;
  }
  /**
   * @brief Mesh cap setting : no cap
   */
  static constexpr unsigned int NO_CAP()
  {
    return Mesh::_NO_CAP;
  }
  /**
   * @brief Mesh cap setting : one cap at the beginning of the mesh
   */
  static constexpr unsigned int CAP_START()
  {
    return Mesh::_CAP_START;
  }
  /**
   * @brief Mesh cap setting : one cap at the end of the mesh
   */
  static constexpr unsigned int CAP_END()
  {
    return Mesh::_CAP_END;
  }
  /**
   * @brief Mesh cap setting : two caps, one at the beginning  and one at the
   * end of the mesh
   */
  static constexpr unsigned int CAP_ALL()
  {
    return Mesh::_CAP_ALL;
  }

public:
  template <typename... Ts>
  static MeshPtr New(Ts&&... args)
  {
    auto mesh = shared_ptr_t<Mesh>(new Mesh(::std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~Mesh() override;

  /**
   * @brief Returns the string "Mesh".
   */
  const string_t getClassName() const override;

  virtual IReflect::Type type() const override;

  /** Methods **/

  /**
   * @brief Returns a string.
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  string_t toString(bool fullDetails = false);

  void addSubMesh(SubMesh* subMesh);

  /**
   * @brief Hidden
   */
  void _unBindEffect() override;

  /**
   * @brief Gets the list of {BABYLON.MeshLODLevel} associated with the current
   * mesh
   * @returns an array of {BABYLON.MeshLODLevel}
   */
  vector_t<MeshLODLevel*> getLODLevels();

  /**
   * @brief Add a mesh as LOD level triggered at the given distance.
   * tuto : http://doc.babylonjs.com/tutorials/How_to_use_LOD
   * @param distance The distance from the center of the object to show this
   * level
   * @param mesh The mesh to be added as LOD level (can be null)
   * @return This mesh (for chaining)
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
   * @brief Returns the registered LOD mesh distant from the parameter `camera`.
   * position if any, else returns the current mesh.
   * tuto : http://doc.babylonjs.com/how_to/how_to_use_lod
   */
  AbstractMesh* getLOD(const CameraPtr& camera,
                       BoundingSphere* boundingSphere = nullptr) override;

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
   * @returns null if the mesh has no geometry or no vertex buffer.
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
   * @returns `null` if the mesh has no geometry.
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
  VertexBuffer* getVertexBuffer(unsigned int kind) const;

  bool isVerticesDataPresent(unsigned int kind) const override;

  /**
   * @brief Returns a boolean defining if the vertex data for the requested
   * `kind` is updatable.
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
  bool isVertexBufferUpdatable(unsigned int kind) const;

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
  Uint32Array getVerticesDataKinds() const;

  /**
   * @brief Returns a positive integer : the total number of indices in this
   * mesh geometry.
   * @returns Zero if the mesh has no geometry.
   */
  size_t getTotalIndices() const;

  /**
   * @brief Returns an array of integers or a typed array (Int32Array,
   * Uint32Array, Uint16Array) populated with the mesh indices.
   * @param copyWhenShared If true (default false) and and if the mesh geometry
   * is shared among some other meshes, the returned array is a copy of the
   * internal one.
   * @param forceCopy defines a boolean indicating that the returned array must
   * be cloned upon returning it
   * @returns the indices array or an empty array if the mesh has no geometry
   */
  IndicesArray getIndices(bool copyWhenShared = false,
                          bool forceCopy      = false) override;

  /**
   * @brief Determine if the current mesh is ready to be rendered
   * @param completeCheck defines if a complete check (including materials and
   * lights) has to be done (false by default)
   * @param forceInstanceSupport will check if the mesh will be ready when used
   * with instances (false by default)
   * @returns true if all associated assets are ready (material, textures,
   * shaders)
   */
  bool isReady(bool completeCheck        = false,
               bool forceInstanceSupport = false) override;

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

  /**
   * @brief Hidden
   */
  void _preActivate() override;

  /**
   * @brief Hidden
   */
  void _preActivateForIntermediateRendering(int renderId) override;

  /**
   * @brief Hidden
   */
  Mesh& _registerInstanceForRenderId(InstancedMesh* instance, int renderId);

  /**
   * @brief This method recomputes and sets a new BoundingInfo to the mesh
   * unless it is locked.
   * This means the mesh underlying bounding box and sphere are recomputed.
   * @returns The Mesh.
   */
  Mesh& refreshBoundingInfo();

  /**
   * @brief Hidden
   */
  Mesh& _refreshBoundingInfo(bool applySkeleton);

  /**
   * @brief Hidden
   */
  shared_ptr_t<SubMesh> _createGlobalSubMesh(bool force);

  void subdivide(size_t count);
  Mesh* setVerticesData(unsigned int kind, const Float32Array& data,
                        bool updatable                   = false,
                        const nullable_t<size_t>& stride = nullopt_t) override;

  void markVerticesDataAsUpdatable(unsigned int kind, bool updatable = true);

  /**
   * @brief Sets the mesh VertexBuffer.
   * @returns The Mesh.
   */
  Mesh& setVerticesBuffer(unique_ptr_t<VertexBuffer>&& buffer);

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
    ::std::function<void(Float32Array& positions)> positionFunction,
    bool computeNormals = true);

  /**
   * @brief Creates a un-shared specific occurence of the geometry for the mesh.
   * @returns The Mesh.
   */
  Mesh& makeGeometryUnique();

  Mesh* setIndices(const IndicesArray& indices, size_t totalVertices = 0,
                   bool updatable = false) override;

  /**
   * @brief Update the current index buffer
   * Expects an array populated with integers or a typed array (Int32Array,
   * Uint32Array, Uint16Array)
   * @returns The Mesh.
   */
  Mesh& updateIndices(const IndicesArray& indices, int offset = 0);

  /**
   * @brief Invert the geometry to move from a right handed system to a left
   * handed one.
   * @returns The Mesh.
   */
  Mesh& toLeftHanded();

  /**
   * @brief Hidden
   */
  virtual void _bind(SubMesh* subMesh, Effect* effect, unsigned int fillMode);

  /**
   * @brief Hidden
   */
  virtual void _draw(SubMesh* subMesh, int fillMode, size_t instancesCount = 0,
                     bool alternate = false);

  /**
   * @brief Registers for this mesh a javascript function called just before the
   * rendering process.
   * This function is passed the current mesh.
   * @returns The Mesh.
   */
  Mesh& registerBeforeRender(
    const ::std::function<void(Mesh* mesh, EventState& es)>& func);

  /**
   * @brief Disposes a previously registered javascript function called before
   * the rendering.
   * This function is passed the current mesh.
   * @returns The Mesh.
   */
  Mesh& unregisterBeforeRender(
    const ::std::function<void(Mesh* mesh, EventState&)>& func);

  /**
   * @brief Registers for this mesh a javascript function called just after the
   * rendering is complete.
   * This function is passed the current mesh.
   * @returns The Mesh.
   */
  Mesh& registerAfterRender(
    const ::std::function<void(Mesh* mesh, EventState&)>& func);

  /**
   * @brief Disposes a previously registered javascript function called after
   * the rendering.
   * This function is passed the current mesh.
   * @returns The Mesh.
   */
  Mesh& unregisterAfterRender(
    const ::std::function<void(Mesh* mesh, EventState&)>& func);

  /**
   * @brief Hidden
   */
  _InstancesBatch* _getInstancesRenderList(size_t subMeshId);

  /**
   * @brief Hidden
   */
  Mesh& _renderWithInstances(SubMesh* subMesh, unsigned int fillMode,
                             _InstancesBatch* batch, Effect* effect,
                             Engine* engine);

  /**
   * @brief Hidden
   */
  Mesh&
  _processRendering(SubMesh* subMesh, Effect* effect, int fillMode,
                    _InstancesBatch* batch, bool hardwareInstancedRendering,
                    ::std::function<void(bool isInstance, const Matrix& world,
                                         Material* effectiveMaterial)>
                      onBeforeDraw,
                    Material* effectiveMaterial = nullptr);

  /**
   * @brief Triggers the draw call for the mesh. Usually, you don't need to call
   * this method by your own because the mesh rendering is handled by the scene
   * rendering manager.
   * @param subMesh defines the subMesh to render
   * @param enableAlphaMode defines if alpha mode can be changed
   * @returns the current mesh
   */
  Mesh& render(SubMesh* subMesh, bool enableAlphaMode);

  /**
   * @brief Returns an array populated with IParticleSystem objects whose the
   * mesh is the emitter.
   */
  vector_t<IParticleSystemPtr> getEmittedParticleSystems();

  /**
   * @brief Returns an array populated with IParticleSystem objects whose the
   * mesh or its children are the emitter.
   */
  vector_t<IParticleSystemPtr> getHierarchyEmittedParticleSystems();

  /**
   * @brief Normalize matrix weights so that all vertices have a total weight
   * set to 1.
   */
  void cleanMatrixWeights();

  vector_t<NodePtr> getChildren();

  /**
   * @brief Hidden
   */
  Mesh& _checkDelayState();

  /**
   * @brief Returns `true` if the mesh is within the frustum defined by the
   * passed array of planes. A mesh is in the frustum if its bounding box
   * intersects the frustum
   * @param frustumPlanes defines the frustum to test
   * @returns true if the mesh is in the frustum planes
   */
  bool isInFrustum(const array_t<Plane, 6>& frustumPlanes,
                   unsigned int strategy = 0) override;

  /**
   * @brief Sets the mesh material by the material or multiMaterial `id`
   * property.
   * The material `id` is a string identifying the material or the
   * multiMaterial.
   * @returns the Mesh.
   */
  Mesh& setMaterialByID(const string_t& id);

  /**
   * @brief Returns as a new array populated with the mesh material and/or
   * skeleton, if any.
   */
  vector_t<IAnimatablePtr> getAnimatables();

  /**
   * @brief Modifies the mesh geometry according to the passed transformation
   * matrix. This method returns nothing but it really modifies the mesh even if
   * it's originally not set as updatable. The mesh normals are modified using
   * the same transformation. tuto :
   * http://doc.babylonjs.com/resources/baking_transformations Note that, under
   * the hood, this method sets a new VertexBuffer each call. Returns the Mesh.
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

  /**
   * @brief Hidden
   */
  Mesh& _resetPointsArrayCache();

  /**
   * @brief Hidden
   */
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
  MeshPtr clone(const string_t& name, Node* newParent = nullptr,
                bool doNotCloneChildren   = false,
                bool clonePhysicsImpostor = true);

  /** Dispose **/

  /**
   * @brief Releases resources associated with this mesh.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false) override;

  /** Geometric tools **/

  /**
   * @brief Modifies the mesh geometry according to a displacement map.
   * A displacement map is a colored image. Each pixel color value (actually a
   * gradient computed from red, green, blue values) will give the displacement
   * to apply to each mesh vertex. The mesh must be set as updatable. Its
   * internal geometry is directly modified, no new buffer are allocated. This
   * method returns nothing.
   * @param url is a string, the URL from the image file is to be downloaded.
   * @param minHeight is the lower limit of the displacement.
   * @param maxHeight is the upper limit of the displacement.
   * @param onSuccess is an optional Javascript function to be called just after
   * the mesh is modified. It is passed the modified mesh and must return
   * nothing.
   * @param uvOffset is an optional vector2 used to offset UV.
   * @param uvScale is an optional vector2 used to scale UV.
   * @param forceUpdate defines whether or not to force an update of the
   * generated buffers. This is usefull to apply on a deserialized model for
   * instance.
   * @returns the Mesh.
   */
  void applyDisplacementMap(const string_t& url, int minHeight, int maxHeight,
                            const ::std::function<void(Mesh* mesh)> onSuccess
                            = nullptr,
                            const nullable_t<Vector2>& uvOffset = nullopt_t,
                            const nullable_t<Vector2>& uvScale  = nullopt_t,
                            bool boolforceUpdate                = false);

  /**
   * @brief Modifies the mesh geometry according to a displacementMap buffer.
   * A displacement map is a colored image. Each pixel color value (actually a
   * gradient computed from red, green, blue values) will give the displacement
   * to apply to each mesh vertex. The mesh must be set as updatable. Its
   * internal geometry is directly modified, no new buffer are allocated.
   * @param buffer is a `Uint8Array` buffer containing series of `Uint8` lower
   * than 255, the red, green, blue and alpha values of each successive pixel.
   * @param heightMapWidth is the width of the buffer image.
   * @param heightMapHeight is the height of the buffer image.
   * @param minHeight is the lower limit of the displacement.
   * @param maxHeight is the upper limit of the displacement.
   * @param onSuccess is an optional Javascript function to be called just after
   * the mesh is modified. It is passed the modified mesh and must return
   * nothing.
   * @param uvOffset is an optional vector2 used to offset UV.
   * @param uvScale is an optional vector2 used to scale UV.
   * @param forceUpdate defines whether or not to force an update of the
   * generated buffers. This is usefull to apply on a deserialized model for
   * instance.
   * @returns the Mesh.
   */
  void applyDisplacementMapFromBuffer(
    const Uint8Array& buffer, unsigned int heightMapWidth,
    unsigned int heightMapHeight, int minHeight, int maxHeight,
    const nullable_t<Vector2>& uvOffset = nullopt_t,
    const nullable_t<Vector2>& uvScale = nullopt_t, bool forceUpdate = false);

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
   * model. Please make sure to call mesh.makeGeometryUnique() if you are
   * calling createInstance on a previously cloned mesh. Only these properties
   * of each instance can then be set individually :
   * - position
   * - rotation
   * - rotationQuaternion
   * - setPivotMatrix
   * - scaling
   *
   * @see http://doc.babylonjs.com/how_to/how_to_use_instances
   * Warning : this method is not supported for Line mesh and LineSystem
   */
  InstancedMeshPtr createInstance(const string_t& name);

  /**
   * @brief Synchronises all the mesh instance submeshes to the current mesh
   * submeshes, if any.
   * After this call, all the mesh instances have the same submeshes than the
   * current mesh.
   * @returns the Mesh.
   */
  Mesh& synchronizeInstances();

  /**
   * @brief Optimization of the mesh's indices, in case a mesh has duplicated
   * vertices. The function will only reorder the indices and will not remove
   * unused vertices to avoid problems with submeshes.
   * This should be used together with the simplification to avoid disappearing
   * triangles.
   * @param successCallback an optional success callback to be called after the
   * optimization finished.
   */
  void
  optimizeIndices(const ::std::function<void(Mesh* mesh)>& successCallback);

  /**
   * @brief Hidden
   */
  void _syncGeometryWithMorphTargetManager();

  /** Statics **/

  /**
   * @brief Returns a new Mesh object parsed from the source provided.
   * The parameter `parsedMesh` is the source.
   * The parameter `rootUrl` is a string, it's the root URL to prefix the
   * `delayLoadingFile` property with
   */
  static MeshPtr Parse(const Json::value& parsedMesh, Scene* scene,
                       const string_t& rootUrl);

  /**
   * @brief Creates a ribbon mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The ribbon is a parametric shape :
   * http://doc.babylonjs.com/how_to/parametric_shapes.  It has no predefined
   * shape. Its final shape will depend on the input parameters.
   *
   * Please read this full tutorial to understand how to design a ribbon :
   * http://doc.babylonjs.com/tutorials/Ribbon_Tutorial The parameter
   * `pathArray` is a required array of paths, what are each an array of
   * successive Vector3. The pathArray parameter depicts the ribbon geometry.
   * The parameter `closeArray` (boolean, default false) creates a seam between
   * the first and the last paths of the path array. The parameter `closePath`
   * (boolean, default false) creates a seam between the first and the last
   * points of each path of the path array. The parameter `offset` (positive
   * integer, default : rounded half size of the pathArray length), is taken in
   * account only if the `pathArray` is containing a single path. It's the
   * offset to join together the points from the same path. Ex : offset = 10
   * means the point 1 is joined to the point 11. The optional parameter
   * `instance` is an instance of an existing Ribbon object to be updated with
   * the passed `pathArray` parameter :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#ribbon
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr
  CreateRibbon(const string_t& name,
               const vector_t<vector_t<Vector3>>& pathArray,
               bool closeArray = false, bool closePath = false, int offset = -1,
               Scene* = nullptr, bool updatable = false,
               unsigned int sideOrientation  = Mesh::DEFAULTSIDE(),
               const MeshPtr& ribbonInstance = nullptr);

  /**
   * @brief Creates a plane polygonal mesh.  By default, this is a disc.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `radius` sets the radius size (float) of the polygon (default
   * 0.5). The parameter `tessellation` sets the number of polygon sides
   * (positive integer, default 64). So a tessellation valued to 3 will build a
   * triangle, to 4 a square, etc. You can also set the mesh side orientation
   * with the values : BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE
   * or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr CreateDisc(const string_t& name, float radius = 0.5f,
                            unsigned int tessellation = 64, Scene* = nullptr,
                            bool updatable               = false,
                            unsigned int sideOrientation = Mesh::DEFAULTSIDE());

  /**
   * @brief Creates a box mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `size` sets the size (float) of each box side (default 1).
   * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr CreateBox(const string_t& name, float size = 1.f,
                           Scene* = nullptr, bool updatable = false,
                           unsigned int sideOrientation = Mesh::DEFAULTSIDE());

  /**
   * @brief Creates a sphere mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `diameter` sets the diameter size (float) of the sphere
   * (default 1). The parameter `segments` sets the sphere number of horizontal
   * stripes (positive integer, default 32). You can also set the mesh side
   * orientation with the values : BABYLON.Mesh.FRONTSIDE (default),
   * BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr
  CreateSphere(const string_t& name, unsigned int segments = 32,
               float diameter = 1.f, Scene* = nullptr, bool updatable = false,
               unsigned int sideOrientation = Mesh::DEFAULTSIDE());

  /**
   * @brief Creates a cylinder or a cone mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `height` sets the height size (float) of the cylinder/cone
   * (float, default 2). The parameter `diameter` sets the diameter of the top
   * and bottom cap at once (float, default 1). The parameters `diameterTop` and
   * `diameterBottom` overwrite the parameter `diameter` and set respectively
   * the top cap and bottom cap diameter (floats, default 1). The parameter
   * "diameterBottom" can't be zero. The parameter `tessellation` sets the
   * number of cylinder sides (positive integer, default 24). Set it to 3 to get
   * a prism for instance. The parameter `subdivisions` sets the number of rings
   * along the cylinder height (positive integer, default 1). You can also set
   * the mesh side orientation with the values : BABYLON.Mesh.FRONTSIDE
   * (default), BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr
  CreateCylinder(const string_t& name, float height = 2.f,
                 float diameterTop = 1.f, float diameterBottom = 1.f,
                 unsigned int tessellation = 24, unsigned int subdivisions = 1,
                 Scene* = nullptr, bool updatable = false,
                 unsigned int sideOrientation = Mesh::DEFAULTSIDE());

  // Torus  (Code from SharpDX.org)
  /**
   * @brief Creates a torus mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `diameter` sets the diameter size (float) of the torus
   * (default 1). The parameter `thickness` sets the diameter size of the tube
   * of the torus (float, default 0.5). The parameter `tessellation` sets the
   * number of torus sides (postive integer, default 16). You can also set the
   * mesh side orientation with the values : BABYLON.Mesh.FRONTSIDE (default),
   * BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr
  CreateTorus(const string_t& name, float diameter = 1.f,
              float thickness = 0.5f, unsigned int tessellation = 16,
              Scene* = nullptr, bool updatable = false,
              unsigned int sideOrientation = Mesh::DEFAULTSIDE());

  /**
   * @brief Creates a torus knot mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `radius` sets the global radius size (float) of the torus
   * knot (default 2). The parameter `radialSegments` sets the number of sides
   * on each tube segments (positive integer, default 32). The parameter
   * `tubularSegments` sets the number of tubes to decompose the knot into
   * (positive integer, default 32). The parameters `p` and `q` are the number
   * of windings on each axis (positive integers, default 2 and 3). You can also
   * set the mesh side orientation with the values : BABYLON.Mesh.FRONTSIDE
   * (default), BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr
  CreateTorusKnot(const string_t& name, float radius = 2.f, float tube = 0.5f,
                  unsigned int radialSegments  = 32,
                  unsigned int tubularSegments = 32, float p = 2.f,
                  float q = 3.f, Scene* = nullptr, bool updatable = false,
                  unsigned int sideOrientation = Mesh::DEFAULTSIDE());

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
  static LinesMeshPtr CreateLines(const string_t& name,
                                  const vector_t<Vector3>& points, Scene*,
                                  bool updatable                    = false,
                                  const LinesMeshPtr& linesInstance = nullptr);

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
  static LinesMeshPtr CreateDashedLines(
    const string_t& name, vector_t<Vector3>& points, float dashSize = 3.f,
    float gapSize = 1.f, unsigned int dashNb = 200, Scene* = nullptr,
    bool updatable = false, const LinesMeshPtr& instance = nullptr);
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
  static MeshPtr
  CreatePolygon(const string_t& name, const vector_t<Vector3>& shape,
                Scene* scene, const vector_t<vector_t<Vector3>>& holes,
                bool updatable               = false,
                unsigned int sideOrientation = Mesh::DEFAULTSIDE());

  /**
   * @brief Creates an extruded polygon mesh, with depth in the Y direction.
   * Please consider using the same method from the MeshBuilder class instead.
   */
  static MeshPtr ExtrudePolygon(
    const string_t& name, const vector_t<Vector3>& shape, float depth,
    Scene* scene, const vector_t<vector_t<Vector3>>& holes,
    bool updatable = false, unsigned int sideOrientation = Mesh::DEFAULTSIDE());

  /**
   * @brief Creates an extruded shape mesh.
   * The extrusion is a parametric shape :
   * http://doc.babylonjs.com/how_to/parametric_shapes.  It has no predefined
   * shape. Its final shape will depend on the input parameters. Please consider
   * using the same method from the MeshBuilder class instead.
   *
   * Please read this full tutorial to understand how to design an extruded
   * shape : http://doc.babylonjs.com/how_to/parametric_shapes#extruded-shapes
   * The parameter `shape` is a required array of successive Vector3. This array
   * depicts the shape to be extruded in its local space : the shape must be
   * designed in the xOy plane and will be extruded along the Z axis. The
   * parameter `path` is a required array of successive Vector3. This is the
   * axis curve the shape is extruded along. The parameter `rotation` (float,
   * default 0 radians) is the angle value to rotate the shape each step (each
   * path point), from the former step (so rotation added each step) along the
   * curve. The parameter `scale` (float, default 1) is the value to scale the
   * shape. The parameter `cap` sets the way the extruded shape is capped.
   * Possible values : BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START,
   * BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL The optional parameter
   * `instance` is an instance of an existing ExtrudedShape object to be updated
   * with the passed `shape`, `path`, `scale` or `rotation` parameters :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#extruded-shape
   * Remember you can only change the shape or path point positions, not their
   * number when updating an extruded shape. You can also set the mesh side
   * orientation with the values : BABYLON.Mesh.FRONTSIDE (default),
   * BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr
  ExtrudeShape(const string_t& name, const vector_t<Vector3>& shape,
               const vector_t<Vector3>& path, float scale, float rotation,
               unsigned int cap, Scene*, bool updatable = false,
               unsigned int sideOrientation = Mesh::DEFAULTSIDE(),
               const MeshPtr& instance      = nullptr);

  /**
   * @brief Creates an custom extruded shape mesh.
   * The custom extrusion is a parametric shape :
   * http://doc.babylonjs.com/how_to/parametric_shapes.  It has no predefined
   * shape. Its final shape will depend on the input parameters. Please consider
   * using the same method from the MeshBuilder class instead.
   *
   * Please read this full tutorial to understand how to design a custom
   * extruded shape :
   * http://doc.babylonjs.com/how_to/parametric_shapes#extruded-shapes The
   * parameter `shape` is a required array of successive Vector3. This array
   * depicts the shape to be extruded in its local space : the shape must be
   * designed in the xOy plane and will be extruded along the Z axis. The
   * parameter `path` is a required array of successive Vector3. This is the
   * axis curve the shape is extruded along. The parameter `rotationFunction`
   * (JS function) is a custom Javascript function called on each path point.
   * This function is passed the position i of the point in the path and the
   * distance of this point from the begining of the path :
   * ```javascript
   * var rotationFunction = function(i, distance) {
   *     // do things
   *     return rotationValue; }
   * ```
   * It must returns a float value that will be the rotation in radians applied
   * to the shape on each path point. The parameter `scaleFunction` (JS
   * function) is a custom Javascript function called on each path point. This
   * function is passed the position i of the point in the path and the distance
   * of this point from the begining of the path :
   * ```javascript
   * var scaleFunction = function(i, distance) {
   *     // do things
   *    return scaleValue;}
   * ```
   * It must returns a float value that will be the scale value applied to the
   * shape on each path point. The parameter `ribbonClosePath` (boolean, default
   * false) forces the extrusion underlying ribbon to close all the paths in its
   * `pathArray`. The parameter `ribbonCloseArray` (boolean, default false)
   * forces the extrusion underlying ribbon to close its `pathArray`. The
   * parameter `cap` sets the way the extruded shape is capped. Possible values
   * : BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START,
   * BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL The optional parameter
   * `instance` is an instance of an existing ExtrudedShape object to be updated
   * with the passed `shape`, `path`, `scale` or `rotation` parameters :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#extruded-shape
   * Remember you can only change the shape or path point positions, not their
   * number when updating an extruded shape. You can also set the mesh side
   * orientation with the values : BABYLON.Mesh.FRONTSIDE (default),
   * BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr ExtrudeShapeCustom(
    const string_t& name, const vector_t<Vector3>& shape,
    const vector_t<Vector3>& path,
    const ::std::function<float(float i, float distance)>& scaleFunction,
    const ::std::function<float(float i, float distance)>& rotationFunction,
    bool ribbonCloseArray, bool ribbonClosePath, unsigned int cap, Scene*,
    bool updatable = false, unsigned int sideOrientation = Mesh::DEFAULTSIDE(),
    const MeshPtr& instance = nullptr);

  /**
   * @brief Creates lathe mesh.
   * The lathe is a shape with a symetry axis : a 2D model shape is rotated
   * around this axis to design the lathe. Please consider using the same method
   * from the MeshBuilder class instead. The parameter `shape` is a required
   * array of successive Vector3. This array depicts the shape to be rotated in
   * its local space : the shape must be designed in the xOy plane and will be
   * rotated around the Y axis. It's usually a 2D shape, so the Vector3 z
   * coordinates are often set to zero. The parameter `radius` (positive float,
   * default 1) is the radius value of the lathe. The parameter `tessellation`
   * (positive integer, default 64) is the side number of the lathe. You can
   * also set the mesh side orientation with the values : BABYLON.Mesh.FRONTSIDE
   * (default), BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr
  CreateLathe(const string_t& name, const vector_t<Vector3>& shape,
              float radius, unsigned int tessellation, Scene* = nullptr,
              bool updatable               = false,
              unsigned int sideOrientation = Mesh::DEFAULTSIDE());

  /**
   * @brief Creates a plane mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `size` sets the size (float) of both sides of the plane at
   * once (default 1). You can also set the mesh side orientation with the
   * values : BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr CreatePlane(const string_t& name, float size, Scene* = nullptr,
                             bool updatable = false,
                             unsigned int sideOrientation
                             = Mesh::DEFAULTSIDE());

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
  static MeshPtr CreateGround(const string_t& name, unsigned int width = 1,
                              unsigned int height       = 1,
                              unsigned int subdivisions = 1, Scene* = nullptr,
                              bool updatable = false);

  /**
   * @brief Creates a ground mesh from a height map.
   * tuto : http://doc.babylonjs.com/babylon101/height_map
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `url` sets the URL of the height map image resource.
   * The parameters `width` and `height` (positive floats, default 10) set the
   * ground width and height sizes. The parameter `subdivisions` (positive
   * integer, default 1) sets the number of subdivision per side. The parameter
   * `minHeight` (float, default 0) is the minimum altitude on the ground. The
   * parameter `maxHeight` (float, default 1) is the maximum altitude on the
   * ground. The parameter `onReady` is a javascript callback function that will
   * be called  once the mesh is just built (the height map download can last
   * some time). This function is passed the newly built mesh :
   * ```javascript
   * function(mesh) { // do things
   *     return; }
   * ```
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr CreateTiledGround(const string_t& name, float xmin, float zmin,
                                   float xmax, float zmax,
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
  static GroundMeshPtr CreateGroundFromHeightMap(
    const string_t& name, const string_t& url, unsigned int width,
    unsigned int height, unsigned int subdivisions, unsigned int minHeight,
    unsigned int maxHeight, Scene*, bool updatable = false,
    const ::std::function<void(GroundMesh* mesh)>& onReady = nullptr);

  /**
   * @brief Creates a tube mesh.
   * The tube is a parametric shape :
   * http://doc.babylonjs.com/how_to/parametric_shapes.  It has no predefined
   * shape. Its final shape will depend on the input parameters. Please consider
   * using the same method from the MeshBuilder class instead. The parameter
   * `path` is a required array of successive Vector3. It is the curve used as
   * the axis of the tube. The parameter `radius` (positive float, default 1)
   * sets the tube radius size. The parameter `tessellation` (positive float,
   * default 64) is the number of sides on the tubular surface. The parameter
   * `radiusFunction` (javascript function, default null) is a vanilla
   * javascript function. If it is not null, it overwrittes the parameter
   * `radius`. This function is called on each point of the tube path and is
   * passed the index `i` of the i-th point and the distance of this point from
   * the first point of the path. It must return a radius value (positive float)
   * :
   * ```javascript
   * var radiusFunction = function(i, distance) {
   *     // do things
   *     return radius; }
   * ```
   * The parameter `cap` sets the way the extruded shape is capped. Possible
   * values : BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START,
   * BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL The optional parameter
   * `instance` is an instance of an existing Tube object to be updated with the
   * passed `pathArray` parameter :
   * http://doc.babylonjs.com/tutorials/How_to_dynamically_morph_a_mesh#tube You
   * can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr CreateTube(
    const string_t& name, const vector_t<Vector3>& path, float radius = 1.f,
    unsigned int tessellation = 64,
    const ::std::function<float(unsigned int i, float distance)>& radiusFunction
    = nullptr,
    unsigned int cap = Mesh::NO_CAP(), Scene* = nullptr, bool updatable = false,
    unsigned int sideOrientation = Mesh::DEFAULTSIDE(),
    const MeshPtr& instance      = nullptr);

  /**
   * @brief Creates a polyhedron mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * The parameter `type` (positive integer, max 14, default 0) sets the
   * polyhedron type to build among the 15 embbeded types. Please refer to the
   * type sheet in the tutorial to choose the wanted type. The parameter `size`
   * (positive float, default 1) sets the polygon size. You can overwrite the
   * `size` on each dimension bu using the parameters `sizeX`, `sizeY` or
   * `sizeZ` (positive floats, default to `size` value). You can build other
   * polyhedron types than the 15 embbeded ones by setting the parameter
   * `custom` (`polyhedronObject`, default null). If you set the parameter
   * `custom`, this overwrittes the parameter `type`. A `polyhedronObject` is a
   * formatted javascript object. You'll find a full file with pre-set polyhedra
   * here : https://github.com/BabylonJS/Extensions/tree/master/Polyhedron You
   * can set the color and the UV of each side of the polyhedron with the
   * parameters `faceColors` (Color4, default `(1, 1, 1, 1)`) and faceUV
   * (Vector4, default `(0, 0, 1, 1)`). To understand how to set `faceUV` or
   * `faceColors`, please read this by considering the right number of faces of
   * your polyhedron, instead of only 6 for the box :
   * http://doc.babylonjs.com/tutorials/CreateBox_Per_Face_Textures_And_Colors
   * The parameter `flat` (boolean, default true). If set to false, it gives the
   * polyhedron a single global face, so less vertices and shared normals. In
   * this case, `faceColors` and `faceUV` are ignored. You can also set the mesh
   * side orientation with the values : BABYLON.Mesh.FRONTSIDE (default),
   * BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr CreatePolyhedron(const string_t& name,
                                  PolyhedronOptions& options, Scene*);

  /**
   * @brief Creates a sphere based upon an icosahedron with 20 triangular faces
   * which can be subdivided. Please consider using the same method from the
   * MeshBuilder class instead. The parameter `radius` sets the radius size
   * (float) of the icosphere (default 1). You can set some different icosphere
   * dimensions, for instance to build an ellipsoid, by using the parameters
   * `radiusX`, `radiusY` and `radiusZ` (all by default have the same value than
   * `radius`). The parameter `subdivisions` sets the number of subdivisions
   * (postive integer, default 4). The more subdivisions, the more faces on the
   * icosphere whatever its size. The parameter `flat` (boolean, default true)
   * gives each side its own normals. Set it to false to get a smooth continuous
   * light reflection on the surface. You can also set the mesh side orientation
   * with the values : BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE
   * or BABYLON.Mesh.DOUBLESIDE Detail here :
   * http://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once
   * created.
   */
  static MeshPtr CreateIcoSphere(const string_t& name,
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
  static MeshPtr CreateDecal(const string_t& name, AbstractMesh* sourceMesh,
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
  Mesh* applySkeleton(const SkeletonPtr& skeleton);

  /** Tools **/

  /**
   * @brief Returns an object `{min:` Vector3`, max:` Vector3`}`
   * This min and max Vector3 are the minimum and maximum vectors of each mesh
   * bounding box from the passed array, in the World system
   */
  static MinMax GetMinMax(const vector_t<AbstractMeshPtr>& meshes);

  /**
   * @brief Returns a Vector3, the center of the `{min:` Vector3`, max:`
   * Vector3`}` or the center of MinMax vector3 computed from a mesh array.
   */
  static Vector3 Center(const MinMaxDistance& MinMaxVectorAndDistance);
  static Vector3 Center(const MinMax& minMaxVector);
  static Vector3 Center(const vector_t<AbstractMeshPtr>& meshes);

  /**
   * @brief Merge the array of meshes into a single mesh for performance
   * reasons.
   * @param meshes - The vertices source.  They should all be of the same
   * material.  Entries can empty
   * @param disposeSource - When true (default), dispose of the vertices from
   * the source meshes
   * @param allow32BitsIndices - When the sum of the vertices > 64k, this must
   * be set to true.
   * @param meshSubclass - When set, vertices inserted into this Mesh.  Meshes
   * can then be merged into a Mesh sub-class.
   * @param subdivideWithSubMeshes - When true (false default), subdivide mesh
   * to his subMesh array with meshes source.
   */
  static MeshPtr MergeMeshes(const vector_t<MeshPtr>& meshes,
                             bool disposeSource          = true,
                             bool allow32BitsIndices     = true,
                             MeshPtr meshSubclass        = nullptr,
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
  Mesh(const string_t& name, Scene* scene, Node* parent = nullptr,
       Mesh* source = nullptr, bool doNotCloneChildren = true,
       bool clonePhysicsImpostor = true);

  /**
   * An event triggered before rendering the mesh
   */
  Observable<Mesh>& get_onBeforeRenderObservable();

  /**
   * An event triggered after rendering the mesh
   */
  Observable<Mesh>& get_onAfterRenderObservable();

  /**
   * An event triggered before drawing the mesh
   */
  Observable<Mesh>& get_onBeforeDrawObservable();

  void
  set_onBeforeDraw(const ::std::function<void(Mesh*, EventState&)>& callback);

  MorphTargetManagerPtr& get_morphTargetManager();
  void set_morphTargetManager(const MorphTargetManagerPtr& value);

  Mesh*& get_source();

  bool get_isUnIndexed() const;
  void set_isUnIndexed(bool value);

  /**
   * @brief Returns if the mesh has some Levels Of Details (LOD).
   */
  bool get_hasLODLevels() const;

  /**
   * @brief Returns the mesh internal Geometry object.
   */
  Geometry*& get_geometry();

  bool get_isBlocked() const override;

  /**
   * @brief Returns true if the normals aren't to be recomputed on next mesh
   * `positions` array update.
   * This property is pertinent only for updatable parametric shapes.
   */
  bool get_areNormalsFrozen() const;

  vector_t<Vector3>& get__positions();

private:
  void _sortLODLevels();
  Float32Array _getPositionData(bool applySkeleton);
  Mesh& _onBeforeDraw(bool isInstance, Matrix& world,
                      Material* effectiveMaterial);
  Mesh& _queueLoad(Scene* scene);

public:
  /** Events **/

  /**
   * An event triggered before rendering the mesh
   */
  ReadOnlyProperty<Mesh, Observable<Mesh>> onBeforeRenderObservable;

  /**
   * An event triggered after rendering the mesh
   */
  ReadOnlyProperty<Mesh, Observable<Mesh>> onAfterRenderObservable;

  /**
   * An event triggered before drawing the mesh
   */
  ReadOnlyProperty<Mesh, Observable<Mesh>> onBeforeDrawObservable;

  WriteOnlyProperty<Mesh, ::std::function<void(Mesh*, EventState&)>>
    onBeforeDraw;

  // Members
  int delayLoadState;
  vector_t<InstancedMesh*> instances;
  string_t delayLoadingFile;
  /** Hidden */
  string_t _binaryInfo;
  ::std::function<void(float distance, Mesh* mesh, Mesh* selectedLevel)>
    onLODLevelSelection;

  // Morph
  Property<Mesh, MorphTargetManagerPtr> morphTargetManager;

  /** Hidden */
  Geometry* _geometry;
  /** Hidden */
  Uint32Array _delayInfoKinds;
  /** Hidden */
  ::std::function<void(const Json::value& parsedGeometry, const MeshPtr& mesh)>
    _delayLoadingFunction;
  /** Hidden */
  unique_ptr_t<_VisibleInstances> _visibleInstances;
  /** Hidden */
  bool _shouldGenerateFlatShading;
  // Use by builder only to know what orientation were the mesh build in.
  /** Hidden */
  unsigned int _originalBuilderSideOrientation;
  nullable_t<unsigned int> overrideMaterialSideOrientation;

  /**
   * Will be used to save a source mesh reference, If any
   */
  ReadOnlyProperty<Mesh, Mesh*> source;

  Property<Mesh, bool> isUnIndexed;

  /**
   * True if the mesh has some Levels Of Details (LOD)
   * @returns a boolean.
   */
  ReadOnlyProperty<Mesh, bool> hasLODLevels;

  /**
   * Returns the mesh internal Geometry object
   */
  ReadOnlyProperty<Mesh, Geometry*> geometry;

  /**
   * Boolean : true if the normals aren't to be recomputed on next mesh
   * `positions` array update. This property is pertinent only for updatable
   * parametric shapes.
   */
  ReadOnlyProperty<Mesh, bool> areNormalsFrozen;

  /**
   * Cache
   */
  ReadOnlyProperty<Mesh, vector_t<Vector3>> _positions;

private:
  // Events
  Observable<Mesh> _onBeforeRenderObservable;
  Observable<Mesh> _onAfterRenderObservable;
  Observable<Mesh> _onBeforeDrawObservable;
  // Members
  Observer<Mesh>::Ptr _onBeforeDrawObserver;
  vector_t<unique_ptr_t<MeshLODLevel>> _LODLevels;
  vector_t<Vector3> _emptyPositions;
  // Morph
  MorphTargetManagerPtr _morphTargetManager;
  vector_t<VertexBuffer*> _delayInfo;
  Int32Array _renderIdForInstances;
  unique_ptr_t<_InstancesBatch> _batchCache;
  unsigned int _instancesBufferSize;
  unique_ptr_t<Buffer> _instancesBuffer;
  Float32Array _instancesData;
  size_t _overridenInstanceCount;
  MaterialPtr _effectiveMaterial;
  int _preActivateId;
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
  vector_t<vector_t<Vector3>> _pathArray;
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
