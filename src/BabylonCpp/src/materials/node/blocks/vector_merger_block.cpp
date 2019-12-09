#include <babylon/materials/node/blocks/vector_merger_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

VectorMergerBlock::VectorMergerBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , x{this, &VectorMergerBlock::get_x}
    , y{this, &VectorMergerBlock::get_y}
    , z{this, &VectorMergerBlock::get_z}
    , w{this, &VectorMergerBlock::get_w}
    , xyzw{this, &VectorMergerBlock::get_xyzw}
    , xyz{this, &VectorMergerBlock::get_xyz}
    , xy{this, &VectorMergerBlock::get_xy}
{
  registerInput("x", NodeMaterialBlockConnectionPointTypes::Float);
  registerInput("y", NodeMaterialBlockConnectionPointTypes::Float);
  registerInput("z", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("w", NodeMaterialBlockConnectionPointTypes::Float, true);

  registerOutput("xyzw", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerOutput("xyz", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("xy", NodeMaterialBlockConnectionPointTypes::Vector2);
}

VectorMergerBlock::~VectorMergerBlock() = default;

std::string VectorMergerBlock::getClassName() const
{
  return "VectorMergerBlock";
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_x()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_y()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_z()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_w()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_xyzw()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_xyz()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& VectorMergerBlock::get_xy()
{
  return _outputs[1];
}

VectorMergerBlock& VectorMergerBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& xInput = x();
  const auto& yInput = y();
  const auto& zInput = z();
  const auto& wInput = w();

  const auto& v4Output = _outputs[0];
  const auto& v3Output = _outputs[1];
  const auto& v2Output = _outputs[2];

  if (v4Output->hasEndpoints()) {
    state.compilationString
      += _declareOutput(v4Output, state)
         + String::printf(" = vec4(%s, %s, %s, %s);\r\n", _writeVariable(xInput).c_str(),
                          _writeVariable(yInput).c_str(),
                          zInput->isConnected() ? _writeVariable(zInput).c_str() : "0.0",
                          zInput->isConnected() ? _writeVariable(wInput).c_str() : "0.0");
  }
  else if (v3Output->hasEndpoints()) {
    state.compilationString
      += _declareOutput(v3Output, state)
         + String::printf(" = vec3(%s, %s, %s);\r\n", _writeVariable(xInput).c_str(),
                          _writeVariable(yInput).c_str(),
                          zInput->isConnected() ? _writeVariable(zInput).c_str() : "0.0");
  }
  else if (v2Output->hasEndpoints()) {
    state.compilationString
      += _declareOutput(v2Output, state)
         + String::printf(" = vec2(%s, %s);\r\n", _writeVariable(xInput).c_str(),
                          _writeVariable(yInput).c_str());
  }

  return *this;
}

} // end of namespace BABYLON
