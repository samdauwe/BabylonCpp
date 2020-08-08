#ifndef BABYLON_PARTICLES_SOLID_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_SOLID_PARTICLE_SYSTEM_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/maths/matrix.h>
#include <babylon/particles/solid_particle.h>

namespace BABYLON {

struct DepthSortedParticle;
class Material;
class Mesh;
class MultiMaterial;
class PickingInfo;
class SolidParticleSystem;
class SolidParticleVertex;
class Scene;
class TargetCamera;
using BoundingInfoPtr        = std::shared_ptr<BoundingInfo>;
using MaterialPtr            = std::shared_ptr<Material>;
using MeshPtr                = std::shared_ptr<Mesh>;
using ModelShapePtr          = std::shared_ptr<ModelShape>;
using MultiMaterialPtr       = std::shared_ptr<MultiMaterial>;
using TargetCameraPtr        = std::shared_ptr<TargetCamera>;
using SolidParticlePtr       = std::shared_ptr<SolidParticle>;
using SolidParticleSystemPtr = std::shared_ptr<SolidParticleSystem>;

struct SolidParticleSystemOptions {
  std::optional<bool> updatable            = std::nullopt;
  bool isPickable                          = false;
  bool enableDepthSort                     = false;
  bool particleIntersection                = false;
  bool boundingSphereOnly                  = false;
  std::optional<float> bSphereRadiusFactor = std::nullopt;
  bool expandable                          = false;
  bool useModelMaterial                    = false;
  bool enableMultiMaterial                 = false;
}; // end of struct SolidParticleSystemOptions

struct SolidParticleSystemDigestOptions {
  size_t facetNb                                       = 1;
  size_t number                                        = 0;
  int delta                                            = 0;
  std::optional<std::vector<SolidParticlePtr>> storage = std::nullopt;
}; // end of struct SolidParticleSystemOptions

struct SolidParticleSystemMeshBuilderOptions {
  std::function<void(SolidParticle* particle, size_t i, size_t s)> positionFunction = nullptr;
  std::function<void(SolidParticle* particle,
                     const std::variant<Vector3, SolidParticleVertex>& vertex, size_t i)>
    vertexFunction                                     = nullptr;
  std::optional<std::vector<SolidParticlePtr>> storage = std::nullopt;
}; // end of struct SolidParticleSystemMeshBuilderOptions

/**
 * @brief The SPS is a single updatable mesh. The solid particles are simply separate parts or faces
 * fo this big mesh. As it is just a mesh, the SPS has all the same properties than any other BJS
 * mesh : not more, not less. It can be scaled, rotated, translated, enlighted, textured, moved,
 * etc. The SPS is also a particle system. It provides some methods to manage the particles. However
 * it is behavior agnostic. This means it has no emitter, no particle physics, no particle recycler.
 * You have to implement your own behavior.
 *
 * Full documentation here : https://doc.babylonjs.com/how_to/Solid_Particle_System
 */
class BABYLON_SHARED_EXPORT SolidParticleSystem : public IDisposable {

public:
  template <typename... Ts>
  static SolidParticleSystemPtr New(Ts&&... args)
  {
    return std::shared_ptr<SolidParticleSystem>(new SolidParticleSystem(std::forward<Ts>(args)...));
  }
  ~SolidParticleSystem() override; // = default

  /**
   * @brief Builds the SPS underlying mesh. Returns a standard Mesh.
   * If no model shape was added to the SPS, the returned mesh is just a single triangular plane.
   * @returns the created mesh
   */
  MeshPtr buildMesh();

  /**
   * @brief Digests the mesh and generates as many solid particles in the system as wanted. Returns
   * the SPS. These particles will have the same geometry than the mesh parts and will be positioned
   * at the same localisation than the mesh original places. Thus the particles generated from
   * `digest()` have their property `position` set yet.
   * @param mesh ( Mesh ) is the mesh to be digested
   * @param options {facetNb} (optional integer, default 1) is the number of mesh facets per
   * particle, this parameter is overriden by the parameter `number` if any {delta} (optional
   * integer, default 0) is the random extra number of facets per particle , each particle will have
   * between `facetNb` and `facetNb + delta` facets {number} (optional positive integer) is the
   * wanted number of particles : each particle is built with `mesh_total_facets / number` facets
   * {storage} (optional existing array) is an array where the particles will be stored for a
   * further use instead of being inserted in the SPS.
   * @returns the current SPS
   */
  SolidParticleSystem& digest(Mesh* mesh, std::optional<SolidParticleSystemDigestOptions>& options);

