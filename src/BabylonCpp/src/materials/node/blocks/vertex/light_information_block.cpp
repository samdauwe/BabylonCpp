#include <babylon/materials/node/blocks/vertex/light_information_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/lights/light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

LightInformationBlock::LightInformationBlock(const std::string& iName)
    : NodeMaterialBlock(iName, NodeMaterialBlockTargets::Vertex)
    , worldPosition{this, &LightInformationBlock::get_worldPosition}
    , direction{this, &LightInformationBlock::get_direction}
    , color{this, &LightInformationBlock::get_color}
{
  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Vertex);
  registerOutput("direction", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("color", NodeMaterialBlockConnectionPointTypes::Color3);
}

LightInformationBlock::~LightInformationBlock()
{
}

std::string LightInformationBlock::getClassName() const
{
  return "LightInformationBlock";
}

NodeMaterialConnectionPointPtr& LightInformationBlock::get_worldPosition()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& LightInformationBlock::get_direction()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& LightInformationBlock::get_color()
{
  return _outputs[1];
}

void LightInformationBlock::bind(const EffectPtr& effect, const NodeMaterialPtr& /*nodeMaterial*/,
                                 Mesh* mesh)
{
  if (!mesh) {
    return;
  }

  if (!light || !light->isEnabled()) {
    effect->setFloat3(_lightDataDefineName, 0.f, 0.f, 0.f);
    effect->setFloat3(_lightColorDefineName, 0.f, 0.f, 0.f);
    return;
  }

  light->transferToNodeMaterialEffect(effect, _lightDataDefineName);

  effect->setColor3(_lightColorDefineName, light->diffuse);
}

LightInformationBlock& LightInformationBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  state.sharedData->bindableBlocks.emplace_back(this);

  const auto& _direction = direction();
  const auto& _color     = color();

  if (!light) {
    state.compilationString += _declareOutput(_direction, state) + " = vec3(0.);\r\n";
    state.compilationString += _declareOutput(_color, state) + " = vec3(0.);\r\n";
  }
  else {
    _lightDataDefineName = state._getFreeDefineName("lightData");
    state._emitUniformFromString(_lightDataDefineName, "vec3");

    _lightColorDefineName = state._getFreeDefineName("lightColor");
    state._emitUniformFromString(_lightColorDefineName, "vec3");

    if (light->type() == Type::POINTLIGHT) {
      state.compilationString
        += _declareOutput(_direction, state)
           + String::printf(" = normalize(%s - %s.xyz);\r\n)", _lightDataDefineName.c_str(),
                            worldPosition()->associatedVariableName().c_str());
    }
    else {
      state.compilationString += _declareOutput(_direction, state)
                                 + String::printf(" = %s;\r\n", _lightDataDefineName.c_str());
    }

    state.compilationString
      += _declareOutput(_color, state) + String::printf(" = %s;\r\n", _lightDataDefineName.c_str());
  }

  return *this;
}

json LightInformationBlock::serialize() const
{
  return nullptr;
}

void LightInformationBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                         const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
