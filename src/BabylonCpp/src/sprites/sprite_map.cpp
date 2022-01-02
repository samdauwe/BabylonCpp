#include <babylon/sprites/sprite_map.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

SpriteMap::SpriteMap(const std::string& iName, const ISpriteJSONAtlas& iAtlasJSON,
                     const TexturePtr& iSpriteSheet, const ISpriteMapOptions& iOptions,
                     Scene* scene)
    : spriteCount{this, &SpriteMap::get_spriteCount}
    , position{this, &SpriteMap::get_position, &SpriteMap::set_position}
    , rotation{this, &SpriteMap::get_rotation, &SpriteMap::set_rotation}
    , animationMap{this, &SpriteMap::get_animationMap, &SpriteMap::set_animationMap}
    , _frameMap{nullptr}
    , _animationMap{nullptr}
    , _material{nullptr}
    , _output{nullptr}
{
  name        = iName;
  sprites     = {};
  atlasJSON   = iAtlasJSON;
  sprites     = iAtlasJSON.frames;
  spriteSheet = iSpriteSheet;

  /**
   * Run through the options and set what ever defaults are needed that where not declared.
   */
  options                    = iOptions;
  options.stageSize          = options.stageSize.value_or(Vector2(1.f, 1.f));
  options.outputSize         = options.outputSize.value_or(*options.stageSize);
  options.outputPosition     = options.outputPosition.value_or(Vector3::Zero());
  options.outputRotation     = options.outputRotation.value_or(Vector3::Zero());
  options.layerCount         = options.layerCount.value_or(1);
  options.maxAnimationFrames = options.maxAnimationFrames.value_or(0);
  options.baseTile           = options.baseTile.value_or(0);
  options.flipU              = options.flipU.value_or(false);
  options.colorMultiply      = options.colorMultiply.value_or(Vector3(1.f, 1.f, 1.f));

  _scene = scene;

  _frameMap = _createFrameBuffer();

  _tileMaps = {};
  for (size_t i = 0; i < options.layerCount; ++i) {
    _tileMaps.emplace_back(_createTileBuffer(false, i));
  }

  _animationMap = _createTileAnimationBuffer(std::nullopt);

  std::vector<std::string> defines;
  defines.emplace_back("#define LAYERS " + std::to_string(*options.layerCount));

  if (options.flipU) {
    defines.emplace_back("#define FLIPU");
  }

  defines.emplace_back(
    StringTools::printf("#define MAX_ANIMATION_FRAMES %u.0", *options.maxAnimationFrames));

  auto shaderString = Effect::ShadersStore()["spriteMapPixelShader"];

  std::string layerSampleString;
  if (_scene->getEngine()->webGLVersion == 1.f) {
    layerSampleString = "";
    for (size_t i = 0; i < options.layerCount; ++i) {
      const auto iStr = std::to_string(i);
      layerSampleString += StringTools::printf(
        "if (%s == i) { frameID = texture2D(tileMaps[%s], (tileID + 0.5) / stageSize, 0.).x; }",
        iStr.c_str(), iStr.c_str());
    }
  }
  else {
    layerSampleString = "switch(i) {";
    for (size_t i = 0; i < options.layerCount; ++i) {
      const auto iStr = std::to_string(i);
      layerSampleString += "case " + iStr + " : frameID = texture(tileMaps[" + iStr
                           + "], (tileID + 0.5) / stageSize, 0.).x;";
      layerSampleString += "break;";
    }
    layerSampleString += "}";
  }

  Effect::ShadersStore()[StringTools::printf("spriteMap%sPixelShader", name.c_str())]
    = StringTools::replace(shaderString, "#define LAYER_ID_SWITCH", layerSampleString);

  // Create shader material
  IShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.defines    = defines;
  shaderMaterialOptions.attributes = {"position", "normal", "uv"};
  shaderMaterialOptions.uniforms = {"worldViewProjection", "time",        "stageSize", "outputSize",
                                    "spriteMapSize",       "spriteCount", "time",      "colorMul",
                                    "mousePosition",       "curTile",     "flipU"};
  shaderMaterialOptions.samplers = {"spriteSheet", "frameMap", "tileMaps", "animationMap"};
  shaderMaterialOptions.needAlphaBlending = true;
  _material
    = ShaderMaterial::New("spriteMap:" + name, scene, "spriteMap" + name, shaderMaterialOptions);

  _time = 0.f;

  _material->setFloat("spriteCount", static_cast<float>(spriteCount));
  _material->setVector2("stageSize", options.stageSize.value_or(0.f));
  _material->setVector2("outputSize", options.outputSize.value_or(0.f));
  _material->setTexture("spriteSheet", spriteSheet);
  _material->setVector2("spriteMapSize", Vector2(1.f, 1.f));
  _material->setVector3("colorMul", *options.colorMultiply);

  const std::function<void()> bindSpriteTexture = [this, iSpriteSheet]() -> void {
    if ((iSpriteSheet) && iSpriteSheet->isReady()) {
      if (iSpriteSheet->_texture) {
        _material->setVector2("spriteMapSize", Vector2(iSpriteSheet->_texture->baseWidth,
                                                       iSpriteSheet->_texture->baseHeight));
        return;
      }
    }
  };

  bindSpriteTexture();

  _material->setVector3("colorMul", *options.colorMultiply);
  _material->setTexture("frameMap", _frameMap);
  _material->setTextureArray("tileMaps", _getTileMaps());
  _material->setTexture("animationMap", _animationMap);
  _material->setFloat("time", _time);

  _output              = Mesh::CreatePlane(name + ":output", 1, scene, true);
  _output->scaling().x = options.outputSize->x;
  _output->scaling().y = options.outputSize->y;
  position             = *options.outputPosition;
  rotation             = *options.outputRotation;

  const auto obfunction = [this](Scene* /*scene*/, EventState& /*es*/) -> void {
    _time += _scene->getEngine()->getDeltaTime();
    _material->setFloat("time", _time);
  };

  _scene->onBeforeRenderObservable.add(obfunction);
  _output->material = _material;
}

