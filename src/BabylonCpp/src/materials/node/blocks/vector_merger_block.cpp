#include <babylon/materials/node/blocks/vector_merger_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

VectorMergerBlock::VectorMergerBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , xyzIn{this, &VectorMergerBlock::get_xyzIn}
    , xyIn{this, &VectorMergerBlock::get_xyIn}
    , zwIn{this, &VectorMergerBlock::get_zwIn}
    , x{this, &VectorMergerBlock::get_x}
    , y{this, &VectorMergerBlock::get_y}
    , z{this, &VectorMergerBlock::get_z}
    , w{this, &VectorMergerBlock::get_w}
    , xyzw{this, &VectorMergerBlock::get_xyzw}
    , xyzOut{this, &VectorMergerBlock::get_xyzOut}
    , xyOut{this, &VectorMergerBlock::get_xyOut}
    , zwOut{this, &VectorMergerBlock::get_zwOut}
{
  registerInput("xyz ", NodeMaterialBlockConnectionPointTypes::Vector3, true);
  registerInput("xy ", NodeMaterialBlockConnectionPointTypes::Vector2, true);
  registerInput("zw ", NodeMaterialBlockConnectionPointTypes::Vector2, true);
  registerInput("x", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("y", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("z", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("w", NodeMaterialBlockConnectionPointTypes::Float, true);

  registerOutput("xyzw", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerOutput("xyz", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("xy", NodeMaterialBlockConnectionPointTypes::Vector2);
  registerOutput("zw", NodeMaterialBlockConnectionPointTypes::Vector2);
}

VectorMergerBlock::~VectorMergerBlock() = default;

std::string VectorMergerBlock::getClassName() const
{
  return "VectorMergerBlock";
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_xyzIn()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_xyIn()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_zwIn()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_x()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_y()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_z()
{
  return _inputs[5];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_w()
{
  return _inputs[6];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_xyzw()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_xyzOut()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_xyOut()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_zwOut()
{
  return _outputs[3];
}

std::string VectorMergerBlock::_inputRename(const std::string& name)
{
  if (name == "xyz ") {
    return "xyzIn";
  }
  if (name == "xy ") {
    return "xyIn";
  }
  if (name == "zw ") {
    return "zwIn";
  }
  return name;
}

VectorMergerBlock& VectorMergerBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& xInput   = x();
  const auto& yInput   = y();
  const auto& zInput   = z();
  const auto& wInput   = w();
  const auto& xyInput  = xyIn();
  const auto& zwInput  = zwIn();
  const auto& xyzInput = xyzIn();

  const auto& v4Output     = _outputs[0];
  const auto& v3Output     = _outputs[1];
  const auto& v2Output     = _outputs[2];
  const auto& v2CompOutput = _outputs[3];

  if (xyzInput->isConnected()) {
    if (v4Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(v4Output, state)
           + StringTools::printf(" = vec4(%s, %s);\r\n", xyzInput->associatedVariableName().c_str(),
                                 wInput->isConnected() ? _writeVariable(wInput).c_str() : "0.0");
    }

    if (v3Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(v3Output, state)
           + StringTools::printf(" = %s;\r\n", xyzInput->associatedVariableName().c_str());
    }

    if (v2Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(v2Output, state)
           + StringTools::printf(" = %s.xy;\r\n", xyzInput->associatedVariableName().c_str());
    }
  }
  else if (xyInput->isConnected()) {
    if (v4Output->hasEndpoints()) {
      if (zwInput->isConnected()) {
        state.compilationString += _declareOutput(v4Output, state)
                                   + StringTools::printf(" = vec4(%s, %s);\r\n",
                                                         xyInput->associatedVariableName().c_str(),
                                                         zwInput->associatedVariableName().c_str());
      }
      else {
        state.compilationString
          += _declareOutput(v4Output, state)
             + StringTools::printf(" = vec4(%s, %s, %s);\r\n",
                                   xyInput->associatedVariableName().c_str(),
                                   zInput->isConnected() ? _writeVariable(zInput).c_str() : "0.0",
                                   wInput->isConnected() ? _writeVariable(wInput).c_str() : "0.0");
      }
    }

    if (v3Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(v3Output, state)
           + StringTools::printf(" = vec3(%s, %s);\r\n", xyInput->associatedVariableName().c_str(),
                                 zInput->isConnected() ? _writeVariable(zInput).c_str() : "0.0");
    }

    if (v2Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(v2Output, state)
           + StringTools::printf(" = %s;\r\n", xyInput->associatedVariableName().c_str());
    }

    if (v2CompOutput->hasEndpoints()) {
      if (zwInput->isConnected()) {
        state.compilationString
          += _declareOutput(v2CompOutput, state)
             + StringTools::printf(" = %s;\r\n", zwInput->associatedVariableName().c_str());
      }
      else {
        state.compilationString
          += _declareOutput(v2CompOutput, state)
             + StringTools::printf(" = vec2(%s, %s);\r\n",
                                   zInput->isConnected() ? _writeVariable(zInput).c_str() : "0.0",
                                   wInput->isConnected() ? _writeVariable(wInput).c_str() : "0.0");
      }
    }
  }
  else {
    if (v4Output->hasEndpoints()) {
      if (zwInput->isConnected()) {
        state.compilationString
          += _declareOutput(v4Output, state)
             + StringTools::printf(" = vec4(%s, %s, %s);\r\n",
                                   xInput->isConnected() ? _writeVariable(xInput).c_str() : "0.0",
                                   yInput->isConnected() ? _writeVariable(yInput).c_str() : "0.0",
                                   zwInput->associatedVariableName().c_str());
      }
      else {
        state.compilationString
          += _declareOutput(v4Output, state)
             + StringTools::printf(" = vec4(%s, %s, %s, %s);\r\n",
                                   xInput->isConnected() ? _writeVariable(xInput).c_str() : "0.0",
                                   yInput->isConnected() ? _writeVariable(yInput).c_str() : "0.0",
                                   zInput->isConnected() ? _writeVariable(zInput).c_str() : "0.0",
                                   wInput->isConnected() ? _writeVariable(wInput).c_str() : "0.0");
      }
    }

    if (v3Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(v3Output, state)
           + StringTools::printf(" = vec3(%s, %s, %s);\r\n",
                                 xInput->isConnected() ? _writeVariable(xInput).c_str() : "0.0",
                                 yInput->isConnected() ? _writeVariable(yInput).c_str() : "0.0",
                                 zInput->isConnected() ? _writeVariable(zInput).c_str() : "0.0");
    }

    if (v2Output->hasEndpoints()) {
      state.compilationString
        += _declareOutput(v2Output, state)
           + StringTools::printf(" = vec2(%s, %s);\r\n",
                                 xInput->isConnected() ? _writeVariable(xInput).c_str() : "0.0",
                                 yInput->isConnected() ? _writeVariable(yInput).c_str() : "0.0");
    }

    if (v2CompOutput->hasEndpoints()) {
      if (zwInput->isConnected()) {
        state.compilationString
          += _declareOutput(v2CompOutput, state)
             + StringTools::printf(" = %s;\r\n", zwInput->associatedVariableName().c_str());
      }
      else {
        state.compilationString
          += _declareOutput(v2CompOutput, state)
             + StringTools::printf(" = vec2(%s, %s);\r\n",
                                   zInput->isConnected() ? _writeVariable(zInput).c_str() : "0.0",
                                   wInput->isConnected() ? _writeVariable(wInput).c_str() : "0.0");
      }
    }
  }

  return *this;
}

} // end of namespace BABYLON
