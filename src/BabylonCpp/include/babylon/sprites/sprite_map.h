#ifndef BABYLON_SPRITES_SPRITE_MAP_H
#define BABYLON_SPRITES_SPRITE_MAP_H

#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/sprites/isprite_map.h>

namespace BABYLON {

class BaseTexture;
class Mesh;
class RawTexture;
class Scene;
class ShaderMaterial;
using BaseTexturePtr    = std::shared_ptr<BaseTexture>;
using MeshPtr           = std::shared_ptr<Mesh>;
using RawTexturePtr     = std::shared_ptr<RawTexture>;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;

/**
 * @brief Class used to manage a grid restricted sprite deployment on an Output plane.
 */
class BABYLON_SHARED_EXPORT SpriteMap : public ISpriteMap {

public:
  /**
   * @brief Creates a new SpriteMap.
   * @param name defines the SpriteMaps Name
   * @param atlasJSON is the JSON file that controls the Sprites Frames and Meta
   * @param spriteSheet is the Texture that the Sprites are on.
   * @param options a basic deployment configuration
   * @param scene The Scene that the map is deployed on
   */
  SpriteMap(const std::string& name, const ISpriteJSONAtlas& atlasJSON,
            const TexturePtr& spriteSheet, const ISpriteMapOptions& options, Scene* scene);
  ~SpriteMap() override; // = default

  /**
   * @brief Returns tileID location.
   * @returns Vector2 the cell position ID
   */
  Vector2 getTileID();

  /**
   * @brief Gets the UV location of the mouse over the SpriteMap.
   * @returns Vector2 the UV position of the mouse interaction
   */
  Vector2 getMousePosition();

  /**
   * @brief Modifies the data of the animationMap.
   * @param cellID is the Index of the Sprite
   * @param _frame is the target Animation frame
   * @param toCell is the Target Index of the next frame of the animation
   * @param time is a value between 0-1 that is the trigger for when the frame should change tiles
   * @param speed is a global scalar of the time variable on the map.
   */
  void addAnimationToTile(size_t cellID = 0, size_t _frame = 0, size_t toCell = 0, float time = 0.f,
                          float speed = 1.f);

  /**
   * @brief Exports the .tilemaps file.
   */
  void saveTileMaps();

  /**
   * @brief Imports the .tilemaps file.
   * @param url of the .tilemaps file
   */
  void loadTileMaps(const std::string& url);

  /**
   * @brief Release associated resources.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Returns the Number of Sprites in the System.
   */
  size_t get_spriteCount() const;

  /**
   * @brief Returns the Position of Output Plane.
   */
  Vector3& get_position();

  /**
   * @brief Returns the Position of Output Plane.
   */
  void set_position(const Vector3& v);

  /**
   * @brief Sets the Rotation of Output Plane.
   */
  Vector3& get_rotation();

  /**
   * @brief Returns the Rotation of Output Plane.
   */
  void set_rotation(const Vector3& v);

  /**
   * @brief Sets the AnimationMap.
   */
  RawTexturePtr& get_animationMap();

  /**
   * @brief Sets the AnimationMap.
   */
  void set_animationMap(const RawTexturePtr& v);

  /**
   * @brief Modifies the data of the tileMaps.
   * @param _layer is the ID of the layer you want to edit on the SpriteMap
   * @param pos is the iVector2 Coordinates of the Tile
   * @param tile The SpriteIndex of the new Tile
   */
  void changeTiles(size_t _layer = 0, const Vector2& pos = Vector2(), size_t tile = 0);

  /**
   * @brief Modifies the data of the tileMaps.
   * @param _layer is the ID of the layer you want to edit on the SpriteMap
   * @param pos is the iVector2 Coordinates of the Tile
   * @param tile The SpriteIndex of the new Tile
   */
  void changeTiles(size_t _layer = 0, const std::vector<Vector2>& pos = {}, size_t tile = 0);

private:
  /**
   * @brief Creates the "frame" texture Buffer.
   * ------------------------------------------
   * Structure of frames
   *  "filename": "Falling-Water-2.png",
   * "frame": {"x":69,"y":103,"w":24,"h":32},
   * "rotated": true,
   * "trimmed": true,
   * "spriteSourceSize": {"x":4,"y":0,"w":24,"h":32},
   * "sourceSize": {"w":32,"h":32}
   * @returns RawTexture of the frameMap
   */
  RawTexturePtr _createFrameBuffer();

  /**
   * @brief Creates the tileMap texture Buffer.
   * @param buffer normally and array of numbers, or a false to generate from scratch
   * @param _layer indicates what layer for a logic trigger dealing with the baseTile.  The system
   * uses this
   * @returns RawTexture of the tileMap
   */
  RawTexturePtr _createTileBuffer(const std::variant<bool, Float32Array>& buffer,
                                  size_t _layer = 0);

  /**
   * @brief Creates the animationMap texture Buffer.
   * @param buffer normally and array of numbers, or a false to generate from scratch
   * @returns RawTexture of the animationMap
   */
  RawTexturePtr _createTileAnimationBuffer(const std::optional<ArrayBufferView>& buffer);

  /**
   * @brief Hidden
   */
  std::vector<BaseTexturePtr> _getTileMaps() const;

public:
  /**
   * Public Sprite Storage array, parsed from atlasJSON
   */
  std::vector<ISpriteJSONSprite> sprites;

  /**
   * Returns the Number of Sprites in the System
   */
  ReadOnlyProperty<SpriteMap, size_t> spriteCount;

  /**
   * Returns or sets the Position of Output Plane
   */
  Property<SpriteMap, Vector3> position;

  /**
   * Sets the Rotation of Output Plane
   */
  Property<SpriteMap, Vector3> rotation;

  /**
   * Sets the AnimationMap
   */
  Property<SpriteMap, RawTexturePtr> animationMap;

private:
  /**
   * Scene that the SpriteMap was created in
   */
  Scene* _scene;

  /**
   * Texture Buffer of Float32 that holds tile frame data
   */
  RawTexturePtr _frameMap;

  /**
   * Texture Buffers of Float32 that holds tileMap data
   */
  std::vector<RawTexturePtr> _tileMaps;

  /**
   * Texture Buffer of Float32 that holds Animation Data
   */
  RawTexturePtr _animationMap;

  /**
   * Custom ShaderMaterial Central to the System
   */
  ShaderMaterialPtr _material;

  /**
   * Custom ShaderMaterial Central to the System
   */
  MeshPtr _output;

  /**
   * Systems Time Ticker
   */
  float _time;

}; // end of class SpriteJSONSprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_MAP_H