  /**
   * @brief Adds some particles to the SPS from the model shape. Returns the shape id.
   * Please read the doc :
   * https://doc.babylonjs.com/how_to/Solid_Particle_System#create-an-immutable-sps
   * @param mesh is any Mesh object that will be used as a model for the solid particles.
   * @param nb (positive integer) the number of particles to be created from this model
   * @param options {positionFunction} is an optional javascript function to called for each
   * particle on SPS creation. {vertexFunction} is an optional javascript function to called for
   * each vertex of each particle on SPS creation {storage} (optional existing array) is an array
   * where the particles will be stored for a further use instead of being inserted in the SPS.
   * @returns the number of shapes in the system
   */
  int addShape(const MeshPtr& mesh, size_t nb, SolidParticleSystemMeshBuilderOptions& options);

  /**
   * @brief Adds some particles to the SPS from the model shape. Returns the shape id.
   * Please read the doc :
   * https://doc.babylonjs.com/how_to/Solid_Particle_System#create-an-immutable-sps
   * @param mesh is any Mesh object that will be used as a model for the solid particles.
   * @param nb (positive integer) the number of particles to be created from this model
   * @param options {positionFunction} is an optional javascript function to called for each
   * particle on SPS creation. {vertexFunction} is an optional javascript function to called for
   * each vertex of each particle on SPS creation {storage} (optional existing array) is an array
   * where the particles will be stored for a further use instead of being inserted in the SPS.
   * @returns the number of shapes in the system
   */
  int addShape(const MeshPtr& mesh, size_t nb,
               std::optional<SolidParticleSystemMeshBuilderOptions>& options);

  /**
   * @brief Rebuilds the whole mesh and updates the VBO : custom positions and vertices are
   * recomputed if needed.
   * @param reset boolean, default false : if the particles must be reset at position and rotation
   * zero, scaling 1, color white, initial UVs and not parented.
   * @returns the SPS.
   */
  SolidParticleSystem& rebuildMesh(bool reset = false);

  /**
   * @brief Removes the particles from the start-th to the end-th included from an expandable SPS
   * (required). Returns an array with the removed particles. If the number of particles to remove
   * is lower than zero or greater than the global remaining particle number, then an empty array is
   * returned. The SPS can't be empty so at least one particle needs to remain in place. Under the
   * hood, the VertexData array, so the VBO buffer, is recreated each call.
   * @param start index of the first particle to remove
   * @param end index of the last particle to remove (included)
   * @returns an array populated with the removed particles
   */
  std::vector<SolidParticlePtr> removeParticles(size_t start, size_t end);

  /**
   * @brief Inserts some pre-created particles in the solid particle system so that they can be
   * managed by setParticles().
   * @param solidParticleArray an array populated with Solid Particles objects
   * @returns the SPS
   */
  SolidParticleSystem&
  insertParticlesFromArray(const std::vector<SolidParticlePtr>& solidParticleArray);

  /**
   * @brief Sets all the particles : this method actually really updates the mesh according to the
   * particle positions, rotations, colors, textures, etc. This method calls `updateParticle()` for
   * each particle of the SPS. For an animated SPS, it is usually called within the render loop.
   * This methods does nothing if called on a non updatable or not yet built SPS. Example :
   * buildMesh() not called after having added or removed particles from an expandable SPS.
   * @param start The particle index in the particle array where to start to compute the particle
   * property values _(default 0)_
   * @param end The particle index in the particle array where to stop to compute the particle
   * property values _(default nbParticle - 1)_
   * @param update If the mesh must be finally updated on this call after all the particle
   * computations _(default true)_
   * @returns the SPS.
   */
  SolidParticleSystem& setParticles(size_t start = 0, size_t end = 0, bool update = true);

