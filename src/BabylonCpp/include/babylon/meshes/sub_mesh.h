#ifndef BABYLON_MESHES_SUB_MESH_H
#define BABYLON_MESHES_SUB_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/culling/icullable.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/plane.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

class IntersectionInfo;
class WebGLDataBuffer;
FWD_STRUCT_SPTR(DrawWrapper)
FWD_STRUCT_SPTR(IMaterialContext)
FWD_STRUCT_SPTR(MaterialDefines)
FWD_CLASS_SPTR(SubMesh)
FWD_CLASS_SPTR(WebGLDataBuffer)

/** @hidden */
struct BABYLON_SHARED_EXPORT ICustomEffect {
  EffectPtr effect    = nullptr;
  std::string defines = "";
}; // end of struct ICustomEffect

/**
 * @brief Defines a subdivision inside a mesh.
 */
class BABYLON_SHARED_EXPORT SubMesh : public ICullable {

public:
  using TrianglePickingPredicate
    = std::function<bool(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Ray& ray)>;

public:
  template <typename... Ts>
  static SubMeshPtr New(Ts&&... args)
  {
    auto subMesh = std::shared_ptr<SubMesh>(new SubMesh(std::forward<Ts>(args)...));
    subMesh->addToMesh(subMesh);

    return subMesh;
  }
  ~SubMesh() override; // = default

  /**
   * @brief Hidden
   */
  DrawWrapperPtr _getDrawWrapper(const std::string& name, bool createIfNotExisting = false);

  /**
   * @brief Hidden
   */
  std::optional<ICustomEffect> _getCustomEffect(const std::string& name,
                                                bool createIfNotExisting = true);

  /**
   * @brief Hidden
   */
  void _removeCustomEffect(const std::string& name);

  /**
   * @brief Hidden
   */
  void _setMainDrawWrapperOverride(const DrawWrapperPtr& wrapper);

  /**
   * @brief Sets associated effect (effect used to render this submesh).
   * @param effect defines the effect to associate with
   * @param defines defines the set of defines used to compile this effect
   */
  void setEffect(const EffectPtr& effect, const MaterialDefinesPtr& defines = nullptr,
                 const IMaterialContextPtr& materialContext = nullptr);

  void addToMesh(const std::shared_ptr<SubMesh>& newSubMesh);
  bool isGlobal() const;

  /**
   * @brief Returns the submesh BoundingInfo object.
   * @returns current bounding info (or mesh's one if the submesh is global)
   */
  BoundingInfoPtr& getBoundingInfo();

  /**
   * @brief Sets the submesh BoundingInfo.
   * @returns The SubMesh.
   */
  SubMesh& setBoundingInfo(const BoundingInfo& boundingInfo);

  /**
   * @brief Returns the mesh of the current submesh.
   */
  AbstractMeshPtr& getMesh();

  /**
   * @brief Returns the rendering mesh of the submesh.
   */
  MeshPtr& getRenderingMesh();

  /**
   * @brief Returns the replacement mesh of the submesh.
   * @returns the replacement mesh (could be different from parent mesh)
   */
  AbstractMeshPtr getReplacementMesh() const;

  /**
   * @brief Returns the effective mesh of the submesh.
   * @returns the effective mesh (could be different from parent mesh)
   */
  AbstractMeshPtr getEffectiveMesh() const;

  /**
   * @brief Returns the submesh material.
   */
  MaterialPtr getMaterial();

  /** Methods **/

  /**
   * @brief Sets a new updated BoundingInfo object to the submesh.
   * @param data defines an optional position array to use to determine the bounding info
   * @returns the SubMesh
   */
  SubMesh& refreshBoundingInfo(const Float32Array& data = {});

  /**
   * @brief Hidden
   */
  bool _checkCollision(const Collider& collider);

  /**
   * @brief Updates the submesh BoundingInfo.
   * @returns The Submesh.
   */
  SubMesh& updateBoundingInfo(const Matrix& world);

