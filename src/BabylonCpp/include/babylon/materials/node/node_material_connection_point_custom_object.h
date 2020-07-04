#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_CUSTOM_OBJECT_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_CUSTOM_OBJECT_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

class NodeMaterialConnectionPointCustomObject;
using NodeMaterialConnectionPointCustomObjectPtr
  = std::shared_ptr<NodeMaterialConnectionPointCustomObject>;

/**
 * @brief Defines a connection point to be used for points with a custom object type.
 */
class BABYLON_SHARED_EXPORT NodeMaterialConnectionPointCustomObject
    : public NodeMaterialConnectionPoint {

public:
  template <typename... Ts>
  static NodeMaterialConnectionPointCustomObjectPtr New(Ts&&... args)
  {
    return std::shared_ptr<NodeMaterialConnectionPointCustomObject>(
      new NodeMaterialConnectionPointCustomObject(std::forward<Ts>(args)...));
  }
  ~NodeMaterialConnectionPointCustomObject() = default;

protected:
  /**
   * @brief Creates a new connection point.
   * @param name defines the connection point name
   * @param ownerBlock defines the block hosting this connection point
   * @param direction defines the direction of the connection point
   */
  NodeMaterialConnectionPointCustomObject(
    const std::string& name, const NodeMaterialBlockPtr& ownerBlock,
    NodeMaterialConnectionPointDirection direction,
    const std::function<NodeMaterialBlockPtr(const std::string& name)>&
      nodeMaterialBlockConstructor,
    const std::string& blockName, const std::string& nameForCheking = "");

  /**
   * @brief Gets a number indicating if the current point can be connected to another point.
   * @param connectionPoint defines the other connection point
   * @returns a number defining the compatibility state
   */
  NodeMaterialConnectionPointCompatibilityStates
  checkCompatibilityState(const NodeMaterialConnectionPointPtr& connectionPoint) const;

  /**
   * @brief Creates a block suitable to be used as an input for this input point.
   * If null is returned, a block based on the point type will be created.
   * @returns The returned string parameter is the name of the output point of NodeMaterialBlock
   * (first parameter of the returned array) that can be connected to the input
   */
  std::pair<NodeMaterialBlockPtr, std::string> createCustomInputBlock() const;

private:
  std::function<NodeMaterialBlockPtr(const std::string& name)> _nodeMaterialBlockConstructor;
  std::string _blockName;
  std::string _nameForCheking;

}; // end of enum class NodeMaterialConnectionPointCustomObject

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_CONNECTION_POINT_CUSTOM_OBJECT_H