  /**
   * @brief Disposes the SPS.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Returns an object {idx: numbern faceId: number} for the picked particle from the passed
   * pickingInfo object. idx is the particle index in the SPS faceId is the picked face index
   * counted within this particle. Returns null if the pickInfo can't identify a picked particle.
   * @param pickingInfo (PickingInfo object)
   * @returns {idx: number, faceId: number} or null
   */
  std::optional<PickedParticle> pickedParticle(const PickingInfo& pickingInfo);

  /**
   * @brief Returns a SolidParticle object from its identifier : particle.id
   * @param id (integer) the particle Id
   * @returns the searched particle or null if not found in the SPS.
   */
  SolidParticlePtr getParticleById(size_t id);

  /**
   * @brief Returns a new array populated with the particles having the passed shapeId.
   * @param shapeId (integer) the shape identifier
   * @returns a new solid particle array
   */
  std::vector<SolidParticlePtr> getParticlesByShapeId(int shapeId);

  /**
   * @brief Populates the passed array "ref" with the particles having the passed shapeId.
   * @param shapeId the shape identifier
   * @returns the SPS
   * @param ref
   */
  SolidParticleSystem& getParticlesByShapeIdToRef(int shapeId, std::vector<SolidParticlePtr>& ref);

  /**
   * @brief Computes the required SubMeshes according the materials assigned to the particles.
   * @returns the solid particle system.
   * Does nothing if called before the SPS mesh is built.
   */
  SolidParticleSystem& computeSubMeshes();

  /**
   * @brief Visibilty helper : Recomputes the visible size according to the mesh
   * bounding box
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   * @returns the SPS.
   */
  SolidParticleSystem& refreshVisibleSize();

  /**
   * @brief Visibility helper : Sets the size of a visibility box, this sets the underlying mesh
   * bounding box.
   * @param size the size (float) of the visibility box
   * note : this doesn't lock the SPS mesh bounding box.
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  void setVisibilityBox(float size);

  /**
   * @brief Gets whether the SPS as always visible or not.
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  bool isAlwaysVisible() const;

  /**
   * @brief Sets the SPS as always visible or not.
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  void setIsAlwaysVisible(bool val);

  /**
   * @brief Sets the SPS visibility box as locked or not. This enables/disables the underlying mesh
   * bounding box updates.
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  void setIsVisibilityBoxLocked(bool val);

  /**
   * @brief Sets the SPS visibility box as locked or not. This enables/disables the underlying mesh
   * bounding box updates.
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  bool isVisibilityBoxLocked() const;

  /**
   * @brief Sets the SPS MultiMaterial from the passed materials.
   * Note : the passed array is internally copied and not used then by reference.
   * @param materials an array of material objects. This array indexes are the materialIndex values
   * of the particles.
   */
  void setMultiMaterial(const std::vector<MaterialPtr>& materials);

  // =======================================================================
  // Particle behavior logic
  // these following methods may be overwritten by the user to fit his needs

  /**
   * @brief This function does nothing. It may be overwritten to set all the particle first values.
   * The SPS doesn't call this function, you may have to call it by your own.
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#particle-management
   */
  virtual void initParticles();

  /**
   * @brief This function does nothing. It may be overwritten to recycle a particle.
   * The SPS doesn't call this function, you may have to call it by your own.
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#particle-management
   * @param particle The particle to recycle
   * @returns the recycled particle
   */
  virtual SolidParticle* recycleParticle(SolidParticle* particle);

  /**
   * @brief Updates a particle : this function should  be overwritten by the user.
   * It is called on each particle by `setParticles()`. This is the place to code each particle
   * behavior. doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#particle-management
   * @example : just set a particle position or velocity and recycle conditions
   * @param particle The particle to update
   * @returns the updated particle
   */
  virtual SolidParticle* updateParticle(SolidParticle* particle);

