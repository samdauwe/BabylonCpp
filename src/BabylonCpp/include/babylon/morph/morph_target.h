#ifndef BABYLON_MORPH_MORPH_TARGET_H
#define BABYLON_MORPH_MORPH_TARGET_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/misc/observable.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
struct AnimationPropertiesOverride;
class MorphTarget;
class Scene;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using MorphTargetPtr  = std::shared_ptr<MorphTarget>;

/**
 * @brief Defines a target to use with MorphTargetManager.
 * @see http://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
class BABYLON_SHARED_EXPORT MorphTarget : public IAnimatable {

public:
  template <typename... Ts>
  static MorphTargetPtr New(Ts&&... args)
  {
    return std::shared_ptr<MorphTarget>(
      new MorphTarget(std::forward<Ts>(args)...));
  }
  ~MorphTarget() override;

  Type type() const override;

  /**
   * @brief Gets a property.
   */
  virtual AnimationValue
  getProperty(const std::vector<std::string>& targetPropertyPath) override;

  /**
   * @brief Sets a property.
   */
  virtual void setProperty(const std::vector<std::string>& targetPropertyPath,
                           const AnimationValue& value) override;

  /**
   * @brief Gets the animations.
   */
  std::vector<AnimationPtr> getAnimations() override;

  /**
   * @brief Affects position data to this target.
   * @param data defines the position data to use
   */
  void setPositions(const Float32Array& data);

  /**
   * @brief Gets the position data stored in this target.
   * @returns a FloatArray containing the position data (or null if not present)
   */
  Float32Array& getPositions();

  /**
   * @brief Gets the position data stored in this target.
   * @returns a FloatArray containing the position data (or null if not present)
   */
  const Float32Array& getPositions() const;

  /**
   * @brief Affects normal data to this target.
   * @param data defines the normal data to use
   */
  void setNormals(const Float32Array& data);

  /**
   * @brief Gets the normal data stored in this target.
   * @returns a FloatArray containing the normal data (or null if not present)
   */
  Float32Array& getNormals();

  /**
   * @brief Gets the normal data stored in this target.
   * @returns a FloatArray containing the normal data (or null if not present)
   */
  const Float32Array& getNormals() const;

  /**
   * @brief Affects tangent data to this target.
   * @param data defines the tangent data to use
   */
  void setTangents(const Float32Array& data);

  /**
   * @brief Gets the tangent data stored in this target.
   * @returns a FloatArray containing the tangent data (or null if not present)
   */
  Float32Array& getTangents();

  /**
   * @brief Gets the tangent data stored in this target.
   * @returns a FloatArray containing the tangent data (or null if not present)
   */
  const Float32Array& getTangents() const;

  /**
   * @brief Serializes the current target into a Serialization object.
   * @returns the serialized object.
   */
  json serialize() const;

  /**
   * @brief Returns the string "MorphTarget".
   * @returns "MorphTarget"
   */
  std::string getClassName() const;

  // Statics

  /**
   * @brief Creates a new target from serialized data.
   * @param serializationObject defines the serialized data to use
   * @returns a new MorphTarget
   */
  static std::unique_ptr<MorphTarget> Parse(const json& serializationObject);

  /**
   * @brief Creates a MorphTarget from mesh data.
   * @param mesh defines the source mesh
   * @param name defines the name to use for the new target
   * @param influence defines the influence to attach to the target
   * @returns a new MorphTarget
   */
  static MorphTargetPtr FromMesh(const AbstractMeshPtr& mesh, std::string name,
                                 float influence = 0.f);

protected:
  /**
   * @brief Gets the influence of this target (ie. its weight in the overall
   * morphing).
   */
  float get_influence() const;

  /**
   * @brief Sets the influence of this target (ie. its weight in the overall
   * morphing).
   */
  void set_influence(float influence);

  /**
   * @brief Gets the animation properties override
   */
  AnimationPropertiesOverride*& get_animationPropertiesOverride();

  /**
   * @brief Sets the animation properties override
   */
  void
  set_animationPropertiesOverride(AnimationPropertiesOverride* const& value);

  /**
   * @brief Gets a boolean defining if the target contains position data.
   */
  bool get_hasPositions() const;

  /**
   * @brief Gets a boolean defining if the target contains normal data.
   */
  bool get_hasNormals() const;

  /**
   * @brief Gets a boolean defining if the target contains tangent data.
   */
  bool get_hasTangents() const;

public:
  /**
   * @brief Creates a new MorphTarget.
   * @param name defines the name of the target
   * @param influence defines the influence to use
   * @param scene defines the scene the morphtarget belongs to
   */
  MorphTarget(const std::string& name, float influence = 0.f,
              Scene* scene = nullptr);

  /**
   * Gets or sets the list of animations
   */
  std::vector<AnimationPtr> animations;

  /**
   * Observable raised when the influence changes
   */
  Observable<bool> onInfluenceChanged;

  /**
   * Hidden
   */
  Observable<void> _onDataLayoutChanged;

  /**
   * Influence of this target (ie. its weight in the overall morphing).
   */
  Property<MorphTarget, float> influence;

  /**
   * Gets or sets the id of the morph Target
   */
  std::string id;

  /**
   * Animation properties override
   */
  Property<MorphTarget, AnimationPropertiesOverride*>
    animationPropertiesOverride;

  /**
   * Boolean defining if the target contains position data
   */
  ReadOnlyProperty<MorphTarget, bool> hasPositions;

  /**
   * Boolean defining if the target contains normal data
   */
  ReadOnlyProperty<MorphTarget, bool> hasNormals;

  /**
   * Boolean defining if the target contains tangent data
   */
  ReadOnlyProperty<MorphTarget, bool> hasTangents;

private:
  /**
   * Defines the name of the target
   */
  std::string _name;

  Scene* _scene;
  Float32Array _positions;
  Float32Array _normals;
  Float32Array _tangents;
  float _influence;
  AnimationPropertiesOverride* _animationPropertiesOverride;

}; // end of class MorphTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MORPH_MORPH_TARGET_H
