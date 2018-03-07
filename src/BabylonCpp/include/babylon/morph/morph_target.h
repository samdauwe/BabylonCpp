#ifndef BABYLON_MORPH_MORPH_TARGET_H
#define BABYLON_MORPH_MORPH_TARGET_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MorphTarget {

public:
  MorphTarget(const string_t& name, float influence = 0.f);
  ~MorphTarget();

  vector_t<Animation*>& animations();
  const vector_t<Animation*>& animations() const;
  void setPositions(const Float32Array& data);
  Float32Array& getPositions();
  const Float32Array& getPositions() const;
  void setNormals(const Float32Array& data);
  Float32Array& getNormals();
  const Float32Array& getNormals() const;
  void setTangents(const Float32Array& data);
  Float32Array& getTangents();
  const Float32Array& getTangents() const;

  /**
   * @brief Serializes the current target into a Serialization object.
   * Returns the serialized object.
   */
  Json::object serialize() const;

  // Statics
  static unique_ptr_t<MorphTarget>
  Parse(const Json::value& serializationObject);
  static unique_ptr_t<MorphTarget> FromMesh(AbstractMesh* mesh, string_t name,
                                            float influence = 0.f);

private:
  float get_influence() const;
  void set_influence(float influence);
  bool get_hasPositions() const;
  bool get_hasNormals() const;
  bool get_hasTangents() const;

public:
  Observable<bool> onInfluenceChanged;
  Property<MorphTarget, float> influence;
  ReadOnlyProperty<MorphTarget, bool> hasPositions;
  ReadOnlyProperty<MorphTarget, bool> hasNormals;
  ReadOnlyProperty<MorphTarget, bool> hasTangents;

private:
  string_t _name;
  vector_t<Animation*> _animations;
  Float32Array _positions;
  Float32Array _normals;
  Float32Array _tangents;
  float _influence;

}; // end of class MorphTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MORPH_MORPH_TARGET_H