  /**
   * @brief Updates a vertex of a particle : it can be overwritten by the user.
   * This will be called on each vertex particle by `setParticles()` if `computeParticleVertex` is
   * set to true only.
   * @param particle the current particle
   * @param vertex the current vertex of the current particle : a SolidParticleVertex object
   * @param pt the index of the current vertex in the particle shape
   * doc : https://doc.babylonjs.com/how_to/Solid_Particle_System#update-each-particle-shape
   * @example : just set a vertex particle position or color
   * @returns the sps
   */
  virtual SolidParticleSystem& updateParticleVertex(SolidParticle* particle,
                                                    const SolidParticleVertex& vertex, size_t pt);

  /**
   * @brief This will be called before any other treatment by `setParticles()` and will be passed
   * three parameters. This does nothing and may be overwritten by the user.
   * @param start the particle index in the particle array where to stop to iterate, same than the
   * value passed to setParticle()
   * @param stop the particle index in the particle array where to stop to iterate, same than the
   * value passed to setParticle()
   * @param update the boolean update value actually passed to setParticles()
   */
  virtual void beforeUpdateParticles(size_t start, size_t stop, bool update);

  /**
   * @brief This will be called  by `setParticles()` after all the other treatments and just before
   * the actual mesh update. This will be passed three parameters. This does nothing and may be
   * overwritten by the user.
   * @param start the particle index in the particle array where to stop to iterate, same than the
   * value passed to setParticle()
   * @param stop the particle index in the particle array where to stop to iterate, same than the
   * value passed to setParticle()
   * @param update the boolean update value actually passed to setParticles()
   */
  virtual void afterUpdateParticles(size_t start, size_t stop, bool update);

protected:
  /**
   * @brief Creates a SPS (Solid Particle System) object.
   * @param name (String) is the SPS name, this will be the underlying mesh name.
   * @param scene (Scene) is the scene in which the SPS is added.
   * @param options defines the options of the sps e.g.
   * * updatable (optional boolean, default true) : if the SPS must be updatable or immutable.
   * * isPickable (optional boolean, default false) : if the solid particles must be pickable.
   * * enableDepthSort (optional boolean, default false) : if the solid particles must be sorted in
   * the geometry according to their distance to the camera.
   * * useModelMaterial (optional boolean, defaut false) : if the model materials must be used to
   * create the SPS multimaterial. This enables the multimaterial supports of the SPS.
   * * enableMultiMaterial (optional boolean, default false) : if the solid particles can be given
   * different materials.
   * * expandable (optional boolean, default false) : if particles can still be added after the
   * initial SPS mesh creation.
   * * particleIntersection (optional boolean, default false) : if the solid particle intersections
   * must be computed.
   * * boundingSphereOnly (optional boolean, default false) : if the particle intersection must be
   * computed only with the bounding sphere (no bounding box computation, so faster).
   * * bSphereRadiusFactor (optional float, default 1.0) : a number to multiply the boundind sphere
   * radius by in order to reduce it for instance.
   * @example bSphereRadiusFactor = 1.0 / Math.sqrt(3.0) => the bounding sphere exactly matches a
   * spherical mesh.
   */
  SolidParticleSystem(const std::string& name, Scene* scene,
                      const std::optional<SolidParticleSystemOptions>& options = std::nullopt);

  /**
   * @brief Tells to `setParticles()` to compute the particle rotations or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle rotations aren't stored values, so setting `computeParticleRotation` to
   * false will prevents the particle to rotate.
   */
  void set_computeParticleRotation(bool val);

  /**
   * @brief Tells to `setParticles()` to compute the particle colors or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle colors are stored values, so setting `computeParticleColor` to false will
   * keep yet the last colors set.
   */
  void set_computeParticleColor(bool val);

  /**
   * @brief Tells to `setParticles()` to compute the particle texture or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle colors are stored values, so setting `computeParticleTexture` to false will
   * keep yet the last texture set.
   */
  void set_computeParticleTexture(bool val);

  /**
   * @brief Tells to `setParticles()` to call the vertex function for each vertex of each particle,
   * or not. Default value : false. The SPS is faster when it's set to false. Note : the particle
   * custom vertex positions aren't stored values.
   */
  void set_computeParticleVertex(bool val);

