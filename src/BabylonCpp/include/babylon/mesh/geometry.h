#ifndef BABYLON_MESH_GEOMETRY_H
#define BABYLON_MESH_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/core/structs.h>
#include <babylon/mesh/iget_set_vertices_data.h>

namespace BABYLON {

/**
 * @brief Class used to store geometry data (vertex buffers + index buffer).
 */
class BABYLON_SHARED_EXPORT Geometry : public IGetSetVerticesData {

public:
  friend class Mesh;

public:
  template <typename... Ts>
  static Geometry* New(Ts&&... args)
  {
    auto geometry = new Geometry(::std::forward<Ts>(args)...);
    geometry->addToScene(static_cast<unique_ptr_t<Geometry>>(geometry));

    return geometry;
  }
  virtual ~Geometry();

  /**
   * @brief Adds the geometry to the scene.
   * @param newGeometry
   */
  void addToScene(unique_ptr_t<Geometry>&& newGeometry);

  /**
   * @brief Gets the Bias Vector to apply on the bounding elements (box/sphere),
   * the max extend is computed as v += v * bias.x + bias.y, the min is computed
   * as v -= v * bias.x + bias.y
   * @returns The Bias Vector
   */
  const Vector2& boundingBias() const;

  /**
   *  @brief Sets the Bias Vector to apply on the bounding elements.
   * (box/sphere), the max extend is computed as v += v * bias.x + bias.y, the
   * min is computed as v -= v * bias.x + bias.y
   */
  void setBoundingBias(const Vector2& value);

  /**
   * @brief Static function used to attach a new empty geometry to a mesh.
   * @param mesh defines the mesh to attach the geometry to
   * @returns the new {BABYLON.Geometry}
   */
  static Geometry* CreateGeometryForMesh(Mesh* mesh);

  /**
   * @brief Gets the current extend of the geometry.
   */
  const MinMax& extend() const;

  /**
   * @brief Gets the hosting scene.
   * @returns the hosting {BABYLON.Scene}
   */
  Scene* getScene();

  /**
   * @brief Gets the hosting engine.
   * @returns the hosting {BABYLON.Engine}
   */
  Engine* getEngine();

  /**
   * @brief Defines if the geometry is ready to use.
   * @returns true if the geometry is ready to be used
   */
  bool isReady() const;

  /**
   * @brief Gets a value indicating that the geometry should not be serialized.
   */
  bool doNotSerialize() const;

  /**
   * @brief Hidden
   */
  void _rebuild();

  /**
   * @brief Affects all gemetry data in one call.
   * @param vertexData defines the geometry data
   * @param updatable defines if the geometry must be flagged as updatable
   * (false as default)
   */
  void setAllVerticesData(VertexData* vertexData, bool updatable = false);

  /**
   * @brief Set specific vertex data.
   * @param kind defines the data kind (Position, normal, etc...)
   * @param data defines the vertex data to use
   * @param updatable defines if the vertex must be flagged as updatable (false
   * as default)
   * @param stride defines the stride to use (0 by default). This value is
   * deduced from the kind value if not specified
   */
  AbstractMesh* setVerticesData(unsigned int kind, const Float32Array& data,
                                bool updatable = false,
                                int stride     = -1) override;

  /**
   * @brief Removes a specific vertex data.
   * @param kind defines the data kind (Position, normal, etc...)
   */
  void removeVerticesData(unsigned int kind);

  /**
   * @brief Affect a vertex buffer to the geometry. the vertexBuffer.getKind()
   * function is used to determine where to store the data.
   * @param buffer defines the vertex buffer to use
   * @param totalVertices defines the total number of vertices for position kind
   * (could be null)
   */
  void setVerticesBuffer(unique_ptr_t<VertexBuffer>&& buffer,
                         const Nullable<size_t>& totalVertices = nullptr);

  /**
   * @brief Update a specific vertex buffer.
   * This function will directly update the underlying WebGLBuffer according to
   * the passed numeric array or Float32Array It will do nothing if the buffer
   * is not updatable
   * @param kind defines the data kind (Position, normal, etc...)
   * @param data defines the data to use
   * @param offset defines the offset in the target buffer where to store the
   * data
   * @param useBytes set to true if the offset is in bytes
   */
  void updateVerticesDataDirectly(unsigned int kind, const Float32Array& data,
                                  size_t offset, bool useBytes = false);

  /**
   * @brief Update a specific vertex buffer.
   * This function will create a new buffer if the current one is not updatable
   * @param kind defines the data kind (Position, normal, etc...)
   * @param data defines the data to use
   * @param updateExtends defines if the geometry extends must be recomputed
   * (false by default)
   */
  AbstractMesh* updateVerticesData(unsigned int kind, const Float32Array& data,
                                   bool updateExtends = false,
                                   bool makeItUnique  = false) override;

