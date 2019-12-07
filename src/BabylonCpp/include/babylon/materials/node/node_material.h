#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>

namespace BABYLON {

class ImageProcessingConfiguration;
struct INodeMaterialOptions;
struct INodeMaterialEditorOptions;
class InputBlock;
class NodeMaterial;
class NodeMaterialBlock;
class NodeMaterialBuildState;
class NodeMaterialDefines;
struct NodeMaterialOptimizer;
struct NodeMaterialBuildStateSharedData;
class ReflectionTextureBlock;
class TextureBlock;
using ImageProcessingConfigurationPtr     = std::shared_ptr<ImageProcessingConfiguration>;
using INodeMaterialEditorOptionsPtr       = std::shared_ptr<INodeMaterialEditorOptions>;
using INodeMaterialOptionsPtr             = std::shared_ptr<INodeMaterialOptions>;
using InputBlockPtr                       = std::shared_ptr<InputBlock>;
using NodeMaterialPtr                     = std::shared_ptr<NodeMaterial>;
using NodeMaterialBlockPtr                = std::shared_ptr<NodeMaterialBlock>;
using NodeMaterialBuildStatePtr           = std::shared_ptr<NodeMaterialBuildState>;
using NodeMaterialOptimizerPtr            = std::shared_ptr<NodeMaterialOptimizer>;
using NodeMaterialBuildStateSharedDataPtr = std::shared_ptr<NodeMaterialBuildStateSharedData>;
using ReflectionTextureBlockPtr           = std::shared_ptr<ReflectionTextureBlock>;
using TextureBlockPtr                     = std::shared_ptr<TextureBlock>;

/**
 * @brief Class used to create a node based material built by assembling shader
 * blocks.
 */
class BABYLON_SHARED_EXPORT NodeMaterial : public PushMaterial,
                                           public std::enable_shared_from_this<NodeMaterial> {

public:
  template <typename... Ts>
  static NodeMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<NodeMaterial>(new NodeMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~NodeMaterial() override; // = default

  /**
   * @brief Gets the current class name of the material e.g. "NodeMaterial".
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Get a block by its name.
   * @param name defines the name of the block to retrieve
   * @returns the required block or null if not found
   */
  NodeMaterialBlockPtr getBlockByName(const std::string& name) const;

  /**
   * @brief Get a block by its name.
   * @param predicate defines the predicate used to find the good candidate
   * @returns the required block or null if not found
   */
  NodeMaterialBlockPtr
  getBlockByPredicate(const std::function<bool(const NodeMaterialBlockPtr& block)>& predicate);

  /**
   * @brief Get an input block by its name.
   * @param predicate defines the predicate used to find the good candidate
   * @returns the required input block or null if not found
   */
  InputBlockPtr
  getInputBlockByPredicate(const std::function<bool(const InputBlockPtr& block)>& predicate);

  /**
   * @brief Gets the list of input blocks attached to this material.
   * @returns an array of InputBlocks
   */
  std::vector<InputBlockPtr> getInputBlocks() const;

  /**
   * @brief Adds a new optimizer to the list of optimizers.
   * @param optimizer defines the optimizers to add
   * @returns the current material
   */
  NodeMaterial& registerOptimizer(const NodeMaterialOptimizerPtr& optimizer);

  /**
   * @brief Remove an optimizer from the list of optimizers.
   * @param optimizer defines the optimizers to remove
   * @returns the current material
   */
  NodeMaterial& unregisterOptimizer(const NodeMaterialOptimizerPtr& optimizer);

  /**
   * @brief Add a new block to the list of output nodes.
   * @param node defines the node to add
   * @returns the current material
   */
  NodeMaterial& addOutputNode(const NodeMaterialBlockPtr& node);

  /**
   * @brief Remove a block from the list of root nodes.
   * @param node defines the node to remove
   * @returns the current material
   */
  NodeMaterial& removeOutputNode(const NodeMaterialBlockPtr& node);

  /**
   * @brief Specifies if the material will require alpha blending.
   * @returns a boolean specifying if alpha blending is needed
   */
  bool needAlphaBlending() const override;

  /**
   * @brief Specifies if this material should be rendered in alpha test mode.
   * @returns a boolean specifying if an alpha test is needed.
   */
  bool needAlphaTesting() const override;

  /**
   * @brief Build the material and generates the inner effect.
   * @param verbose defines if the build should log activity
   */
  void build(bool verbose = false);

  /**
   * @brief Runs an otpimization phase to try to improve the shader code.
   */
  void optimize();

  /**
   * @brief Get if the submesh is ready to be used and all its information available. Child classes
   * can use it to update shaders.
   * @param mesh defines the mesh to check
   * @param subMesh defines which submesh to check
   * @param useInstances specifies that instances should be used
   * @returns a boolean indicating that the submesh is ready or not
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;

  /**
   * @brief Get a string representing the shaders built by the current node graph.
   */
  std::string compiledShaders() const;

  /**
   * @brief Binds the world matrix to the material
   * @param world defines the world transformation matrix
   */
  void bindOnlyWorldMatrix(Matrix& world) override;

  /**
   * @brief Binds the submesh to this material by preparing the effect and shader to draw
   * @param world defines the world transformation matrix
   * @param mesh defines the mesh containing the submesh
   * @param subMesh defines the submesh to bind the material to
   */
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;

  /**
   * @brief Gets the active textures from the material.
   * @returns an array of textures
   */
  std::vector<BaseTexturePtr> getActiveTextures() const override;

  /**
   * @brief Gets the list of texture blocks.
   * @returns an array of texture blocks
   */
  std::vector<std::variant<TextureBlockPtr, ReflectionTextureBlockPtr>> getTextureBlocks();

  /**
   * @brief Specifies if the material uses a texture.
   * @param texture defines the texture to check against the material
   * @returns a boolean specifying if the material uses the texture
   */
  bool hasTexture(const BaseTexturePtr& texture) const override;

  /**
   * @brief Disposes the material.
   * @param forceDisposeEffect specifies if effects should be forcefully disposed
   * @param forceDisposeTextures specifies if textures should be forcefully disposed
   * @param notBoundToMesh specifies if the material that is being disposed is known to be not bound
   * to any mesh
   */
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;

  /**
   * @brief Launch the node material editor.
   * @param config Define the configuration of the editor
   * @return a promise fulfilled when the node editor is visible
   */
  void edit(const INodeMaterialEditorOptionsPtr& config = nullptr);

  /**
   * @brief Clear the current material.
   */
  void clear();

  /**
   * @brief Clear the current material and set it to a default state.
   */
  void setToDefault();

  /**
   * @brief Loads the current Node Material from a url pointing to a file save by the Node Material
   * Editor.
   * @param url defines the url to load from
   * @returns a promise that will fullfil when the material is fully loaded
   */
  void loadAsync(const std::string& url);

  /**
   * @brief Generate a string containing the code declaration required to create an equivalent of
   * this material
   * @returns a string
   */
  std::string generateCode();

  /**
   * @brief Serializes this material in a JSON representation.
   * @returns the serialized material object
   */
  json serialize() const;

  /**
   * @brief Clear the current graph and load a new one from a serialization object.
   * @param source defines the JSON representation of the material
   * @param rootUrl defines the root URL to use to load textures and relative dependencies
   */
  void loadFromSerialization(const json& source, const std::string& rootUrl = "");

  /**
   * @brief Creates a node material from parsed material data.
   * @param source defines the JSON representation of the material
   * @param scene defines the hosting scene
   * @param rootUrl defines the root URL to use to load textures and relative dependencies
   * @returns a new node material
   */
  static NodeMaterialPtr Parse(const json& source, Scene* scene, const std::string& rootUrl = "");

  /**
   * @brief Creates a new node material set to default basic configuration.
   * @param name defines the name of the material
   * @param scene defines the hosting scene
   * @returns a new NodeMaterial
   */
  static NodeMaterialPtr CreateDefault(const std::string& name, Scene* scene = nullptr);

protected:
  /**
   * @brief Create a new node based material.
   * @param name defines the material name
   * @param scene defines the hosting scene
   * @param options defines creation option
   */
  NodeMaterial(const std::string& name, Scene* scene,
               const INodeMaterialOptionsPtr& options = nullptr);

  /**
   * @brief Gets options to control the node material overall behavior.
   */
  INodeMaterialOptionsPtr& get_options();

  /**
   * @brief Sets options to control the node material overall behavior.
   */
  void set_options(const INodeMaterialOptionsPtr& value);

  /**
   * @brief Gets the image processing configuration used either in this material.
   */
  ImageProcessingConfigurationPtr& get_imageProcessingConfiguration();

  /**
   * @brief Sets the Default image processing configuration used either in the this material.
   *
   * If sets to null, the scene one is in use.
   */
  void set_imageProcessingConfiguration(const ImageProcessingConfigurationPtr& value);

  /**
   * @brief Attaches a new image processing configuration to the Standard Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(const ImageProcessingConfigurationPtr& configuration);

private:
  NodeMaterial& _addVertexOutputNode(const NodeMaterialBlockPtr& node);
  NodeMaterial& _removeVertexOutputNode(const NodeMaterialBlockPtr& node);
  NodeMaterial& _addFragmentOutputNode(const NodeMaterialBlockPtr& node);
  NodeMaterial& _removeFragmentOutputNode(const NodeMaterialBlockPtr& node);
  void _initializeBlock(const NodeMaterialBlockPtr& node, const NodeMaterialBuildStatePtr& state,
                        std::vector<NodeMaterialBlockPtr>& nodesToProcessForOtherBuildState);
  void _resetDualBlocks(const NodeMaterialBlockPtr& node, size_t id);
  void _prepareDefinesForAttributes(AbstractMesh* mesh, NodeMaterialDefines& defines);
  /**
   * @brief CCreates the node editor window.
   */
  void _createNodeEditor();
  void _gatherBlocks(const NodeMaterialBlockPtr& rootNode, std::vector<NodeMaterialBlockPtr>& list);
  void _restoreConnections(const NodeMaterialBlockPtr& block, const json& source,
                           const std::unordered_map<size_t, NodeMaterialBlockPtr>& map);

public:
  /**
   * Gets or sets a boolean indicating that alpha value must be ignored (This will turn alpha
   * blending off even if an alpha value is produced by the material)
   */
  bool ignoreAlpha;

  /**
   * Defines the maximum number of lights that can be used in the material
   */
  unsigned int maxSimultaneousLights;

  /**
   * Observable raised when the material is built
   */
  Observable<NodeMaterial> onBuildObservable;

  /**
   * Gets or sets the root nodes of the material vertex shader
   */
  std::vector<NodeMaterialBlockPtr> _vertexOutputNodes;

  /**
   * Gets or sets the root nodes of the material fragment (pixel) shader
   */
  std::vector<NodeMaterialBlockPtr> _fragmentOutputNodes;

  /**
   * Gets or sets options to control the node material overall behavior
   */
  Property<NodeMaterial, INodeMaterialOptionsPtr> options;

  /**
   * Gets or sets the image processing configuration used either in this material.
   */
  Property<NodeMaterial, ImageProcessingConfigurationPtr> imageProcessingConfiguration;

  /**
   * Gets an array of blocks that needs to be serialized even if they are not yet connected
   */
  std::vector<NodeMaterialBlockPtr> attachedBlocks;

protected:
  /**
   * Default configuration related to image processing available in the standard Material.
   */
  ImageProcessingConfigurationPtr _imageProcessingConfiguration;

private:
  static size_t _BuildIdGenerator;
  INodeMaterialOptionsPtr _options;
  NodeMaterialBuildStatePtr _vertexCompilationState;
  NodeMaterialBuildStatePtr _fragmentCompilationState;
  NodeMaterialBuildStateSharedDataPtr _sharedData;
  size_t _buildId;
  bool _buildWasSuccessful;
  Matrix _cachedWorldViewMatrix;
  Matrix _cachedWorldViewProjectionMatrix;
  std::vector<NodeMaterialOptimizerPtr> _optimizers;
  int _animationFrame;

  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  Observer<ImageProcessingConfiguration>::Ptr _imageProcessingObserver;

}; // end of enum class NodeMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_H
