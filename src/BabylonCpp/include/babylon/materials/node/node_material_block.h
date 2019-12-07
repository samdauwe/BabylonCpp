#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_BLOCK_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_BLOCK_H

#include <memory>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/materials/node/node_material_block_connection_point_types.h>
#include <babylon/materials/node/node_material_block_targets.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class Effect;
class EffectFallbacks;
class Mesh;
class NodeMaterial;
class NodeMaterialBlock;
class NodeMaterialConnectionPoint;
struct NodeMaterialDefines;
class NodeMaterialBuildState;
class Scene;
using EffectPtr                      = std::shared_ptr<Effect>;
using NodeMaterialPtr                = std::shared_ptr<NodeMaterial>;
using NodeMaterialBlockPtr           = std::shared_ptr<NodeMaterialBlock>;
using NodeMaterialConnectionPointPtr = std::shared_ptr<NodeMaterialConnectionPoint>;

struct NodeMaterialBlockConnectionOptions {
  std::string input{};
  std::string output{};
  std::string outputSwizzle{};
}; // end of struct NodeMaterialBlockConnectionOptions

/**
 * @brief Defines a block that can be used inside a node based material.
 */
class BABYLON_SHARED_EXPORT NodeMaterialBlock
    : public std::enable_shared_from_this<NodeMaterialBlock> {

public:
  template <typename... Ts>
  static NodeMaterialBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<NodeMaterialBlock>(new NodeMaterialBlock(std::forward<Ts>(args)...));
  }
  virtual ~NodeMaterialBlock();

  /**
   * @brief Find an input by its name.
   * @param name defines the name of the input to look for
   * @returns the input or null if not found
   */
  NodeMaterialConnectionPointPtr getInputByName(const std::string& name) const;

  /**
   * @brief Find an output by its name.
   * @param name defines the name of the outputto look for
   * @returns the output or null if not found
   */
  NodeMaterialConnectionPointPtr getOutputByName(const std::string& name) const;

  /**
   * @brief Initialize the block and prepare the context for build.
   * @param state defines the state that will be used for the build
   */
  virtual void initialize(NodeMaterialBuildState& state);

  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  virtual void bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial,
                    Mesh* mesh = nullptr);

  /**
   * @brief Gets the current class name e.g. "NodeMaterialBlock".
   * @returns the class name
   */
  virtual std::string getClassName() const;

  /**
   * @brief Register a new input. Must be called inside a block constructor.
   * @param name defines the connection point name
   * @param type defines the connection point type
   * @param isOptional defines a boolean indicating that this input can be omitted
   * @param target defines the target to use to limit the connection point (will be
   * VertexAndFragment by default)
   * @returns the current block
   */
  NodeMaterialBlock&
  registerInput(const std::string& name, const NodeMaterialBlockConnectionPointTypes& type,
                bool isOptional                                       = false,
                const std::optional<NodeMaterialBlockTargets>& target = std::nullopt);

  /**
   * @brief Register a new output. Must be called inside a block constructor.
   * @param name defines the connection point name
   * @param type defines the connection point type
   * @param target defines the target to use to limit the connection point (will be
   * VertexAndFragment by default)
   * @returns the current block
   */
  NodeMaterialBlock&
  registerOutput(const std::string& name, const NodeMaterialBlockConnectionPointTypes& type,
                 const std::optional<NodeMaterialBlockTargets>& target = std::nullopt);

  /**
   * @brief Will return the first available input e.g. the first one which is
   * not an uniform or an attribute.
   * @param forOutput defines an optional connection point to check compatibility with
   * @returns the first available input or null
   */
  NodeMaterialConnectionPointPtr
  getFirstAvailableInput(const NodeMaterialConnectionPointPtr& forOutput = nullptr);

  /**
   * @brief Will return the first available output e.g. the first one which is not yet connected and
   * not a varying.
   * @param forBlock defines an optional block to check compatibility with
   * @returns the first available input or null
   */
  NodeMaterialConnectionPointPtr getFirstAvailableOutput(const NodeMaterialBlockPtr& forBlock
                                                         = nullptr);

  /**
   * @brief Gets the sibling of the given output.
   * @param current defines the current output
   * @returns the next output in the list or null
   */
  NodeMaterialConnectionPointPtr getSiblingOutput(const NodeMaterialConnectionPointPtr& current);

  /**
   * @brief Connect current block with another block.
   * @param other defines the block to connect with
   * @param options define the various options to help pick the right connections
   * @returns the current block
   */
  NodeMaterialBlock& connectTo(const NodeMaterialBlockPtr& other,
                               const std::optional<NodeMaterialBlockConnectionOptions>& options
                               = std::nullopt);

  /**
   * @brief Add uniforms, samplers and uniform buffers at compilation time.
   * @param state defines the state to update
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   */
  virtual void updateUniformsAndSamples(NodeMaterialBuildState& state,
                                        const NodeMaterialPtr& nodeMaterial,
                                        const NodeMaterialDefines& defines);

  /**
   * @brief Add potential fallbacks if shader compilation fails.
   * @param mesh defines the mesh to be rendered
   * @param fallbacks defines the current prioritized list of fallbacks
   */
  virtual void provideFallbacks(AbstractMesh* mesh, EffectFallbacks* fallbacks);

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  virtual void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                              NodeMaterialDefines& defines, bool useInstances = false);

  /**
   * @brief Initialize defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to be prepared
   * @param useInstances specifies that instances should be used
   */
  virtual void initializeDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                 NodeMaterialDefines& defines, bool useInstances = false);

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  virtual void autoConfigure(const NodeMaterialPtr& nodeMaterial);

  /**
   * @brief Function called when a block is declared as repeatable content generator.
   * @param vertexShaderState defines the current compilation state for the vertex shader
   * @param fragmentShaderState defines the current compilation state for the fragment shader
   * @param mesh defines the mesh to be rendered
   * @param defines defines the material defines to update
   */
  virtual void replaceRepeatableContent(NodeMaterialBuildState& vertexShaderState,
                                        const NodeMaterialBuildState& fragmentShaderState,
                                        AbstractMesh* mesh, NodeMaterialDefines& defines);

  /**
   * @brief Checks if the block is ready.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   * @returns true if the block is ready
   */
  virtual bool isReady(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                       const NodeMaterialDefines& defines, bool useInstances = false);

  /**
   * @brief Compile the current node and generate the shader code.
   * @param state defines the current compilation state (uniforms, samplers, current string)
   * @param activeBlocks defines the list of active blocks (i.e. blocks to compile)
   * @returns true if already built
   */
  bool build(NodeMaterialBuildState& state, const std::vector<NodeMaterialBlockPtr>& activeBlocks);

  /**
   * @brief Hidden
   */
  std::string _dumpCode(std::vector<std::string>& uniqueNames,
                        std::vector<NodeMaterialBlockPtr>& alreadyDumped);

  /**
   * @brief Clone the current block to a new identical block.
   * @param scene defines the hosting scene
   * @param rootUrl defines the root URL to use to load textures and relative dependencies
   * @returns a copy of the current block
   */
  NodeMaterialPtr clone(Scene* scene, const std::string& rootUrl = "");

  /**
   * @brief Serializes this block in a JSON representation.
   * @returns the serialized block object
   */
  virtual json serialize() const;

  /**
   * @brief Hidden
   */
  virtual void _deserialize(const json& serializationObject, Scene* scene,
                            const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new NodeMaterialBlock.
   * @param name defines the block name
   * @param target defines the target of that block (Vertex by default)
   * @param isFinalMerger defines a boolean indicating that this block is an end block (e.g. it is
   * generating a system value). Default is false
   * @param isInput defines a boolean indicating that this block is an input (e.g. it sends data to
   * the shader). Default is false
   */
  NodeMaterialBlock(const std::string& name,
                    NodeMaterialBlockTargets target = NodeMaterialBlockTargets::Vertex,
                    bool isFinalMerger = false, bool isInput = false);

  /**
   * @brief Gets a boolean indicating that this block is an end block (e.g. it is generating a
   * system value).
   */
  bool get_isFinalMerger() const;

  /**
   * @brief Gets a boolean indicating that this block is an input (e.g. it sends data to the
   * shader).
   */
  bool get_isInput() const;

  /**
   * @brief Gets the build Id.
   */
  size_t get_buildId() const;

  /**
   * @brief Sets the build Id.
   */
  void set_buildId(size_t value);

  /**
   * @brief Gets the target of the block.
   */
  virtual NodeMaterialBlockTargets& get_target();

  /**
   * @brief Sets the target of the block.
   */
  void set_target(const NodeMaterialBlockTargets& value);

  /**
   * @brief Gets the list of input points.
   */
  std::vector<NodeMaterialConnectionPointPtr>& get_inputs();

  /**
   * @brief Gets the list of output points.
   */
  std::vector<NodeMaterialConnectionPointPtr>& get_outputs();

  std::string _declareOutput(const NodeMaterialConnectionPointPtr& output,
                             const NodeMaterialBuildState& state) const;
  std::string _writeVariable(const NodeMaterialConnectionPointPtr& currentPoint) const;
  std::string _writeFloat(float value) const;
  virtual NodeMaterialBlock& _buildBlock(NodeMaterialBuildState& state);
  void _linkConnectionTypes(size_t inputIndex0, size_t inputIndex1);
  virtual std::string _inputRename(const std::string& name);
  virtual std::string _outputRename(const std::string& name);
  virtual std::string _dumpPropertiesCode();

private:
  void _processBuild(const NodeMaterialBlockPtr& block, NodeMaterialBuildState& state,
                     const NodeMaterialConnectionPointPtr& input,
                     const std::vector<NodeMaterialBlockPtr>& activeBlocks);

public:
  /** Hidden */
  std::string _codeVariableName;

  /** Hidden */
  std::vector<NodeMaterialConnectionPointPtr> _inputs;
  /** Hidden */
  std::vector<NodeMaterialConnectionPointPtr> _outputs;

  /** Hidden */
  size_t _preparationId;

  /**
   * Gets or sets the name of the block
   */
  std::string name;

  /**
   * Gets or sets the unique id of the node
   */
  size_t uniqueId;

  /**
   * Gets a boolean indicating that this block is an end block (e.g. it is generating a system
   * value)
   */
  ReadOnlyProperty<NodeMaterialBlock, bool> isFinalMerger;

  /**
   * Gets a boolean indicating that this block is an input (e.g. it sends data to the shader)
   */
  ReadOnlyProperty<NodeMaterialBlock, bool> isInput;

  /**
   * Gets or sets the build Id
   */
  Property<NodeMaterialBlock, size_t> buildId;

  /**
   * Gets or sets the target of the block
   */
  Property<NodeMaterialBlock, NodeMaterialBlockTargets> target;

  /**
   * Gets the list of input points
   */
  ReadOnlyProperty<NodeMaterialBlock, std::vector<NodeMaterialConnectionPointPtr>> inputs;

  /**
   * Gets the list of output points
   */
  ReadOnlyProperty<NodeMaterialBlock, std::vector<NodeMaterialConnectionPointPtr>> outputs;

private:
  size_t _buildId;
  NodeMaterialBlockTargets _buildTarget;
  NodeMaterialBlockTargets _target;
  bool _isFinalMerger;
  bool _isInput;

}; // end of enum class NodeMaterialBlockTargets

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_BLOCK_H
