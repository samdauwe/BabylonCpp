#ifndef BABYLON_MORPH_MORPH_TARGET_MANAGER_H
#define BABYLON_MORPH_MORPH_TARGET_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MorphTargetManager {

public:
  template <typename... Ts>
  static MorphTargetManager* New(Ts&&... args)
  {
    auto mtm = new MorphTargetManager(std::forward<Ts>(args)...);
    mtm->addToScene(static_cast<std::unique_ptr<MorphTargetManager>>(mtm));

    return mtm;
  }
  ~MorphTargetManager();

  void addToScene(std::unique_ptr<MorphTargetManager>&& newMorphTargetManager);

  unsigned int uniqueId() const;
  size_t vertexCount() const;
  bool supportsNormals() const;
  size_t numInfluencers() const;
  Float32Array& influences();
  MorphTarget* getActiveTarget(size_t index);
  void addTarget(std::unique_ptr<MorphTarget>&& target);
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
  void _onInfluenceChanged(bool needUpdate);
  void _syncActiveTargets(bool needUpdate);

private:
  std::vector<std::unique_ptr<MorphTarget>> _targets;
  std::vector<Observer<bool>::Ptr> _targetObservable;
  std::vector<MorphTarget*> _activeTargets;
  Scene* _scene;
  Float32Array _influences;
  bool _supportsNormals;
  size_t _vertexCount;
  unsigned int _uniqueId;

}; // end of class MorphTargetManager

} // end of namespace BABYLON

#endif // end of BABYLON_MORPH_MORPH_TARGET_MANAGER_H
