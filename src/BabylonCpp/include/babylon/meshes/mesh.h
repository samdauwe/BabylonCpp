#ifndef BABYLON_MESHES_MESH_H
#define BABYLON_MESHES_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/maths/isize.h>
#include <babylon/maths/path3d.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/iget_set_vertices_data.h>
#include <babylon/meshes/vertex_data_constants.h>

namespace BABYLON {

struct _CreationDataStorage;
struct _InstancesBatch;
struct _InstanceDataStorage;
struct _InternalMeshDataInfo;
struct _ThinInstanceDataStorage;
struct _VisibleInstances;
class Buffer;
class Effect;
class Geometry;
class GroundMesh;
class IAnimatable;
class IcoSphereOptions;
class InstancedMesh;
class IParticleSystem;
class LinesMesh;
class Mesh;
class MeshLODLevel;
class MorphTargetManager;
class PolyhedronOptions;
class VertexBuffer;
using _CreationDataStoragePtr = std::shared_ptr<_CreationDataStorage>;
using _InstancesBatchPtr      = std::shared_ptr<_InstancesBatch>;
using BufferPtr               = std::shared_ptr<Buffer>;
using EffectPtr               = std::shared_ptr<Effect>;
using GroundMeshPtr           = std::shared_ptr<GroundMesh>;
using IAnimatablePtr          = std::shared_ptr<IAnimatable>;
using InstancedMeshPtr        = std::shared_ptr<InstancedMesh>;
using IParticleSystemPtr      = std::shared_ptr<IParticleSystem>;
using LinesMeshPtr            = std::shared_ptr<LinesMesh>;
using MeshLODLevelPtr         = std::shared_ptr<MeshLODLevel>;
using MeshPtr                 = std::shared_ptr<Mesh>;
using MorphTargetManagerPtr   = std::shared_ptr<MorphTargetManager>;
using SubMeshPtr              = std::shared_ptr<SubMesh>;
using VertexBufferPtr         = std::shared_ptr<VertexBuffer>;

/**
 * @brief Represents a custom thin buffer that will be instanced.
 */
struct _UserThinInstanceBuffersStorage {
  std::unordered_map<std::string, Float32Array> data;
  std::unordered_map<std::string, size_t> sizes;
  std::unordered_map<std::string, VertexBufferPtr> vertexBuffers;
  std::unordered_map<std::string, size_t> strides;
}; // end of struct UserThinInstanceBuffersStorage

struct SkinningValidationResult {
  bool skinned;
  bool valid;
  std::string report;
}; // end of struct SkinningValidationResult

/**
 * @brief Class used to represent renderable models.
 */
class BABYLON_SHARED_EXPORT Mesh : public AbstractMesh {

  friend class MeshBuilder;

public:
  /** Consts **/

  /**
   * Mesh side orientation : usually the external or front surface
   */
  static constexpr unsigned int FRONTSIDE = VertexDataConstants::FRONTSIDE;

  /**
   * Mesh side orientation : usually the internal or back surface
   */
  static constexpr unsigned int BACKSIDE = VertexDataConstants::BACKSIDE;

  /**
   * Mesh side orientation : both internal and external or front and back
   * surfaces
   */
  static constexpr unsigned int DOUBLESIDE = VertexDataConstants::DOUBLESIDE;

  /**
   * Mesh side orientation : by default, `FRONTSIDE`
   */
  static constexpr unsigned int DEFAULTSIDE = VertexDataConstants::DEFAULTSIDE;

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

  /**
   * Mesh pattern setting : no flip or rotate
   */
  static constexpr unsigned int NO_FLIP = 0;

  /**
   * Mesh pattern setting : flip (reflect in y axis) alternate tiles on each row
   * or column
   */
  static constexpr unsigned int FLIP_TILE = 1;

  /**
   * Mesh pattern setting : rotate (180degs) alternate tiles on each row or
   * column
   */
  static constexpr unsigned int ROTATE_TILE = 2;

  /**
   * Mesh pattern setting : flip (reflect in y axis) all tiles on alternate rows
   */
  static constexpr unsigned int FLIP_ROW = 3;

  /**
   * Mesh pattern setting : rotate (180degs) all tiles on alternate rows
   */
  static constexpr unsigned int ROTATE_ROW = 4;

  /**
   * Mesh pattern setting : flip and rotate alternate tiles on each row or
   * column
   */
  static constexpr unsigned int FLIP_N_ROTATE_TILE = 5;

  /**
   * Mesh pattern setting : rotate pattern and rotate
   */
  static constexpr unsigned int FLIP_N_ROTATE_ROW = 6;

  /**
   * Mesh tile positioning : part tiles same on left/right or top/bottom
   */
  static constexpr unsigned int CENTER = 0;

  /**
   * Mesh tile positioning : part tiles on left
   */
  static constexpr unsigned int LEFT = 1;

  /**
   * Mesh tile positioning : part tiles on right
   */
  static constexpr unsigned int RIGHT = 2;

  /**
   * Mesh tile positioning : part tiles on top
   */
  static constexpr unsigned int TOP = 3;

  /**
   * Mesh tile positioning : part tiles on bottom
   */
  static constexpr unsigned int BOTTOM = 4;

  /**
   * @brief Gets the default side orientation.
   * @param orientation the orientation to value to attempt to get
   * @returns the default orientation
   */
  static constexpr unsigned int
  _GetDefaultSideOrientation(const std::optional<unsigned int>& orientation = std::nullopt)
  {
    // works as Mesh.FRONTSIDE is 0
    return orientation.value_or(Mesh::FRONTSIDE);
  }

public:
  template <typename... Ts>
  static MeshPtr New(const std::string& iName, Scene* scene, Node* iParent = nullptr,
                     Mesh* source = nullptr, bool doNotCloneChildren = true,
                     bool clonePhysicsImpostor = true)
  {
    auto mesh = std::shared_ptr<Mesh>(
      new Mesh(iName, scene, iParent, source, doNotCloneChildren, clonePhysicsImpostor, true));
    mesh->_initialize(scene, iParent, source, doNotCloneChildren, clonePhysicsImpostor);
    mesh->addToScene(mesh);

    return mesh;
  }
  ~Mesh() override; // = default

  // Methods
  TransformNodePtr instantiateHierarchy(
    TransformNode* newParent                                   = nullptr,
    const std::optional<InstantiateHierarychyOptions>& options = std::nullopt,
    const std::function<void(TransformNode* source, TransformNode* clone)>& onNewNodeCreated
    = nullptr);

  /**
   * @brief Gets the class name.
   * @returns the string "Mesh".
   */
  std::string getClassName() const override;

  /**
   * @brief Returns the class type.
   * @return the class type.
   */
  Type type() const override;

  /** Methods **/

  /**
   * @brief Returns a description of this mesh.
   * @param fullDetails define if full details about this mesh must be used
   * @returns a descriptive string representing this mesh
   */
  std::string toString(bool fullDetails = false);

  void addSubMesh(SubMesh* subMesh);

  /**
   * @brief Hidden
   */
  void _unBindEffect() override;

  /**
   * @brief Gets the list of MeshLODLevel associated with the current mesh.
   * @returns an array of MeshLODLevel
   */
  std::vector<MeshLODLevelPtr>& getLODLevels();

  /**
   * @brief Add a mesh as LOD level triggered at the given distance.
   * @see https://doc.babylonjs.com/how_to/how_to_use_lod
   * @param distance The distance from the center of the object to show this
   * level
   * @param mesh The mesh to be added as LOD level (can be null)
   * @return This mesh (for chaining)
   */
  Mesh& addLODLevel(float distance, const MeshPtr& mesh);

  /**
   * @brief Returns the LOD level mesh at the passed distance or null if not
   * found.
   * @see https://doc.babylonjs.com/how_to/how_to_use_lod
   * @param distance The distance from the center of the object to show this
   * level
   * @returns a Mesh or `null`
   */
  MeshPtr getLODLevelAtDistance(float distance);

  /**
   * @brief Remove a mesh from the LOD array.
   * @see https://doc.babylonjs.com/how_to/how_to_use_lod
   * @param mesh defines the mesh to be removed
   * @return This mesh (for chaining)
   */
  Mesh& removeLODLevel(const MeshPtr& mesh);

  /**
   * @brief Returns the registered LOD mesh distant from the parameter `camera`
   * position if any, else returns the current mesh.
   * @see https://doc.babylonjs.com/how_to/how_to_use_lod
   * @param camera defines the camera to use to compute distance
   * @param boundingSphere defines a custom bounding sphere to use instead of
   * the one from this mesh
   * @return This mesh (for chaining)
   */
  AbstractMesh* getLOD(const CameraPtr& camera, BoundingSphere* boundingSphere = nullptr) override;

  void setGeometry(Geometry* geometry);

  /**
   * @brief Returns the total number of vertices within the mesh geometry or
   * zero if the mesh has no geometry.
   * @returns the total number of vertices
   */
  size_t getTotalVertices() const override;