  /**
   * @brief Tells to `setParticles()` to compute or not the mesh bounding box when computing the
   * particle positions.
   */
  void set_computeBoundingBox(bool val);

  /**
   * @brief Tells to `setParticles()` to sort or not the distance between each particle and the
   * camera. Skipped when `enableDepthSort` is set to `false` (default) at construction time.
   * Default : `true`
   */
  void set_depthSortParticles(bool val);

  /**
   * @brief Gets if `setParticles()` computes the particle rotations or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle rotations aren't stored values, so setting `computeParticleRotation` to
   * false will prevents the particle to rotate.
   */
  bool get_computeParticleRotation() const;

  /**
   * @brief Gets if `setParticles()` computes the particle colors or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle colors are stored values, so setting `computeParticleColor` to false will
   * keep yet the last colors set.
   */
  bool get_computeParticleColor() const;

  /**
   * @brief Gets if `setParticles()` computes the particle textures or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle textures are stored values, so setting `computeParticleTexture` to false
   * will keep yet the last colors set.
   */
  bool get_computeParticleTexture() const;

  /**
   * @brief Gets if `setParticles()` calls the vertex function for each vertex of each particle, or
   * not. Default value : false. The SPS is faster when it's set to false. Note : the particle
   * custom vertex positions aren't stored values.
   */
  bool get_computeParticleVertex() const;

  /**
   * @brief Gets if `setParticles()` computes or not the mesh bounding box when computing the
   * particle positions.
   */
  bool get_computeBoundingBox() const;

  /**
   * @brief Gets if `setParticles()` sorts or not the distance between each particle and the camera.
   * Skipped when `enableDepthSort` is set to `false` (default) at construction time.
   * Default : `true`
   */
  bool get_depthSortParticles() const;

  /**
   * @brief Gets if the SPS is created as expandable at construction time.
   * Default : `false`
   */
  bool get_expandable() const;

  /**
   * @brief Gets if the SPS supports the Multi Materials
   */
  bool get_multimaterialEnabled() const;

  /**
   * @brief Gets if the SPS uses the model materials for its own multimaterial.
   */
  bool get_useModelMaterial() const;

  /**
   * @brief The SPS used material array.
   */
  std::vector<MaterialPtr>& get_materials();

  /**
   * @brief Gets the SPS computed multimaterial object.
   */
  MultiMaterialPtr& get_multimaterial();

  /**
   * @brief Sets the SPS computed multimaterial object.
   */
  void set_multimaterial(const MultiMaterialPtr& mm);

  /**
   * @brief Gets if the subMeshes must be updated on the next call to setParticles().
   */
  bool get_autoUpdateSubMeshes() const;

  /**
   * @brief Sets if the subMeshes must be updated on the next call to setParticles().
   */
  void set_autoUpdateSubMeshes(bool val);

private:
  /**
   * @brief Unrotate the fixed normals in case the mesh was built with pre-rotated particles, ex :
   * use of positionFunction in addShape()
   * @hidden
   */
  void _unrotateFixedNormals();

  /**
   * @brief Resets the temporary working copy particle.
   * @hidden
   */
  void _resetCopy();

  /**
   * @brief Inserts the shape model geometry in the global SPS mesh by updating the positions,
   * indices, normals, colors, uvs arrays
   * @param p the current index in the positions array to be updated
   * @param ind the current index in the indices array
   * @param shape a Vector3 array, the shape geometry
   * @param positions the positions array to be updated
   * @param meshInd the shape indices array
   * @param indices the indices array to be updated
   * @param meshUV the shape uv array
   * @param uvs the uv array to be updated
   * @param meshCol the shape color array
   * @param colors the color array to be updated
   * @param meshNor the shape normals array
   * @param normals the normals array to be updated
   * @param idx the particle indexs
   * @param idxInShape the particle index in its shape
   * @param options the addShape() method  passed options
   * @model the particle model
   * @hidden
   */
  SolidParticle* _meshBuilder(size_t p, size_t ind, const std::vector<Vector3>& shape,
                              Float32Array& positions, const Uint32Array& meshInd,
                              Uint32Array& indices, const Float32Array& meshUV, Float32Array& uvs,
                              const Float32Array& meshCol, Float32Array& colors,
                              const Float32Array& meshNor, Float32Array& normals, size_t idx,
                              size_t idxInShape,
                              const std::optional<SolidParticleSystemMeshBuilderOptions>& options,
                              const ModelShapePtr& model);

