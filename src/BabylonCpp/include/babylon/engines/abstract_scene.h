#ifndef BABYLON_ENGINES_ABSTRACT_SCENE_H
#define BABYLON_ENGINES_ABSTRACT_SCENE_H

#include <functional>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/any.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractActionManager;
class AbstractMesh;
class Animation;
class AnimationGroup;
class AssetContainer;
class BaseTexture;
class Camera;
class EffectLayer;
class Geometry;
class GlowLayer;
class HighlightLayer;
class IParticleSystem;
class Layer;
class LensFlareSystem;
class Light;
class Material;
class MorphTargetManager;
class MultiMaterial;
class Node;
class ReflectionProbe;
class Scene;
class Skeleton;
class Sound;
class TransformNode;
using AbstractActionManagerPtr = std::shared_ptr<AbstractActionManager>;
using AbstractMeshPtr          = std::shared_ptr<AbstractMesh>;
using AnimationPtr             = std::shared_ptr<Animation>;
using AnimationGroupPtr        = std::shared_ptr<AnimationGroup>;
using AssetContainerPtr        = std::shared_ptr<AssetContainer>;
using BaseTexturePtr           = std::shared_ptr<BaseTexture>;
using CameraPtr                = std::shared_ptr<Camera>;
using EffectLayerPtr           = std::shared_ptr<EffectLayer>;
using GeometryPtr              = std::shared_ptr<Geometry>;
using GlowLayerPtr             = std::shared_ptr<GlowLayer>;
using HighlightLayerPtr        = std::shared_ptr<HighlightLayer>;
using IParticleSystemPtr       = std::shared_ptr<IParticleSystem>;
using LayerPtr                 = std::shared_ptr<Layer>;
using LensFlareSystemPtr       = std::shared_ptr<LensFlareSystem>;
using LightPtr                 = std::shared_ptr<Light>;
using MaterialPtr              = std::shared_ptr<Material>;
using MorphTargetManagerPtr    = std::shared_ptr<MorphTargetManager>;
using MultiMaterialPtr         = std::shared_ptr<MultiMaterial>;
using NodePtr                  = std::shared_ptr<Node>;
using ReflectionProbePtr       = std::shared_ptr<ReflectionProbe>;
using SkeletonPtr              = std::shared_ptr<Skeleton>;
using SoundPtr                 = std::shared_ptr<Sound>;
using TransformNodePtr         = std::shared_ptr<TransformNode>;

/**
 * Defines how the parser contract is defined.
 * These parsers are used to parse a list of specific assets (like particle
 * systems, etc..)
 */
using BabylonFileParser
  = std::function<void(const nlohmann::json& parsedData, Scene* scene,
                       AssetContainer& container, const std::string& rootUrl)>;

/**
 * Defines how the individual parser contract is defined.
 * These parser can parse an individual asset
 */
using IndividualBabylonFileParser = std::function<any(
  const nlohmann::json& parsedData, Scene* scene, const std::string& rootUrl)>;

/**
 * @brief Base class of the scene acting as a container for the different
 * elements composing a scene. This class is dynamically extended by the
 * different components of the scene increasing flexibility and reducing
 * coupling
 */