  /**
   * @brief Returns the content of an associated vertex buffer.
   * @param kind defines which buffer to read from (positions, indices, normals,
   * etc). Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   * @param copyWhenShared defines a boolean indicating that if the mesh
   * geometry is shared among some other meshes, the returned array is a copy of
   * the internal one
   * @param forceCopy defines a boolean forcing the copy of the buffer no matter
   * what the value of copyWhenShared is
   * @returns a FloatArray or null if the mesh has no geometry or no vertex
   * buffer for this kind.
   */
  Float32Array getVerticesData(const std::string& kind, bool copyWhenShared = false,
                               bool forceCopy = false) override;

  /**
   * @brief Returns the mesh VertexBuffer object from the requested `kind`.
   * @param kind defines which buffer to read from (positions, indices, normals,
   * etc). Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.NormalKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   * @returns a FloatArray or null if the mesh has no vertex buffer for this
   * kind.
   */
  VertexBufferPtr getVertexBuffer(const std::string& kind) const;

  /**
   * @brief Tests if a specific vertex buffer is associated with this mesh.
   * @param kind defines which buffer to check (positions, indices, normals,
   * etc). Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.NormalKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   * @returns a boolean
   */
  bool isVerticesDataPresent(const std::string& kind) const override;

  /**
   * @brief Returns a boolean defining if the vertex data for the requested
   * `kind` is updatable.
   * @param kind defines which buffer to check (positions, indices, normals,
   * etc). Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   * @returns a boolean
   */
  bool isVertexBufferUpdatable(const std::string& kind) const;

  /**
   * @brief Returns a string which contains the list of existing `kinds` of
   * Vertex Data associated with this mesh.
   * @param kind defines which buffer to read from (positions, indices, normals,
   * etc). Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.NormalKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   * @returns an array of strings
   */
  std::vector<std::string> getVerticesDataKinds() const;

  /**
   * @brief Returns a positive integer : the total number of indices in this
   * mesh geometry.
   * @returns the numner of indices or zero if the mesh has no geometry.
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
  IndicesArray getIndices(bool copyWhenShared = false, bool forceCopy = false) override;

  /**
   * @brief Determine if the current mesh is ready to be rendered
   * @param completeCheck defines if a complete check (including materials and
   * lights) has to be done (false by default)
   * @param forceInstanceSupport will check if the mesh will be ready when used
   * with instances (false by default)
   * @returns true if all associated assets are ready (material, textures,
   * shaders)
   */
  bool isReady(bool completeCheck = false, bool forceInstanceSupport = false) override;

  /**
   * @brief This function affects parametric shapes on vertex position update
   * only : ribbons, tubes, etc. It has no effect at all on other shapes. It
   * prevents the mesh normals from being recomputed on next `positions` array
   * update.
   * @returns the current mesh
   */
  Mesh& freezeNormals();

  /**
   * @brief This function affects parametric shapes on vertex position update
   * only : ribbons, tubes, etc. It has no effect at all on other shapes. It
   * reactivates the mesh normals computation if it was previously frozen.
   * @returns the current mesh
   */
  Mesh& unfreezeNormals();

  /** Methods **/

  /**
   * @brief Hidden
   */
  void _preActivate() override;

  /**
   * @brief Hidden
   */
  Mesh* _preActivateForIntermediateRendering(int renderId) override;

  /**
   * @brief Hidden
   */
  Mesh& _registerInstanceForRenderId(InstancedMesh* instance, int renderId);

  /**
   * @brief Hidden
   */
  virtual void _postActivate() override;

  /**
   * @brief This method recomputes and sets a new BoundingInfo to the mesh
   * unless it is locked. This means the mesh underlying bounding box and sphere
   * are recomputed.
   * @param applySkeleton defines whether to apply the skeleton before computing
   * the bounding info
   * @returns the current mesh
   */
  Mesh& refreshBoundingInfo(bool applySkeleton = false);

  /**
   * @brief Hidden
   */
  SubMeshPtr _createGlobalSubMesh(bool force);

  /**
   * @brief This function will subdivide the mesh into multiple submeshes.
   * @param count defines the expected number of submeshes
   */
  void subdivide(size_t count);

  /**
   * @brief Copy a FloatArray into a specific associated vertex buffer.
   * @param kind defines which buffer to write to (positions, indices, normals,
   * etc). Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   * @param data defines the data source
   * @param updatable defines if the updated vertex buffer must be flagged as
   * updatable
   * @param stride defines the data stride size (can be null)
   * @returns the current mesh
   */
  AbstractMesh* setVerticesData(const std::string& kind, const Float32Array& data,
                                bool updatable                      = false,
                                const std::optional<size_t>& stride = std::nullopt) override;

  /**
   * @brief Delete a vertex buffer associated with this mesh.
   * @param kind defines which buffer to delete (positions, indices, normals, etc). Possible `kind`
   * values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   */
  void removeVerticesData(const std::string& kind);

  /**
   * @brief Flags an associated vertex buffer as updatable.
   * @param kind defines which buffer to use (positions, indices, normals, etc).
   * Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   * @param updatable defines if the updated vertex buffer must be flagged as
   * updatable
   */
  void markVerticesDataAsUpdatable(const std::string& kind, bool updatable = true);

  /**
   * @brief Sets the mesh global Vertex Buffer.
   * @param buffer defines the buffer to use
   * @returns the current mesh
   */
  Mesh& setVerticesBuffer(const VertexBufferPtr& buffer);

  /**
   * @brief Update a specific associated vertex buffer.
   * @param kind defines which buffer to write to (positions, indices, normals,
   * etc). Possible `kind` values :
   * - VertexBuffer.PositionKind
   * - VertexBuffer.UVKind
   * - VertexBuffer.UV2Kind
   * - VertexBuffer.UV3Kind
   * - VertexBuffer.UV4Kind
   * - VertexBuffer.UV5Kind
   * - VertexBuffer.UV6Kind
   * - VertexBuffer.ColorKind
   * - VertexBuffer.MatricesIndicesKind
   * - VertexBuffer.MatricesIndicesExtraKind
   * - VertexBuffer.MatricesWeightsKind
   * - VertexBuffer.MatricesWeightsExtraKind
   * @param data defines the data source
   * @param updateExtends defines if extends info of the mesh must be updated
   * (can be null). This is mostly useful for "position" kind
   * @param makeItUnique defines if the geometry associated with the mesh must
   * be cloned to make the change only for this mesh (and not all meshes
   * associated with the same geometry)
   * @returns the current mesh
   */
  AbstractMesh* updateVerticesData(const std::string& kind, const Float32Array& data,
                                   bool updateExtends = false, bool makeItUnique = false) override;

  /**
   * @brief This method updates the vertex positions of an updatable mesh
   * according to the `positionFunction` returned values.
   * @see
   * https://doc.babylonjs.com/how_to/how_to_dynamically_morph_a_mesh#other-shapes-updatemeshpositions
   * @param positionFunction is a simple JS function what is passed the mesh
   * `positions` array. It doesn't need to return anything
   * @param computeNormals is a boolean (default true) to enable/disable the
   * mesh normal recomputation after the vertex position update
   * @returns the current mesh
   */
  Mesh& updateMeshPositions(std::function<void(Float32Array& positions)> positionFunction,
                            bool computeNormals = true);

  /**
   * @brief Creates a un-shared specific occurence of the geometry for the mesh.
   * @returns the current mesh
   */
  Mesh& makeGeometryUnique();

  /**
   * @brief Set the index buffer of this mesh.
   * @param indices defines the source data
   * @param totalVertices defines the total number of vertices referenced by
   * this index data (can be null)
   * @param updatable defines if the updated index buffer must be flagged as
   * updatable (default is false)
   * @returns the current mesh
   */
  AbstractMesh* setIndices(const IndicesArray& indices, size_t totalVertices = 0,
                           bool updatable = false) override;

  /**
   * @brief Update the current index buffer.
   * @param indices defines the source data
   * @param offset defines the offset in the index buffer where to store the new
   * data (can be null)
   * @param gpuMemoryOnly defines a boolean indicating that only the GPU memory
   * must be updated leaving the CPU version of the indices unchanged (false by
   * default)
   * @returns the current mesh
   */
  AbstractMesh& updateIndices(const IndicesArray& indices,
                              const std::optional<int>& offset = std::nullopt,
                              bool gpuMemoryOnly               = false) override;

  /**
   * @brief Invert the geometry to move from a right handed system to a left
   * handed one.
   * @returns the current mesh
   */
  Mesh& toLeftHanded();

  /**
   * @brief Hidden
   */
  virtual void _bind(SubMesh* subMesh, const EffectPtr& effect, unsigned int fillMode);

  /**
   * @brief Hidden
   */
  virtual void _draw(SubMesh* subMesh, int fillMode, size_t instancesCount = 0,
                     bool alternate = false);

