#ifndef BABYLON_PARTICLES_POINTS_CLOUD_SYSTEM_H
#define BABYLON_PARTICLES_POINTS_CLOUD_SYSTEM_H

#include <functional>
#include <memory>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/maths/color4.h>

namespace BABYLON {

class CloudPoint;
class Mesh;
class PointsGroup;
class Scene;
using CloudPointPtr  = std::shared_ptr<CloudPoint>;
using MeshPtr        = std::shared_ptr<Mesh>;
using PointsGroupPtr = std::shared_ptr<PointsGroup>;

struct PointsCloudSystemOptions {
  std::optional<bool> updatable = std::nullopt;
}; // end of struct PointsCloudSystemOptions

/**
 * @brief Defines the 4 color options
 */
enum class PointColor {
  /** color value */
  Color = 2,
  /** uv value */
  UV = 1,
  /** random value */
  Random = 0,
  /** stated value */
  Stated = 3
}; // end of enum class PointColor

/**
 * @brief The PointCloudSystem (PCS) is a single updatable mesh. The points corresponding to the
 * vertices of this big mesh. As it is just a mesh, the PointCloudSystem has all the same properties
 * as any other BJS mesh : not more, not less. It can be scaled, rotated, translated, enlighted,
 * textured, moved, etc.
 *
 * The PointCloudSytem is also a particle system, with each point being a particle. It provides some
 * methods to manage the particles. However it is behavior agnostic. This means it has no emitter,
 * no particle physics, no particle recycler. You have to implement your own behavior.
 *
 * Full documentation here : TO BE ENTERED
 */
class BABYLON_SHARED_EXPORT PointsCloudSystem : public IDisposable {

public:
  /**
   * @brief Creates a PCS (Points Cloud System) object.
   * @param name (String) is the PCS name, this will be the underlying mesh name
   * @param pointSize (number) is the size for each point
   * @param scene (Scene) is the scene in which the PCS is added
   * @param options defines the options of the PCS e.g.
   * * updatable (optional boolean, default true) : if the PCS must be updatable or immutable
   */
  PointsCloudSystem(const std::string& name, size_t pointSize, Scene* scene,
                    const std::optional<PointsCloudSystemOptions>& options = std::nullopt);
  ~PointsCloudSystem() override; // = default;

  /**
   * @brief Builds the PCS underlying mesh. Returns a standard Mesh.
   * If no points were added to the PCS, the returned mesh is just a single point.
   * @returns a promise for the created mesh
   */
  MeshPtr buildMeshSync();

  /**
   * @brief Adds points to the PCS in random positions within a unit sphere.
   * @param nb (positive integer) the number of particles to be created from this model
   * @param pointFunction is an optional javascript function to be called for each particle on PCS
   * creation
   * @returns the number of groups in the system
   */
  size_t
  addPoints(size_t nb,
            const std::function<void(CloudPoint* particle, size_t i, size_t s)>& pointFunction
            = nullptr);

  /**
   * @brief Adds points to the PCS from the surface of the model shape.
   * @param mesh is any Mesh object that will be used as a surface model for the points
   * @param nb (positive integer) the number of particles to be created from this model
   * @param colorWith determines whether a point is colored using color (default), uv, random,
   * stated or none (invisible)
   * @param color (color4) to be used when colorWith is stated or color (number) when used to
   * specify texture position
   * @param range (number from 0 to 1) to determine the variation in shape and tone for a stated
   * color
   * @returns the number of groups in the system
   */
  size_t addSurfacePoints(const MeshPtr& mesh, size_t nb,
                          const std::optional<PointColor> colorWith                = std::nullopt,
                          const std::optional<std::variant<Color4, size_t>>& color = std::nullopt,
                          const std::optional<int> range                           = std::nullopt);

  /**
   * @brief Adds points to the PCS inside the model shape.
   * @param mesh is any Mesh object that will be used as a surface model for the points
   * @param nb (positive integer) the number of particles to be created from this model
   * @param colorWith determines whether a point is colored using color (default), uv, random,
   * stated or none (invisible)
   * @param color (color4) to be used when colorWith is stated or color (number) when used to
   * specify texture position
   * @param range (number from 0 to 1) to determine the variation in shape and tone for a stated
   * color
   * @returns the number of groups in the system
   */
  size_t addVolumePoints(const MeshPtr& mesh, size_t nb,
                         const std::optional<PointColor> colorWith                = std::nullopt,
                         const std::optional<std::variant<Color4, size_t>>& color = std::nullopt,
                         const std::optional<int> range                           = std::nullopt);

  /**
   * @brief Sets all the particles : this method actually really updates the mesh according to the
   * particle positions, rotations, colors, textures, etc. This method calls `updateParticle()` for
   * each particle of the SPS. For an animated SPS, it is usually called within the render loop.
   * @param start The particle index in the particle array where to start to compute the particle
   * property values _(default 0)_
   * @param end The particle index in the particle array where to stop to compute the particle
   * property values _(default nbParticle - 1)_
   * @param update If the mesh must be finally updated on this call after all the particle
   * computations _(default true)_
   * @returns the PCS.
   */
  PointsCloudSystem& setParticles(size_t start = 0, size_t end = 0, bool update = true);

  /**
   * @brief Disposes the PCS.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Visibilty helper : Recomputes the visible size according to the mesh bounding box.
   * doc :
   * @returns the PCS.
   */
  PointsCloudSystem& refreshVisibleSize();

