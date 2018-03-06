#ifndef BABYLON_MORPH_MORPH_TARGET_MANAGER_H
#define BABYLON_MORPH_MORPH_TARGET_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/morph/morph_target.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MorphTargetManager {

public:
  template <typename... Ts>
  static MorphTargetManager* New(Ts&&... args)
  {
    auto mtm = new MorphTargetManager(std::forward<Ts>(args)...);
    mtm->addToScene(static_cast<unique_ptr_t<MorphTargetManager>>(mtm));

    return mtm;
  }
  ~MorphTargetManager();

  void addToScene(unique_ptr_t<MorphTargetManager>&& newMorphTargetManager);

  size_t uniqueId() const;
  std::size_t vertexCount() const;
  bool supportsNormals() const;
  bool supportsTangents() const;
  std::size_t numTargets() const;
  size_t numInfluencers() const;
  Float32Array& influences();
  MorphTarget* getActiveTarget(std::size_t index);
  MorphTarget* getTarget(std::size_t index);
  void addTarget(unique_ptr_t<MorphTarget>&& target);
  void removeTarget(MorphTarget* target);

  /**
   * Serializes the current manager into a Serialization object.
   * Returns the serialized object.
   */
  Json::object serialize();

  // Statics
  static MorphTargetManager* Parse(const Json::value& serializationObject,
                                   Scene* scene);

protected:
  MorphTargetManager(Scene* scene);

private:
  void _syncActiveTargets(bool needUpdate);

private:
  vector_t<shared_ptr_t<MorphTarget>> _targets;
  vector_t<Observer<bool>::Ptr> _targetObservable;
  vector_t<MorphTarget*> _activeTargets;
  Scene* _scene;
  Float32Array _influences;
  bool _supportsNormals;
  bool _supportsTangents;
  std::size_t _vertexCount;
  size_t _uniqueId;
  Float32Array _tempInfluences;

}; // end of class MorphTargetManager

} // end of namespace BABYLON

#endif // end of BABYLON_MORPH_MORPH_TARGET_MANAGER_H
