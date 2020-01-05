#ifndef BABYLON_PARTICLES_PARTICLE_SYSTEM_SET_H
#define BABYLON_PARTICLES_PARTICLE_SYSTEM_SET_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/particles/particle_system_set_emitter_creation_options.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class IParticleSystem;
class Scene;
class TransformNode;
using AbstractMeshPtr  = std::shared_ptr<AbstractMesh>;
using TransformNodePtr = std::shared_ptr<TransformNode>;

/**
 * @brief Represents a set of particle systems working together to create a specific effect.
 */
class BABYLON_SHARED_EXPORT ParticleSystemSet : public IDisposable {

public:
  /**
   * Gets or sets base Assets URL
   */
  static constexpr const char* BaseAssetsUrl = "https://assets.babylonjs.com/particles";

public:
  ParticleSystemSet();
  ~ParticleSystemSet() override; // = default

  /**
   * @brief Creates a new emitter mesh as a sphere.
   * @param options defines the options used to create the sphere
   * @param renderingGroupId defines the renderingGroupId to use for the sphere
   * @param scene defines the hosting scene
   */
  void setEmitterAsSphere(const EmitterCreationOptions& options, unsigned int renderingGroupId,
                          Scene* scene);

  /**
   * @brief Starts all particle systems of the set.
   * @param emitter defines an optional mesh to use as emitter for the particle systems
   */
  void start(const AbstractMeshPtr& emitter = nullptr);

  /**
   * @brief Release all associated resources.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Serialize the set into a JSON compatible object.
   * @returns a JSON compatible representation of the set
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Parse a new ParticleSystemSet from a serialized source.
   * @param data defines a JSON compatible representation of the set
   * @param scene defines the hosting scene
   * @param gpu defines if we want GPU particles or CPU particles
   * @returns a new ParticleSystemSet
   */
  static std::unique_ptr<ParticleSystemSet> Parse(const json& data, Scene* scene, bool gpu = false);

protected:
  /**
   * @brief Gets the emitter node used with this set.
   */
  TransformNodePtr& get_emitterNode();

public:
  /**
   * Gets the particle system list
   */
  std::vector<IParticleSystem*> systems;

  /**
   * Gets the emitter node used with this set.
   */
  ReadOnlyProperty<ParticleSystemSet, TransformNodePtr> emitterNode;

private:
  ParticleSystemSetEmitterCreationOptions _emitterCreationOptions;
  TransformNodePtr _emitterNode;

}; // end of class ParticleSystemSetEmitterCreationOptions

} // end of namespace BABYLON

#endif // end of
       // BABYLON_PARTICLES_PARTICLE_SYSTEM_SET_H
