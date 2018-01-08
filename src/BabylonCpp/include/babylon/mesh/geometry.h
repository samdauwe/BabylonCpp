#ifndef BABYLON_MESH_GEOMETRY_H
#define BABYLON_MESH_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/mesh/iget_set_vertices_data.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Geometry : public IGetSetVerticesData,
                                       public IDisposable {

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
   * @brief The Bias Vector to apply on the bounding elements (box/sphere), the
   * max extend is computed as v += v * bias.x + bias.y, the min is computed as
   * v -= * v * bias.x + bias.y
   * @returns The Bias Vector
   */
  const Vector2& boundingBias() const;

  void setBoundingBias(const Vector2& value);
  static Geometry* CreateGeometryForMesh(Mesh* mesh);
  const MinMax& extend() const;
  Scene* getScene();
  Engine* getEngine();
  bool isReady() const;
  bool doNotSerialize() const;
  void _rebuild();
  void setAllVerticesData(VertexData* vertexData, bool updatable = false);
  AbstractMesh* setVerticesData(unsigned int kind, const Float32Array& data,
                                bool updatable = false,
                                int stride     = -1) override;
  void removeVerticesData(unsigned int kind);
  void setVerticesBuffer(unique_ptr_t<VertexBuffer>&& buffer);
  void updateVerticesDataDirectly(unsigned int kind, const Float32Array& data,
                                  int offset);
  AbstractMesh* updateVerticesData(unsigned int kind, const Float32Array& data,
                                   bool updateExtends = false,
                                   bool makeItUnique  = false) override;
  void _bind(Effect* effect, GL::IGLBuffer* indexToBind = nullptr);
  size_t getTotalVertices() const;
  Float32Array getVerticesData(unsigned int kind, bool copyWhenShared = false,
                               bool forceCopy = false) override;

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

  VertexBuffer* getVertexBuffer(unsigned int kind) const;
  unordered_map_t<string_t, VertexBuffer*> getVertexBuffers();
  bool isVerticesDataPresent(unsigned int kind) override;
  Uint32Array getVerticesDataKinds();
  void updateIndices(const IndicesArray& indices, int offset = 0);
  AbstractMesh* setIndices(const IndicesArray& indices,
                           size_t totalVertices = 0,
                           bool updatable       = false) override;
  size_t getTotalIndices();
  IndicesArray getIndices(bool copyWhenShared = false) override;
  GL::IGLBuffer* getIndexBuffer();
  void _releaseVertexArrayObject(Effect* effect);
  void releaseForMesh(Mesh* mesh, bool shouldDispose = true);
  void applyToMesh(Mesh* mesh);
  void load(Scene* scene, const ::std::function<void()>& onLoaded = nullptr);

  /**
   * Invert the geometry to move from a right handed system to a left handed
   * one.
   */
  void toLeftHanded();

  // Cache
  void _resetPointsArrayCache();
  bool _generatePointsArray();

  bool isDisposed() const;
  void dispose(bool doNotRecurse = false) override;
  Geometry* copy(const string_t& id);
  Json::object serialize() const;
  Json::object serializeVerticeData() const;

  /** Statics **/
  static Geometry* ExtractFromMesh(Mesh* mesh, const string_t& id);
  static string_t RandomId();
  static void ImportGeometry(const Json::value& parsedGeometry, Mesh* mesh);
  static void _CleanMatricesWeights(Float32Array& matricesWeights,
                                    unsigned int influencers);
  static Geometry* Parse(const Json::value& parsedVertexData, Scene* scene,
                         const string_t& rootUrl);

protected:
  Geometry(const string_t& id, Scene* scene, VertexData* vertexData = nullptr,
           bool updatable = false, Mesh* mesh = nullptr);

private:
  void updateBoundingInfo(bool updateExtends, const Float32Array& data);
  void updateExtend(const Float32Array& data, int stride = 3);
  void _applyToMesh(Mesh* mesh);
  void notifyUpdate(unsigned int kind = 1);
  void _queueLoad(Scene* scene, const ::std::function<void()>& onLoaded);
  void _disposeVertexArrayObjects();

public:
  string_t id;
  int delayLoadState;
  string_t delayLoadingFile;
  ::std::function<void(Geometry* geometry, unsigned int kind)>
    onGeometryUpdated;
  Uint32Array _delayInfoKinds;
  unique_ptr_t<BoundingInfo> _boundingInfo;
  ::std::function<void(const Json::value& parsedVertexData, Geometry* geometry)>
    _delayLoadingFunction;
  int _softwareSkinningRenderId;
  vector_t<Vector3> _positions; // Cache
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
  Uint32Array _delayInfo;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  bool _indexBufferIsUpdatable;

}; // end of class Geometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_GEOMETRY_H
