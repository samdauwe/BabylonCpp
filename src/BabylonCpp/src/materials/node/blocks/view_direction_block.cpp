#include <babylon/materials/node/blocks/view_direction_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_system_values.h>

namespace BABYLON {

ViewDirectionBlock::ViewDirectionBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , worldPosition{this, &ViewDirectionBlock::get_worldPosition}
    , cameraPosition{this, &ViewDirectionBlock::get_cameraPosition}
    , output{this, &ViewDirectionBlock::get_output}
{
  registerInput("worldPosition",
                NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("cameraPosition",
                NodeMaterialBlockConnectionPointTypes::Vector3);

  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Vector3);
}

ViewDirectionBlock::~ViewDirectionBlock()
{
}

const std::string ViewDirectionBlock::getClassName() const
{
  return "ViewDirectionBlock";
}

NodeMaterialConnectionPointPtr& ViewDirectionBlock::get_worldPosition()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ViewDirectionBlock::get_cameraPosition()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ViewDirectionBlock::get_output()
{
  return _outputs[0];
}

void ViewDirectionBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!cameraPosition()->isConnected()) {
    auto cameraPositionInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->systemValue().has_value()
                 && b->systemValue().value()
                      == NodeMaterialSystemValues::CameraPosition;
        });

    if (!cameraPositionInput) {
      cameraPositionInput = InputBlock::New("cameraPosition");
      cameraPositionInput->setAsSystemValue(
        NodeMaterialSystemValues::CameraPosition);
    }
    cameraPositionInput->output()->connectTo(cameraPosition);
  }
}

ViewDirectionBlock&
ViewDirectionBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];

  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = normalize(%s - %s).xyz);\r\n",
                        cameraPosition()->associatedVariableName().c_str(),
                        worldPosition()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
