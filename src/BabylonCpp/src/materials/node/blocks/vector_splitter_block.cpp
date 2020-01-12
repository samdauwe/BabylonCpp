#include <babylon/materials/node/blocks/vector_splitter_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

VectorSplitterBlock::VectorSplitterBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , xyzw{this, &VectorSplitterBlock::get_xyzw}
    , xyzIn{this, &VectorSplitterBlock::get_xyzIn}
    , xyIn{this, &VectorSplitterBlock::get_xyIn}
    , xyzOut{this, &VectorSplitterBlock::get_xyzOut}
    , xyOut{this, &VectorSplitterBlock::get_xyOut}
    , x{this, &VectorSplitterBlock::get_x}
    , y{this, &VectorSplitterBlock::get_y}
    , z{this, &VectorSplitterBlock::get_z}
    , w{this, &VectorSplitterBlock::get_w}
{
  registerInput("xyzw", NodeMaterialBlockConnectionPointTypes::Vector4, true);
  registerInput("xyz ", NodeMaterialBlockConnectionPointTypes::Vector3, true);
  registerInput("xy ", NodeMaterialBlockConnectionPointTypes::Vector2, true);

  registerOutput("xyz", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("xy", NodeMaterialBlockConnectionPointTypes::Vector2);
  registerOutput("x", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("y", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("z", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("w", NodeMaterialBlockConnectionPointTypes::Float);
}

VectorSplitterBlock::~VectorSplitterBlock() = default;

std::string VectorSplitterBlock::getClassName() const
{
  return "VectorSplitterBlock";
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_xyzw()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_xyzIn()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_xyIn()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_xyzOut()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_xyOut()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_x()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_y()
{
  return _outputs[3];
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_z()
{
  return _outputs[4];
}

NodeMaterialConnectionPointPtr& VectorSplitterBlock::get_w()
{
  return _outputs[5];
}

std::string VectorSplitterBlock::_inputRename(const std::string& iName)
{
  if (iName == "xy ") {
    return "xyIn";
  }
  else if (iName == "xyz ") {
    return "xyzIn";
  }
  return iName;
}

std::string VectorSplitterBlock::_outputRename(const std::string& iName)
{
  if (iName == "xy") {
    return "xyOut";
  }
  else if (iName == "xyz") {
    return "xyzOut";
  }
  return iName;
}

VectorSplitterBlock& VectorSplitterBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& input = xyzw()->isConnected() ? xyzw() : xyzIn()->isConnected() ? xyzIn() : xyIn();

  const auto& xyzOutput = _outputs[0];
  const auto& xyOutput  = _outputs[1];
  const auto& xOutput   = _outputs[2];
  const auto& yOutput   = _outputs[3];
  const auto& zOutput   = _outputs[4];
  const auto& wOutput   = _outputs[5];

  if (xyzOutput->hasEndpoints()) {
    if (input == xyIn()) {
      state.compilationString
        += _declareOutput(xyzOutput, state)
           + StringTools::printf(" = vec3(%s, 0.0);\r\n", input->associatedVariableName().c_str());
    }
    else {
      state.compilationString
        += _declareOutput(xyzOutput, state)
           + StringTools::printf(" = %s.xyz;\r\n", input->associatedVariableName().c_str());
    }
  }
  if (xyOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(xyOutput, state)
         + StringTools::printf(" = %s.xy;\r\n", input->associatedVariableName().c_str());
  }
  if (xOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(xOutput, state)
         + StringTools::printf(" = %s.x;\r\n", input->associatedVariableName().c_str());
  }
  if (yOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(yOutput, state)
         + StringTools::printf(" = %s.y;\r\n", input->associatedVariableName().c_str());
  }
  if (zOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(zOutput, state)
         + StringTools::printf(" = %s.z;\r\n", input->associatedVariableName().c_str());
  }
  if (wOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(wOutput, state)
         + StringTools::printf(" = %s.w;\r\n", input->associatedVariableName().c_str());
  }

  return *this;
}

} // end of namespace BABYLON
