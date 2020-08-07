#ifndef BABYLON_SPRITES_SPRITE_MANAGER_H
#define BABYLON_SPRITES_SPRITE_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>
#include <babylon/sprites/isprite_manager.h>
#include <babylon/sprites/sprite.h>

namespace BABYLON {

class Buffer;
class Camera;
class Effect;
class PickingInfo;
class Ray;
class SpriteManager;
class Texture;
class VertexBuffer;
class WebGLDataBuffer;
using CameraPtr          = std::shared_ptr<Camera>;
using EffectPtr          = std::shared_ptr<Effect>;
using SpriteManagerPtr   = std::shared_ptr<SpriteManager>;
using TexturePtr         = std::shared_ptr<Texture>;
using VertexBufferPtr    = std::shared_ptr<VertexBuffer>;
using WebGLDataBufferPtr = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief Class used to manage multiple sprites on the same spritesheet.
 * @see https://doc.babylonjs.com/babylon101/sprites
 */
class BABYLON_SHARED_EXPORT SpriteManager : public ISpriteManager {

public:
  /**
   * Define the Url to load snippets
   */
  static constexpr const char* SnippetUrl = "https://snippet.babylonjs.com";

  /**
   * Snippet ID if the manager was created from the snippet server
   */
  std::string snippetId;

public:
  template <typename... Ts>
  static SpriteManagerPtr New(Ts&&... args)
  {
    auto spriteManager
      = std::shared_ptr<SpriteManager>(new SpriteManager(std::forward<Ts>(args)...));
    spriteManager->addToScene(spriteManager);

    return spriteManager;
  }
  ~SpriteManager() override; // = default

  /**
   * @brief Hidden
   */
  void addToScene(const SpriteManagerPtr& newSpriteManager);

  /**
   * @brief Returns the string "SpriteManager".
   * @returns "SpriteManager"
   */
  std::string getClassName() const;

  /**
   * @brief Intersects the sprites with a ray.
   * @param ray defines the ray to intersect with
   * @param camera defines the current active camera
   * @param predicate defines a predicate used to select candidate sprites
   * @param fastCheck defines if a fast check only must be done (the first potential sprite is will
   * be used and not the closer)
   * @returns null if no hit or a PickingInfo
   */
  std::optional<PickingInfo> intersects(const Ray& ray, const CameraPtr& camera,
                                        const std::function<bool(Sprite* sprite)>& predicate,
                                        bool fastCheck) override;

  /**
   * @brief Intersects the sprites with a ray.
   * @param ray defines the ray to intersect with
   * @param camera defines the current active camera
   * @param predicate defines a predicate used to select candidate sprites
   * @returns null if no hit or a PickingInfo array
   */
  std::vector<PickingInfo>
  multiIntersects(const Ray& ray, const CameraPtr& camera,
                  const std::function<bool(Sprite* sprite)>& predicate) override;

  /**
   * @brief Render all child sprites.
   */
  void render() override;

  /**
   * @brief Release associated resources.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Serializes the sprite manager to a JSON object.
   * @param serializeTexture defines if the texture must be serialized as well
   * @returns the JSON object
   */
  json serialize(bool serializeTexture = false) const;

