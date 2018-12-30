#ifndef BABYLON_LAYER_EFFECT_LAYER_H
#define BABYLON_LAYER_EFFECT_LAYER_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/layer/ieffect_layer_options.h>
#include <babylon/math/color4.h>
#include <babylon/math/size.h>
#include <babylon/tools/observable.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class BaseTexture;
class Effect;
class Engine;
class Material;
class Mesh;
class PostProcess;
class RenderTargetTexture;
class Scene;
class SubMesh;
class VertexBuffer;
using BaseTexturePtr         = std::shared_ptr<BaseTexture>;
using EffectPtr              = std::shared_ptr<Effect>;
using MaterialPtr            = std::shared_ptr<Material>;
using MeshPtr                = std::shared_ptr<Mesh>;
using PostProcessPtr         = std::shared_ptr<PostProcess>;
using SubMeshPtr             = std::shared_ptr<SubMesh>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

struct EmissiveTextureAndColor {
  BaseTexturePtr texture = nullptr;
  Color4 color;
}; // end of struct EmissiveTextureAndColor

/**
 * @brief The effect layer Helps adding post process effect blended with the
 * main pass.
 *
 * This can be for instance use to generate glow or higlight effects on the
 * scene.
 *
 * The effect layer class can not be used directly and is intented to inherited
 * from to be customized per effects.
 */
