#ifndef BABYLON_MESHES_GEOMETRY_H
#define BABYLON_MESHES_GEOMETRY_H

#include <functional>
#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/meshes/iget_set_vertices_data.h>

using json = nlohmann::json;

namespace BABYLON {

class BoundingInfo;
class Effect;
class Engine;
class Geometry;
class Mesh;
class Scene;
class VertexBuffer;
class VertexData;
using EffectPtr       = std::shared_ptr<Effect>;
using GeometryPtr     = std::shared_ptr<Geometry>;
using MeshPtr         = std::shared_ptr<Mesh>;
using VertexBufferPtr = std::shared_ptr<VertexBuffer>;

namespace GL {
class IGLBuffer;
class IGLVertexArrayObject;
} // end of namespace GL

/**
 * @brief Class used to store geometry data (vertex buffers + index buffer).
 */
class BABYLON_SHARED_EXPORT Geometry : public IGetSetVerticesData {

public:
  friend class Mesh;

public:
  template <typename... Ts>
  static GeometryPtr New(Ts&&... args)
  {
    auto geometry
      = std::shared_ptr<Geometry>(new Geometry(std::forward<Ts>(args)...));
    geometry->addToScene(geometry);

    return geometry;
  }
  virtual ~Geometry(); // = default

  /**
   * @brief Adds the geometry to the scene.
   * @param newGeometry
   */
  void addToScene(const GeometryPtr& newGeometry);

  /**
   * @brief Static function used to attach a new empty geometry to a mesh
   * @param mesh defines the mesh to attach the geometry to
   * @returns the new Geometry
   */
  static GeometryPtr CreateGeometryForMesh(Mesh* mesh);

  /**
   * @brief Gets the hosting scene.
   * @returns the hosting Scene
   */
  Scene* getScene();

  /**
   * @brief Gets the hosting engine.
   * @returns the hosting Engine
   */
  Engine* getEngine();

  /**
   * @brief Defines if the geometry is ready to use.
   * @returns true if the geometry is ready to be used
   */
  bool isReady() const;

  /**
   * @brief Hidden
   */
  void _rebuild();

  /**
   * @brief Affects all geometry data in one call.
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
  AbstractMesh*
  setVerticesData(const std::string& kind, const Float32Array& data,
                  bool updatable                      = false,
                  const std::optional<size_t>& stride = std::nullopt) override;

  /**
   * @brief Removes a specific vertex data.
   * @param kind defines the data kind (Position, normal, etc...)
   */
  void removeVerticesData(const std::string& kind);

  /**
   * @brief Affect a vertex buffer to the geometry. the vertexBuffer.getKind()
   * function is used to determine where to store the data.
   * @param buffer defines the vertex buffer to use
   * @param totalVertices defines the total number of vertices for position kind
   * (could be null)
   */
  void setVerticesBuffer(const VertexBufferPtr& buffer,
                         const std::optional<size_t>& totalVertices
                         = std::nullopt);

  /**
   * @brief Update a specific vertex buffer.
   * This function will directly update the underlying DataBuffer according to
   * the passed numeric array or Float32Array It will do nothing if the buffer
   * is not updatable
   * @param kind defines the data kind (Position, normal, etc...)
   * @param data defines the data to use
   * @param offset defines the offset in the target buffer where to store the
   * data
   * @param useBytes set to true if the offset is in bytes
   */
  void updateVerticesDataDirectly(const std::string& kind,
                                  const Float32Array& data, size_t offset,
                                  bool useBytes = false);

  /**
   * @brief Update a specific vertex buffer.
   * This function will create a new buffer if the current one is not updatable
   * @param kind defines the data kind (Position, normal, etc...)
   * @param data defines the data to use
   * @param updateExtends defines if the geometry extends must be recomputed
   * (false by default)
   */
  AbstractMesh* updateVerticesData(const std::string& kind,
                                   const Float32Array& data,
                                   bool updateExtends = false,
                                   bool makeItUnique  = false) override;

