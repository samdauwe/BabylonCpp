#ifndef BABYLON_LENSFLARES_LENS_FLARE_SYSTEM_H
#define BABYLON_LENSFLARES_LENS_FLARE_SYSTEM_H

#include <functional>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class Camera;
class Effect;
class IShadowLight;
class LensFlare;
class LensFlareSystem;
class Light;
class Mesh;
class Scene;
class TransformNode;
class Vector3;
class VertexBuffer;
class Viewport;
class WebGLDataBuffer;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;
using CameraPtr          = std::shared_ptr<Camera>;
using EffectPtr          = std::shared_ptr<Effect>;
using IShadowLightPtr    = std::shared_ptr<IShadowLight>;
using LensFlarePtr       = std::shared_ptr<LensFlare>;
using LensFlareSystemPtr = std::shared_ptr<LensFlareSystem>;
using LightPtr           = std::shared_ptr<Light>;
using MeshPtr            = std::shared_ptr<Mesh>;
using TransformNodePtr   = std::shared_ptr<TransformNode>;
using VertexBufferPtr    = std::shared_ptr<VertexBuffer>;
using WebGLDataBufferPtr = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief This represents a Lens Flare System or the shiny effect created by the light reflection on
 * the camera lenses. It is usually composed of several `lensFlare`.
 * @see http://doc.babylonjs.com/how_to/how_to_use_lens_flares
 */
class BABYLON_SHARED_EXPORT LensFlareSystem {

  using LensFlareEmitterType = std::variant<CameraPtr, IShadowLightPtr, MeshPtr>;

public:
  template <typename... Ts>
  static LensFlareSystemPtr New(Ts&&... args)
  {
    auto lensFlareSystem
      = std::shared_ptr<LensFlareSystem>(new LensFlareSystem(std::forward<Ts>(args)...));
    lensFlareSystem->addToScene(lensFlareSystem);

    return lensFlareSystem;
  }
  virtual ~LensFlareSystem(); // = default

  void addToScene(const LensFlareSystemPtr& lensFlareSystem);

  /**
   * @brief Get the scene the effects belongs to.
   * @returns the scene holding the lens flare system
   */
  Scene* getScene();

  /**
   * @brief Get the emitter of the lens flare system.
   * It defines the source of the lens flares (it can be a camera, a light or a mesh).
   * @returns the emitter of the lens flare system
   */
  LensFlareEmitterType& getEmitter();

  /**
   * @brief Set the emitter of the lens flare system.
   * It defines the source of the lens flares (it can be a camera, a light or a mesh).
   * @param newEmitter Define the new emitter of the system
   */
  void setEmitter(const LensFlareEmitterType& newEmitter);

  /**
   * @brief Get the lens flare system emitter position.
   * The emitter defines the source of the lens flares (it can be a camera, a light or a mesh).
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
  [[nodiscard]] json serialize() const;

  // Statics

  /**
   * @brief Parse a lens flare system from a JSON repressentation.
   * @param parsedLensFlareSystem Define the JSON to parse
   * @param scene Define the scene the parsed system should be instantiated in
   * @param rootUrl Define the rootUrl of the load sequence to easily find a load relative
   * dependencies such as textures
   * @returns the parsed system
   */
  static LensFlareSystemPtr Parse(const json& parsedLensFlareSystem, Scene* scene,
                                  const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a lens flare system.
   * This represents a Lens Flare System or the shiny effect created by the light reflection on the
   * camera lenses. It is usually composed of several `lensFlare`.
   * @see http://doc.babylonjs.com/how_to/how_to_use_lens_flares
   * @param name Define the name of the lens flare system in the scene
   * @param emitter Define the source (the emitter) of the lens flares (it can be a camera, a light
   * or a mesh).
   * @param scene Define the scene the lens flare system belongs to
   */
  LensFlareSystem(const std::string& name, const LensFlareEmitterType& emitter, Scene* scene);

private:
  [[nodiscard]] bool get_isEnabled() const;
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
   * Define a predicate which could limit the list of meshes able to occlude the effect.
   */
  std::function<bool(const AbstractMeshPtr& mesh)> meshesSelectionPredicate;

  /**
   * Restricts the rendering of the effect to only the camera rendering this layer mask.
   */
  unsigned int layerMask;

  /**
   * Define if the lens flare system is enabled.
   */
  Property<LensFlareSystem, bool> isEnabled;

private:
  Scene* _scene;
  LensFlareEmitterType _emitter;
  // Float32Array _vertexDeclaration;
  // int _vertexStrideSize;
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  WebGLDataBufferPtr _indexBuffer;
  EffectPtr _effect;
  float _positionX;
  float _positionY;
  bool _isEnabled;

}; // end of class LensFlareSystem

} // end of namespace BABYLON

#endif // end of BABYLON_LENSFLARES_LENS_FLARE_SYSTEM_H