  /**
   * @brief Returns a shape Vector3 array from positions float array.
   * @param positions float array
   * @returns a vector3 array
   * @hidden
   */
  std::vector<Vector3> _posToShape(const Float32Array& positions);

  /**
   * @brief Returns a shapeUV array from a float uvs (array deep copy).
   * @param uvs as a float array
   * @returns a shapeUV array
   * @hidden
   */
  Float32Array _uvsToShapeUV(const Float32Array& uvs);

  /**
   * @brief Adds a new particle object in the particles array.
   * @param idx particle index in particles array
   * @param id particle id
   * @param idxpos positionIndex : the starting index of the particle vertices in the SPS
   * "positions" array
   * @param idxind indiceIndex : he starting index of the particle indices in the SPS "indices"
   * array
   * @param model particle ModelShape object
   * @param shapeId model shape identifier
   * @param idxInShape index of the particle in the current model
   * @param bInfo model bounding info object
   * @hidden
   */
  SolidParticlePtr _addParticle(size_t idx, size_t id, size_t idxpos, size_t idxind,
                                const ModelShapePtr& model, int shapeId, size_t idxInShape,
                                const BoundingInfo& bInfo);

  /**
   * @brief Adds a new particle object in the particles array.
   * @param idx particle index in particles array
   * @param id particle id
   * @param idxpos positionIndex : the starting index of the particle vertices in the SPS
   * "positions" array
   * @param idxind indiceIndex : he starting index of the particle indices in the SPS "indices"
   * array
   * @param model particle ModelShape object
   * @param shapeId model shape identifier
   * @param idxInShape index of the particle in the current model
   * @param bInfo model bounding info object
   * @param storage target storage array, if any
   * @hidden
   */
  SolidParticlePtr _addParticle(size_t idx, size_t id, size_t idxpos, size_t idxind,
                                const ModelShapePtr& model, int shapeId, size_t idxInShape,
                                const BoundingInfo& bInfo, std::vector<SolidParticlePtr>& storage);

  /**
   * @brief Rebuilds a particle back to its just built status : if needed, recomputes the custom
   * positions and vertices
   * @hidden
   */
  void _rebuildParticle(SolidParticle* particle, bool reset = false);

  /**
   * @brief Creates a new particle and modifies the SPS mesh geometry :
   * - calls _meshBuilder() to increase the SPS mesh geometry step by step
   * - calls _addParticle() to populate the particle array
   * factorized code from addShape() and insertParticlesFromArray()
   * @param idx particle index in the particles array
   * @param i particle index in its shape
   * @param modelShape particle ModelShape object
   * @param shape shape vertex array
   * @param meshInd shape indices array
   * @param meshUV shape uv array
   * @param meshCol shape color array
   * @param meshNor shape normals array
   * @param bbInfo shape bounding info
   * @param storage target particle storage
   * @param options addShape() passed options
   * @hidden
   */
  SolidParticlePtr _insertNewParticle(
    size_t idx, size_t i, const ModelShapePtr& modelShape, const std::vector<Vector3>& shape,
    const IndicesArray& meshInd, const Float32Array& meshUV, const Float32Array& meshCol,
    const Float32Array& meshNor, const BoundingInfoPtr& bbInfo,
    std::optional<std::vector<SolidParticlePtr>>& storage,
    const std::optional<SolidParticleSystemMeshBuilderOptions>& options = std::nullopt);

  /**
   * @brief Sorts the solid particles by material when MultiMaterial is enabled.
   * Updates the indices32 array.
   * Updates the indicesByMaterial array.
   * Updates the mesh indices array.
   * @returns the SPS
   * @hidden
   */
  SolidParticleSystem& _sortParticlesByMaterial();