  /**
   * @brief Hidden
   */
  void _bind(const EffectPtr& effect, GL::IGLBuffer* indexToBind = nullptr);

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
  Float32Array getVerticesData(const std::string& kind,
                               bool copyWhenShared = false,
                               bool forceCopy      = false) override;

  /**
   * @brief Returns a boolean defining if the vertex data for the requested
   * `kind` is updatable.
   * @param kind defines the data kind (Position, normal, etc...)
   * @returns true if the vertex buffer with the specified kind is updatable
   */
  bool isVertexBufferUpdatable(const std::string& kind) const;

  /**
   * @brief Gets a specific vertex buffer.
   * @param kind defines the data kind (Position, normal, etc...)
   * @returns a VertexBuffer
   */
  VertexBufferPtr getVertexBuffer(const std::string& kind);

  /**
   * @brief Gets a specific vertex buffer.
   * @param kind defines the data kind (Position, normal, etc...)
   * @returns a VertexBuffer
   */
  std::unordered_map<std::string, VertexBufferPtr> getVertexBuffers();

  /**
   * @brief Gets a boolean indicating if specific vertex buffer is present.
   * @param kind defines the data kind (Position, normal, etc...)
   * @returns true if data is present
   */
  bool isVerticesDataPresent(const std::string& kind) const override;

  /**
   * @brief Gets a list of all attached data kinds (Position, normal, etc...).
   * @returns a list of string containing all kinds
   */
  std::vector<std::string> getVerticesDataKinds();

  /**
   * @brief Update index buffer.
   * @param indices defines the indices to store in the index buffer
   * @param offset defines the offset in the target buffer where to store the
   * data
   * @param gpuMemoryOnly defines a boolean indicating that only the GPU memory
   * must be updated leaving the CPU version of the indices unchanged (false by
   * default)
   */
  void updateIndices(const IndicesArray& indices, int offset = 0,
                     bool gpuMemoryOnly = false);

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
   * @brief Gets the index buffer array
   * @param copyWhenShared defines if the returned array must be cloned upon
   * returning it if the current geometry is shared between multiple meshes
   * @param forceCopy defines a boolean indicating that the returned array must
   * be cloned upon returning it
   * @returns the index buffer array
   */
  IndicesArray getIndices(bool copyWhenShared = false,
                          bool forceCopy      = false) override;

  /**
   * @brief Gets the index buffer.
   * @return the index buffer
   */
  GL::IGLBuffer* getIndexBuffer();

  /**
   * @brief Hidden
   */
  void _releaseVertexArrayObject(const EffectPtr& effect = nullptr);

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
  void load(Scene* scene, const std::function<void()>& onLoaded = nullptr);

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
  GeometryPtr copy(const std::string& id);

  /**
   * @brief Serialize the current geometry info (and not the vertices data) into
   * a JSON object.
   * @return a JSON representation of the current geometry data (without the
   * vertices data)
   */
  json serialize() const;

  /**
   * @brief Serialize all vertices data into a JSON oject.
   * @returns a JSON representation of the current geometry data
   */
  json serializeVerticeData() const;

  /** Statics **/

  /**
   * @brief Extracts a clone of a mesh geometry.
   * @param mesh defines the source mesh
   * @param id defines the unique ID of the new geometry object
   * @returns the new geometry object
   */
  static GeometryPtr ExtractFromMesh(Mesh* mesh, const std::string& id);

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
  static std::string RandomId();

  /**
   * @brief Hidden
   */
  static void _ImportGeometry(const json& parsedGeometry, const MeshPtr& mesh);

  /**
   * @brief Hidden
   */
  static void _CleanMatricesWeights(const json& parsedGeometry,
                                    const MeshPtr& mesh);

