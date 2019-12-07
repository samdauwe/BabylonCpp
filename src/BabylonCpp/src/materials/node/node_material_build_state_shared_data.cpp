#include <babylon/materials/node/node_material_build_state_shared_data.h>

#include <babylon/materials/node/node_material_block.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <sstream>

namespace BABYLON {

NodeMaterialBuildStateSharedData::NodeMaterialBuildStateSharedData()
{
  // Exclude usual attributes from free variable names
  variableNames["position"]             = 0;
  variableNames["normal"]               = 0;
  variableNames["tangent"]              = 0;
  variableNames["uv"]                   = 0;
  variableNames["uv2"]                  = 0;
  variableNames["uv3"]                  = 0;
  variableNames["uv4"]                  = 0;
  variableNames["uv4"]                  = 0;
  variableNames["uv5"]                  = 0;
  variableNames["uv6"]                  = 0;
  variableNames["color"]                = 0;
  variableNames["matricesIndices"]      = 0;
  variableNames["matricesWeights"]      = 0;
  variableNames["matricesIndicesExtra"] = 0;
  variableNames["matricesWeightsExtra"] = 0;
  variableNames["diffuseBase"]          = 0;
  variableNames["specularBase"]         = 0;
  variableNames["worldPos"]             = 0;

  // Exclude known varyings
  variableNames["vTBN"] = 0;

  // Exclude defines
  defineNames["MAINUV0"] = 0;
  defineNames["MAINUV1"] = 0;
  defineNames["MAINUV2"] = 0;
  defineNames["MAINUV3"] = 0;
  defineNames["MAINUV4"] = 0;
  defineNames["MAINUV5"] = 0;
  defineNames["MAINUV6"] = 0;
  defineNames["MAINUV7"] = 0;
}

NodeMaterialBuildStateSharedData::~NodeMaterialBuildStateSharedData()
{
}

void NodeMaterialBuildStateSharedData::emitErrors()
{
  std::ostringstream errorMessage;

  if (!checks.emitVertex) {
    errorMessage << "NodeMaterial does not have a vertex output. You need to at least add "
                    "a block that generates a glPosition value.\r\n";
  }
  if (!checks.emitFragment) {
    errorMessage << "NodeMaterial does not have a fragment output. You need to at least "
                    "add a block that generates a glFragColor value.\r\n";
  }
  for (const auto& notConnectedInput : checks.notConnectedNonOptionalInputs) {
    errorMessage << "input " << notConnectedInput->name << " from block "
                 << notConnectedInput->ownerBlock()->name << "["
                 << notConnectedInput->ownerBlock()->getClassName()
                 << "] is not connected and is not optional.\r\n";
  }

  if (errorMessage) {
    throw std::runtime_error("Build of NodeMaterial failed:\r\n" + errorMessage.str());
  }
}

} // end of namespace BABYLON