SpriteMap::~SpriteMap() = default;

size_t SpriteMap::get_spriteCount() const
{
  return sprites.size();
}

Vector3& SpriteMap::get_position()
{
  return _output->position();
}

void SpriteMap::set_position(const Vector3& v)
{
  _output->position = v;
}

Vector3& SpriteMap::get_rotation()
{
  return _output->rotation();
}

void SpriteMap::set_rotation(const Vector3& v)
{
  _output->rotation = v;
}

RawTexturePtr& SpriteMap::get_animationMap()
{
  return _animationMap;
}

void SpriteMap::set_animationMap(const RawTexturePtr& v)
{
  auto buffer = v->_texture->_bufferView;
  auto am     = _createTileAnimationBuffer(buffer);
  _animationMap->dispose();
  _animationMap = am;
  _material->setTexture("animationMap", _animationMap);
}

Vector2 SpriteMap::getTileID()
{
  auto p = getMousePosition();
  p.multiplyInPlace(options.stageSize.value_or(Vector2::Zero()));
  p.x = std::floor(p.x);
  p.y = std::floor(p.y);
  return p;
}

Vector2 SpriteMap::getMousePosition()
{
  const auto& out = _output;
  auto pickinfo
    = _scene->pick(_scene->pointerX, _scene->pointerY, [out](const AbstractMeshPtr& mesh) -> bool {
        if (mesh != out) {
          return false;
        }
        return true;
      });

  if (((!pickinfo) || !pickinfo->hit) || !pickinfo->getTextureCoordinates()) {
    return Vector2(-1.f, -1.f);
  }

  auto coords = pickinfo->getTextureCoordinates();
  if (coords) {
    return *coords;
  }

  return Vector2(-1.f, -1.f);
}

