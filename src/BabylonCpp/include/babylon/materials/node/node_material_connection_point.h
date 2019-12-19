#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_H

#include <memory>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/materials/node/enums/node_material_block_connection_point_types.h>
#include <babylon/materials/node/enums/node_material_block_targets.h>
#include <babylon/misc/observable.h>

using json = nlohmann::json;

namespace BABYLON {

class InputBlock;
class NodeMaterialBlock;
class NodeMaterialConnectionPoint;
using InputBlockPtr                  = std::shared_ptr<InputBlock>;
using NodeMaterialBlockPtr           = std::shared_ptr<NodeMaterialBlock>;
using NodeMaterialConnectionPointPtr = std::shared_ptr<NodeMaterialConnectionPoint>;

/**
 * @brief  Enum used to define the compatibility state between two connection points.
 */
enum class BABYLON_SHARED_EXPORT NodeMaterialConnectionPointCompatibilityStates {
  /** Points are compatibles */
  Compatible,
  /** Points are incompatible because of their types */
  TypeIncompatible,
  /** Points are incompatible because of their targets (vertex vs fragment) */
  TargetIncompatible
}; // end of enum class NodeMaterialConnectionPointCompatibilityStates

/**
 * @brief Defines the direction of a connection point.
 */
enum class BABYLON_SHARED_EXPORT NodeMaterialConnectionPointDirection {
  /** Input */
  Input,
  /** Output */
  Output
}; // end of enum class NodeMaterialConnectionPointDirection

/**
 * @brief Defines a connection point for a block.
 */
class BABYLON_SHARED_EXPORT NodeMaterialConnectionPoint
    : public std::enable_shared_from_this<NodeMaterialConnectionPoint> {

public:
  template <typename... Ts>
  static NodeMaterialConnectionPointPtr New(Ts&&... args)
  {
    return std::shared_ptr<NodeMaterialConnectionPoint>(
      new NodeMaterialConnectionPoint(std::forward<Ts>(args)...));
  }
  ~NodeMaterialConnectionPoint() = default;

  /**
   * @brief Gets the current class name e.g. "NodeMaterialConnectionPoint".
   * @returns the class name
   */
  std::string getClassName() const;

  /**
   * @brief Gets a boolean indicating if the current point can be connected to another point.
   * @param connectionPoint defines the other connection point
   * @returns a boolean
   */
  bool canConnectTo(const NodeMaterialConnectionPoint& connectionPoint);

  /**
   * @brief Gets a number indicating if the current point can be connected to another point.
   * @param connectionPoint defines the other connection point
   * @returns a number defining the compatibility state
   */
  NodeMaterialConnectionPointCompatibilityStates
  checkCompatibilityState(const NodeMaterialConnectionPoint& connectionPoint);

  /**
   * @brief Connect this point to another connection point.
   * @param connectionPoint defines the other connection point
   * @param ignoreConstraints defines if the system will ignore connection type constraints (default
   * is false)
   * @returns the current connection point
   */
  NodeMaterialConnectionPoint& connectTo(const NodeMaterialConnectionPointPtr& connectionPoint,
                                         bool ignoreConstraints = false);

  /**
   * @brief Disconnect this point from one of his endpoint.
   * @param endpoint defines the other connection point
   * @returns the current connection point
   */
  NodeMaterialConnectionPoint& disconnectFrom(const NodeMaterialConnectionPointPtr& endpoint);

  /**
   * @brief Serializes this point in a JSON representation.
   * @returns the serialized point object
   */
  json serialize() const;

  /**
   * @brief Release resources.
   */
  void dispose();

protected:
  /**
   * @brief Creates a new connection point
   * @param name defines the connection point name
   * @param ownerBlock defines the block hosting this connection point
   * @param direction defines the direction of the connection point
   */
  NodeMaterialConnectionPoint(const std::string& name, const NodeMaterialBlockPtr& ownerBlock,
                              const NodeMaterialConnectionPointDirection& direction);

  /**
   * @brief Gets the direction of the point
   */
  NodeMaterialConnectionPointDirection& get_direction();

  /**
   * @brief Gets the associated variable name in the shader.
   */
  std::string get_associatedVariableName() const;

  /**
   * @brief Sets the associated variable name in the shader.
   */
  void set_associatedVariableName(std::string value);

  /**
   * @brief Gets the connection point type (default is float).
   */
  NodeMaterialBlockConnectionPointTypes& get_type();

  /**
   * @brief Sets the connection point type (default is float).
   */
  void set_type(const NodeMaterialBlockConnectionPointTypes& value);

  /**
   * @brief Gets the target of that connection point.
   */
  NodeMaterialBlockTargets& get_target();

  /**
   * @brief Sets the target of that connection point.
   */
  void set_target(const NodeMaterialBlockTargets& value);

  /**
   * @brief Gets a boolean indicating that the current point is connected.
   */
  bool get_isConnected() const;

  /**
   * @brief Gets a boolean indicating that the current point is connected to an input block.
   */
  bool get_isConnectedToInputBlock() const;

  /**
   * @brief Gets a the connected input block (if any).
   */
  InputBlockPtr& get_connectInputBlock();

  /**
   * @brief Get the other side of the connection (if any).
   */
  NodeMaterialConnectionPointPtr& get_connectedPoint();

  /**
   * @brief Get the block that owns this connection point.
   */
  NodeMaterialBlockPtr& get_ownerBlock();

  /**
   * @brief Get the block connected on the other side of this connection (if
   * any).
   */
  NodeMaterialBlockPtr& get_sourceBlock();

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
   * @brief Gets a boolean indicating if that output point is connected to at least one input.
   */
  bool get_hasEndpoints() const;

  /**
   * @brief Gets a boolean indicating that this connection will be used in the vertex shader.
   */
  bool get_isConnectedInVertexShader() const;

  /**
   * @brief Gets a boolean indicating that this connection will be used in the fragment shader.
   */
  bool get_isConnectedInFragmentShader() const;

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
   * Gets the direction of the point
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, NodeMaterialConnectionPointDirection> direction;

  /**
   * Gets or sets the additional types supported by this connection point.
   */
  std::vector<NodeMaterialBlockConnectionPointTypes> acceptedConnectionPointTypes;

  /**
   * Gets or sets the additional types excluded by this connection point
   */
  std::vector<NodeMaterialBlockConnectionPointTypes> excludedConnectionPointTypes;

  /**
   * Observable triggered when this point is connected
   */
  Observable<NodeMaterialConnectionPoint> onConnectionObservable;

  /**
   * Gets or sets the associated variable name in the shader
   */
  Property<NodeMaterialConnectionPoint, std::string> associatedVariableName;

  /**
   * Hidden
   */
  Property<NodeMaterialConnectionPoint, NodeMaterialBlockConnectionPointTypes> type;

  /**
   * Gets or sets the connection point name
   */
  std::string name;

  /**
   * Gets or sets a boolean indicating that this connection point can be omitted
   */
  bool isOptional;

  /**
   * Gets or sets a string indicating that this uniform must be defined under a #ifdef
   */
  std::string define;

  /**
   * Hidden
   */
  bool _prioritizeVertex;

  /**
   * Gets or sets the target of that connection point
   */
  Property<NodeMaterialConnectionPoint, NodeMaterialBlockTargets> target;

  /**
   * Gets a boolean indicating that the current point is connected.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, bool> isConnected;

  /**
   * Gets a boolean indicating that the current point is connected to an input block.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, bool> isConnectedToInputBlock;

  /**
   * Gets a the connected input block (if any).
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, InputBlockPtr> connectInputBlock;

  /**
   * Get the other side of the connection (if any).
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, NodeMaterialConnectionPointPtr> connectedPoint;

  /**
   * Get the block that owns this connection point.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, NodeMaterialBlockPtr> ownerBlock;

  /**
   * Get the block connected on the other side of this connection (if any).
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, NodeMaterialBlockPtr> sourceBlock;

  /**
   * Get the block connected on the endpoints of this connection (if any).
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, std::vector<NodeMaterialBlockPtr>> connectedBlocks;

  /**
   * Gets the list of connected endpoints.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, std::vector<NodeMaterialConnectionPointPtr>>
    endpoints;

  /**
   * Gets a boolean indicating if that output point is connected to at least one input.
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, bool> hasEndpoints;

  /**
   * Gets a boolean indicating that this connection will be used in the vertex shader
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, bool> isConnectedInVertexShader;

  /**
   * Gets a boolean indicating that this connection will be used in the fragment shader
   */
  ReadOnlyProperty<NodeMaterialConnectionPoint, bool> isConnectedInFragmentShader;

private:
  std::vector<NodeMaterialConnectionPointPtr> _endpoints;
  std::string _associatedVariableName;
  NodeMaterialConnectionPointDirection _direction;
  NodeMaterialBlockTargets _target;
  NodeMaterialBlockConnectionPointTypes _type;
  InputBlockPtr _connectInputBlock;
  NodeMaterialBlockPtr _sourceBlock;
  std::vector<NodeMaterialBlockPtr> _connectedBlocks;
  NodeMaterialBlockTargets _tmpTarget;

}; // end of enum class NodeMaterialConnectionPoint

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_H
