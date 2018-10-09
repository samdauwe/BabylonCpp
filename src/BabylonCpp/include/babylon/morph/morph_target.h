#ifndef BABYLON_MORPH_MORPH_TARGET_H
#define BABYLON_MORPH_MORPH_TARGET_H

#include <map>

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/tools/observable.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class MorphTarget;
struct AnimationPropertiesOverride;
class Scene;
using MorphTargetPtr = std::shared_ptr<MorphTarget>;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

/**
 * @brief Defines a target to use with MorphTargetManager.
 * @see http://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
class BABYLON_SHARED_EXPORT MorphTarget : public IAnimatable {

public:
  /**
   * @brief Creates a new MorphTarget.
   * @param name defines the name of the target
   * @param influence defines the influence to use
   */
  MorphTarget(const std::string& name, float influence = 0.f,
              Scene* scene = nullptr);
  ~MorphTarget() override;

  IReflect::Type type() const override;

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
  Json::object serialize() const;

  // Statics

  /**
   * @brief Creates a new target from serialized data.
   * @param serializationObject defines the serialized data to use
   * @returns a new MorphTarget
   */
  static std::unique_ptr<MorphTarget>
  Parse(const Json::value& serializationObject);

  /**
   * @brief Creates a MorphTarget from mesh data.
   * @param mesh defines the source mesh
   * @param name defines the name to use for the new target
   * @param influence defines the influence to attach to the target
   * @returns a new MorphTarget
   */
  static std::unique_ptr<MorphTarget>
  FromMesh(AbstractMesh* mesh, std::string name, float influence = 0.f);

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
   * Gets or sets the list of animations
   */
  std::vector<Animation*> animations;

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