  /**
   * @brief Registers for this mesh a javascript function called just before the
   * rendering process.
   * @param func defines the function to call before rendering this mesh
   * @returns the current mesh
   */
  Mesh& registerBeforeRender(const std::function<void(Mesh* mesh, EventState& es)>& func);

  /**
   * @brief Disposes a previously registered javascript function called before
   * the rendering.
   * @param func defines the function to remove
   * @returns the current mesh
   */
  Mesh& unregisterBeforeRender(const std::function<void(Mesh* mesh, EventState&)>& func);

  /**
   * @brief Registers for this mesh a javascript function called just after the
   * rendering is complete
   * @param func defines the function to call after rendering this mesh
   * @returns the current mesh
   */
  Mesh& registerAfterRender(const std::function<void(Mesh* mesh, EventState&)>& func);

  /**
   * @brief Disposes a previously registered javascript function called after
   * the rendering.
   * @param func defines the function to remove
   * @returns the current mesh
   */
  Mesh& unregisterAfterRender(const std::function<void(Mesh* mesh, EventState&)>& func);

  /**
   * @brief Hidden
   */
  _InstancesBatchPtr _getInstancesRenderList(size_t subMeshId, bool isReplacementMode = false);

  /**
   * @brief Hidden
   */
  Mesh& _renderWithInstances(SubMesh* subMesh, unsigned int fillMode,
                             const _InstancesBatchPtr& batch, const EffectPtr& effect,
                             Engine* engine);

  /** Thin instances **/

  /**
   * @brief Creates a new thin instance.
   * @param matrix the matrix of the thin instance(s) to create
   * @param refresh true to refresh the underlying gpu buffer (default: true). If you do multiple
   * calls to this method in a row, set refresh to true only for the last call to save performance
   * @returns the thin instance index number. If you pass an array of matrices, other instance
   * indexes are index+1, index+2, etc
   */
  size_t thinInstanceAdd(Matrix& matrix, bool refresh = true);

  /**
   * @brief Creates a new thin instance.
   * @param matrix array of matrices (position, rotation, scale) of the thin instance(s) to create
   * @param refresh true to refresh the underlying gpu buffer (default: true). If you do multiple
   * calls to this method in a row, set refresh to true only for the last call to save performance
   * @returns the thin instance index number. If you pass an array of matrices, other instance
   * indexes are index+1, index+2, etc
   */
  size_t thinInstanceAdd(std::vector<Matrix>& matrix, bool refresh = true);

  /**
   * @brief Adds the transformation (matrix) of the current mesh as a thin instance.
   * @param refresh true to refresh the underlying gpu buffer (default: true). If you do multiple
   * calls to this method in a row, set refresh to true only for the last call to save performance
   * @returns the thin instance index number
   */
  size_t thinInstanceAddSelf(bool refresh);

  /**
   * @brief Registers a custom attribute to be used with thin instances.
   * @param kind name of the attribute
   * @param stride size in floats of the attribute
   */
  void thinInstanceRegisterAttribute(const std::string& kind, unsigned int stride);

  /**
   * @brief Sets the matrix of a thin instance.
   * @param index index of the thin instance
   * @param matrix matrix to set
   * @param refresh true to refresh the underlying gpu buffer (default: true). If you do multiple
   * calls to this method in a row, set refresh to true only for the last call to save performance
   */
  bool thinInstanceSetMatrixAt(size_t index, Matrix& matrix, bool refresh = true);

  /**
   * @brief Sets the value of a custom attribute for a thin instance.
   * @param kind name of the attribute
   * @param index index of the thin instance
   * @param value value to set
   * @param refresh true to refresh the underlying gpu buffer (default: true). If you do multiple
   * calls to this method in a row, set refresh to true only for the last call to save performance
   */
  bool thinInstanceSetAttributeAt(const std::string& kind, size_t index, const Float32Array& value,
                                  bool refresh = true);

  /**
   * @brief Sets a buffer to be used with thin instances. This method is a faster way to setup
   * multiple instances than calling thinInstanceAdd repeatedly.
   * @param kind name of the attribute. Use "matrix" to setup the buffer of matrices
   * @param buffer buffer to set
   * @param stride size in floats of each value of the buffer
   * @param staticBuffer indicates that the buffer is static, so that you won't change it after it
   * is set (better performances - false by default)
   */
  void thinInstanceSetBuffer(const std::string& kind, const Float32Array& buffer,
                             unsigned int stride = 0, bool staticBuffer = false);

  /**
   * @brief Gets the list of world matrices.
   * @return an array containing all the world matrices from the thin instances
   */
  std::vector<Matrix> thinInstanceGetWorldMatrices();

  /**
   * @brief Synchronize the gpu buffers with a thin instance buffer. Call this method if you update
   * later on the buffers passed to thinInstanceSetBuffer.
   * @param kind name of the attribute to update. Use "matrix" to update the buffer of matrices
   */
  void thinInstanceBufferUpdated(const std::string& kind);

  /**
   * @brief Refreshes the bounding info, taking into account all the thin instances defined
   * @param forceRefreshParentInfo true to force recomputing the mesh bounding info and use it to
   * compute the aggregated bounding info.
   */
  void thinInstanceRefreshBoundingInfo(bool forceRefreshParentInfo);

  /**
   * @brief Hidden
   */
  void _thinInstanceInitializeUserStorage();

  /**
   * @brief Hidden
   */
  void _thinInstanceUpdateBufferSize(const std::string& kind, size_t numInstances);

  /**
   * @brief Hidden
   */
  void _renderWithThinInstances(SubMesh* subMesh, unsigned int fillMode, const EffectPtr& effect,
                                Engine* engine);

  /**
   * @brief Register a custom buffer that will be instanced.
   * @see https://doc.babylonjs.com/how_to/how_to_use_instances#custom-buffers
   * @param kind defines the buffer kind
   * @param stride defines the stride in floats
   */
  void registerInstancedBuffer(const std::string& kind, size_t stride);

  /**
   * @brief Hidden
   */
  void _processInstancedBuffers(const std::vector<InstancedMesh*>& visibleInstances,
                                bool renderSelf);

  /**
   * @brief Hidden
   */
  Mesh& _processRendering(
    AbstractMesh* renderingMesh, SubMesh* subMesh, const EffectPtr& effect, int fillMode,
    const _InstancesBatchPtr& batch, bool hardwareInstancedRendering,
    std::function<void(bool isInstance, const Matrix& world, Material* effectiveMaterial)>
      onBeforeDraw,
    Material* effectiveMaterial = nullptr);

  /**
   * @brief Hidden
   */
  void _rebuild() override;

  /**
   * @brief Hidden
   */
  void _freeze() override;

  /**
   * @brief Hidden
   */
  void _unFreeze() override;

  /**
   * @brief Triggers the draw call for the mesh. Usually, you don't need to call this method by your
   * own because the mesh rendering is handled by the scene rendering manager
   * @param subMesh defines the subMesh to render
   * @param enableAlphaMode defines if alpha mode can be changed
   * @param effectiveMeshReplacement defines an optional mesh used to provide info for the rendering
   * @returns the current mesh
   */
  Mesh& render(SubMesh* subMesh, bool enableAlphaMode,
               const AbstractMeshPtr& effectiveMeshReplacement = nullptr);

  /**
   * @brief Returns an array populated with IParticleSystem objects whose the
   * mesh is the emitter.
   * @returns an array of IParticleSystem
   */
  std::vector<IParticleSystemPtr> getEmittedParticleSystems();

  /**
   * @brief Returns an array populated with IParticleSystem objects whose the
   * mesh or its children are the emitter.
   * @returns an array of IParticleSystem
   */
  std::vector<IParticleSystemPtr> getHierarchyEmittedParticleSystems();

  /**
   * @brief Renormalize the mesh and patch it up if there are no weights
   * Similar to normalization by adding the weights compute the reciprocal and
   * multiply all elements, this wil ensure that everything adds to 1. However
   * in the case of zero weights then we set just a single influence to 1. We
   * check in the function for extra's present and if so we use the
   * normalizeSkinWeightsWithExtras rather than the FourWeights version.
   */
  void cleanMatrixWeights();

  /**
   * @brief Hidden
   */
  std::vector<NodePtr> getChildren();

  /**
   * @brief ValidateSkinning is used to determine that a mesh has valid skinning
   * data along with skin metrics, if missing weights, or not normalized it is
   * returned as invalid mesh the string can be used for console logs, or on
   * screen messages to let the user know there was an issue with importing the
   * mesh.
   * @returns a validation object with skinned, valid and report string
   */
  SkinningValidationResult validateSkinning();

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
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes, unsigned int strategy = 0) override;

  /**
   * @brief Sets the mesh material by the material or multiMaterial `id`
   * property.
   * @param id is a string identifying the material or the multiMaterial
   * @returns the current mesh
   */
  Mesh& setMaterialByID(const std::string& id);

  /**
   * @brief Returns as a new array populated with the mesh material and/or
   * skeleton, if any.
   * @returns an array of IAnimatable
   */
  std::vector<IAnimatablePtr> getAnimatables() override;

