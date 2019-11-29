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
class Mesh;
class SolidParticleSystem;
class Scene;
class TargetCamera;
using MeshPtr                = std::shared_ptr<Mesh>;
using TargetCameraPtr        = std::shared_ptr<TargetCamera>;
using SolidParticleSystemPtr = std::shared_ptr<SolidParticleSystem>;

struct SolidParticleSystemOptions {
  std::optional<bool> updatable            = std::nullopt;
  bool isPickable                          = false;
  bool enableDepthSort                     = false;
  bool particleIntersection                = false;
  bool boundingSphereOnly                  = false;
  std::optional<float> bSphereRadiusFactor = std::nullopt;
}; // end of struct SolidParticleSystemOptions

struct SolidParticleSystemDigestOptions {
  size_t facetNb = 1;
  size_t number  = 0;
  int delta      = 0;
}; // end of struct SolidParticleSystemOptions

struct SolidParticleSystemMeshBuilderOptions {
  std::function<void(SolidParticle* particle, unsigned int i, unsigned int s)> positionFunction
    = nullptr;
  std::function<void(SolidParticle* particle, const Vector3& vertex, size_t i)> vertexFunction
    = nullptr;
}; // end of struct SolidParticleSystemMeshBuilderOptions

/**
 * @brief The SPS is a single updatable mesh. The solid particles are simply
 * separate parts or faces fo this big mesh. As it is just a mesh, the SPS has
 * all the same properties than any other BJS mesh : not more, not less. It can
 * be scaled, rotated, translated, enlighted, textured, moved, etc.
 *
 * The SPS is also a particle system. It provides some methods to manage the
 * particles. However it is behavior agnostic. This means it has no emitter, no
 * particle physics, no particle recycler. You have to implement your own
 * behavior.
 *
 * Full documentation here :
 * http://doc.babylonjs.com/how_to/Solid_Particle_System
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
   * If no model shape was added to the SPS, the returned mesh is just a single
   * triangular plane.
   * @returns the created mesh
   */
  MeshPtr buildMesh();

  /**
   * @brief Digests the mesh and generates as many solid particles in the system
   * as wanted. Returns the SPS. These particles will have the same geometry
   * than the mesh parts and will be positioned at the same localisation than
   * the mesh original places. Thus the particles generated from `digest()` have
   * their property `position` set yet.
   * @param mesh ( Mesh ) is the mesh to be digested
   * @param options {facetNb} (optional integer, default 1) is the number of
   * mesh facets per particle, this parameter is overriden by the parameter
   * `number` if any {delta} (optional integer, default 0) is the random extra
   * number of facets per particle , each particle will have between `facetNb`
   * and `facetNb + delta` facets {number} (optional positive integer) is the
   * wanted number of particles : each particle is built with `mesh_total_facets
   * / number` facets
   * @returns the current SPS
   */
  SolidParticleSystem& digest(Mesh* mesh, const SolidParticleSystemDigestOptions& options);

  /**
   * @brief Adds some particles to the SPS from the model shape. Returns the
   * shape id. Please read the doc :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#create-an-immutable-sps
   * @param mesh is any Mesh object that will be used as a model for the solid
   * particles.
   * @param nb (positive integer) the number of particles to be created from
   * this model
   * @param options {positionFunction} is an optional javascript function to
   * called for each particle on SPS creation. {vertexFunction} is an optional
   * javascript function to called for each vertex of each particle on SPS
   * creation
   * @returns the number of shapes in the system
   */
  int addShape(const MeshPtr& mesh, size_t nb,
               const SolidParticleSystemMeshBuilderOptions& options);

  /**
   * @brief Rebuilds the whole mesh and updates the VBO : custom positions and
   * vertices are recomputed if needed.
   * @returns the SPS.
   */
  SolidParticleSystem& rebuildMesh();

  /**
   * @brief Sets all the particles : this method actually really updates the
   * mesh according to the particle positions, rotations, colors, textures, etc.
   * This method calls `updateParticle()` for each particle of the SPS.
   * For an animated SPS, it is usually called within the render loop.
   * @param start The particle index in the particle array where to start to
   * compute the particle property values _(default 0)_
   * @param end The particle index in the particle array where to stop to
   * compute the particle property values _(default nbParticle - 1)_
   * @param update If the mesh must be finally updated on this call after all
   * the particle computations _(default true)_
   * @returns the SPS.
   */
  SolidParticleSystem& setParticles(unsigned int start = 0, unsigned int end = 0,
                                    bool update = true);

  /**
   * @brief Disposes the SPS.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Visibilty helper : Recomputes the visible size according to the mesh
   * bounding box doc :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   * @returns the SPS.
   */
  SolidParticleSystem& refreshVisibleSize();

  /**
   * @brief Visibility helper : Sets the size of a visibility box, this sets the
   * underlying mesh bounding box.
   * @param size the size (float) of the visibility box
   * note : this doesn't lock the SPS mesh bounding box.
   * doc : http://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  void setVisibilityBox(float size);

  /**
   * @brief Gets whether the SPS as always visible or not
   * doc : http://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  [[nodiscard]] bool isAlwaysVisible() const;

  /**
   * @brief Sets the SPS as always visible or not
   * doc : http://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  void setIsAlwaysVisible(bool val);

  /**
   * @brief Sets the SPS visibility box as locked or not. This enables/disables
   * the underlying mesh bounding box updates. doc :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  void setIsVisibilityBoxLocked(bool val);

  /**
   * @brief Gets if the SPS visibility box as locked or not. This
   * enables/disables the underlying mesh bounding box updates. doc :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#sps-visibility
   */
  [[nodiscard]] bool isVisibilityBoxLocked() const;

  /**
   * @brief Tells to `setParticles()` to compute the particle rotations or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle rotations aren't stored values, so setting
   * `computeParticleRotation` to false will prevents the particle to rotate.
   */
  void setComputeParticleRotation(bool val);

  /**
   * @brief Tells to `setParticles()` to compute the particle colors or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle colors are stored values, so setting
   * `computeParticleColor` to false will keep yet the last colors set.
   */
  void setComputeParticleColor(bool val);

  /**
   * @brief Tells to `setParticles()` to compute the particle textures or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle textures are stored values, so setting
   * `computeParticleTexture` to false will keep yet the last colors set.
   */
  void setComputeParticleTexture(bool val);

  /**
   * @brief Tells to `setParticles()` to call the vertex function for each
   * vertex of each particle, or not. Default value : false. The SPS is faster
   * when it's set to false. Note : the particle custom vertex positions aren't
   * stored values.
   */
  void setComputeParticleVertex(bool val);

  /**
   * @brief Tells to `setParticles()` to compute or not the mesh bounding box
   * when computing the particle positions.
   */
  void setComputeBoundingBox(bool val);

  /**
   * @brief Tells to `setParticles()` to sort or not the distance between each
   * particle and the camera. Skipped when `enableDepthSort` is set to `false`
   * (default) at construction time. Default : `true`
   */
  void setDepthSortParticles(bool val);

  /**
   * @brief Gets if `setParticles()` computes the particle rotations or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle rotations aren't stored values, so setting
   * `computeParticleRotation` to false will prevents the particle to rotate.
   */
  [[nodiscard]] bool computeParticleRotation() const;

  /**
   * @brief Gets if `setParticles()` computes the particle colors or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle colors are stored values, so setting
   * `computeParticleColor` to false will keep yet the last colors set.
   */
  [[nodiscard]] bool computeParticleColor() const;

  /**
   * @brief Gets if `setParticles()` computes the particle textures or not.
   * Default value : true. The SPS is faster when it's set to false.
   * Note : the particle textures are stored values, so setting
   * `computeParticleTexture` to false will keep yet the last colors set.
   */
  [[nodiscard]] bool computeParticleTexture() const;

  /**
   * @brief Gets if `setParticles()` calls the vertex function for each vertex
   * of each particle, or not. Default value : false. The SPS is faster when
   * it's set to false. Note : the particle custom vertex positions aren't
   * stored values.
   */
  [[nodiscard]] bool computeParticleVertex() const;

  /**
   * @brief Gets if `setParticles()` computes or not the mesh bounding box when
   * computing the particle positions.
   */
  [[nodiscard]] bool computeBoundingBox() const;

  /**
   * @brief Gets if `setParticles()` sorts or not the distance between each
   * particle and the camera. Skipped when `enableDepthSort` is set to `false`
   * (default) at construction time. Default : `true`
   */
  [[nodiscard]] bool depthSortParticles() const;

  // =======================================================================
  // Particle behavior logic
  // these following methods may be overwritten by the user to fit his needs

  /**
   * @brief This function does nothing. It may be overwritten to set all the
   * particle first values. The SPS doesn't call this function, you may have to
   * call it by your own. doc :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#particle-management
   */
  virtual void initParticles();

  /**
   * @brief This function does nothing. It may be overwritten to recycle a
   * particle. The SPS doesn't call this function, you may have to call it by
   * your own. doc :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#particle-management
   * @param particle The particle to recycle
   * @returns the recycled particle
   */
  virtual SolidParticle* recycleParticle(SolidParticle* particle);

  /**
   * @brief Updates a particle : this function should  be overwritten by the
   * user. It is called on each particle by `setParticles()`. This is the place
   * to code each particle behavior. doc :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#particle-management
   * @example : just set a particle position or velocity and recycle conditions
   * @param particle The particle to update
   * @returns the updated particle
   */
  virtual SolidParticle* updateParticle(SolidParticle* particle);

  /**
   * @brief Updates a vertex of a particle : it can be overwritten by the user.
   * This will be called on each vertex particle by `setParticles()` if
   * `computeParticleVertex` is set to true only.
   * @param particle the current particle
   * @param vertex the current index of the current particle
   * @param pt the index of the current vertex in the particle shape
   * doc :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#update-each-particle-shape
   * @example : just set a vertex particle position
   * @returns the updated vertex
   */
  virtual Vector3 updateParticleVertex(SolidParticle* particle, const Vector3& vertex, size_t pt);

  /**
   * @brief This will be called before any other treatment by `setParticles()`
   * and will be passed three parameters. This does nothing and may be
   * overwritten by the user.
   * @param start the particle index in the particle array where to stop to
   * iterate, same than the value passed to setParticle()
   * @param stop the particle index in the particle array where to stop to
   * iterate, same than the value passed to setParticle()
   * @param update the boolean update value actually passed to setParticles()
   */
  virtual void beforeUpdateParticles(unsigned int start, unsigned int stop, bool update);

  /**
   * @brief This will be called  by `setParticles()` after all the other
   * treatments and just before the actual mesh update. This will be passed
   * three parameters. This does nothing and may be overwritten by the user.
   * @param start the particle index in the particle array where to stop to
   * iterate, same than the value passed to setParticle()
   * @param stop the particle index in the particle array where to stop to
   * iterate, same than the value passed to setParticle()
   * @param update the boolean update value actually passed to setParticles()
   */
  virtual void afterUpdateParticles(unsigned int start, unsigned int stop, bool update);