  /**
   * @brief Returns if the submesh bounding box intersects the frustum defined by the passed array
   * of planes.
   */
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes, unsigned int strategy = 0) override;

  /**
   * @brief Returns if the submesh bounding box is completely inside the frustum defined by the
   * passed array of planes.
   */
  bool isCompletelyInFrustum(const std::array<Plane, 6>& frustumPlanes) override;

  /**
   * @brief Renders the submesh.
   * @returns The Submesh.
   */
  SubMesh& render(bool enableAlphaMode);

  /**
   * @brief Returns a new Index Buffer.
   * @returns The WebGLBuffer.
   */
  WebGLDataBufferPtr& _getLinesIndexBuffer(const IndicesArray& indices, Engine* engine);

  /**
   * @brief Returns if the passed Ray intersects the submesh bounding box.
   */
  bool canIntersects(const Ray& ray);

  /**
   * @brief Intersects current submesh with a ray.
   * @param ray defines the ray to test
   * @param positions defines mesh's positions array
   * @param indices defines mesh's indices array
   * @param fastCheck defines if the first intersection will be used (and not the closest)
   * @param trianglePredicate defines an optional predicate used to select faces when a mesh
   * intersection is detected
   * @returns intersection info or null if no intersection
   */
  std::optional<IntersectionInfo>
  intersects(Ray& ray, const std::vector<Vector3>& positions, const IndicesArray& indices,
             bool fastCheck = false, const TrianglePickingPredicate& trianglePredicate = nullptr);

  /**
   * @brief Hidden
   */
  void _rebuild();

  /** Clone **/

  /**
   * @brief Creates a new Submesh from the passed Mesh.
   */
  SubMeshPtr clone(const AbstractMeshPtr& newMesh, const MeshPtr& newRenderingMesh);

  /** Dispose **/

  /**
   * @brief Disposes the Submesh.
   */
  void dispose();

  /**
   * @brief Gets the class name
   * @returns the string "SubMesh".
   */
  std::string getClassName() const;

  /** Statics **/

  static SubMeshPtr AddToMesh(unsigned int materialIndex, unsigned int verticesStart,
                              size_t verticesCount, unsigned int indexStart, size_t indexCount,
                              const AbstractMeshPtr& mesh, const MeshPtr& renderingMesh = nullptr,
                              bool createBoundingBox = true);

  /**
   * @brief Creates a new Submesh from the passed parameters.
   * @param materialIndex (integer) : the index of the main mesh material.
   * @param startIndex (integer) : the index where to start the copy in the mesh indices array.
   * @param indexCount (integer) : the number of indices to copy then from the startIndex.
   * @param mesh (Mesh) : the main mesh to create the submesh from.
   * @param renderingMesh (optional Mesh) : rendering mesh.
   * @return The created SubMesh object.
   */
  static SubMeshPtr CreateFromIndices(unsigned int materialIndex, unsigned int startIndex,
                                      size_t indexCount, const AbstractMeshPtr& mesh,
                                      const MeshPtr& renderingMesh = nullptr);

protected:
  /**
   * @brief Creates a new submesh.
   * @param materialIndex defines the material index to use
   * @param verticesStart defines vertex index start
   * @param verticesCount defines vertices count
   * @param indexStart defines index start
   * @param indexCount defines indices count
   * @param mesh defines the parent mesh
   * @param renderingMesh defines an optional rendering mesh
   * @param createBoundingBox defines if bounding box should be created for this submesh
   * @param addToMesh defines a boolean indicating that the submesh must be added to the
   * mesh.subMeshes array (true by default)
   */
  SubMesh(unsigned int materialIndex, unsigned int verticesStart, size_t verticesCount,
          unsigned int indexStart, size_t indexCount, const AbstractMeshPtr& mesh,
          const MeshPtr& renderingMesh = nullptr, bool createBoundingBox = true,
          bool addToMesh = true);

  /**
   * @brief Gets material defines used by the effect associated to the sub mesh.
   */
  MaterialDefinesPtr& get_materialDefines();

  /**
   * @brief Sets material defines used by the effect associated to the sub mesh.
   */
  void set_materialDefines(const MaterialDefinesPtr& defines);

  /**
   * @brief Gets associated (main) effect (possibly the effect override if defined)
   */
  EffectPtr& get_effect();

  /**
   * @brief Hidden
   */
  DrawWrapperPtr& get__drawWrapper();