  /**
   * @brief Modifies the mesh geometry according to the passed transformation
   * matrix. This method returns nothing but it really modifies the mesh even if
   * it's originally not set as updatable. The mesh normals are modified using
   * the same transformation. Note that, under the hood, this method sets a new
   * VertexBuffer each call.
   * @param transform defines the transform matrix to use
   * @see https://doc.babylonjs.com/resources/baking_transformations
   * @returns the current mesh
   */
  Mesh& bakeTransformIntoVertices(const Matrix& transform);

  /**
   * @brief Modifies the mesh geometry according to its own current World Matrix.
   * The mesh World Matrix is then reset.
   * This method returns nothing but really modifies the mesh even if it's originally not set as
   * updatable. Note that, under the hood, this method sets a new VertexBuffer each call.
   * @see https://doc.babylonjs.com/resources/baking_transformations
   * @param bakeIndependenlyOfChildren indicates whether to preserve all child nodes' World Matrix
   * during baking
   * @returns the current mesh
   */
  Mesh& bakeCurrentTransformIntoVertices(bool bakeIndependenlyOfChildren = true);

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
   * properties. This method must not get confused with createInstance()
   * @param name is a string, the name given to the new mesh
   * @param newParent can be any Node object (default `null`)
   * @param doNotCloneChildren allows/denies the recursive cloning of the
   * original mesh children if any (default `false`)
   * @param clonePhysicsImpostor allows/denies the cloning in the same time of
   * the original mesh `body` used by the physics engine, if any (default
   * `true`)
   * @returns a new mesh
   */
  MeshPtr clone(const std::string& name = "", Node* newParent = nullptr,
                bool doNotCloneChildren = false, bool clonePhysicsImpostor = true);

  /** Dispose **/

  /**
   * @brief Releases resources associated with this mesh.
   * @param doNotRecurse Set to true to not recurse into each children (recurse into each children
   * by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced materials and textures
   * (false by default)
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Hidden
   */
  void _disposeInstanceSpecificData();

  /**
   * @brief Hidden
   */
  void _disposeThinInstanceSpecificData();

  /** Geometric tools **/

  /**
   * @brief Modifies the mesh geometry according to a displacement map.
   * A displacement map is a colored image. Each pixel color value (actually a
   * gradient computed from red, green, blue values) will give the displacement
   * to apply to each mesh vertex. The mesh must be set as updatable. Its
   * internal geometry is directly modified, no new buffer are allocated.
   * @param url is a string, the URL from the image file is to be downloaded.
   * @param minHeight is the lower limit of the displacement.
   * @param maxHeight is the upper limit of the displacement.
   * @param onSuccess is an optional Javascript function to be called just after
   * the mesh is modified. It is passed the modified mesh and must return
   * nothing.
   * @param uvOffset is an optional vector2 used to offset UV.
   * @param uvScale is an optional vector2 used to scale UV.
   * @param forceUpdate defines whether or not to force an update of the
   * generated buffers. This is useful to apply on a deserialized model for
   * instance.
   * @returns the Mesh.
   */
  Mesh& applyDisplacementMap(const std::string& url, float minHeight, float maxHeight,
                             std::function<void(Mesh* mesh)> onSuccess = nullptr,
                             const std::optional<Vector2>& uvOffset    = std::nullopt,
                             const std::optional<Vector2>& uvScale     = std::nullopt,
                             bool boolforceUpdate                      = false);

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
   * generated buffers. This is useful to apply on a deserialized model for
   * instance.
   * @returns the Mesh.
   */
  void applyDisplacementMapFromBuffer(const Uint8Array& buffer, unsigned int heightMapWidth,
                                      unsigned int heightMapHeight, float minHeight,
                                      float maxHeight,
                                      const std::optional<Vector2>& uvOffset = std::nullopt,
                                      const std::optional<Vector2>& uvScale  = std::nullopt,
                                      bool forceUpdate                       = false);

  /**
   * @brief Modify the mesh to get a flat shading rendering.
   * This means each mesh facet will then have its own normals. Usually new
   * vertices are added in the mesh geometry to get this result. Warning : the
   * mesh is really modified even if not set originally as updatable and, under
   * the hood, a new VertexBuffer is allocated.
   * @returns current mesh
   */
  Mesh& convertToFlatShadedMesh();

  /**
   * @brief This method removes all the mesh indices and add new vertices
   * (duplication) in order to unfold facets into buffers. In other words, more
   * vertices, no more indices and a single bigger VBO. The mesh is really
   * modified even if not set originally as updatable. Under the hood, a new
   * VertexBuffer is allocated.
   * @returns current mesh
   */
  Mesh& convertToUnIndexedMesh();

  /**
   * @brief Inverses facet orientations.
   * Warning : the mesh is really modified even if not set originally as
   * updatable. A new VertexBuffer is created under the hood each call.
   * @param flipNormals will also inverts the normals
   * @returns current mesh
   */
  Mesh& flipFaces(bool flipNormals = false);

  /**
   * @brief Increase the number of facets and hence vertices in a mesh.
   * Vertex normals are interpolated from existing vertex normals.
   * Warning : the mesh is really modified even if not set originally as
   * updatable. A new VertexBuffer is created under the hood each call.
   * @param numberPerEdge the number of new vertices to add to each edge of a
   * facet, optional default 1
   */
  void increaseVertices(size_t numberPerEdge);

  /**
   * @brief Force adjacent facets to share vertices and remove any facets that
   * have all vertices in a line This will undo any application of
   * covertToFlatShadedMesh Warning : the mesh is really modified even if not
   * set originally as updatable. A new VertexBuffer is created under the hood
   * each call.
   */
  void forceSharedVertices();

  /** Instances **/

  /**
   * @brief Creates a new InstancedMesh object from the mesh model.
   * Warning : this method is not supported for Line mesh and LineSystem
   * @see https://doc.babylonjs.com/how_to/how_to_use_instances
   * @param name defines the name of the new instance
   * @returns a new InstancedMesh
   */
  InstancedMeshPtr createInstance(const std::string& name);

  /**
   * @brief Synchronises all the mesh instance submeshes to the current mesh
   * submeshes, if any. After this call, all the mesh instances have the same
   * submeshes than the current mesh.
   * @returns the current mesh
   */
  Mesh& synchronizeInstances();

  /**
   * @brief Optimization of the mesh's indices, in case a mesh has duplicated
   * vertices. The function will only reorder the indices and will not remove
   * unused vertices to avoid problems with submeshes. This should be used
   * together with the simplification to avoid disappearing triangles.
   * @param successCallback an optional success callback to be called after the
   * optimization finished.
   * @returns the current mesh
   */
  void optimizeIndices(const std::function<void(Mesh* mesh)>& successCallback);

  /**
   * @brief This function will remove some indices and vertices from a mesh. It
   * removes facets where two of its vertices share the same position and forces
   * vertices to share normals. So it will, therefore, also change a flat shaded
   * mesh to a smooth (for the want of a better word) shaded mesh.
   *
   * Sometimes the additional facets and vertices are necessary, for example to
   * prevent seams when using textures. So use carefully.
   *
   * More information on the need for extra facets, which turn out to be lines,
   * can be found in https://babylonjsguide.github.io/advanced/Facets.html
   * @see https://babylonjsguide.github.io/snippets/Minimise_Vertices
   */
  void minimizeVertices();

  /**
   * @brief Serialize current mesh.
   * @param serializationObject defines the object which will receive the
   * serialization data
   */
  virtual void serialize(json& serializationObject) const;

  /**
   * @brief Hidden
   */
  void _syncGeometryWithMorphTargetManager();

  /**
   * @brief Creates points inside a mesh. This utility enables you to create and
   * store Vector3 points each of which is randomly positioned inside a given
   * mesh.
   * @param pointsNb defines how many inside points to generate
   * @return a list of inner mesh points
   * @see https://doc.babylonjs.com/snippets/innermeshpoints
   */
  std::vector<Vector3> createInnerPoints(size_t pointsNb);

  /**
   * @brief Check whether or not a point is inside this mesh
   * @param point the point to check
   * @return true if point is inside mesh, false otherwise.
   * @see https://doc.babylonjs.com/snippets/isinside
   */
  bool pointIsInside(Vector3 point);

  /** Statics **/

  /**
   * @brief Returns a new Mesh object parsed from the source provided.
   * @param parsedMesh is the source
   * @param scene defines the hosting scene
   * @param rootUrl is the root URL to prefix the `delayLoadingFile` property
   * with
   * @returns a new Mesh
   */
  static MeshPtr Parse(const json& parsedMesh, Scene* scene, const std::string& rootUrl);