class BABYLON_SHARED_EXPORT EffectLayer
    : public std::enable_shared_from_this<EffectLayer> {

public:
  virtual ~EffectLayer();

  /**
   * @brief Get the effect name of the layer.
   * @return The effect name
   */
  virtual std::string getEffectName() const = 0;

  /**
   * @brief Checks for the readiness of the element composing the layer.
   * @param subMesh the mesh to check for
   * @param useInstances specify wether or not to use instances to render the
   * mesh
   * @return true if ready otherwise, false
   */
  virtual bool isReady(SubMesh* subMesh, bool useInstances) = 0;

  /**
   * @brief Returns wether or nood the layer needs stencil enabled during the
   * mesh rendering.
   * @returns true if the effect requires stencil during the main canvas render
   * pass.
   */
  virtual bool needStencil() const = 0;

  /**
   * @brief Free any resources and references associated to a mesh.
   * Internal use
   * @param mesh The mesh to free.
   */
  virtual void _disposeMesh(Mesh* mesh) = 0;

  /**
   * @brief Serializes this layer (Glow or Highlight for example).
   * @returns a serialized layer object
   */
  virtual json serialize() const = 0;

  /**
   * @brief Renders the glowing part of the scene by blending the blurred
   * glowing meshes on top of the rendered scene.
   */
  void render();

  /**
   * @brief Determine if a given mesh will be used in the current effect.
   * @param mesh mesh to test
   * @returns true if the mesh will be used
   */
  virtual bool hasMesh(AbstractMesh* mesh) const;

  /**
   * @brief Returns true if the layer contains information to display, otherwise
   * false.
   * @returns true if the glow layer should be rendered
   */
  virtual bool shouldRender() const;

  /**
   * @brief Rebuild the required buffers.
   * Internal use only.
   */
  void _rebuild();

  /**
   * @brief Dispose the highlight layer and free resources.
   */
  void dispose();

  /**
   * @brief Gets the class name of the effect layer.
   * @returns the string with the class name of the effect layer
   */
  virtual std::string getClassName() const;

  /**
   * @brief Creates an effect layer from parsed effect layer data.
   * @param parsedEffectLayer defines effect layer data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing the effect layer information
   * @returns a parsed effect Layer
   */
  static EffectLayer* Parse(const json& parsedEffectLayer, Scene* scene,
                            const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a new effect Layer and references it in the scene.
   * @param name The name of the layer
   * @param scene The scene to use the layer in
   */
  EffectLayer(const std::string& name, Scene* scene);

  /**
   * @brief Gets the camera attached to the layer.
   */
  CameraPtr& get_camera();

  /**
   * @brief Gets the rendering group id the layer should render in.
   */
  int get_renderingGroupId() const;

  /**
   * @brief Create the merge effect. This is the shader use to blit the
   * information back to the main canvas at the end of the scene rendering.
   * @returns The effect containing the shader used to merge the effect on the
   * main canvas
   */
  virtual EffectPtr _createMergeEffect() = 0;

  /**
   * @brief Creates the render target textures and post processes used in the
   * effect layer.
   */
  virtual void _createTextureAndPostProcesses() = 0;

  /**
   * @brief Implementation specific of rendering the generating effect on the
   * main canvas.
   * @param effect The effect used to render through
   */
  virtual void _internalRender(const EffectPtr& effect) = 0;

  /**
   * @brief Sets the required values for both the emissive texture and and the
   * main color.
   */
  virtual void _setEmissiveTextureAndColor(const MeshPtr& mesh,
                                           SubMesh* subMesh,
                                           const MaterialPtr& material)
    = 0;

  /**
   * @brief Initializes the effect layer with the required options.
   * @param options Sets of none mandatory options to use with the layer (see
   * IEffectLayerOptions for more information)
   */
  void _init(const IEffectLayerOptions& options);

  /**
   * @brief Creates the main texture for the effect layer.
   */
  void _createMainTexture();

  /**
   * @brief Checks for the readiness of the element composing the layer.
   * @param subMesh the mesh to check for
   * @param useInstances specify wether or not to use instances to render the
   * mesh
   * @param emissiveTexture the associated emissive texture used to generate the
   * glow
   * @return true if ready otherwise, false
   */
  bool _isReady(SubMesh* subMesh, bool useInstances,
                const BaseTexturePtr& emissiveTexture);

  /**
   * @brief Returns true if the mesh should render, otherwise false.
   * @param mesh The mesh to render
   * @returns true if it should render otherwise false
   */
  virtual bool _shouldRenderMesh(const MeshPtr& mesh) const;

  /**
   * @brief Returns true if the mesh should render, otherwise false.
   * @param mesh The mesh to render
   * @returns true if it should render otherwise false
   */
  virtual bool _shouldRenderEmissiveTextureForMesh(Mesh* mesh) const;

  /**
   * @brief Renders the submesh passed in parameter to the generation map.
   */
  void _renderSubMesh(SubMesh* subMesh);

private:
  /**
   * @brief Generates the index buffer of the full screen quad blending to the
   * main canvas.
   */
  void _generateIndexBuffer();

  /**
   * @brief Generates the vertex buffer of the full screen quad blending to the
   * main canvas.
   */
  void _generateVertexBuffer();

  /**
   * @brief Sets the main texture desired size which is the closest power of two
   * of the engine canvas size.
   */
  void _setMainTextureSize();

  /**
   * @brief Dispose only the render target textures and post process.
   */
  void _disposeTextureAndPostProcesses();

public:
  /**
   * The Friendly of the effect in the scene.
   */
  std::string name;

  /**
   * The clear color of the texture used to generate the glow map.
   */
  Color4 neutralColor;

  /**
   * Specifies wether the highlight layer is enabled or not.
   */
  bool isEnabled;

  /**
   * Gets the camera attached to the layer.
   */
  ReadOnlyProperty<EffectLayer, CameraPtr> camera;

  /**
   * Gets the rendering group id the layer should render in.
   */
  ReadOnlyProperty<EffectLayer, int> renderingGroupId;

  /**
   * An event triggered when the effect layer has been disposed.
   */
  Observable<EffectLayer> onDisposeObservable;

  /**
   * An event triggered when the effect layer is about rendering the main
   * texture with the glowy parts.
   */
  Observable<EffectLayer> onBeforeRenderMainTextureObservable;

  /**
   * An event triggered when the generated texture is being merged in the scene.
   */
  Observable<EffectLayer> onBeforeComposeObservable;

  /**
   * An event triggered when the generated texture has been merged in the scene.
   */
  Observable<EffectLayer> onAfterComposeObservable;

  /**
   * An event triggered when the efffect layer changes its size.
   */
  Observable<EffectLayer> onSizeChangedObservable;

  RenderTargetTexturePtr _mainTexture;

protected:
  Scene* _scene;
  Engine* _engine;
  int _maxSize;
  ISize _mainTextureDesiredSize;
  bool _shouldRender;
  std::vector<PostProcessPtr> _postProcesses;
  std::vector<BaseTexturePtr> _textures;
  EmissiveTextureAndColor _emissiveTextureAndColor;

private:
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  std::string _cachedDefines;
  EffectPtr _effectLayerMapGenerationEffect;
  IEffectLayerOptions _effectLayerOptions;
  EffectPtr _mergeEffect;

}; // end of struct EffectLayer

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_EFFECT_LAYER_H