protected:
  /**
   * @brief Creates a SPS (Solid Particle System) object.
   * @param name (String) is the SPS name, this will be the underlying mesh
   * name.
   * @param scene (Scene) is the scene in which the SPS is added.
   * @param options defines the options of the sps e.g.
   * * updatable (optional boolean, default true) : if the SPS must be updatable
   * or immutable.
   * * isPickable (optional boolean, default false) : if the solid particles
   * must be pickable.
   * * enableDepthSort (optional boolean, default false) : if the solid
   * particles must be sorted in the geometry according to their distance to the
   * camera.
   * * particleIntersection (optional boolean, default false) : if the solid
   * particle intersections must be computed.
   * * boundingSphereOnly (optional boolean, default false) : if the particle
   * intersection must be computed only with the bounding sphere (no bounding
   * box computation, so faster).
   * * bSphereRadiusFactor (optional float, default 1.0) : a number to multiply
   * the boundind sphere radius by in order to reduce it for instance.
   * @example bSphereRadiusFactor = 1.0 / Math.sqrt(3.0) => the bounding sphere
   * exactly matches a spherical mesh.
   */
  SolidParticleSystem(const std::string& name, Scene* scene,
                      const std::optional<SolidParticleSystemOptions>& options = std::nullopt);

private:
  /**
   * @brief Unrotate the fixed normals in case the mesh was built with
   * pre-rotated particles, ex : use of positionFunction in addShape().
   */
  void _unrotateFixedNormals();

  /**
   * @brief Reset copy.
   */
  void _resetCopy();

  /**
   * @brief Inserts the shape model in the global SPS mesh.
   */
  SolidParticle* _meshBuilder(unsigned int p, const std::vector<Vector3>& shape,
                              Float32Array& positions, Uint32Array& meshInd, Uint32Array& indices,
                              const Float32Array& meshUV, Float32Array& uvs,
                              const Float32Array& meshCol, Float32Array& colors,
                              const Float32Array& meshNor, Float32Array& normals, unsigned int idx,
                              unsigned int idxInShape,
                              const SolidParticleSystemMeshBuilderOptions& options);

  /**
   * @brief Returns a shape array from positions array.
   */
  std::vector<Vector3> _posToShape(const Float32Array& positions);

  /**
   * @brief Returns a shapeUV array from a Vector4 uvs.
   */
  Float32Array _uvsToShapeUV(const Float32Array& uvs);

  /**
   * @brief Adds a new particle object in the particles array.
   */
  SolidParticle* _addParticle(unsigned int idx, unsigned int idxpos, unsigned int idxind,
                              std::unique_ptr<ModelShape>&& model, int shapeId,
                              unsigned int idxInShape, const BoundingInfo& bInfo);

  /**
   * @brief Rebuilds a particle back to its just built status : if needed,
   * recomputes the custom positions and vertices.
   */
  void _rebuildParticle(SolidParticle* particle);