  /**
   * @brief Creates a ribbon mesh. Please consider using the same method from
   * the MeshBuilder class instead.
   * @see https://doc.babylonjs.com/how_to/parametric_shapes
   * @param name defines the name of the mesh to create
   * @param pathArray is a required array of paths, what are each an array of
   * successive Vector3. The pathArray parameter depicts the ribbon geometry.
   * @param closeArray creates a seam between the first and the last paths of
   * the path array (default is false)
   * @param closePath creates a seam between the first and the last points of
   * each path of the path array
   * @param offset is taken in account only if the `pathArray` is containing a
   * single path
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @param instance defines an instance of an existing Ribbon object to be
   * updated with the passed `pathArray` parameter
   * (https://doc.babylonjs.com/how_to/How_to_dynamically_morph_a_mesh#ribbon)
   * @returns a new Mesh
   */
  static MeshPtr CreateRibbon(const std::string& name,
                              const std::vector<std::vector<Vector3>>& pathArray,
                              bool closeArray = false, bool closePath = false, int offset = -1,
                              Scene* = nullptr, const std::optional<bool>& updatable = std::nullopt,
                              const std::optional<unsigned int>& sideOrientation = std::nullopt,
                              const MeshPtr& ribbonInstance                      = nullptr);

  /**
   * @brief Creates a plane polygonal mesh.  By default, this is a disc. Please
   * consider using the same method from the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param radius sets the radius size (float) of the polygon (default 0.5)
   * @param tessellation sets the number of polygon sides (positive integer,
   * default 64). So a tessellation valued to 3 will build a triangle, to 4 a
   * square, etc
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreateDisc(const std::string& name, float radius = 0.5f,
                            unsigned int tessellation = 64, Scene* = nullptr,
                            const std::optional<bool>& updatable               = std::nullopt,
                            const std::optional<unsigned int>& sideOrientation = std::nullopt);

  /**
   * @brief Creates a box mesh. Please consider using the same method from the
   * MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param size sets the size (float) of each box side (default 1)
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreateBox(const std::string& name, float size = 1.f, Scene* = nullptr,
                           const std::optional<bool>& updatable               = std::nullopt,
                           const std::optional<unsigned int>& sideOrientation = std::nullopt);

  /**
   * @brief Creates a sphere mesh. Please consider using the same method from
   * the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param segments sets the sphere number of horizontal stripes (positive
   * integer, default 32)
   * @param diameter sets the diameter size (float) of the sphere (default 1)
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreateSphere(const std::string& name, unsigned int segments = 32,
                              float diameter = 1.f, Scene* = nullptr,
                              const std::optional<bool>& updatable               = std::nullopt,
                              const std::optional<unsigned int>& sideOrientation = std::nullopt);

  /**
   * @brief Creates a hemisphere mesh. Please consider using the same method
   * from the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param segments sets the sphere number of horizontal stripes (positive
   * integer, default 32)
   * @param diameter sets the diameter size (float) of the sphere (default 1)
   * @param scene defines the hosting scene
   * @returns a new Mesh
   */
  static MeshPtr CreateHemisphere(const std::string& name, unsigned int segments = 32,
                                  float diameter = 1.f, Scene* = nullptr);

  /**
   * @brief Creates a cylinder or a cone mesh. Please consider using the same
   * method from the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param height sets the height size (float) of the cylinder/cone (float,
   * default 2)
   * @param diameterTop set the top cap diameter (floats, default 1)
   * @param diameterBottom set the bottom cap diameter (floats, default 1). This
   * value can't be zero
   * @param tessellation sets the number of cylinder sides (positive integer,
   * default 24). Set it to 3 to get a prism for instance
   * @param subdivisions sets the number of rings along the cylinder height
   * (positive integer, default 1)
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreateCylinder(const std::string& name, float height = 2.f,
                                float diameterTop = 1.f, float diameterBottom = 1.f,
                                unsigned int tessellation = 24, unsigned int subdivisions = 1,
                                Scene*                                             = nullptr,
                                const std::optional<bool>& updatable               = std::nullopt,
                                const std::optional<unsigned int>& sideOrientation = std::nullopt);

  // Torus  (Code from SharpDX.org)
  /**
   * @brief Creates a torus mesh. Please consider using the same method from the
   * MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param diameter sets the diameter size (float) of the torus (default 1)
   * @param thickness sets the diameter size of the tube of the torus (float,
   * default 0.5)
   * @param tessellation sets the number of torus sides (postive integer,
   * default 16)
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreateTorus(const std::string& name, float diameter = 1.f, float thickness = 0.5f,
                             unsigned int tessellation = 16, Scene* = nullptr,
                             const std::optional<bool>& updatable               = std::nullopt,
                             const std::optional<unsigned int>& sideOrientation = std::nullopt);

  /**
   * @brief Creates a torus knot mesh. Please consider using the same method
   * from the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param radius sets the global radius size (float) of the torus knot
   * (default 2)
   * @param tube sets the diameter size of the tube of the torus (float, default
   * 0.5)
   * @param radialSegments sets the number of sides on each tube segments
   * (positive integer, default 32)
   * @param tubularSegments sets the number of tubes to decompose the knot into
   * (positive integer, default 32)
   * @param p the number of windings on X axis (positive integers, default 2)
   * @param q the number of windings on Y axis (positive integers, default 3)
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreateTorusKnot(const std::string& name, float radius = 2.f, float tube = 0.5f,
                                 unsigned int radialSegments  = 32,
                                 unsigned int tubularSegments = 32, float p = 2.f, float q = 3.f,
                                 Scene*                                             = nullptr,
                                 const std::optional<bool>& updatable               = std::nullopt,
                                 const std::optional<unsigned int>& sideOrientation = std::nullopt);

  /**
   * @brief Creates a line mesh. Please consider using the same method from the
   * MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param points is an array successive Vector3
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param instance is an instance of an existing LineMesh object to be updated
   * with the passed `points` parameter
   * (https://doc.babylonjs.com/how_to/How_to_dynamically_morph_a_mesh#lines-and-dashedlines).
   * @returns a new Mesh
   */
  static LinesMeshPtr CreateLines(const std::string& name, const std::vector<Vector3>& points,
                                  Scene*, bool updatable = false,
                                  const LinesMeshPtr& linesInstance = nullptr);

  /**
   * @brief Creates a dashed line mesh. Please consider using the same method
   * from the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param points is an array successive Vector3
   * @param dashSize is the size of the dashes relatively the dash number
   * (positive float, default 3)
   * @param gapSize is the size of the gap between two successive dashes
   * relatively the dash number (positive float, default 1)
   * @param dashNb is the intended total number of dashes (positive integer,
   * default 200)
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param instance is an instance of an existing LineMesh object to be updated
   * with the passed `points` parameter
   * (https://doc.babylonjs.com/how_to/How_to_dynamically_morph_a_mesh#lines-and-dashedlines)
   * @returns a new Mesh
   */
  static LinesMeshPtr CreateDashedLines(const std::string& name, std::vector<Vector3>& points,
                                        float dashSize = 3.f, float gapSize = 1.f,
                                        unsigned int dashNb = 200, Scene* = nullptr,
                                        bool updatable               = false,
                                        const LinesMeshPtr& instance = nullptr);

  /**
   * @brief Creates a polygon mesh.Please consider using the same method from
   * the MeshBuilder class instead.
   * @see https://doc.babylonjs.com/how_to/parametric_shapes#non-regular-polygon
   * @param name defines the name of the mesh to create
   * @param shape is a required array of successive Vector3 representing the
   * corners of the polygon in th XoZ plane, that is y = 0 for all vectors
   * @param scene defines the hosting scene
   * @param holes is a required array of arrays of successive Vector3 used to
   * defines holes in the polygon
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreatePolygon(const std::string& name, const std::vector<Vector3>& shape,
                               Scene* scene, const std::vector<std::vector<Vector3>>& holes,
                               bool updatable               = false,
                               unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates an extruded polygon mesh, with depth in the Y direction.
   * Please consider using the same method from the MeshBuilder class instead.
   * @see
   * https://doc.babylonjs.com/how_to/parametric_shapes#extruded-non-regular-polygon
   * @param name defines the name of the mesh to create
   * @param shape is a required array of successive Vector3 representing the
   * corners of the polygon in th XoZ plane, that is y = 0 for all vectors
   * @param depth defines the height of extrusion
   * @param scene defines the hosting scene
   * @param holes is a required array of arrays of successive Vector3 used to
   * defines holes in the polygon
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr ExtrudePolygon(const std::string& name, const std::vector<Vector3>& shape,
                                float depth, Scene* scene,
                                const std::vector<std::vector<Vector3>>& holes,
                                bool updatable               = false,
                                unsigned int sideOrientation = Mesh::DEFAULTSIDE);

  /**
   * @brief Creates an extruded shape mesh.
   * The extrusion is a parametric shape. It has no predefined shape. Its final
   * shape will depend on the input parameters. Please consider using the same
   * method from the MeshBuilder class instead.
   * @see https://doc.babylonjs.com/how_to/parametric_shapes
   * @see https://doc.babylonjs.com/how_to/parametric_shapes#extruded-shapes
   * @param name defines the name of the mesh to create
   * @param shape is a required array of successive Vector3. This array depicts
   * the shape to be extruded in its local space : the shape must be designed in
   * the xOy plane and will be extruded along the Z axis
   * @param path is a required array of successive Vector3. This is the axis
   * curve the shape is extruded along
   * @param scale is the value to scale the shape
   * @param rotation is the angle value to rotate the shape each step (each path
   * point), from the former step (so rotation added each step) along the curve
   * @param cap sets the way the extruded shape is capped. Possible values :
   * BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START,
   * BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @param instance is an instance of an existing ExtrudedShape object to be
   * updated with the passed `shape`, `path`, `scale` or `rotation` parameters
   * (https://doc.babylonjs.com/how_to/How_to_dynamically_morph_a_mesh#extruded-shape)
   * @returns a new Mesh
   */
  static MeshPtr ExtrudeShape(const std::string& name, const std::vector<Vector3>& shape,
                              const std::vector<Vector3>& path, float scale, float rotation,
                              unsigned int cap, Scene*,
                              const std::optional<bool>& updatable               = std::nullopt,
                              const std::optional<unsigned int>& sideOrientation = std::nullopt,
                              const MeshPtr& instance                            = nullptr);