  /**
   * @brief Sets the material indexes by id materialIndexesById[id] = materialIndex.
   * @hidden
   */
  void _setMaterialIndexesById();

  /**
   * @brief Returns an array with unique values of Materials from the passed array.
   * @param array the material array to be checked and filtered
   * @hidden
   */
  std::vector<MaterialPtr> _filterUniqueMaterialId(const std::vector<MaterialPtr>& array);

  /**
   * @brief Sets a new Standard Material as _defaultMaterial if not already set.
   * @hidden
   */
  MaterialPtr _setDefaultMaterial();

public:
  /**
   * The SPS array of Solid Particle objects. Just access each particle as with any classic array.
   *  Example : var p = SPS.particles[i];
   */
  std::vector<SolidParticlePtr> particles;

  /**
   * The SPS total number of particles. Read only. Use SPS.counter instead if you need to set your
   * own value.
   */
  size_t nbParticles;

  /**
   * If the particles must ever face the camera (default false). Useful for planar particles.
   */
  bool billboard;

  /**
   * Recompute normals when adding a shape
   */
  bool recomputeNormals;

  /**
   * This a counter ofr your own usage. It's not set by any SPS functions.
   */
  int counter;

  /**
   * The SPS name. This name is also given to the underlying mesh.
   */
  std::string name;

  /**
   * The SPS mesh. It's a standard BJS Mesh, so all the methods from the Mesh class are avalaible.
   */
  MeshPtr mesh;

  /**
   * This array is populated when the SPS is set as 'pickable'.
   * Each key of this array is a `faceId` value that you can get from a pickResult object.
   * Each element of this array is an object `{idx: int, faceId: int}`.
   * `idx` is the picked particle index in the `SPS.particles` array
   * `faceId` is the picked face index counted within this particle.
   * This array is the first element of the pickedBySubMesh array : sps.pickBySubMesh[0].
   * It's not pertinent to use it when using a SPS with the support for MultiMaterial enabled.
   * Use the method SPS.pickedParticle(pickingInfo) instead.
   * Please read : https://doc.babylonjs.com/how_to/Solid_Particle_System#pickable-particles
   */
  std::vector<PickedParticle> pickedParticles;

  /**
   * This array is populated when the SPS is set as 'pickable'
   * Each key of this array is a submesh index.
   * Each element of this array is a second array defined like this :
   * Each key of this second array is a `faceId` value that you can get from a pickResult object.
   * Each element of this second array is an object `{idx: int, faceId: int}`.
   * `idx` is the picked particle index in the `SPS.particles` array
   * `faceId` is the picked face index counted within this particle.
   * It's better to use the method SPS.pickedParticle(pickingInfo) rather than using directly this
   * array. Please read : https://doc.babylonjs.com/how_to/Solid_Particle_System#pickable-particles
   */
  std::vector<std::vector<PickedParticle>> pickedBySubMesh;

  /**
   * This array is populated when `enableDepthSort` is set to true.
   * Each element of this array is an instance of the class DepthSortedParticle.
   */
  std::vector<DepthSortedParticle> depthSortedParticles;

  /**
   * If the particle intersection must be computed only with the bounding sphere (no bounding box
   * computation, so faster). (Internal use only)
   * @hidden
   */
  bool _bSphereOnly;

  /**
   * A number to multiply the boundind sphere radius by in order to reduce it for instance.
   * (Internal use only)
   * @hidden
   */
  float _bSphereRadiusFactor;

  /**
   * Tells to `setParticles()` to compute the particle rotations or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle rotations aren't stored values, so setting `computeParticleRotation` to
   * false will prevents the particle to rotate.
   */
  Property<SolidParticleSystem, bool> computeParticleRotation;

  /**
   * Tells to `setParticles()` to compute the particle colors or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle colors are stored values, so setting `computeParticleColor` to false will
   * keep yet the last colors set.
   */
  Property<SolidParticleSystem, bool> computeParticleColor;

