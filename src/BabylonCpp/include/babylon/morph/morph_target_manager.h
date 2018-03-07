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
  size_t get_uniqueId() const;
  size_t get_vertexCount() const;
  bool get_supportsNormals() const;
  bool get_supportsTangents() const;
  size_t get_numTargets() const;
  size_t get_numInfluencers() const;
  Float32Array& get_influences();

  void _syncActiveTargets(bool needUpdate);

public:
  ReadOnlyProperty<MorphTargetManager, size_t> uniqueId;
  ReadOnlyProperty<MorphTargetManager, size_t> vertexCount;
  ReadOnlyProperty<MorphTargetManager, bool> supportsNormals;
  ReadOnlyProperty<MorphTargetManager, bool> supportsTangents;
  ReadOnlyProperty<MorphTargetManager, size_t> numTargets;
  ReadOnlyProperty<MorphTargetManager, size_t> numInfluencers;
  ReadOnlyProperty<MorphTargetManager, Float32Array> influences;

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