  /**
   * @brief Creates an custom extruded shape mesh.
   * The custom extrusion is a parametric shape.
   * It has no predefined shape. Its final shape will depend on the input
   * parameters. Please consider using the same method from the MeshBuilder
   * class instead.
   * @see https://doc.babylonjs.com/how_to/parametric_shapes#extruded-shapes
   * @param name defines the name of the mesh to create
   * @param shape is a required array of successive Vector3. This array depicts
   * the shape to be extruded in its local space : the shape must be designed in
   * the xOy plane and will be extruded along the Z axis
   * @param path is a required array of successive Vector3. This is the axis
   * curve the shape is extruded along
   * @param scaleFunction is a custom Javascript function called on each path
   * point
   * @param rotationFunction is a custom Javascript function called on each path
   * point
   * @param ribbonCloseArray forces the extrusion underlying ribbon to close all
   * the paths in its `pathArray`
   * @param ribbonClosePath forces the extrusion underlying ribbon to close its
   * `pathArray`
   * @param cap sets the way the extruded shape is capped. Possible values :
   * BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START,
   * BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @param instance is an instance of an existing ExtrudedShape object to be
   * updated with the passed `shape`, `path`, `scale` or `rotation` parameters
   * (https://doc.babylonjs.com/how_to/how_to_dynamically_morph_a_mesh#extruded-shape)
   * @returns a new Mesh
   */
  static MeshPtr
  ExtrudeShapeCustom(const std::string& name, const std::vector<Vector3>& shape,
                     const std::vector<Vector3>& path,
                     const std::function<float(float i, float distance)>& scaleFunction,
                     const std::function<float(float i, float distance)>& rotationFunction,
                     bool ribbonCloseArray, bool ribbonClosePath, unsigned int cap, Scene*,
                     const std::optional<bool>& updatable               = std::nullopt,
                     const std::optional<unsigned int>& sideOrientation = std::nullopt,
                     const MeshPtr& instance                            = nullptr);

  /**
   * @brief Creates lathe mesh.
   * The lathe is a shape with a symetry axis : a 2D model shape is rotated
   * around this axis to design the lathe. Please consider using the same method
   * from the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param shape is a required array of successive Vector3. This array depicts
   * the shape to be rotated in its local space : the shape must be designed in
   * the xOy plane and will be rotated around the Y axis. It's usually a 2D
   * shape, so the Vector3 z coordinates are often set to zero
   * @param radius is the radius value of the lathe
   * @param tessellation is the side number of the lathe.
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreateLathe(const std::string& name, const std::vector<Vector3>& shape,
                             float radius, unsigned int tessellation, Scene* = nullptr,
                             const std::optional<bool>& updatable               = std::nullopt,
                             const std::optional<unsigned int>& sideOrientation = std::nullopt);

  /**
   * @brief Creates a plane mesh. Please consider using the same method from the
   * MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param size sets the size (float) of both sides of the plane at once
   * (default 1)
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @returns a new Mesh
   */
  static MeshPtr CreatePlane(const std::string& name, float size, Scene* = nullptr,
                             const std::optional<bool>& updatable               = std::nullopt,
                             const std::optional<unsigned int>& sideOrientation = std::nullopt);

  /**
   * @brief Creates a ground mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param width set the width of the ground
   * @param height set the height of the ground
   * @param subdivisions sets the number of subdivisions per side
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @returns a new Mesh
   */
  static MeshPtr CreateGround(const std::string& name, unsigned int width = 1,
                              unsigned int height = 1, unsigned int subdivisions = 1,
                              Scene*                               = nullptr,
                              const std::optional<bool>& updatable = std::nullopt);

  /**
   * @brief Creates a tiled ground mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * @param name defines the name of the mesh to create
   * @param xmin set the ground minimum X coordinate
   * @param zmin set the ground minimum Y coordinate
   * @param xmax set the ground maximum X coordinate
   * @param zmax set the ground maximum Z coordinate
   * @param subdivisions is an object `{w: positive integer, h: positive
   * integer}` (default `{w: 6, h: 6}`). `w` and `h` are the numbers of
   * subdivisions on the ground width and height. Each subdivision is called a
   * tile
   * @param precision is an object `{w: positive integer, h: positive integer}`
   * (default `{w: 2, h: 2}`). `w` and `h` are the numbers of subdivisions on
   * the ground width and height of each tile
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @returns a new Mesh
   */
  static MeshPtr CreateTiledGround(const std::string& name, float xmin, float zmin, float xmax,
                                   float zmax, const ISize& subdivisions = ISize(1, 1),
                                   const ISize& precision = ISize(1, 1), Scene* = nullptr,
                                   const std::optional<bool>& updatable = std::nullopt);

  /**
   * @brief Creates a ground mesh from a height map.
   * Please consider using the same method from the MeshBuilder class instead.
   * @see https://doc.babylonjs.com/babylon101/height_map
   * @param name defines the name of the mesh to create
   * @param url sets the URL of the height map image resource
   * @param width set the ground width size
   * @param height set the ground height size
   * @param subdivisions sets the number of subdivision per side
   * @param minHeight is the minimum altitude on the ground
   * @param maxHeight is the maximum altitude on the ground
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param onReady  is a callback function that will be called  once the mesh
   * is built (the height map download can last some time)
   * @param alphaFilter will filter any data where the alpha channel is below
   * this value, defaults 0 (all data visible)
   * @returns a new Mesh
   */
  static GroundMeshPtr
  CreateGroundFromHeightMap(const std::string& name, const std::string& url, unsigned int width,
                            unsigned int height, unsigned int subdivisions, unsigned int minHeight,
                            unsigned int maxHeight, Scene*, bool updatable = false,
                            const std::function<void(const GroundMeshPtr& mesh)>& onReady = nullptr,
                            std::optional<float> alphaFilter = std::nullopt);

  /**
   * @brief Creates a tube mesh.
   * The tube is a parametric shape.
   * It has no predefined shape. Its final shape will depend on the input
   * parameters. Please consider using the same method from the MeshBuilder
   * class instead
   * @see https://doc.babylonjs.com/how_to/parametric_shapes
   * @param name defines the name of the mesh to create
   * @param path is a required array of successive Vector3. It is the curve used
   * as the axis of the tube
   * @param radius sets the tube radius size
   * @param tessellation is the number of sides on the tubular surface
   * @param radiusFunction is a custom function. If it is not null, it
   * overwrittes the parameter `radius`. This function is called on each point
   * of the tube path and is passed the index `i` of the i-th point and the
   * distance of this point from the first point of the path
   * @param cap sets the way the extruded shape is capped. Possible values :
   * Mesh.NO_CAP (default), Mesh.CAP_START, Mesh.CAP_END, Mesh.CAP_ALL
   * @param scene defines the hosting scene
   * @param updatable defines if the mesh must be flagged as updatable
   * @param sideOrientation defines the mesh side orientation
   * (https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation)
   * @param instance is an instance of an existing Tube object to be updated
   * with the passed `pathArray` parameter
   * (https://doc.babylonjs.com/how_to/How_to_dynamically_morph_a_mesh#tube)
   * @returns a new Mesh
   */
  static MeshPtr
  CreateTube(const std::string& name, const std::vector<Vector3>& path, float radius = 1.f,
             unsigned int tessellation                                                  = 64,
             const std::function<float(unsigned int i, float distance)>& radiusFunction = nullptr,
             unsigned int cap = Mesh::NO_CAP, Scene* = nullptr,
             const std::optional<bool>& updatable               = std::nullopt,
             const std::optional<unsigned int>& sideOrientation = std::nullopt,
             const MeshPtr& instance                            = nullptr);