RawTexturePtr SpriteMap::_createFrameBuffer()
{
  Float32Array data;
  // Do two Passes
  for (size_t i = 0; i < spriteCount; i++) {
    stl_util::concat(data, {0.f, 0.f, 0.f, 0.f}); // frame
    stl_util::concat(data, {0.f, 0.f, 0.f, 0.f}); // spriteSourceSize
    stl_util::concat(data, {0.f, 0.f, 0.f, 0.f}); // sourceSize, rotated, trimmed
    stl_util::concat(
      data, {0.f, 0.f, 0.f,
             0.f}); // Keep it pow2 cause I'm cool like that... it helps with sampling accuracy as
                    // well. Plus then we have 4 other parameters for future stuff.
  }
  // Second Pass
  data.resize((spriteCount - 1) * 4 + 3 + (spriteCount * 8) + 1);
  for (size_t i = 0; i < spriteCount; i++) {
    const auto& f   = sprites[i].frame;
    const auto& sss = sprites[i].spriteSourceSize;
    const auto& ss  = sprites[i].sourceSize;
    const auto r    = (sprites[i].rotated) ? 1.f : 0.f;
    const auto t    = (sprites[i].trimmed) ? 1.f : 0.f;

    // frame
    data[i * 4]     = f.x;
    data[i * 4 + 1] = f.y;
    data[i * 4 + 2] = f.w;
    data[i * 4 + 3] = f.h;
    // spriteSourceSize
    data[i * 4 + (spriteCount * 4)]     = sss.x;
    data[i * 4 + 1 + (spriteCount * 4)] = sss.y;
    data[i * 4 + 3 + (spriteCount * 4)] = sss.h;
    // sourceSize, rotated, trimmed
    data[i * 4 + (spriteCount * 8)]     = static_cast<float>(ss.w);
    data[i * 4 + 1 + (spriteCount * 8)] = static_cast<float>(ss.h);
    data[i * 4 + 2 + (spriteCount * 8)] = r;
    data[i * 4 + 3 + (spriteCount * 8)] = t;
  }

  const auto& floatArray = data;

  auto t = RawTexture::CreateRGBATexture(floatArray,                        // data
                                         static_cast<int>(spriteCount),     // width
                                         4,                                 // height
                                         _scene,                            // scene
                                         false,                             // generateMipMaps
                                         false,                             // invertY
                                         TextureConstants::NEAREST_NEAREST, // samplingMode
                                         Constants::TEXTURETYPE_FLOAT       // type
  );

  return t;
}

RawTexturePtr SpriteMap::_createTileBuffer(const std::variant<bool, Float32Array>& buffer,
                                           size_t _layer)
{
  Float32Array data;
  auto _ty = static_cast<int>(options.stageSize ? (*options.stageSize).y : 0.f);
  auto _tx = static_cast<int>(options.stageSize ? (*options.stageSize).x : 0.f);

  if (std::holds_alternative<bool>(buffer)) {
    auto bt = options.baseTile.value_or(0);
    if (_layer != 0) {
      bt = 0;
    }

    for (int y = 0; y < _ty; ++y) {
      for (int x = 0; x < _tx * 4; x += 4) {
        stl_util::concat(data, {static_cast<float>(bt), 0.f, 0.f, 0.f});
      }
    }
  }
  else if (std::holds_alternative<Float32Array>(buffer)) {
    data = std::get<Float32Array>(buffer);
  }

  const auto& floatArray = data;

  auto t = RawTexture::CreateRGBATexture(floatArray,                        // data
                                         _tx,                               // width
                                         _ty,                               // height
                                         _scene,                            // scene
                                         false,                             // generateMipMaps
                                         false,                             // invertY
                                         TextureConstants::NEAREST_NEAREST, // samplingMode
                                         Constants::TEXTURETYPE_FLOAT       // type
  );

  return t;
}

void SpriteMap::changeTiles(size_t _layer, const Vector2& pos, size_t tile)
{
  std::vector<Vector2> positions = {pos};
  changeTiles(_layer, positions, tile);
}