  /**
   * Tells to `setParticles()` to compute the particle texture or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle colors are stored values, so setting `computeParticleTexture` to false will
   * keep yet the last texture set.
   */
  Property<SolidParticleSystem, bool> computeParticleTexture;

  /**
   * Tells to `setParticles()` to call the vertex function for each vertex of each particle,
   * or not. Default value : false. The SPS is faster when it's set to false. Note : the particle
   * custom vertex positions aren't stored values.
   */
  Property<SolidParticleSystem, bool> computeParticleVertex;

  /**
   * Tells to `setParticles()` to compute or not the mesh bounding box when computing the
   * particle positions.
   */
  Property<SolidParticleSystem, bool> computeBoundingBox;

  /**
   * Tells to `setParticles()` to sort or not the distance between each particle and the camera.
   * Skipped when `enableDepthSort` is set to `false` (default) at construction time. Default :
   * `true`
   */
  Property<SolidParticleSystem, bool> depthSortParticles;

  /**
   * Gets if the SPS is created as expandable at construction time.
   * Default : `false`
   */
  ReadOnlyProperty<SolidParticleSystem, bool> expandable;

  /**
   * Gets if the SPS supports the Multi Materials
   */
  ReadOnlyProperty<SolidParticleSystem, bool> multimaterialEnabled;

  /**
   * Gets if the SPS uses the model materials for its own multimaterial.
   */
  ReadOnlyProperty<SolidParticleSystem, bool> useModelMaterial;

  /**
   * The SPS used material array.
   */
  ReadOnlyProperty<SolidParticleSystem, std::vector<MaterialPtr>> materials;

  /**
   * Gets or sets the SPS computed multimaterial object.
   */
  Property<SolidParticleSystem, MultiMaterialPtr> multimaterial;

  /**
   * Gets or sets if the subMeshes must be updated on the next call to setParticles().
   */
  Property<SolidParticleSystem, bool> autoUpdateSubMeshes;

private:
  // members
  Scene* _scene;
  Float32Array _positions;
  Uint32Array _indices;
  Float32Array _normals;
  Float32Array _colors;
  Float32Array _uvs;
  // used as depth sorted array if depth sort enabled, else used as typed
  // indices
  IndicesArray _indices32;
  Float32Array _positions32;   // updated positions for the VBO
  Float32Array _normals32;     // updated normals for the VBO
  Float32Array _fixedNormal32; // initial normal references
  Float32Array _colors32;
  Float32Array _uvs32;
  size_t _index; // indices index
  bool _updatable;
  bool _pickable;
  bool _isVisibilityBoxLocked;
  bool _alwaysVisible;
  bool _depthSort;
  bool _expandable;
  int _shapeCounter;
  std::unique_ptr<SolidParticle> _copy;
  std::unique_ptr<Color4> _color;
  bool _computeParticleColor;
  bool _computeParticleTexture;
  bool _computeParticleRotation;
  bool _computeParticleVertex;
  bool _computeBoundingBox;
  bool _depthSortParticles;
  TargetCameraPtr _camera;
  bool _mustUnrotateFixedNormals;
  Vector3 _scale;
  Vector3 _translation;
  bool _particlesIntersect;
  bool _needs32Bits;
  bool _isNotBuilt;
  size_t _lastParticleId;
  std::vector<size_t> _idxOfId; // array : key = particle.id / value = particle.idx
  bool _multimaterialEnabled;
  bool _useModelMaterial;
  std::vector<size_t> _indicesByMaterial;
  std::vector<size_t> _materialIndexes;
  std::function<bool(const DepthSortedParticle& p1, const DepthSortedParticle& p2)>
    _depthSortFunction;
  std::function<bool(const DepthSortedParticle& p1, const DepthSortedParticle& p2)>
    _materialSortFunction;
  std::vector<MaterialPtr> _materials;
  MultiMaterialPtr _multimaterial;
  std::unordered_map<size_t, size_t> _materialIndexesById;
  MaterialPtr _defaultMaterial;
  bool _autoUpdateSubMeshes;
  std::unique_ptr<SolidParticleVertex> _tmpVertex;

}; // end of class SolidParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SOLID_PARTICLE_SYSTEM_H