  /**
   * @brief Creates a polyhedron mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * * The parameter `type` (positive integer, max 14, default 0) sets the
   * polyhedron type to build among the 15 embbeded types. Please refer to the
   * type sheet in the tutorial to choose the wanted type
   * * The parameter `size` (positive float, default 1) sets the polygon size
   * * You can overwrite the `size` on each dimension bu using the parameters
   * `sizeX`, `sizeY` or `sizeZ` (positive floats, default to `size` value)
   * * You can build other polyhedron types than the 15 embbeded ones by setting
   * the parameter `custom` (`polyhedronObject`, default null). If you set the
   * parameter `custom`, this overwrittes the parameter `type`
   * * A `polyhedronObject` is a formatted javascript object. You'll find a full
   * file with pre-set polyhedra here :
   * https://github.com/BabylonJS/Extensions/tree/master/Polyhedron
   * * You can set the color and the UV of each side of the polyhedron with the
   * parameters `faceColors` (Color4, default `(1, 1, 1, 1)`) and faceUV
   * (Vector4, default `(0, 0, 1, 1)`)
   * * To understand how to set `faceUV` or `faceColors`, please read this by
   * considering the right number of faces of your polyhedron, instead of only 6
   * for the box :
   * https://doc.babylonjs.com/how_to/createbox_per_face_textures_and_colors
   * * The parameter `flat` (boolean, default true). If set to false, it gives
   * the polyhedron a single global face, so less vertices and shared normals.
   * In this case, `faceColors` and `faceUV` are ignored
   * * You can also set the mesh side orientation with the values :
   * Mesh.FRONTSIDE (default), Mesh.BACKSIDE or Mesh.DOUBLESIDE
   * * If you create a double-sided mesh, you can choose what parts of the
   * texture image to crop and stick respectively on the front and the back
   * sides with the parameters `frontUVs` and `backUVs` (Vector4). Detail here :
   * https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once created
   * @param name defines the name of the mesh to create
   * @param options defines the options used to create the mesh
   * @param scene defines the hosting scene
   * @returns a new Mesh
   */
  static MeshPtr CreatePolyhedron(const std::string& name, PolyhedronOptions& options, Scene*);

  /**
   * @brief Creates a sphere based upon an icosahedron with 20 triangular faces
   * which can be subdivided.
   * * The parameter `radius` sets the radius size (float) of the icosphere
   * (default 1)
   * * You can set some different icosphere dimensions, for instance to build an
   * ellipsoid, by using the parameters `radiusX`, `radiusY` and `radiusZ` (all
   * by default have the same value than `radius`)
   * * The parameter `subdivisions` sets the number of subdivisions (postive
   * integer, default 4). The more subdivisions, the more faces on the icosphere
   * whatever its size
   * * The parameter `flat` (boolean, default true) gives each side its own
   * normals. Set it to false to get a smooth continuous light reflection on the
   * surface
   * * You can also set the mesh side orientation with the values :
   * Mesh.FRONTSIDE (default), Mesh.BACKSIDE or Mesh.DOUBLESIDE
   * * If you create a double-sided mesh, you can choose what parts of the
   * texture image to crop and stick respectively on the front and the back
   * sides with the parameters `frontUVs` and `backUVs` (Vector4). Detail here :
   * https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once created
   * @param name defines the name of the mesh
   * @param options defines the options used to create the mesh
   * @param scene defines the hosting scene
   * @returns a new Mesh
   * @see https://doc.babylonjs.com/how_to/polyhedra_shapes#icosphere
   */
  static MeshPtr CreateIcoSphere(const std::string& name, IcoSphereOptions& options, Scene* scene);

  /**
   * @brief Creates a decal mesh.
   * Please consider using the same method from the MeshBuilder class instead.
   * A decal is a mesh usually applied as a model onto the surface of another
   * mesh
   * @param name  defines the name of the mesh
   * @param sourceMesh defines the mesh receiving the decal
   * @param position sets the position of the decal in world coordinates
   * @param normal sets the normal of the mesh where the decal is applied onto
   * in world coordinates
   * @param size sets the decal scaling
   * @param angle sets the angle to rotate the decal
   * @returns a new Mesh
   */
  static MeshPtr CreateDecal(const std::string& name, const AbstractMeshPtr& sourceMesh,
                             const Vector3& position, const Vector3& normal, const Vector3& size,
                             float angle = 0.f);

  /** Skeletons **/

  /**
   * @brief Prepare internal position array for software CPU skinning.
   * @returns original positions used for CPU skinning. Useful for integrating
   * Morphing with skeletons in same mesh
   */
  Float32Array& setPositionsForCPUSkinning();

  /**
   * @brief Prepare internal normal array for software CPU skinning.
   * @returns original normals used for CPU skinning. Useful for integrating
   * Morphing with skeletons in same mesh.
   */
  Float32Array& setNormalsForCPUSkinning();

  /**
   * @brief Updates the vertex buffer by applying transformation from the bones.
   * @param skeleton defines the skeleton to apply to current mesh
   * @returns the current mesh
   */
  Mesh* applySkeleton(const SkeletonPtr& skeleton);

  /** Tools **/

  /**
   * @brief Returns an object containing a min and max Vector3 which are the
   * minimum and maximum vectors of each mesh bounding box from the passed
   * array, in the world coordinates.
   * @param meshes defines the list of meshes to scan
   * @returns an object `{min:` Vector3`, max:` Vector3`}`
   */
  static MinMax GetMinMax(const std::vector<AbstractMeshPtr>& meshes);

  /**
   * @brief Returns the center of the `{min:` Vector3`, max:` Vector3`}` or the
   * center of MinMax vector3 computed from a mesh array.
   * @param meshesOrMinMaxVector could be an array of meshes or a `{min:`
   * Vector3`, max:` Vector3`}` object
   * @returns a vector3
   */
  static Vector3 Center(const MinMaxDistance& MinMaxVectorAndDistance);
  static Vector3 Center(const MinMax& minMaxVector);
  static Vector3 Center(const std::vector<AbstractMeshPtr>& meshes);

  /**
   * @brief Merge the array of meshes into a single mesh for performance
   * reasons.
   * @param meshes defines he vertices source.  They should all be of the same
   * material.  Entries can empty
   * @param disposeSource when true (default), dispose of the vertices from the
   * source meshes
   * @param allow32BitsIndices when the sum of the vertices > 64k, this must be
   * set to true
   * @param meshSubclass when set, vertices inserted into this Mesh.  Meshes can
   * then be merged into a Mesh sub-class.
   * @param subdivideWithSubMeshes when true (false default), subdivide mesh to
   * his subMesh array with meshes source.
   * @param multiMultiMaterials when true (false default), subdivide mesh and
   * accept multiple multi materials, ignores subdivideWithSubMeshes.
   * @returns a new mesh
   */
  static MeshPtr MergeMeshes(const std::vector<MeshPtr>& meshes, bool disposeSource = true,
                             bool allow32BitsIndices = true, MeshPtr meshSubclass = nullptr,
                             bool subdivideWithSubMeshes = false, bool multiMultiMaterials = false);

  /**
   * @brief Hidden
   */
  void addInstance(InstancedMesh* instance);

  /**
   * @brief Hidden
   */
  void removeInstance(InstancedMesh* instance);

protected:
  /**
   * @brief Constructor
   * @param name The value used by scene.getMeshByName() to do a lookup.
   * @param scene The scene to add this mesh to.
   * @param parent The parent of this mesh, if it has one
   * @param source An optional Mesh from which geometry is shared, cloned.
   * @param doNotCloneChildren When cloning, skip cloning child meshes of
   * source, default False. When false, achieved by calling a clone(), also
   * passing False. This will make creation of children, recursive.
   * @param clonePhysicsImpostor When cloning, include cloning mesh physics
   * impostor, default True.
   */
  Mesh(const std::string& name, Scene* scene, Node* parent = nullptr, Mesh* source = nullptr,
       bool doNotCloneChildren = true, bool clonePhysicsImpostor = true,
       bool postInitialize = false);

  /**
   * @brief Hidden
   */
  void _initialize(Scene* scene, Node* iParent, Mesh* source, bool doNotCloneChildren,
                   bool clonePhysicsImpostor);

  /**
   * @brief Gets to use shaders on bones.
   */
  bool get_computeBonesUsingShaders() const;

  /**
   * @brief Sets to use shaders on bones.
   */
  void set_computeBonesUsingShaders(bool value);

  /**
   * @brief An event triggered before rendering the mesh.
   */
  Observable<Mesh>& get_onBeforeRenderObservable();

  /**
   * @brief An event triggered before binding the mesh.
   */
  Observable<Mesh>& get_onBeforeBindObservable();

  /**
   * @brief An event triggered after rendering the mesh.
   */
  Observable<Mesh>& get_onAfterRenderObservable();

  /**
   * @brief An event triggered before drawing the mesh.
   */
  Observable<Mesh>& get_onBeforeDrawObservable();

  /**
   * @brief Sets a callback to call before drawing the mesh. It is recommended
   * to use onBeforeDrawObservable instead.
   */
  void set_onBeforeDraw(const std::function<void(Mesh*, EventState&)>& callback);