  /**
   * @brief Parses a JSON object to create a new sprite manager.
   * @param parsedManager The JSON object to parse
   * @param scene The scene to create the sprite managerin
   * @param rootUrl The root url to use to load external dependencies like texture
   * @returns the new sprite manager
   */
  static SpriteManagerPtr Parse(const json& parsedManager, Scene* scene,
                                const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new sprite manager.
   * @param name defines the manager's name
   * @param imgUrl defines the sprite sheet url
   * @param capacity defines the maximum allowed number of sprites
   * @param cellSize defines the size of a sprite cell
   * @param scene defines the hosting scene
   * @param epsilon defines the epsilon value to align texture (0.01 by default)
   * @param samplingMode defines the smapling mode to use with spritesheet
   * @param fromPacked set to false; do not alter
   * @param spriteJSON null otherwise a JSON object defining sprite sheet data; do not alter
   */
  SpriteManager(const std::string& name, const std::string& imgUrl, unsigned int capacity,
                const ISize& cellSize, Scene* scene, float epsilon = 0.01f,
                unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
                bool fromPacked = false, const std::string& spriteJSON = "");

  /**
   * @brief Sets the callback called when the manager is disposed.
   */
  void set_onDispose(const std::function<void(SpriteManager*, EventState&)>& callback);

  /**
   * @brief Gets the array of sprites
   */
  std::vector<SpritePtr>& get_children();

  /**
   * @brief Gets the hosting scene
   */
  Scene*& get_scene() override;

  /**
   * @brief Gets the capacity of the manager
   */
  size_t get_capacity() const;

  /**
   * @brief Gets the spritesheet texture.
   */
  TexturePtr& get_texture() override;

  /**
   * @brief Sets the spritesheet texture.
   */
  void set_texture(const TexturePtr& value) override;

  /**
   * @brief Gets theBlend mode use to render the particle, it can be any of the static
   * Constants.ALPHA_x properties provided in this class. Default value is Constants.ALPHA_COMBINE
   */
  unsigned int get_blendMode() const;

  /**
   * @brief Sets theBlend mode use to render the particle, it can be any of the static
   * Constants.ALPHA_x properties provided in this class. Default value is Constants.ALPHA_COMBINE
   */
  void set_blendMode(unsigned int blendMode);

private:
  void _makePacked(const std::string& imgUrl, const std::string& spriteJSON);
  void _appendSpriteVertex(size_t index, Sprite& sprite, int offsetX, int offsetY,
                           const ISize& baseSize);
  bool _checkTextureAlpha(Sprite& sprite, const Ray& ray, float distance, const Vector3& min,
                          const Vector3& max);

public:
  /**
   * Defines the manager's name
   */
  std::string name;

  /**
   * A JSON object defining sprite sheet data
   */
  std::string spriteJSON;

  /**
   * Gets or sets a boolean indicating if the manager must consider scene fog when rendering
   */
  bool fogEnabled;

  /**
   * An event triggered when the manager is disposed.
   */
  Observable<SpriteManager> onDisposeObservable;

  /**
   * Callback called when the manager is disposed
   */
  WriteOnlyProperty<SpriteManager, std::function<void(SpriteManager*, EventState&)>> onDispose;

  /**
   * Gets or sets the unique id of the sprite
   */
  size_t uniqueId;

  /**
   * Gets the array of sprites
   */
  ReadOnlyProperty<SpriteManager, std::vector<SpritePtr>> children;

  /**
   * Gets the capacity of the manager
   */
  ReadOnlyProperty<SpriteManager, size_t> capacity;

  /**
   * Blend mode use to render the particle, it can be any of
   * the static Constants.ALPHA_x properties provided in this class.
   * Default value is Constants.ALPHA_COMBINE
   */
  Property<SpriteManager, unsigned int> blendMode;

  /**
   * Disables writing to the depth buffer when rendering the sprites.
   * It can be handy to disable depth writing when using textures without alpha channel
   * and setting some specific blend modes.
   */
  bool disableDepthWrite;

private:
  /**
   * Associative array from JSON sprite data file
   */
  std::string _cellData;

  /**
   * Array of sprite names from JSON sprite data file
   */
  std::vector<std::string> _spriteMap;

  /**
   * True when packed cell data from JSON file is ready
   */
  bool _packedAndReady;

  Uint8Array _textureContent;

  Observer<SpriteManager>::Ptr _onDisposeObserver;
  size_t _capacity;
  bool _fromPacked;
  TexturePtr _spriteTexture;
  float _epsilon;
  Scene* _scene;
  Float32Array _vertexData;
  std::unique_ptr<Buffer> _buffer;
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  WebGLDataBufferPtr _indexBuffer;
  EffectPtr _effectBase;
  EffectPtr _effectFog;
  unsigned int _blendMode;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_MANAGER_H
