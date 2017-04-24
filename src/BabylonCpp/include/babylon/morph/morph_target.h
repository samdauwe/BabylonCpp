#ifndef BABYLON_MORPH_MORPH_TARGET_H
#define BABYLON_MORPH_MORPH_TARGET_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MorphTarget {

public:
  MorphTarget(const std::string& name, float influence = 0.f);
  ~MorphTarget();

  float influence() const;
  void setInfluence(float influence);
  bool hasNormals() const;
  void setPositions(const Float32Array& data);
  Float32Array& getPositions();
  const Float32Array& getPositions() const;
  void setNormals(const Float32Array& data);
  Float32Array& getNormals();
  const Float32Array& getNormals() const;

  /**
   * @brief Serializes the current target into a Serialization object.
   * Returns the serialized object.
   */
  Json::object serialize() const;

  // Statics
  static std::unique_ptr<MorphTarget>
  Parse(const Json::value& serializationObject);
  static std::unique_ptr<MorphTarget>
  FromMesh(AbstractMesh* mesh, const std::string& name, float influence = 0.f);

public:
  Observable<bool> onInfluenceChanged;

private:
  std::string _name;
  Float32Array _positions;
  Float32Array _normals;
  float _influence;

}; // end of class MorphTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MORPH_MORPH_TARGET_H