void SpriteMap::changeTiles(size_t _layer, const std::vector<Vector2>& pos, size_t tile)
{
  std::optional<ArrayBufferView> buffer = std::nullopt;
  buffer                                = _tileMaps[_layer]->_texture->_bufferView;
  if (!buffer.has_value()) {
    return;
  }

  auto p = pos;

  auto _tx = options.stageSize ? (*options.stageSize).x : 0.f;

  for (auto& _p : p) {
    _p.x                       = std::floor(_p.x);
    _p.y                       = std::floor(_p.y);
    auto id                    = static_cast<size_t>((_p.x * 4) + (_p.y * (_tx * 4)));
    (*buffer).uint8Array()[id] = static_cast<uint8_t>(tile);
  }

  auto t = _createTileBuffer(buffer->float32Array());
  _tileMaps[_layer]->dispose();
  _tileMaps[_layer] = t;
  _material->setTextureArray("tileMap", _getTileMaps());
}

RawTexturePtr SpriteMap::_createTileAnimationBuffer(const std::optional<ArrayBufferView>& buffer)
{
  Float32Array floatArray;
  if (!buffer) {
    for (size_t i = 0; i < spriteCount; ++i) {
      stl_util::concat(floatArray, {0.f, 0.f, 0.f, 0.f});
      size_t count = 1;
      while (count < options.maxAnimationFrames.value_or(4)) {
        stl_util::concat(floatArray, {0.f, 0.f, 0.f, 0.f});
        count++;
      }
    }
  }
  else {
    floatArray = buffer.value().float32Array();
  }

  auto t = RawTexture::CreateRGBATexture(
    floatArray,                                               // data
    static_cast<int>(spriteCount),                            // width
    static_cast<int>(options.maxAnimationFrames.value_or(4)), // height
    _scene,                                                   // scene
    false,                                                    // generateMipMaps
    false,                                                    // invertY
    TextureConstants::NEAREST_NEAREST,                        // samplingMode
    Constants::TEXTURETYPE_FLOAT                              // type
  );

  return t;
}

std::vector<BaseTexturePtr> SpriteMap::_getTileMaps() const
{
  std::vector<BaseTexturePtr> _tileMapsTmp;
  for (const auto& tileMap : _tileMaps) {
    _tileMapsTmp.emplace_back(std::static_pointer_cast<BaseTexture>(tileMap));
  }
  return _tileMapsTmp;
}

void SpriteMap::addAnimationToTile(size_t cellID, size_t _frame, size_t toCell, float time,
                                   float speed)
{
  auto buffer = _animationMap->_texture->_bufferView;
  auto id     = (cellID * 4) + (spriteCount * 4 * _frame);
  if (buffer.empty()) {
    return;
  }
  buffer[id]     = static_cast<uint8_t>(toCell);
  buffer[id + 1] = static_cast<uint8_t>(time);
  buffer[id + 2] = static_cast<uint8_t>(speed);
  auto t         = _createTileAnimationBuffer(buffer);
  _animationMap->dispose();
  _animationMap = t;
  _material->setTexture("animationMap", _animationMap);
}

void SpriteMap::saveTileMaps()
{
}

void SpriteMap::loadTileMaps(const std::string& url)
{
  const auto onErrorFunction = [url](const std::string& errorMessage) {
    BABYLON_LOGF_WARN("SpriteMap", "Error loading file %s, message: %s", url.c_str(),
                      errorMessage.c_str())
  };

  const auto onLoadFunction = [onErrorFunction](const std::string& /*response*/) {};

  FileTools::LoadAssetSync_Text(url, onLoadFunction, onErrorFunction);
}

void SpriteMap::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  _output->dispose();
  _material->dispose();
  _animationMap->dispose();
  for (const auto& tm : _tileMaps) {
    tm->dispose();
  }
  _frameMap->dispose();
}

} // end of namespace BABYLON