  /**
   * @brief Visibility helper : Sets the size of a visibility box, this sets the underlying mesh
   * bounding box.
   * @param size the size (float) of the visibility box
   * note : this doesn't lock the PCS mesh bounding box.
   * doc :
   */
  void setVisibilityBox(float size);

  // =======================================================================
  // Particle behavior logic
  // these following methods may be overwritten by users to fit their needs

  /**
   * @brief This function does nothing. It may be overwritten to set all the particle first values.
   * The PCS doesn't call this function, you may have to call it by your own.
   * doc :
   */
  virtual void initParticles();

  /**
   * @brief This function does nothing. It may be overwritten to recycle a particle
   * The PCS doesn't call this function, you can to call it
   * doc :
   * @param particle The particle to recycle
   * @returns the recycled particle
   */
  virtual CloudPointPtr recycleParticle(const CloudPointPtr& particle);

  /**
   * @brief Updates a particle : this function should  be overwritten by the user.
   * It is called on each particle by `setParticles()`. This is the place to code each particle
   * behavior. doc :
   * @example : just set a particle position or velocity and recycle conditions
   * @param particle The particle to update
   * @returns the updated particle
   */
  virtual CloudPointPtr updateParticle(const CloudPointPtr& particle);

  /**
   * @brief This will be called before any other treatment by `setParticles()` and will be passed
   * three parameters. This does nothing and may be overwritten by the user.
   * @param start the particle index in the particle array where to start to iterate, same than the
   * value passed to setParticle()
   * @param stop the particle index in the particle array where to stop to iterate, same than the
   * value passed to setParticle()
   * @param update the boolean update value actually passed to setParticles()
   */
  virtual void beforeUpdateParticles(std::optional<size_t> start = std::nullopt,
                                     std::optional<size_t> stop  = std::nullopt,
                                     std::optional<bool> update  = std::nullopt);

  /**
   * @brief This will be called  by `setParticles()` after all the other treatments and just before
   * the actual mesh update. This will be passed three parameters. This does nothing and may be
   * overwritten by the user.
   * @param start the particle index in the particle array where to start to iterate, same than the
   * value passed to setParticle()
   * @param stop the particle index in the particle array where to stop to iterate, same than the
   * value passed to setParticle()
   * @param update the boolean update value actually passed to setParticles()
   */
  virtual void afterUpdateParticles(std::optional<size_t> start = std::nullopt,
                                    std::optional<size_t> stop  = std::nullopt,
                                    std::optional<bool> update  = std::nullopt);

private:
  /**
   * @hidden
   */
  MeshPtr _buildMesh();

  /**
   * @brief Adds a new particle object in the particles array.
   */
  CloudPointPtr _addParticle(size_t idx, const PointsGroupPtr& group, size_t groupId,
                             size_t idxInGroup);

  void _randomUnitVector(CloudPoint& particle);
  Color4 _getColorIndicesForCoord(const PointsGroup& pointsGroup, uint32_t x, uint32_t y,
                                  uint32_t width) const;
  void _setPointsColorOrUV(const MeshPtr& mesh, const PointsGroupPtr& pointsGroup, bool isVolume,
                           const std::optional<bool>& colorFromTexture = std::nullopt,
                           const std::optional<bool>& hasTexture       = std::nullopt,
                           const std::optional<Color4>& color          = std::nullopt,
                           std::optional<int> range                    = std::nullopt);

  /**
   * @brief Stores mesh texture in dynamic texture for color pixel retrievalwhen pointColor type is
   * color for surface points
   */
  void _colorFromTexture(const MeshPtr& mesh, const PointsGroupPtr& pointsGroup, bool isVolume);

  /**
   * @brief Calculates the point density per facet of a mesh for surface points.
   */
  Float32Array _calculateDensity(size_t nbPoints, const Float32Array& positions,
                                 const IndicesArray& indices);

public:
  /**
   *  The PCS array of cloud point objects. Just access each particle as with any classic array.
   *  Example : var p = SPS.particles[i];
   */
  std::vector<CloudPointPtr> particles;
  /**
   * The PCS total number of particles. Read only. Use PCS.counter instead if you need to set your
   * own value.
   */
  size_t nbParticles;
  /**
   * This a counter for your own usage. It's not set by any SPS functions.
   */
  size_t counter;
  /**
   * The PCS name. This name is also given to the underlying mesh.
   */
  std::string name;
  /**
   * The PCS mesh. It's a standard BJS Mesh, so all the methods from the Mesh class are avalaible.
   */
  MeshPtr mesh;
  /**
   * This empty object is intended to store some PCS specific or temporary values in order to lower
   * the Garbage Collector activity. Please read :
   */
  std::string vars;
  /**
   * @hidden
   */
  size_t _size; // size of each point particle

private:
  Scene* _scene;
  Float32Array _positions;
  Float32Array _indices;
  Float32Array _normals;
  Float32Array _colors;
  Float32Array _uvs;
  IndicesArray
    _indices32; // used as depth sorted array if depth sort enabled, else used as typed indices
  Float32Array _positions32; // updated positions for the VBO
  Float32Array _colors32;
  Float32Array _uvs32;
  bool _updatable;
  bool _isVisibilityBoxLocked;
  bool _alwaysVisible;
  IndicesArray _groups; // start indices for each group of particles
  size_t _groupCounter;
  bool _computeParticleColor;
  bool _computeParticleTexture;
  bool _computeParticleRotation;
  bool _computeBoundingBox;
  bool _isReady;

}; // end of class PointsCloudSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_POINTS_CLOUD_SYSTEM_H
