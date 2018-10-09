#ifndef BABYLON_MORPH_MORPH_TARGET_MANAGER_H
#define BABYLON_MORPH_MORPH_TARGET_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/morph/morph_target.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief This class is used to deform meshes using morphing between different
 * targets.
 * @see http://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
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

  /**
   * @brief Gets the active target at specified index. An active target is a
   * target with an influence > 0
   * @param index defines the index to check
   * @returns the requested target
   */
  MorphTarget* getActiveTarget(std::size_t index);

  /**
   * @brief Gets the target at specified index.
   * @param index defines the index to check
   * @returns the requested target
   */
  MorphTarget* getTarget(std::size_t index);

  /**
   * @brief Add a new target to this manager.
   * @param target defines the target to add
   */
  void addTarget(std::unique_ptr<MorphTarget>&& target);

  /**
   * @brief Removes a target from the manager.
   * @param target defines the target to remove
   */
  void removeTarget(MorphTarget* target);

  /**
   * @brief Serializes the current manager into a Serialization object.
   * @returns the serialized object
   */
  Json::object serialize();

  /**
   * @brief Synchronize the targets with all the meshes using this morph target
   * manager.
   */
  void synchronize();

  // Statics
  static MorphTargetManager* Parse(const Json::value& serializationObject,
                                   Scene* scene);

protected:
  /**
   * @brief Creates a new MorphTargetManager.
   * @param scene defines the current scene
   */
  MorphTargetManager(Scene* scene);

  /**
   * @brief Gets the unique ID of this manager.
   */
  size_t get_uniqueId() const;

  /**
   * @brief Gets the number of vertices handled by this manager.
   */
  size_t get_vertexCount() const;

  /**
   * @brief Gets a boolean indicating if this manager supports morphing of
   * normals.
   */
  bool get_supportsNormals() const;

  /**
   * @brief Gets a boolean indicating if this manager supports morphing of
   * tangents.
   */
  bool get_supportsTangents() const;

  /**
   * @brief Gets the number of targets stored in this manager.
   */
  size_t get_numTargets() const;

  /**
   * @brief Gets the number of influencers (ie. the number of targets with
   * influences > 0)
   */
  size_t get_numInfluencers() const;

  /**
   * @brief Gets the list of influences (one per target).
   */
  Float32Array& get_influences();

  void _syncActiveTargets(bool needUpdate);

public:
  /**
   * Unique ID of this manager
   */
  ReadOnlyProperty<MorphTargetManager, size_t> uniqueId;

  /**
   * Number of vertices handled by this manager
   */
  ReadOnlyProperty<MorphTargetManager, size_t> vertexCount;

  /**
   * Boolean indicating if this manager supports morphing of normals
   */
  ReadOnlyProperty<MorphTargetManager, bool> supportsNormals;

  /**
   * Boolean indicating if this manager supports morphing of tangents
   */
  ReadOnlyProperty<MorphTargetManager, bool> supportsTangents;

  /**
   * Number of targets stored in this manager
   */
  ReadOnlyProperty<MorphTargetManager, size_t> numTargets;

  /**
   * Number of influencers (ie. the number of targets with influences > 0)
   */
  ReadOnlyProperty<MorphTargetManager, size_t> numInfluencers;

  /**
   * List of influences (one per target)
   */
  ReadOnlyProperty<MorphTargetManager, Float32Array> influences;

private:
  std::vector<MorphTargetPtr> _targets;
  std::vector<Observer<bool>::Ptr> _targetInfluenceChangedObservers;
  std::vector<Observer<void>::Ptr> _targetDataLayoutChangedObservers;
  std::vector<MorphTarget*> _activeTargets;
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
