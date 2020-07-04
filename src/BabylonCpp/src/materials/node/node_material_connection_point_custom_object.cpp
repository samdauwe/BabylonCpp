#include <babylon/materials/node/node_material_connection_point_custom_object.h>

namespace BABYLON {

NodeMaterialConnectionPointCustomObject::NodeMaterialConnectionPointCustomObject(
  const std::string& iName, const NodeMaterialBlockPtr& ownerBlock,
  NodeMaterialConnectionPointDirection direction,
  const std::function<NodeMaterialBlockPtr(const std::string& name)>& nodeMaterialBlockConstructor,
  const std::string& blockName, const std::string& nameForCheking)
    : NodeMaterialConnectionPoint{iName, ownerBlock, direction}
    , _nodeMaterialBlockConstructor{nodeMaterialBlockConstructor}
    , _blockName{blockName}
    , _nameForCheking{nameForCheking}
{
  needDualDirectionValidation = true;
}

NodeMaterialConnectionPointCompatibilityStates
NodeMaterialConnectionPointCustomObject::checkCompatibilityState(
  const NodeMaterialConnectionPointPtr& connectionPoint) const
{
  const auto _connectionPoint
    = std::static_pointer_cast<NodeMaterialConnectionPointCustomObject>(connectionPoint);

  return _connectionPoint && _connectionPoint->name == _nameForCheking ?
           NodeMaterialConnectionPointCompatibilityStates::Compatible :
           NodeMaterialConnectionPointCompatibilityStates::TypeIncompatible;
}

std::pair<NodeMaterialBlockPtr, std::string>
NodeMaterialConnectionPointCustomObject::createCustomInputBlock() const
{
  const auto nodeMaterialBlock = _nodeMaterialBlockConstructor(_blockName);

  return std::make_pair(nodeMaterialBlock, name);
}

} // end of namespace BABYLON
