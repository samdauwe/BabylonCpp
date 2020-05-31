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

} // end of namespace BABYLON
