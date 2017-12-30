#ifndef BABYLON_PARTICLES_SOLID_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_SOLID_PARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/particles/solid_particle.h>

namespace BABYLON {

struct SolidParticleSystemOptions {
  bool updatable            = true;
  bool isPickable           = false;
  bool particleIntersection = false;
  bool boundingSphereOnly   = false;
  bool enableDepthSort      = false;
  float bSphereRadiusFactor = 1.f;
}; // end of struct SolidParticleSystemOptions

struct SolidParticleSystemDigestOptions {
  size_t facetNb = 1;
  size_t number  = 0;
  int delta      = 0;
}; // end of struct SolidParticleSystemOptions

struct SolidParticleSystemMeshBuilderOptions {
  ::std::function<void(SolidParticle* particle, unsigned int i, unsigned int s)>
    positionFunction = nullptr;
  ::std::function<void(SolidParticle* particle, const Vector3& vertex,
                       unsigned int i)>
    vertexFunction = nullptr;
}; // end of struct SolidParticleSystemMeshBuilderOptions

class BABYLON_SHARED_EXPORT SolidParticleSystem : public IDisposable {

public:
  /**
   * @brief Creates a SPS (Solid Particle System) object.
   * `name` (String) is the SPS name, this will be the underlying mesh name.
   * `scene` (Scene) is the scene in which the SPS is added.
   * `updatable` (optional boolean, default true) : if the SPS must be updatable
   * or immutable.
   * `isPickable` (optional boolean, default false) : if the solid particles
   * must be pickable.
   * `particleIntersection` (optional boolean, default false) : if the solid
   * particle intersections must be computed.
   * `boundingSphereOnly` (optional boolean, default false) : if the particle
   * intersection must be computed only with the bounding sphere (no bounding
   * box computation, so faster).
   * `bSphereRadiusFactor` (optional float, default 1.0) : a number to multiply
   * the boundind sphere radius by in order to reduce it for instance.
   *  Example : bSphereRadiusFactor = 1.0 / Math.sqrt(3.0) => the bounding
   * sphere exactly matches a spherical mesh.
   */
  SolidParticleSystem(const string_t& name, Scene* scene,
                      const SolidParticleSystemOptions& options);
  virtual ~SolidParticleSystem();

  /**
   * @brief Builds the SPS underlying mesh. Returns a standard Mesh.
   * If no model shape was added to the SPS, the returned mesh is just a
   * single triangular plane.
   */
  Mesh* buildMesh();

  /**
   * @brief Digests the mesh and generates as many solid particles in the system
   * as wanted. Returns the SPS.
   * These particles will have the same geometry than the mesh parts and
   * will be positioned at the same localisation than the mesh original places.
   * Thus the particles generated from `digest()` have their property
   * `position` set yet.
   * `mesh` (`Mesh`) is the mesh to be digested
   * `facetNb` (optional integer, default 1) is the number of mesh facets
   * per particle, this parameter is overriden by the parameter `number` if any
   * `delta` (optional integer, default 0) is the random extra number of
   * facets per particle , each particle will have between `facetNb` and
   * `facetNb + delta` facets
   * `number` (optional positive integer) is the wanted number of particles
   * : each particle is built with `mesh_total_facets / number` facets
   */
  SolidParticleSystem& digest(Mesh* mesh,
                              const SolidParticleSystemDigestOptions& options);

  /**
   * @brief Adds some particles to the SPS from the model shape. Returns the
   * shape id.
   * Please read the doc :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#create-an-immutable-sps
   * `mesh` is any `Mesh` object that will be used as a model for the
   * solid particles.
   * `nb` (positive integer) the number of particles to be created from
   * this model
   * `positionFunction` is an optional javascript function to called for
   * each particle on SPS creation.
   * `vertexFunction` is an optional javascript function to called for
   * each vertex of each particle on SPS creation
   */
  int addShape(Mesh* mesh, size_t nb,
               const SolidParticleSystemMeshBuilderOptions& options);

  /**
   * @brief Rebuilds the whole mesh and updates the VBO : custom positions and
   * vertices are recomputed if needed.
   * @returns The SPS.
   */
  SolidParticleSystem& rebuildMesh();

