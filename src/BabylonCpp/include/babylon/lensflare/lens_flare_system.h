#ifndef BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H
#define BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H

#include <functional>
#include <map>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class AbstractMesh;
class Effect;
class LensFlare;
class LensFlareSystem;
class Mesh;
class Scene;
class Vector3;
class VertexBuffer;
class Viewport;
using LensFlarePtr       = std::shared_ptr<LensFlare>;
using LensFlareSystemPtr = std::shared_ptr<LensFlareSystem>;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

namespace GL {
class IGLBuffer;
} // end of namespace GL

/**
 * @brief This represents a Lens Flare System or the shiny effect created by the
 * light reflection on the  camera lenses. It is usually composed of several
 * `BABYLON.lensFlare`.
 * @see http://doc.babylonjs.com/how_to/how_to_use_lens_flares
 */
class BABYLON_SHARED_EXPORT LensFlareSystem
    : public std::enable_shared_from_this<LensFlareSystem> {

public:
  template <typename... Ts>
  static LensFlareSystemPtr New(Ts&&... args)
  {
    return std::shared_ptr<LensFlareSystem>(
      new LensFlareSystem(std::forward<Ts>(args)...));
  }
  virtual ~LensFlareSystem();

  /**
   * @brief Get the scene the effects belongs to.
   * @returns the scene holding the lens flare system
   */
  Scene* getScene();

  /**
   * @brief Get the emitter of the lens flare system.
   * It defines the source of the lens flares (it can be a camera, a light or a
   * mesh).
   * @returns the emitter of the lens flare system
   */
  Mesh* getEmitter();

  /**
   * @brief Set the emitter of the lens flare system.
   * It defines the source of the lens flares (it can be a camera, a light or a
   * mesh).
   * @param newEmitter Define the new emitter of the system
   */
  void setEmitter(Mesh* newEmitter);

  /**
   * @brief Get the lens flare system emitter position.
   * The emitter defines the source of the lens flares (it can be a camera, a
   * light or a mesh).
   * @returns the position
   */
  Vector3 getEmitterPosition();

  /**
   * @brief Hidden
   */
  bool computeEffectivePosition(Viewport& globalViewport);

  /**
   * @brief Hidden
   */
  bool _isVisible();

  /**
   * @brief Hidden
   */
  bool render();

  /**
   * Dispose and release the lens flare with its associated resources.
   */
  void dispose();

  /**
   * @brief Serialize the current Lens Flare System into a JSON representation.
   * @returns the serialized JSON
   */
  Json::object serialize() const;

  // Statics

  /**
   * @brief Parse a lens flare system from a JSON repressentation.
   * @param parsedLensFlareSystem Define the JSON to parse
   * @param scene Define the scene the parsed system should be instantiated in
   * @param rootUrl Define the rootUrl of the load sequence to easily find a
   * load relative dependencies such as textures
   * @returns the parsed system
   */
  static LensFlareSystemPtr Parse(const Json::value& parsedLensFlareSystem,
                                  Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a lens flare system.
   * This represents a Lens Flare System or the shiny effect created by the
   * light reflection on the  camera lenses. It is usually composed of several
   * `BABYLON.lensFlare`.
   * @see http://doc.babylonjs.com/how_to/how_to_use_lens_flares
   * @param name Define the name of the lens flare system in the scene
   * @param emitter Define the source (the emitter) of the lens flares (it can
   * be a camera, a light or a mesh).
   * @param scene Define the scene the lens flare system belongs to
   */
  LensFlareSystem(const std::string name, Mesh* emitter, Scene* scene);

private:
  bool get_isEnabled() const;
  void set_isEnabled(bool value);

public:
  /**
   * Define the id of the lens flare system in the scene.
   * (equal to name by default)
   */
  std::string id;

  /**
   * Define the name of the lens flare system
   */
  std::string name;

  /**
   * List of lens flares used in this system.
   */
  std::vector<LensFlarePtr> lensFlares;

  /**
   * Define a limit from the border the lens flare can be visible.
   */
  int borderLimit;

  /**
   * Define a viewport border we do not want to see the lens flare in.
   */
  float viewportBorder;

  /**
   * Define a predicate which could limit the list of meshes able to occlude the
   * effect.
   */
  std::function<bool(const AbstractMeshPtr& mesh)> meshesSelectionPredicate;

  /**
   * Restricts the rendering of the effect to only the camera rendering this
   * layer mask.
   */
  unsigned int layerMask;

  /**
   * Define if the lens flare system is enabled.
   */
  Property<LensFlareSystem, bool> isEnabled;

private:
  Scene* _scene;
  Mesh* _emitter;
  // Float32Array _vertexDeclaration;
  // int _vertexStrideSize;
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  float _positionX;
  float _positionY;
  bool _isEnabled;

}; // end of class LensFlareSystem

} // end of namespace BABYLON

#endif // end of BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H