private:
  /** @hidden */
  bool _IsMultiMaterial(const Material& material) const;
  /** @hidden */
  std::optional<IntersectionInfo> _intersectLines(Ray& ray, const std::vector<Vector3>& positions,
                                                  const IndicesArray& indices,
                                                  float intersectionThreshold,
                                                  bool fastCheck = false);
  /** @hidden */
  std::optional<IntersectionInfo> _intersectUnIndexedLines(Ray& ray,
                                                           const std::vector<Vector3>& positions,
                                                           const IndicesArray& indices,
                                                           float intersectionThreshold,
                                                           bool fastCheck = false);
  /** @hidden */
  std::optional<IntersectionInfo>
  _intersectTriangles(Ray& ray, const std::vector<Vector3>& positions, const IndicesArray& indices,
                      unsigned int step, bool checkStopper, bool fastCheck = false,
                      const TrianglePickingPredicate& trianglePredicate = nullptr);
  /** @hidden */
  std::optional<IntersectionInfo>
  _intersectUnIndexedTriangles(Ray& ray, const std::vector<Vector3>& positions,
                               const IndicesArray& indices, bool fastCheck = false,
                               const TrianglePickingPredicate& trianglePredicate = nullptr);

public:
  /** @hidden */
  MaterialDefinesPtr _materialDefines;
  /** @hidden */
  EffectPtr _materialEffect;
  /** @hidden */
  EffectPtr _effectOverride;

  std::unordered_map<std::string, ICustomEffect> _customEffects;

  /**
   * Gets or sets material defines used by the effect associated to the sub mesh
   */
  Property<SubMesh, MaterialDefinesPtr> materialDefines;

  /**
   * Gets associated effect
   */
  ReadOnlyProperty<SubMesh, EffectPtr> effect;

  /**
   * Hidden
   */
  ReadOnlyProperty<SubMesh, DrawWrapperPtr> _drawWrapper;

  DrawWrapperPtr _drawWrapperOverride;

  /** the material index to use */
  unsigned int materialIndex;
  /** vertex index start */
  unsigned int verticesStart;
  /** vertices count */
  size_t verticesCount;
  /** index start */
  unsigned int indexStart;
  /** indices count */
  size_t indexCount;
  bool createBoundingBox;
  size_t _linesIndexCount;
  /** @hidden */
  std::vector<Vector3> _lastColliderWorldVertices;
  /** @hidden */
  std::vector<Plane> _trianglePlanes;
  /** @hidden */
  std::unique_ptr<Matrix> _lastColliderTransformMatrix;
  /** @hidden */
  int _renderId;
  /** @hidden */
  int _alphaIndex;
  /** @hidden */
  float _distanceToCamera;
  /** @hidden */
  size_t _id;

private:
  std::unordered_map<std::string, DrawWrapperPtr> _drawWrappers;
  DrawWrapperPtr
    _mainDrawWrapper; // same thing than _drawWrappers[Constants.SUBMESHEFFECT_MAINMATERIAL] but
                      // faster access
  DrawWrapperPtr _mainDrawWrapperOverride;
  AbstractMeshPtr _mesh;
  MeshPtr _renderingMesh;
  BoundingInfoPtr _boundingInfo;
  WebGLDataBufferPtr _linesIndexBuffer;
  MaterialPtr _currentMaterial;
  bool _addToMesh;

}; // end of class SubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_SUB_MESH_H