  /**
   * @brief Sets all the particles : this method actually really updates the
   * mesh according to the particle positions, rotations, colors, textures, etc.
   *  This method calls `updateParticle()` for each particle of the SPS.
   *  For an animated SPS, it is usually called within the render loop.
   * @param start (default 0) the particle index in the particle array
   * where to start to compute the particle property values
   * @param end (default nbParticle - 1)  the particle index in the
   * particle array where to stop to compute the particle property values
   * @param update (default true) if the mesh must be finally updated on
   * this call after all the particle computations.
   * @returns The SPS.
   */
  SolidParticleSystem& setParticles(unsigned int start = 0,
                                    unsigned int end = 0, bool update = true);

  /**
   * @brief Disposes the SPS.
   */
  void dispose(bool doNotRecurse = false) override;

  /**
   * @brief Visibilty helper : Recomputes the visible size according to the mesh
   * bounding box
   * doc :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#sps-visibility
   * @returns The SPS.
   */
  SolidParticleSystem& refreshVisibleSize();

  /**
   * @brief Visibility helper : Sets the size of a visibility box, this sets the
   * underlying mesh bounding box.
   * @param size the size (float) of the visibility box
   * note : this doesn't lock the SPS mesh bounding box.
   * doc :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#sps-visibility
   */
  void setVisibilityBox(float size);

  /**
   * @brief Returns if the SPS is as always visible or not.
   * @return If the SPS is as always visible or not.
   */
  bool isAlwaysVisible() const;

  /**
   * @brief Sets the SPS as always visible or not
   * doc :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#sps-visibility
   */
  void setIsAlwaysVisible(bool val);

  /**
   * @brief Sets the SPS visibility box as locked or not. This enables/disables
   * the underlying mesh bounding box updates.
   * doc :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#sps-visibility
   */
  void setIsVisibilityBoxLocked(bool val);

  bool isVisibilityBoxLocked() const;

  // Optimizer setters
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
   * vertex of each particle, or not.
   * Default value : false. The SPS is faster when it's set to false.
   * Note : the particle custom vertex positions aren't stored values.
   */
  void setComputeParticleVertex(bool val);

  /**
   * @brief Tells to `setParticles()` to compute or not the mesh bounding box
   * when computing the particle positions.
   */
  void setComputeBoundingBox(bool val);

  /**
   * @brief Tells to `setParticles()` to sort or not the distance between each
   * particle and the camera.
   * Skipped when `enableDepthSort` is set to `false` (default) at construction
   * time.
   * Default : `true`
   */
  void setDepthSortParticles(bool val);

  // getters
  bool computeParticleRotation() const;

  bool computeParticleColor() const;

  bool computeParticleTexture() const;

  bool computeParticleVertex() const;

  bool computeBoundingBox() const;

  bool depthSortParticles() const;

  // =======================================================================
  // Particle behavior logic
  // these following methods may be overwritten by the user to fit his needs

  /**
   * @brief This function does nothing. It may be overwritten to set all the
   * particle first values.
   * The SPS doesn't call this function, you may have to call it by your own.
   * doc :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#particle-management
   */
  virtual void initParticles();

  /**
   * @brief This function does nothing. It may be overwritten to recycle a
   * particle.
   * The SPS doesn't call this function, you may have to call it by your
   * own.
   * doc :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#particle-management
   */
  virtual SolidParticle* recycleParticle(SolidParticle* particle);

  /**
   * @brief Updates a particle : this function should  be overwritten by the
   * user. It is called on each particle by `setParticles()`. This is the place
   * to code each particle behavior.
   * doc :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#particle-management
   * ex : just set a particle position or velocity and recycle conditions
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
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#update-each-particle-shape
   * ex : just set a vertex particle position
   */
  virtual Vector3 updateParticleVertex(SolidParticle* particle,
                                       const Vector3& vertex, unsigned int pt);

  /**
   * @brief This will be called before any other treatment by `setParticles()`
   * and will be passed three parameters.
   * This does nothing and may be overwritten by the user.
   * @param start the particle index in the particle array where to stop to
   * iterate, same than the value passed to setParticle()
   * @param stop the particle index in the particle array where to stop to
   * iterate, same than the value passed to setParticle()
   * @param update the boolean update value actually passed to setParticles()
   */
  virtual void beforeUpdateParticles(unsigned int start, unsigned int stop,
                                     bool update);