  /**
   * @brief Create a new geometry from persisted data (Using .babylon file
   * format).
   * @param parsedVertexData defines the persisted data
   * @param scene defines the hosting scene
   * @param rootUrl defines the root url to use to load assets (like delayed
   * data)
   * @returns the new geometry object
   */
  static GeometryPtr Parse(const json& parsedVertexData, Scene* scene,
                           const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new geometry.
   * @param id defines the unique ID
   * @param scene defines the hosting scene
   * @param vertexData defines the VertexData used to get geometry data
   * @param updatable defines if geometry must be updatable (false by default)
   * @param mesh defines the mesh that will be associated with the geometry
   */
  Geometry(const std::string& id, Scene* scene,
           VertexData* vertexData = nullptr, bool updatable = false,
           Mesh* mesh = nullptr);

  /**
   * @brief Gets the Bias Vector to apply on the bounding elements (box/sphere),
   * the max extend is computed as v += v * bias.x + bias.y, the min is computed
   * as v -= v * bias.x + bias.y
   * @returns The Bias Vector
   */
  std::optional<Vector2>& get_boundingBias();

  /**
   *  @brief Sets the Bias Vector to apply on the bounding elements.
   * (box/sphere), the max extend is computed as v += v * bias.x + bias.y, the
   * min is computed as v -= v * bias.x + bias.y
   */
  void set_boundingBias(const std::optional<Vector2>& value);

  /**
   * @brief Gets the current extend of the geometry.
   */
  MinMax& get_extend();

  /**
   * @brief Gets a value indicating that the geometry should not be serialized.
   */
  bool get_doNotSerialize() const;

private:
  void _updateBoundingInfo(bool updateExtends, const Float32Array& data);
  void _updateExtend(Float32Array data);
  void _applyToMesh(Mesh* mesh);
  void notifyUpdate(const std::string& kind = "");
  void _queueLoad(Scene* scene, const std::function<void()>& onLoaded);
  void _disposeVertexArrayObjects();

public:
  // Members
  /**
   * Gets or sets the unique ID of the geometry
   */
  std::string id;

  /**
   * Gets or sets the unique ID of the geometry
   */
  size_t uniqueId;

  /**
   * Gets the delay loading state of the geometry (none by default which means
   * not delayed)
   */
  int delayLoadState;

  /**
   * Gets the file containing the data to load when running in delay load state
   */
  std::string delayLoadingFile;

  /**
   * Callback called when the geometry is updated
   */
  std::function<void(Geometry* geometry, const std::string& kind)>
    onGeometryUpdated;

  /** Hidden */
  IndicesArray _indices;
  /** Hidden */
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  /** Hidden */
  std::vector<std::string> _delayInfo;
  /** Hidden */
  std::vector<std::string> _delayInfoKinds;
  /** Hidden */
  std::unique_ptr<BoundingInfo> _boundingInfo;
  /** Hidden */
  std::function<void(const json& parsedVertexData, Geometry& geometry)>
    _delayLoadingFunction;
  /** Hidden */
  int _softwareSkinningFrameId;
  // Cache
  /** Hidden */
  std::vector<Vector3> _positions;

  /**
   *  Gets or sets the Bias Vector to apply on the bounding elements
   * (box/sphere), the max extend is computed as v += v * bias.x + bias.y, the
   * min is computed as v -= v * bias.x + bias.y
   */
  Property<Geometry, std::optional<Vector2>> boundingBias;

  std::unordered_map<std::string, std::unique_ptr<GL::IGLVertexArrayObject>>
    _vertexArrayObjects;
  bool _updatable;
  std::vector<Vector3> centroids;

  /**
   * Gets the current extend of the geometry
   */
  ReadOnlyProperty<Geometry, MinMax> extend;

  /**
   * Gets a value indicating that the geometry should not be serialized
   */
  ReadOnlyProperty<Geometry, bool> doNotSerialize;

private:
  Scene* _scene;
  Engine* _engine;
  std::vector<Mesh*> _meshes;
  size_t _totalVertices;
  bool _isDisposed;
  std::optional<MinMax> _extend;
  std::optional<Vector2> _boundingBias;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  bool _indexBufferIsUpdatable;

}; // end of class Geometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_GEOMETRY_H