class BABYLON_SHARED_EXPORT AbstractScene
    : public std::enable_shared_from_this<AbstractScene> {

public:
  /**
   * @brief Constructor
   */
  AbstractScene();
  virtual ~AbstractScene(); // = default

  /**
   * @brief Adds a parser in the list of available ones.
   * @param name Defines the name of the parser
   * @param parser Defines the parser to add
   */
  static void AddParser(const std::string& name,
                        const BabylonFileParser& parser);

  /**
   * @brief Gets a general parser from the list of avaialble ones.
   * @param name Defines the name of the parser
   * @returns the requested parser or null
   */
  static std::optional<BabylonFileParser> GetParser(const std::string& name);

  /**
   * @brief Adds n individual parser in the list of available ones.
   * @param name Defines the name of the parser
   * @param parser Defines the parser to add
   */
  static void AddIndividualParser(const std::string& name,
                                  const IndividualBabylonFileParser& parser);

  /**
   * @brief Gets an individual parser from the list of avaialble ones.
   * @param name Defines the name of the parser
   * @returns the requested parser or null
   */
  static std::optional<IndividualBabylonFileParser>
  GetIndividualParser(const std::string& name);

  /**
   * @brief Parser json data and populate both a scene and its associated
   * container object.
   * @param jsonData Defines the data to parse
   * @param scene Defines the scene to parse the data for
   * @param container Defines the container attached to the parsing sequence
   * @param rootUrl Defines the root url of the data
   */
  static void Parse(const nlohmann::json& jsonData, Scene* scene,
                    const AssetContainerPtr& container,
                    const std::string& rootUrl);

  /**
   * @brief Removes the given effect layer from this scene.
   * @param toRemove defines the effect layer to remove
   * @returns the index of the removed effect layer
   */
  int removeEffectLayer(const EffectLayerPtr& toRemove);

  /**
   * @brief Adds the given effect layer to this scene.
   * @param newEffectLayer defines the effect layer to add
   */
  void addEffectLayer(const EffectLayerPtr& newEffectLayer);

  /**
   * @brief Return a the first highlight layer of the scene with a given name.
   * @param name The name of the highlight layer to look for.
   * @return The highlight layer if found otherwise null.
   */
  GlowLayerPtr getGlowLayerByName(const std::string& name);

  /**
   * @brief Return a the first highlight layer of the scene with a given name.
   * @param name The name of the highlight layer to look for.
   * @return The highlight layer if found otherwise null.
   */
  HighlightLayerPtr getHighlightLayerByName(const std::string& name);

  /**
   * @brief Removes the given lens flare system from this scene.
   * @param toRemove The lens flare system to remove
   * @returns The index of the removed lens flare system
   */
  int removeLensFlareSystem(const LensFlareSystemPtr& toRemove);

  /**
   * @brief Adds the given lens flare system to this scene.
   * @param newLensFlareSystem The lens flare system to add
   */
  void addLensFlareSystem(const LensFlareSystemPtr& newLensFlareSystem);

  /**
   * @brief Gets a lens flare system using its name.
   * @param name defines the name to look for
   * @returns the lens flare system or null if not found
   */
  LensFlareSystemPtr getLensFlareSystemByName(const std::string& name);

  /**
   * @brief Gets a lens flare system using its id.
   * @param id defines the id to look for
   * @returns the lens flare system or null if not found
   */
  LensFlareSystemPtr getLensFlareSystemByID(const std::string& id);

  /**
   * @brief Removes the given reflection probe from this scene.
   * @param toRemove The reflection probe to remove
   * @returns The index of the removed reflection probe
   */
  int removeReflectionProbe(const ReflectionProbePtr& toRemove);

  /**
   * @brief Adds the given reflection probe to this scene.
   * @param newReflectionProbe The reflection probe to add
   */
  void addReflectionProbe(const ReflectionProbePtr& newReflectionProbe);

private:
  /**
   * @brief Adds the individual component parser to the scene parsers.
   */
  void _addIndividualParsers();

  /**
   * @brief Adds the component parser to the scene parsers.
   */
  void _addParsers();

public:
  /**
   * Gets the list of root nodes (ie. nodes with no parent)
   */
  std::vector<NodePtr> rootNodes;

  /**
   * All of the cameras added to this scene
   * @see http://doc.babylonjs.com/babylon101/cameras
   */
  std::vector<CameraPtr> cameras;

  /**
   * All of the lights added to this scene
   * @see http://doc.babylonjs.com/babylon101/lights
   */
  std::vector<LightPtr> lights;

  /**
   * All of the (abstract) meshes added to this scene
   */
  std::vector<AbstractMeshPtr> meshes;

  /**
   * The list of skeletons added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons
   */
  std::vector<SkeletonPtr> skeletons;

  /**
   * All of the particle systems added to this scene
   * @see http://doc.babylonjs.com/babylon101/particles
   */
  std::vector<IParticleSystemPtr> particleSystems;

  /**
   * Gets a list of Animations associated with the scene
   */
  std::vector<AnimationPtr> animations;

  /**
   * All of the animation groups added to this scene
   * @see http://doc.babylonjs.com/how_to/group
   */
  std::vector<AnimationGroupPtr> animationGroups;

  /**
   * All of the multi-materials added to this scene
   * @see http://doc.babylonjs.com/how_to/multi_materials
   */
  std::vector<MultiMaterialPtr> multiMaterials;

  /**
   * All of the materials added to this scene
   * In the context of a Scene, it is not supposed to be modified manually.
   * Any addition or removal should be done using the addMaterial and
   * removeMAterial Scene methods. Note also that the order of the Material
   * wihin the array is not significant and might change.
   * @see http://doc.babylonjs.com/babylon101/materials
   */
  std::vector<MaterialPtr> materials;

  /**
   * The list of morph target managers added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_dynamically_morph_a_mesh
   */
  std::vector<MorphTargetManagerPtr> morphTargetManagers;

  /**
   * The list of geometries used in the scene.
   */
  std::vector<GeometryPtr> geometries;

  /**
   * All of the tranform nodes added to this scene
   * In the context of a Scene, it is not supposed to be modified manually.
   * Any addition or removal should be done using the addTransformNode and
   * removeTransformNode Scene methods. Note also that the order of the
   * TransformNode wihin the array is not significant and might change.
   * @see http://doc.babylonjs.com/how_to/transformnode
   */
  std::vector<TransformNodePtr> transformNodes;

  /**
   * ActionManagers available on the scene.
   */
  std::vector<AbstractActionManagerPtr> actionManagers;

  /**
   * Textures to keep.
   */
  std::vector<BaseTexturePtr> textures;

  /**
   * The list of effect layers (highlights/glow) added to the scene
   * @see http://doc.babylonjs.com/how_to/highlight_layer
   * @see http://doc.babylonjs.com/how_to/glow_layer
   */
  std::vector<EffectLayerPtr> effectLayers;

  /**
   * The list of layers (background and foreground) of the scene
   */
  std::vector<LayerPtr> layers;

  /**
   * The list of lens flare system added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_lens_flares
   */
  std::vector<LensFlareSystemPtr> lensFlareSystems;

  /**
   * The list of reflection probes added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_reflection_probes
   */
  std::vector<ReflectionProbePtr> reflectionProbes;

  /**
   * The list of sounds used in the scene
   */
  std::vector<SoundPtr> sounds;

  /**
   * Environment texture for the scene
   */
  Property<AbstractScene, BaseTexturePtr> environmentTexture;

protected:
  /**
   * @brief Returns the environment texture for the scene.
   */
  virtual BaseTexturePtr& get_environmentTexture();

  /**
   * @brief Sets the environment texture for the scene.
   */
  virtual void set_environmentTexture(const BaseTexturePtr& value);

private:
  /**
   * Stores the list of available parsers in the application.
   */
  static std::unordered_map<std::string, BabylonFileParser> _BabylonFileParsers;

  /**
   * Stores the list of available individual parsers in the application.
   */
  static std::unordered_map<std::string, IndividualBabylonFileParser>
    _IndividualBabylonFileParsers;

  /** Hidden */
  BaseTexturePtr _environmentTexture;

}; // end of class AbstractScene

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ABSTRACT_SCENE_H