  /**
   * @brief This will be called  by `setParticles()` after all the other
   * treatments and just before the actual mesh update.
   * This will be passed three parameters.
   * This does nothing and may be overwritten by the user.
   * @param start the particle index in the particle array where to stop to
   * iterate, same than the value passed to setParticle()
   * @param stop the particle index in the particle array where to stop to
   * iterate, same than the value passed to setParticle()
   * @param update the boolean update value actually passed to setParticles()
   */
  virtual void afterUpdateParticles(unsigned int start, unsigned int stop,
                                    bool update);

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
  SolidParticle*
  _meshBuilder(unsigned int p, const vector_t<Vector3>& shape,
               Float32Array& positions, Uint32Array& meshInd,
               Uint32Array& indices, const Float32Array& meshUV,
               Float32Array& uvs, const Float32Array& meshCol,
               Float32Array& colors, const Float32Array& meshNor,
               Float32Array& normals, unsigned int idx, unsigned int idxInShape,
               const SolidParticleSystemMeshBuilderOptions& options);

  /**
   * @brief Returns a shape array from positions array.
   */
  vector_t<Vector3> _posToShape(const Float32Array& positions);

  /**
   * @brief Returns a shapeUV array from a Vector4 uvs.
   */
  Float32Array _uvsToShapeUV(const Float32Array& uvs);

  /**
   * @brief Adds a new particle object in the particles array.
   */
  SolidParticle* _addParticle(unsigned int idx, unsigned int idxpos,
                              unsigned int idxind,
                              unique_ptr_t<ModelShape>&& model, int shapeId,
                              unsigned int idxInShape,
                              const BoundingInfo& bInfo);

  /**
   * @brief Rebuilds a particle back to its just built status : if needed,
   * recomputes the custom positions and vertices.
   */
  void _rebuildParticle(SolidParticle* particle);
  void _quaternionRotationYPR();
  void _quaternionToRotationMatrix();

public:
  // Members
  /**
   * The SPS array of Solid Particle objects. Just access each particle as with
   * any classic array.
   * Example : var p = SPS.particles[i];
   */
  vector_t<unique_ptr_t<SolidParticle>> particles;

  /**
   * The SPS total number of particles. Read only. Use SPS.counter instead if
   * you
   * need to set your own value.
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
  string_t name;

  /**
   * The SPS mesh. It's a standard BJS Mesh, so all the methods from the Mesh
   * class are avalaible.
   */
  Mesh* mesh;

  /**
   * This array is populated when the SPS is set as 'pickable'.
   * Each key of this array is a `faceId` value that you can get from a
   * pickResult object.
   * Each element of this array is an object `{idx: int, faceId: int}`.
   * `idx` is the picked particle index in the `SPS.particles` array
   * `faceId` is the picked face index counted within this particle.
   * Please read :
   * http://doc.babylonjs.com/overviews/Solid_Particle_System#pickable-particles
   */
  vector_t<PickedParticle> pickedParticles;

  /**
   * This array is populated when `enableDepthSort` is set to true.
   * Each element of this array is an instance of the class DepthSortedParticle.
   */
  vector_t<DepthSortedParticle> depthSortedParticles;

  bool _bSphereOnly;
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
  unique_ptr_t<SolidParticle> _copy;
  vector_t<Vector3> _shape;
  Float32Array _shapeUV;
  unique_ptr_t<Color4> _color;
  bool _computeParticleColor;
  bool _computeParticleTexture;
  bool _computeParticleRotation;
  bool _computeParticleVertex;
  bool _computeBoundingBox;
  bool _depthSortParticles;
  Vector3 _cam_axisZ;
  Vector3 _cam_axisY;
  Vector3 _cam_axisX;
  Vector3 _axisX;
  Vector3 _axisY;
  Vector3 _axisZ;
  TargetCamera* _camera;
  SolidParticle* _particle;
  Vector3 _camDir;
  Vector3 _camInvertedPosition;
  Matrix _rotMatrix;
  Matrix _invertMatrix;
  Vector3 _rotated;
  Quaternion _quaternion;
  Vector3 _vertex;
  Vector3 _normal;
  float _yaw;
  float _pitch;
  float _roll;
  float _halfroll;
  float _halfpitch;
  float _halfyaw;
  float _sinRoll;
  float _cosRoll;
  float _sinPitch;
  float _cosPitch;
  float _sinYaw;
  float _cosYaw;
  bool _mustUnrotateFixedNormals;
  Vector3 _minimum;
  Vector3 _maximum;
  Vector3 _scale;
  Vector3 _translation;
  Vector3 _minBbox;
  Vector3 _maxBbox;
  bool _particlesIntersect;
  ::std::function<int(const DepthSortedParticle& p1,
                      const DepthSortedParticle& p2)>
    _depthSortFunction;
  bool _needs32Bits;

}; // end of class SolidParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SOLID_PARTICLE_SYSTEM_H
