#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_H

#include <memory>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/materials/node/node_material_block_connection_point_types.h>
#include <babylon/materials/node/node_material_block_targets.h>

using json = nlohmann::json;

namespace BABYLON {

class InputBlock;
class NodeMaterialBlock;
class NodeMaterialConnectionPoint;
using InputBlockPtr        = std::shared_ptr<InputBlock>;
using NodeMaterialBlockPtr = std::shared_ptr<NodeMaterialBlock>;
using NodeMaterialConnectionPointPtr
  = std::shared_ptr<NodeMaterialConnectionPoint>;

/**
 * @brief Defines a connection point for a block.
 */
class BABYLON_SHARED_EXPORT NodeMaterialConnectionPoint {

public:
  template <typename... Ts>
  static NodeMaterialConnectionPointPtr New(Ts&&... args)
  {
    return std::shared_ptr<NodeMaterialConnectionPoint>(
      new NodeMaterialConnectionPoint(std::forward<Ts>(args)...));
  }
  ~NodeMaterialConnectionPoint();

  /**
   * @brief Gets the current class name e.g. "NodeMaterialConnectionPoint".
   * @returns the class name
   */
  const std::string getClassName() const;

  /**
   * @brief Gets an boolean indicating if the current point can be connected to
   * another point.
   * @param connectionPoint defines the other connection point
   * @returns true if the connection is possible
   */
  bool canConnectTo(const NodeMaterialConnectionPoint& connectionPoint);

  /**
   * @brief Connect this point to another connection point.
   * @param connectionPoint defines the other connection point
   * @param ignoreConstraints defines if the system will ignore connection type
   * constraints (default is false)
   * @returns the current connection point
   */
  NodeMaterialConnectionPoint&
  connectTo(const NodeMaterialConnectionPoint& connectionPoint,
            bool ignoreConstraints = false);

  /**
   * @brief Disconnect this point from one of his endpoint.
   * @param endpoint defines the other connection point
   * @returns the current connection point
   */
  NodeMaterialConnectionPoint&
  disconnectFrom(const NodeMaterialConnectionPoint& endpoint);

  /**
   * @brief Serializes this point in a JSON representation.
   * @returns the serialized point object
   */
  json serialize() const;

protected:
  /**
   * @brief Creates a new connection point.
   * @param name defines the connection point name
   * @param ownerBlock defines the block hosting this connection point
   */
  NodeMaterialConnectionPoint(const std::string& name,
                              const NodeMaterialBlockPtr& ownerBlock);

  /**
   * @brief Gets the associated variable name in the shader.
   */
  std::string get_associatedVariableName() const;

  /**
   * @brief Sets the associated variable name in the shader.
   */
  void set_associatedVariableName(const std::string& value);

  /**
   * @brief Gets the connection point type (default is float).
   */
  NodeMaterialBlockConnectionPointTypes get_type() const;

  /**
   * @brief Sets the connection point type (default is float).
   */
  void set_type(const NodeMaterialBlockConnectionPointTypes& value);

  /**
   * @brief Gets a boolean indicating that the current point is connected.
   */
  bool get_isConnected() const;

  /**
   * @brief Gets a boolean indicating that the current point is connected to an
   * input block.
   */
  bool get_isConnectedToInputBlock() const;

  /**
   * @brief Gets a the connected input block (if any).
   */
  InputBlockPtr& get_connectInputBlock();

  /**
   * @brief Get the other side of the connection (if any).
   */
  std::optional<NodeMaterialConnectionPoint>& get_connectedPoint();

  /**
   * @brief Get the block that owns this connection point.
   */
  NodeMaterialBlock& get_ownerBlock();

  /**
   * @brief Get the block connected on the other side of this connection (if
   * any).
   */
  NodeMaterialBlock& get_sourceBlock();

  /**
   * @brief Get the block connected on the endpoints of this connection (if
   * any).
   */
  std::vector<NodeMaterialBlockPtr>& get_connectedBlocks();

  /**
   * @brief Gets the list of connected endpoints.
   */
  std::vector<NodeMaterialConnectionPointPtr>& get_endpoints();

  /**
   * @brief Gets a boolean indicating if that output point is connected to at
   * least one input.
   */
  bool get_hasEndpoints() const;

public:
  /** Hidden */
  NodeMaterialBlockPtr _ownerBlock;
  /** Hidden */
  NodeMaterialConnectionPointPtr _connectedPoint;

  /** Hidden */
  NodeMaterialConnectionPointPtr _typeConnectionSource;

  /** Hidden */
  NodeMaterialConnectionPointPtr _linkedConnectionSource;

  /** Hidden */
  bool _enforceAssociatedVariableName;

  /**
   * Gets or sets the additional types supported byt this connection point
   */
  std::vector<NodeMaterialBlockConnectionPointTypes>
    acceptedConnectionPointTypes;

  /**
   * Gets or sets the associated variable name in the shader
   */
  Property<NodeMaterialConnectionPoint, std::string> associatedVariableName;

  Property<NodeMaterialConnectionPoint, NodeMaterialBlockConnectionPointTypes>
    type;

  /**
   * Gets or sets the connection point name
   */
  std::string name;

  /**
   * Gets or sets a boolean indicating that this connection point can be omitted
   */
  bool isOptional;

  /**
   * Gets or sets a string indicating that this uniform must be defined under a
   * #ifdef
   */
  std::string define;

  /**
   * Gets or sets the target of that connection point
   */
  NodeMaterialBlockTargets target;

  /**
   * Gets a boolean indicating that the current point is connected.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, bool> isConnected;

  /**
   * Gets a boolean indicating that the current point is connected to an input
   * block.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, bool> isConnectedToInputBlock;

  /**
   * Gets a the connected input block (if any).
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, InputBlockPtr>
    connectInputBlock;

  /**
   * Get the other side of the connection (if any).
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint,
                   std::optional<NodeMaterialConnectionPoint>>
    connectedPoint;

  /**
   * Get the block that owns this connection point.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, NodeMaterialBlock> ownerBlock;

  /**
   * Get the block connected on the other side of this connection (if
   * any).
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, NodeMaterialBlock> sourceBlock;

  /**
   * Get the block connected on the endpoints of this connection (if any).
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint,
                   std::vector<NodeMaterialBlockPtr>>
    connectedBlocks;

  /**
   * Gets the list of connected endpoints.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint,
                   std::vector<NodeMaterialConnectionPointPtr>>
    endpoints;

  /**
   * Gets a boolean indicating if that output point is connected to at least one
   * input.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, bool> hasEndpoints;

private:
  std::vector<NodeMaterialConnectionPointPtr> _endpoints;
  std::string _associatedVariableName;
  NodeMaterialBlockConnectionPointTypes _type;

}; // end of enum class NodeMaterialConnectionPoint

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_H