  /**
   * @brief Gets a boolean indicating if this mesh has instances.
   */
  bool get_hasInstances() const override;

  /**
   * @brief Gets a boolean indicating if this mesh has thin instances.
   */
  bool get_hasThinInstances() const override;

  /**
   * @brief Gets the morph target manager.
   * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
   */
  MorphTargetManagerPtr& get_morphTargetManager();

  /**
   * @brief Sets the morph target manager.
   * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
   */
  void set_morphTargetManager(const MorphTargetManagerPtr& value);

  /**
   * @brief Gets the source mesh (the one used to clone this one from).
   */
  Mesh*& get_source();

  /**
   * @brief Gets a boolean indicating that this mesh does not use index buffer.
   */
  bool get_isUnIndexed() const;

  /**
   * @brief Sets a boolean indicating that this mesh does not use index buffer.
   */
  void set_isUnIndexed(bool value);

  /**
   * @brief Gets the array buffer used to store the instanced buffer used for instances' world
   * matrices.
   */
  Float32Array& get_worldMatrixInstancedBuffer();

  /**
   * @brief Gets a boolean indicating that the update of the instance buffer of the world matrices
   * is manual.
   */
  bool get_manualUpdateOfWorldMatrixInstancedBuffer() const;

  /**
   * @brief Sets a boolean indicating that the update of the instance buffer of the world matrices
   * is manual.
   */
  void set_manualUpdateOfWorldMatrixInstancedBuffer(bool value);

  /**
   * @brief Hidden
   */
  bool get__isMesh() const;

  /**
   * @brief Gets a boolean indicating if this mesh has LOD.
   */
  bool get_hasLODLevels() const;

  /**
   * @brief Gets the mesh internal Geometry object.
   */
  Geometry*& get_geometry();

  bool get_isBlocked() const override;

  /**
   * @brief Gets a boolean indicating if the normals aren't to be recomputed on
   * next mesh `positions` array update. This property is pertinent only for
   * updatable parametric shapes.
   */
  bool get_areNormalsFrozen() const;

  /**
   * @brief Sets a value overriding the instance count. Only applicable when
   * custom instanced InterleavedVertexBuffer are used rather than
   * InstancedMeshs.
   */
  void set_overridenInstanceCount(size_t count);

  /**
   * @brief Gets the number of thin instances to display. Note that you can't set a number higher
   * than what the underlying buffer can handle.
   */
  size_t get_thinInstanceCount() const;

  /**
   * @brief Sets the number of thin instances to display. Note that you can't set a number higher
   * than what the underlying buffer can handle.
   */
  void set_thinInstanceCount(size_t value);

  /**
   * @brief Hidden
   */
  std::vector<Vector3>& get__positions() override;

  /**
   * @brief Hidden
   */
  void _afterComputeWorldMatrix() override;

private:
  void _sortLODLevels();
  Mesh& _onBeforeDraw(bool isInstance, Matrix& world, Material* effectiveMaterial);
  // Faster 4 weight version
  void normalizeSkinFourWeights();
  // Handle special case of extra verts.  (in theory gltf can handle 12
  // influences)
  void normalizeSkinWeightsAndExtra();
  Mesh& _queueLoad(Scene* scene);

public:
  /**
   * Sets of gets to use shaders on bones
   */
  Property<Mesh, bool> computeBonesUsingShaders;

  /** Events **/

  /**
   * An event triggered before rendering the mesh
   */
  ReadOnlyProperty<Mesh, Observable<Mesh>> onBeforeRenderObservable;

  /**
   * An event triggered before binding the mesh
   */
  ReadOnlyProperty<Mesh, Observable<Mesh>> onBeforeBindObservable;

  /**
   * An event triggered after rendering the mesh
   */
  ReadOnlyProperty<Mesh, Observable<Mesh>> onAfterRenderObservable;

  /**
   * An event triggered before drawing the mesh
   */
  ReadOnlyProperty<Mesh, Observable<Mesh>> onBeforeDrawObservable;

  /**
   * Sets a callback to call before drawing the mesh. It is recommended to use
   * onBeforeDrawObservable instead
   */
  WriteOnlyProperty<Mesh, std::function<void(Mesh*, EventState&)>> onBeforeDraw;

  // Members
  /**
   * Gets the delay loading state of the mesh (when delay loading is turned on)
   * @see https://doc.babylonjs.com/how_to/using_the_incremental_loading_system
   */
  int delayLoadState;

  /**
   * Gets the list of instances created from this mesh
   * it is not supposed to be modified manually.
   * Note also that the order of the InstancedMesh wihin the array is not significant and might
   * change.
   * @see https://doc.babylonjs.com/how_to/how_to_use_instances
   */
  std::vector<InstancedMesh*> instances;

  /**
   * true to use the edge renderer for all instances of this mesh
   */
  bool edgesShareWithInstances;

  /**
   * Gets the file containing delay loading data for this mesh
   */
  std::string delayLoadingFile;

  /**
   * Hidden
   */
  std::string _binaryInfo;

  /**
   * User defined function used to change how LOD level selection is done
   * @see https://doc.babylonjs.com/how_to/how_to_use_lod
   */
  std::function<void(float distance, Mesh* mesh, Mesh* selectedLevel)> onLODLevelSelection;

  /**
   * Gets or sets the morph target manager
   * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
   */
  Property<Mesh, MorphTargetManagerPtr> morphTargetManager;

  /**
   * Hidden
   */
  _CreationDataStoragePtr _creationDataStorage;

  /**
   * Gets the mesh internal Geometry object
   */
  Geometry* _geometry;

  /**
   * Hidden
   */
  std::vector<std::string> _delayInfoKinds;

  /**
   * Hidden
   */
  std::function<void(const json& parsedGeometry, const MeshPtr& mesh)> _delayLoadingFunction;

  /** Hidden */
  bool _shouldGenerateFlatShading;

  // Use by builder only to know what orientation were the mesh build in.
  /**
   * Hidden
   */
  unsigned int _originalBuilderSideOrientation;

  /**
   * Use this property to change the original side orientation defined at
   * construction time
   */
  std::optional<unsigned int> overrideMaterialSideOrientation;

  /**
   * Gets the source mesh (the one used to clone this one from)
   */
  ReadOnlyProperty<Mesh, Mesh*> _source;

  /**
   * Gets or sets a boolean indicating that this mesh does not use index buffer
   */
  Property<Mesh, bool> isUnIndexed;

  /**
   * Gets the array buffer used to store the instanced buffer used for instances' world matrices
   */
  ReadOnlyProperty<Mesh, Float32Array> worldMatrixInstancedBuffer;

  /**
   * Gets or sets a boolean indicating that the update of the instance buffer of the world matrices
   * is manual
   */
  Property<Mesh, bool> manualUpdateOfWorldMatrixInstancedBuffer;

  /**
   * Hidden
   */
  ReadOnlyProperty<Mesh, bool> _isMesh;

  /**
   * Gets a boolean indicating if this mesh has LOD
   */
  ReadOnlyProperty<Mesh, bool> hasLODLevels;

  /**
   * Gets the mesh internal Geometry object
   */
  ReadOnlyProperty<Mesh, Geometry*> geometry;

  /**
   * Gets a boolean indicating if the normals aren't to be recomputed on next
   * mesh `positions` array update. This property is pertinent only for
   * updatable parametric shapes.
   */
  ReadOnlyProperty<Mesh, bool> areNormalsFrozen;

  /**
   * Sets a value overriding the instance count. Only applicable when custom
   * instanced InterleavedVertexBuffer are used rather than InstancedMeshs
   */
  WriteOnlyProperty<Mesh, size_t> overridenInstanceCount;

  /**
   * Gets or sets a boolean defining if we want picking to pick thin instances as well
   */
  bool thinInstanceEnablePicking;

  /**
   * Gets / sets the number of thin instances to display. Note that you can't set a number higher
   * than what the underlying buffer can handle.
   */
  Property<Mesh, size_t> thinInstanceCount;

  /**
   * Hidden
   */
  std::unique_ptr<_InstanceDataStorage> _instanceDataStorage;

private:
  // Internal data
  std::unique_ptr<_InternalMeshDataInfo> _internalMeshDataInfo;
  // Members
  Observer<Mesh>::Ptr _onBeforeDrawObserver;
  // Morph
  std::vector<VertexBuffer*> _delayInfo;
  std::unique_ptr<_ThinInstanceDataStorage> _thinInstanceDataStorage;
  std::unique_ptr<_UserThinInstanceBuffersStorage> _userThinInstanceBuffersStorage;
  MaterialPtr _effectiveMaterial;
  // Instances
  /** @hidden */
  UserInstancedBuffersStorage _userInstancedBuffersStorage;
  // For extrusion and tube
  Path3D _path3D;
  std::vector<std::vector<Vector3>> _pathArray;
  unsigned int _tessellation;
  unsigned int _cap;
  float _arc;

}; // end of class Mesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_MESH_H
