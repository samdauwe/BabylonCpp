#ifndef BABYLON_MORPH_MORPH_TARGET_H
#define BABYLON_MORPH_MORPH_TARGET_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/misc/observable.h>

using json = nlohmann::json;

namespace BABYLON {

struct AnimationPropertiesOverride;
class Scene;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(MorphTarget)

/**
 * @brief Defines a target to use with MorphTargetManager.
 * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
class BABYLON_SHARED_EXPORT MorphTarget : public IAnimatable {

public:
  template <typename... Ts>
  static MorphTargetPtr New(Ts&&... args)
  {
    return std::shared_ptr<MorphTarget>(new MorphTarget(std::forward<Ts>(args)...));
  }
  ~MorphTarget() override; // = default

  [[nodiscard]] Type type() const override;

  /**
   * @brief Gets a property.
   */
  AnimationValue getProperty(const std::vector<std::string>& targetPropertyPath) override;

  /**
   * @brief Sets a property.
   */
  void setProperty(const std::vector<std::string>& targetPropertyPath,
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
  [[nodiscard]] const Float32Array& getPositions() const;

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
  [[nodiscard]] const Float32Array& getNormals() const;

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
  [[nodiscard]] const Float32Array& getTangents() const;

  /**
   * @brief Affects texture coordinates data to this target.
   * @param data defines the texture coordinates data to use
   */
  void setUVs(const Float32Array& data);

  /**
   * @brief Gets the texture coordinates data stored in this target.
   * @returns a FloatArray containing the texture coordinates data (or null if not present)
   */
  [[nodiscard]] const Float32Array& getUVs() const;

  /**
   * @brief Clone the current target.
   * @returns a new MorphTarget
   */
  MorphTargetPtr clone();

  /**
   * @brief Serializes the current target into a Serialization object.
   * @returns the serialized object.
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Returns the string "MorphTarget".
   * @returns "MorphTarget"
   */
  [[nodiscard]] std::string getClassName() const;

  // Statics

  /**
   * @brief Creates a new target from serialized data.
   * @param serializationObject defines the serialized data to use
   * @returns a new MorphTarget
   */
  static MorphTargetPtr Parse(const json& serializationObject);

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
   * @brief Creates a new MorphTarget.
   * @param name defines the name of the target
   * @param influence defines the influence to use
   * @param scene defines the scene the morphtarget belongs to
   */
  MorphTarget(const std::string& name, float influence = 0.f, Scene* scene = nullptr);

  /**
   * @brief Gets the influence of this target (ie. its weight in the overall morphing).
   */
  [[nodiscard]] float get_influence() const;

  /**
   * @brief Sets the influence of this target (ie. its weight in the overall morphing).
   */
  void set_influence(float influence);

  /**
   * @brief Gets the animation properties override
   */
  AnimationPropertiesOverridePtr& get_animationPropertiesOverride() override;

  /**
   * @brief Sets the animation properties override
   */
  void set_animationPropertiesOverride(const AnimationPropertiesOverridePtr& value) override;

  /**
   * @brief Gets the unique ID of this manager.
   */
  [[nodiscard]] size_t get_uniqueId() const;

  /**
   * @brief Gets a boolean defining if the target contains position data.
   */
  [[nodiscard]] bool get_hasPositions() const;

  /**
   * @brief Gets a boolean defining if the target contains normal data.
   */
  [[nodiscard]] bool get_hasNormals() const;

  /**
   * @brief Gets a boolean defining if the target contains tangent data.
   */
  [[nodiscard]] bool get_hasTangents() const;

  /**
   * @brief Gets a boolean defining if the target contains texture coordinates data.
   */
  [[nodiscard]] bool get_hasUVs() const;

public:
  /**
   * Defines the name of the target
   */
  std::string name;

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
   * Gets the unique ID of this manager
   */
  ReadOnlyProperty<MorphTarget, size_t> uniqueId;

  /**
   * Gets a boolean defining if the target contains position data
   */
  ReadOnlyProperty<MorphTarget, bool> hasPositions;

  /**
   * Gets a boolean defining if the target contains normal data
   */
  ReadOnlyProperty<MorphTarget, bool> hasNormals;

  /**
   * Gets a boolean defining if the target contains tangent data
   */
  ReadOnlyProperty<MorphTarget, bool> hasTangents;

  /**
   * Gets a boolean defining if the target contains texture coordinates data
   */
  ReadOnlyProperty<MorphTarget, bool> hasUVs;

private:
  Scene* _scene;
  Float32Array _positions;
  Float32Array _normals;
  Float32Array _tangents;
  Float32Array _uvs;
  float _influence;
  size_t _uniqueId;
  AnimationPropertiesOverridePtr _animationPropertiesOverride;

}; // end of class MorphTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MORPH_MORPH_TARGET_H
