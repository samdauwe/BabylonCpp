#ifndef BABYLON_PROBES_REFLECTION_PROBE_H
#define BABYLON_PROBES_REFLECTION_PROBE_H

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
struct ISize;
class ReflectionProbe;
class RenderTargetTexture;
class Scene;
using AbstractMeshPtr        = std::shared_ptr<AbstractMesh>;
using ReflectionProbePtr     = std::shared_ptr<ReflectionProbe>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief Class used to generate realtime reflection / refraction cube textures.
 * @see http://doc.babylonjs.com/how_to/how_to_use_reflection_probes
 */
class BABYLON_SHARED_EXPORT ReflectionProbe {

public:
  template <typename... Ts>
  static ReflectionProbePtr New(Ts&&... args)
  {
    auto reflectionProbe = std::shared_ptr<ReflectionProbe>(
      new ReflectionProbe(std::forward<Ts>(args)...));
    reflectionProbe->addToScene(reflectionProbe);

    return reflectionProbe;
  }
  virtual ~ReflectionProbe(); // = default

  void addToScene(const ReflectionProbePtr& newReflectionProbe);

  /**
   * @brief Gets the hosting scene.
   * @returns a Scene
   */
  Scene* getScene() const;

  /**
   * @brief Gets the internal CubeTexture used to render to.
   */
  RenderTargetTexturePtr& cubeTexture();

  /**
   * @brief Attach the probe to a specific mesh (Rendering will be done from
   * attached mesh's position)
   * @param mesh defines the mesh to attach to
   */
  void attachToMesh(AbstractMesh* mesh);

  /**
   * @brief Specifies whether or not the stencil and depth buffer are cleared
   * between two rendering groups
   * @param renderingGroupId The rendering group id corresponding to its index
   * @param autoClearDepthStencil Automatically clears depth and stencil between
   * groups if true.
   */
  void setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                         bool autoClearDepthStencil);

  /**
   * @brief Clean all associated resources
   */
  void dispose();

  /**
   * @brief Converts the reflection probe information to a readable string for
   * debug purpose.
   * @param fullDetails Supports for multiple levels of logging within scene
   * loading
   * @returns the human readable reflection probe info
   */
  std::string toString(bool fullDetails = false);

  /**
   * @brief Get the class name of the relfection probe.
   * @returns "ReflectionProbe"
   */
  std::string getClassName() const;

  /**
   * @brief Serialize the reflection probe to a JSON representation we can
   * easily use in the resepective Parse function.
   * @returns The JSON representation of the texture
   */
  void serialize(json& serializationObject);

  /**
   * @brief Parse the JSON representation of a reflection probe in order to
   * recreate the reflection probe in the given scene.
   * @param parsedReflectionProbe Define the JSON representation of the
   * reflection probe
   * @param scene Define the scene the parsed reflection probe should be
   * instantiated in
   * @param rootUrl Define the root url of the parsing sequence in the case of
   * relative dependencies
   * @returns The parsed reflection probe if successful
   */
  static ReflectionProbePtr Parse(const json& parsedReflectionProbe,
                                  Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new reflection probe.
   * @param name defines the name of the probe
   * @param size defines the texture resolution (for each face)
   * @param scene defines the hosting scene
   * @param generateMipMaps defines if mip maps should be generated
   * automatically (true by default)
   * @param useFloat defines if HDR data (flaot data) should be used to store
   * colors (false by default)
   */
  ReflectionProbe(const std::string& name, const ISize& size, Scene* scene,
                  bool generateMipMaps = true, bool useFloat = false);

private:
  unsigned int get_samples() const;
  void set_samples(unsigned int value);
  int get_refreshRate() const;
  void set_refreshRate(int value);
  std::vector<AbstractMesh*>& get_renderList();

public:
  /**
   * Defines the name of the probe
   */
  std::string name;

  /**
   * Gets or sets probe position (center of the cube map)
   */
  Vector3 position;

  /**
   * Gets or sets the number of samples to use for multi-sampling (0 by
   * default). Required WebGL2
   */
  Property<ReflectionProbe, unsigned int> samples;

  /**
   * Gets or sets the refresh rate to use (on every frame by default)
   */
  Property<ReflectionProbe, int> refreshRate;

  /**
   * Gets the list of meshes to render
   */
  ReadOnlyProperty<ReflectionProbe, std::vector<AbstractMesh*>> renderList;

private:
  Scene* _scene;
  RenderTargetTexturePtr _renderTargetTexture;
  Matrix _projectionMatrix;
  Matrix _viewMatrix;
  Vector3 _target;
  Vector3 _add;
  AbstractMesh* _attachedMesh;
  bool _invertYAxis;

}; // end of class ReflectionProbe

} // end of namespace BABYLON

#endif // end of BABYLON_PROBES_REFLECTION_PROBE_H
