#ifndef BABYLON_MORPH_MORPH_TARGET_H
#define BABYLON_MORPH_MORPH_TARGET_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MorphTarget {

public:
  MorphTarget(const string_t& name, float influence = 0.f);
  ~MorphTarget();

  float influence() const;
  void setInfluence(float influence);
  bool hasNormals() const;
  bool hasTangents() const;
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
  static unique_ptr_t<MorphTarget>
  FromMesh(AbstractMesh* mesh, const string_t& name, float influence = 0.f);

public:
  Observable<bool> onInfluenceChanged;

private:
  string_t _name;
  vector_t<Animation*> animations;
  Float32Array _positions;
  Float32Array _normals;
  Float32Array _tangents;
  float _influence;

}; // end of class MorphTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MORPH_MORPH_TARGET_H
