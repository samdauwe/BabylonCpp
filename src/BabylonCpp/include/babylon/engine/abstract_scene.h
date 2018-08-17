#ifndef BABYLON_ENGINE_ABSTRACT_SCENE_H
#define BABYLON_ENGINE_ABSTRACT_SCENE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * Defines how the parser contract is defined.
 * These parsers are used to parse a list of specific assets (like particle
 * systems, etc..)
 */
using BabylonFileParser = std::function<void(
  Json::value& parsedData, Scene* scene, const string_t& rootUrl)>;

/**
 * Defines how the individual parser contract is defined.
 * These parser can parse an individual asset
 */
using IndividualBabylonFileParser = std::function<void(
  Json::value& parsedData, Scene* scene, const string_t& rootUrl)>;

/**
 * @brief Base class of the scene acting as a container for the different
 * elements composing a scene. This class is dynamically extended by the
 * different components of the scene increasing flexibility and reducing
 * coupling
 */
class BABYLON_SHARED_EXPORT AbstractScene
    : public ::std::enable_shared_from_this<AbstractScene> {

public:
  /**
   * @brief Adds a parser in the list of available ones.
   * @param name Defines the name of the parser
   * @param parser Defines the parser to add
   */
  static void AddParser(const string_t& name, const BabylonFileParser& parser);

  /**
   * @brief Gets a general parser from the list of avaialble ones.
   * @param name Defines the name of the parser
   * @returns the requested parser or null
   */
  static nullable_t<BabylonFileParser> GetParser(const string_t& name);

  /**
   * @brief Adds n individual parser in the list of available ones.
   * @param name Defines the name of the parser
   * @param parser Defines the parser to add
   */
  static void AddIndividualParser(const string_t& name,
                                  const IndividualBabylonFileParser& parser);

  /**
   * @brief Gets an individual parser from the list of avaialble ones.
   * @param name Defines the name of the parser
   * @returns the requested parser or null
   */
  static nullable_t<IndividualBabylonFileParser>
  GetIndividualParser(const string_t& name);

  /**
   * @brief Parser json data and populate both a scene and its associated
   * container object.
   * @param jsonData Defines the data to parse
   * @param scene Defines the scene to parse the data for
   * @param container Defines the container attached to the parsing sequence
   * @param rootUrl Defines the root url of the data
   */
  static void Parse(Json::value& jsonData, Scene* scene,
                    const string_t& rootUrl);

public:
  /**
   * All of the cameras added to this scene
   * @see http://doc.babylonjs.com/babylon101/cameras
   */
  vector_t<CameraPtr> cameras;

  /**
   * All of the lights added to this scene
   * @see http://doc.babylonjs.com/babylon101/lights
   */
  vector_t<LightPtr> lights;

  /**
   * All of the (abstract) meshes added to this scene
   */
  vector_t<AbstractMeshPtr> meshes;

  /**
   * The list of skeletons added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons
   */
  vector_t<SkeletonPtr> skeletons;

  /**
   * All of the particle systems added to this scene
   * @see http://doc.babylonjs.com/babylon101/particles
   */
  vector_t<IParticleSystemPtr> particleSystems;

  /**
   * Gets a list of Animations associated with the scene
   */
  vector_t<AnimationPtr> animations;

  /**
   * All of the animation groups added to this scene
   * @see http://doc.babylonjs.com/how_to/group
   */
  vector_t<AnimationGroupPtr> animationGroups;

  /**
   * All of the multi-materials added to this scene
   * @see http://doc.babylonjs.com/how_to/multi_materials
   */
  vector_t<MultiMaterialPtr> multiMaterials;

  /**
   * All of the materials added to this scene
   * @see http://doc.babylonjs.com/babylon101/materials
   */
  vector_t<MaterialPtr> materials;

  /**
   * The list of morph target managers added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_dynamically_morph_a_mesh
   */
  vector_t<MorphTargetManagerPtr> morphTargetManagers;

  /**
   * The list of geometries used in the scene.
   */
  vector_t<GeometryPtr> geometries;

  /**
   * All of the tranform nodes added to this scene
   * @see http://doc.babylonjs.com/how_to/transformnode
   */
  vector_t<TransformNodePtr> transformNodes;

  /**
   * ActionManagers available on the scene.
   */
  vector_t<ActionManagerPtr> actionManagers;

  /**
   * Sounds to keep.
   */
  vector_t<SoundPtr> sounds;

  /**
   * Textures to keep.
   */
  vector_t<BaseTexturePtr> textures;

private:
  /**
   * Stores the list of available parsers in the application.
   */
  static unordered_map_t<string_t, BabylonFileParser> _BabylonFileParsers;

  /**
   * Stores the list of available individual parsers in the application.
   */
  static unordered_map_t<string_t, IndividualBabylonFileParser>
    _IndividualBabylonFileParsers;

}; // end of class PointerInfo

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ABSTRACT_SCENE_H