  /**
   * @brief Hidden
   */
  void _bind(Effect* effect, GL::IGLBuffer* indexToBind = nullptr);

  /**
   * @brief Gets total number of vertices.
   * @returns the total number of vertices
   */
  size_t getTotalVertices() const;

  /**
   * @brief Gets a specific vertex data attached to this geometry. Float data is
   * constructed if the vertex buffer data cannot be returned directly.
   * @param kind defines the data kind (Position, normal, etc...)
   * @param copyWhenShared defines if the returned array must be cloned upon
   * returning it if the current geometry is shared between multiple meshes
   * @param forceCopy defines a boolean indicating that the returned array must
   * be cloned upon returning it
   * @returns a float array containing vertex data
   */
  Float32Array getVerticesData(unsigned int kind, bool copyWhenShared = false,
                               bool forceCopy = false) override;

  /**
   * @brief Returns a boolean defining if the vertex data for the requested
   * `kind` is updatable.
   * @param kind defines the data kind (Position, normal, etc...)
   * @returns true if the vertex buffer with the specified kind is updatable
   */
  bool isVertexBufferUpdatable(unsigned int kind) const;

  /**
   * @brief Gets a specific vertex buffer.
   * @param kind defines the data kind (Position, normal, etc...)
   * @returns a {BABYLON.VertexBuffer}
   */
  VertexBuffer* getVertexBuffer(unsigned int kind) const;

  /**
   * @brief Returns all vertex buffers.
   * @return an object holding all vertex buffers indexed by kind
   */
  unordered_map_t<string_t, VertexBuffer*> getVertexBuffers();

  /**
   * @brief Gets a boolean indicating if specific vertex buffer is present.
   * @param kind defines the data kind (Position, normal, etc...)
   * @returns true if data is present
   */
  bool isVerticesDataPresent(unsigned int kind) override;

  /**
   * @brief Gets a list of all attached data kinds (Position, normal, etc...).
   * @returns a list of string containing all kinds
   */
  Uint32Array getVerticesDataKinds();

  /**
   * @brief Update index buffer.
   * @param indices defines the indices to store in the index buffer
   * @param offset defines the offset in the target buffer where to store the
   * data
   */
  void updateIndices(const IndicesArray& indices, int offset = 0);

  /**
   * @brief Creates a new index buffer.
   * @param indices defines the indices to store in the index buffer
   * @param totalVertices defines the total number of vertices (could be null)
   * @param updatable defines if the index buffer must be flagged as updatable
   * (false by default)
   */
  AbstractMesh* setIndices(const IndicesArray& indices,
                           size_t totalVertices = 0,
                           bool updatable       = false) override;

  /**
   * @brief Return the total number of indices.
   * @returns the total number of indices
   */
  size_t getTotalIndices();

  /**
   * @brief Gets the index buffer array.
   * @param copyWhenShared defines if the returned array must be cloned upon
   * returning it if the current geometry is shared between multiple meshes
   * @returns the index buffer array
   */
  IndicesArray getIndices(bool copyWhenShared = false) override;

  /**
   * @brief Gets the index buffer.
   * @return the index buffer
   */
  GL::IGLBuffer* getIndexBuffer();

  /**
   * @brief Hidden
   */
  void _releaseVertexArrayObject(Effect* effect = nullptr);

  /**
   * @brief Release the associated resources for a specific mesh.
   * @param mesh defines the source mesh
   * @param shouldDispose defines if the geometry must be disposed if there is
   * no more mesh pointing to it
   */
  void releaseForMesh(Mesh* mesh, bool shouldDispose = true);

  /**
   * @brief Apply current geometry to a given mesh.
   * @param mesh defines the mesh to apply geometry to
   */
  void applyToMesh(Mesh* mesh);

  /**
   * @brief Load the geometry if it was flagged as delay loaded.
   * @param scene defines the hosting scene
   * @param onLoaded defines a callback called when the geometry is loaded
   */
  void load(Scene* scene, const ::std::function<void()>& onLoaded = nullptr);

  /**
   * @brief Invert the geometry to move from a right handed system to a left
   * handed one.
   */
  void toLeftHanded();

  // Cache

  /**
   * @brief Hidden
   */
  void _resetPointsArrayCache();

  /**
   * @brief Hidden
   */
  bool _generatePointsArray();

  /**
   * @brief Gets a value indicating if the geometry is disposed.
   * @returns true if the geometry was disposed
   */
  bool isDisposed() const;

