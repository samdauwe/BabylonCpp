#include <babylon/materials/node/blocks/fragment/perturb_normal_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

PerturbNormalBlock::PerturbNormalBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , invertX{false}
    , invertY{false}
    , worldPosition{this, &PerturbNormalBlock::get_worldPosition}
    , worldNormal{this, &PerturbNormalBlock::get_worldNormal}
    , uv{this, &PerturbNormalBlock::get_uv}
    , normalMapColor{this, &PerturbNormalBlock::get_normalMapColor}
    , strength{this, &PerturbNormalBlock::get_strength}
    , output{this, &PerturbNormalBlock::get_output}
{
  // Vertex
  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4,
                false);
  registerInput("worldNormal", NodeMaterialBlockConnectionPointTypes::Vector4,
                false);
  registerInput("uv", NodeMaterialBlockConnectionPointTypes::Vector2, false);
  registerInput("normalMapColor", NodeMaterialBlockConnectionPointTypes::Color3,
                false);
  registerInput("strength", NodeMaterialBlockConnectionPointTypes::Float,
                false);

  // Fragment
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Vector4);
}

PerturbNormalBlock::~PerturbNormalBlock()
{
}

const std::string PerturbNormalBlock::getClassName() const
{
  return "PerturbNormalBlock";
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_worldPosition()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_worldNormal()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_uv()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_normalMapColor()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_strength()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_output()
{
  return _outputs[0];
}

void PerturbNormalBlock::prepareDefines(AbstractMesh* mesh,
                                        const NodeMaterialPtr& nodeMaterial,
                                        const NodeMaterialDefines& defines,
                                        bool useInstances)
{
  // TODO
}

void PerturbNormalBlock::bind(Effect* effect,
                              const NodeMaterialPtr& nodeMaterial, Mesh* mesh)
{
  // TODO
}

void PerturbNormalBlock::autoConfigure(const NodeMaterialPtr& nodeMaterial)
{
  // TODO
}

PerturbNormalBlock&
PerturbNormalBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  // TODO

  return *this;
}

std::string PerturbNormalBlock::_dumpPropertiesCode()
{
  auto codeString
    = String::printf("%s.invertX = %s;\r\n", _codeVariableName.c_str(),
                     invertX ? "true" : "false");

  codeString
    += String::printf("%s.invertY = %s;\r\n", _codeVariableName.c_str(),
                      invertY ? "true" : "false");

  return codeString;
}

json PerturbNormalBlock::serialize() const
{
  return nullptr;
}

void PerturbNormalBlock::_deserialize(const json& /*serializationObject*/,
                                      Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
