#ifndef BABYLON_MESH_GEOMETRY_H
#define BABYLON_MESH_GEOMETRY_H

#include <babylon/babylon_global.h>
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
    auto geometry = new Geometry(std::forward<Ts>(args)...);
    geometry->addToScene(static_cast<std::unique_ptr<Geometry>>(geometry));

    return geometry;
  }
  virtual ~Geometry();

  /**
   * @brief Adds the geometry to the scene.
   * @param newGeometry
   */
  void addToScene(std::unique_ptr<Geometry>&& newGeometry);

  /**
   * @brief The Bias Vector to apply on the bounding elements (box/sphere), the
   * max extend is computed as v += v * bias.x + bias.y, the min is computed as
   * v -= * v * bias.x + bias.y
   * @returns The Bias Vector
   */
  Vector2& boundingBias();

  void setBoundingBias(const Vector2& value);

  MinMax extend() const;
  Scene* getScene();
  Engine* getEngine();
  bool isReady() const;
  bool doNotSerialize() const;
  void setAllVerticesData(VertexData* vertexData, bool updatable = false);
  Mesh* setVerticesData(unsigned int kind, const Float32Array& data,
                        bool updatable = false, int stride = -1) override;
  void removeVerticesData(unsigned int kind);
  void setVerticesBuffer(std::unique_ptr<VertexBuffer>&& buffer);
  void updateVerticesDataDirectly(unsigned int kind, const Float32Array& data,
                                  int offset);
  Mesh* updateVerticesData(unsigned int kind, const Float32Array& data,
                           bool updateExtends = false,
                           bool makeItUnique  = false) override;
  void _bind(Effect* effect, GL::IGLBuffer* indexToBind = nullptr);
  size_t getTotalVertices() const;
  Float32Array getVerticesData(unsigned int kind, bool copyWhenShared = false,
                               bool forceCopy = false) override;
  VertexBuffer* getVertexBuffer(unsigned int kind) const;
  std::unordered_map<std::string, VertexBuffer*> getVertexBuffers();
  bool isVerticesDataPresent(unsigned int kind) override;
  Uint32Array getVerticesDataKinds();
  Mesh* setIndices(const IndicesArray& indices,
                   size_t totalVertices = 0) override;
  size_t getTotalIndices();
  IndicesArray getIndices(bool copyWhenShared = false) override;
  GL::IGLBuffer* getIndexBuffer();
  void _releaseVertexArrayObject(Effect* effect);
  void releaseForMesh(Mesh* mesh, bool shouldDispose = true);
  void applyToMesh(Mesh* mesh);
  void load(Scene* scene, const std::function<void()>& onLoaded = nullptr);

  /**
   * Invert the geometry to move from a right handed system to a left handed
   * one.
   */
  void toLeftHanded();

  bool isDisposed() const;
  void dispose(bool doNotRecurse = false) override;
  Geometry* copy(const std::string& id);
  Json::object serialize() const;
  Json::object serializeVerticeData() const;

  /** Statics **/
  static Geometry* ExtractFromMesh(Mesh* mesh, const std::string& id);
  static std::string RandomId();
  static void ImportGeometry(const Json::value& parsedGeometry, Mesh* mesh);
  static Geometry* Parse(const Json::value& parsedVertexData, Scene* scene,
                         const std::string& rootUrl);

protected:
  Geometry(const std::string& id, Scene* scene,
           VertexData* vertexData = nullptr, bool updatable = false,
           Mesh* mesh = nullptr);

private:
  void updateBoundingInfo(bool updateExtends, const Float32Array& data);
  void updateExtend(const Float32Array& data, int stride = 3);
  void _applyToMesh(Mesh* mesh);
  void notifyUpdate(unsigned int kind = 1);
  void _queueLoad(Scene* scene, const std::function<void()>& onLoaded);
  void _disposeVertexArrayObjects();

public:
  std::string id;
  int delayLoadState;
  std::string delayLoadingFile;
  std::function<void(Geometry* geometry, unsigned int kind)> onGeometryUpdated;
  Uint32Array _delayInfoKinds;
  std::unique_ptr<BoundingInfo> _boundingInfo;
  std::function<void(const Json::value& parsedVertexData, Geometry* geometry)>
    _delayLoadingFunction;
  int _softwareSkinningRenderId;
  std::unordered_map<std::string, std::unique_ptr<GL::IGLVertexArrayObject>>
    _vertexArrayObjects;
  std::vector<Vector3> centroids;

private:
  Scene* _scene;
  Engine* _engine;
  std::vector<Mesh*> _meshes;
  size_t _totalVertices;
  IndicesArray _indices;
  std::unordered_map<unsigned int, std::unique_ptr<VertexBuffer>>
    _vertexBuffers;
  bool _isDisposed;
  bool _extendSet;
  MinMax _extend;
  bool _hasBoundingBias;
  Vector2 _boundingBias;
  Uint32Array _delayInfo;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;

}; // end of class Geometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_GEOMETRY_H