  /**
   * @brief Free all associated resources.
   */
  void dispose();

  /**
   * @brief Clone the current geometry into a new geometry.
   * @param id defines the unique ID of the new geometry
   * @returns a new geometry object
   */
  Geometry* copy(const string_t& id);

  /**
   * @brief Serialize the current geometry info (and not the vertices data) into
   * a JSON object.
   * @return a JSON representation of the current geometry data (without the
   * vertices data)
   */
  Json::object serialize() const;

  /**
   * @brief Serialize all vertices data into a JSON oject.
   * @returns a JSON representation of the current geometry data
   */
  Json::object serializeVerticeData() const;

  /** Statics **/

  /**
   * @brief Extracts a clone of a mesh geometry.
   * @param mesh defines the source mesh
   * @param id defines the unique ID of the new geometry object
   * @returns the new geometry object
   */
  static Geometry* ExtractFromMesh(Mesh* mesh, const string_t& id);

  /**
   * @brief You should now use Tools.RandomId(), this method is still here for
   * legacy reasons. Implementation from
   * http://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid-in-javascript/2117523#answer-2117523
   * Be aware Math.random() could cause collisions, but:
   * "All but 6 of the 128 bits of the ID are randomly generated, which means
   * that for any two ids, there's a 1 in 2^^122 (or 5.3x10^^36) chance they'll
   * collide"
   * @returns a string containing a new GUID
   */
  static string_t RandomId();

  /**
   * @brief Hidden
   */
  static void _ImportGeometry(const Json::value& parsedGeometry, Mesh* mesh);

  /**
   * @brief Hidden
   */
  static void _CleanMatricesWeights(const Json::value& parsedGeometry,
                                    Mesh* mesh);

  /**
   * @brief Create a new geometry from persisted data (Using .babylon file
   * format).
   * @param parsedVertexData defines the persisted data
   * @param scene defines the hosting scene
   * @param rootUrl defines the root url to use to load assets (like delayed
   * data)
   * @returns the new geometry object
   */
  static Geometry* Parse(const Json::value& parsedVertexData, Scene* scene,
                         const string_t& rootUrl);

protected:
  /**
   * @brief Creates a new geometry.
   * @param id defines the unique ID
   * @param scene defines the hosting scene
   * @param vertexData defines the {BABYLON.VertexData} used to get geometry
   * data
   * @param updatable defines if geometry must be updatable (false by default)
   * @param mesh defines the mesh that will be associated with the geometry
   */
  Geometry(const string_t& id, Scene* scene, VertexData* vertexData = nullptr,
           bool updatable = false, Mesh* mesh = nullptr);

private:
  void _updateBoundingInfo(bool updateExtends, const Float32Array& data);
  void _updateExtend(Float32Array data);
  void _applyToMesh(Mesh* mesh);
  void notifyUpdate(unsigned int kind = 1);
  void _queueLoad(Scene* scene, const ::std::function<void()>& onLoaded);
  void _disposeVertexArrayObjects();

public:
  // Members
  /**
   * Gets or sets the unique ID of the geometry
   */
  string_t id;

  /**
   * Gets the delay loading state of the geometry (none by default which means
   * not delayed)
   */
  int delayLoadState;

  /**
   * Gets the file containing the data to load when running in delay load state
   */
  string_t delayLoadingFile;

  /**
   * Callback called when the geometry is updated
   */
  ::std::function<void(Geometry* geometry, unsigned int kind)>
    onGeometryUpdated;

  Uint32Array _delayInfoKinds;
  /** Hidden */
  unique_ptr_t<BoundingInfo> _boundingInfo;
  /** Hidden */
  ::std::function<void(const Json::value& parsedVertexData, Geometry* geometry)>
    _delayLoadingFunction;
  /** Hidden */
  int _softwareSkinningRenderId;
  // Cache
  /** Hidden */
  vector_t<Vector3> _positions;
  unordered_map_t<string_t, unique_ptr_t<GL::IGLVertexArrayObject>>
    _vertexArrayObjects;
  bool _updatable;
  vector_t<Vector3> centroids;

private:
  Scene* _scene;
  Engine* _engine;
  vector_t<Mesh*> _meshes;
  size_t _totalVertices;
  IndicesArray _indices;
  unordered_map_t<unsigned int, unique_ptr_t<VertexBuffer>> _vertexBuffers;
  bool _isDisposed;
  Nullable<MinMax> _extend;
  Nullable<Vector2> _boundingBias;
  /** Hidden */
  Uint32Array _delayInfo;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  bool _indexBufferIsUpdatable;

}; // end of class Geometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_GEOMETRY_H