public:
  /**
   * The SPS array of Solid Particle objects. Just access each particle as with
   * any classic array.
   * Example : var p = SPS.particles[i];
   */
  std::vector<std::unique_ptr<SolidParticle>> particles;

  /**
   * The SPS total number of particles. Read only. Use SPS.counter instead if
   * you need to set your own value.
   */
  unsigned int nbParticles;

  /**
   * If the particles must ever face the camera (default false). Useful for
   * planar particles.
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
   * The SPS mesh. It's a standard BJS Mesh, so all the methods from the Mesh
   * class are avalaible.
   */
  MeshPtr mesh;

  /**
   * This array is populated when the SPS is set as 'pickable'.
   * Each key of this array is a `faceId` value that you can get from a
   * pickResult object. Each element of this array is an object `{idx: int,
   * faceId: int}`. `idx` is the picked particle index in the `SPS.particles`
   * array `faceId` is the picked face index counted within this particle.
   * Please read :
   * http://doc.babylonjs.com/how_to/Solid_Particle_System#pickable-particles
   */
  std::vector<PickedParticle> pickedParticles;

  /**
   * This array is populated when `enableDepthSort` is set to true.
   * Each element of this array is an instance of the class DepthSortedParticle.
   */
  std::vector<DepthSortedParticle> depthSortedParticles;

  /**
   * If the particle intersection must be computed only with the bounding sphere
   * (no bounding box computation, so faster). (Internal use only)
   * Hidden
   */
  bool _bSphereOnly;

  /**
   * A number to multiply the boundind sphere radius by in order to reduce it
   * for instance. (Internal use only)
   * Hidden
   */
  float _bSphereRadiusFactor;

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
  unsigned int _index; // indices index
  bool _updatable;
  bool _pickable;
  bool _isVisibilityBoxLocked;
  bool _alwaysVisible;
  bool _depthSort;
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
  std::function<int(const DepthSortedParticle& p1, const DepthSortedParticle& p2)>
    _depthSortFunction;
  bool _needs32Bits;

}; // end of class SolidParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SOLID_PARTICLE_SYSTEM_H
