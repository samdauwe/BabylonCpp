#include <babylon/materials/node/blocks/vertex/light_information_block.h>

#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

LightInformationBlock::LightInformationBlock(const std::string& iName)
    : NodeMaterialBlock(iName, NodeMaterialBlockTargets::Vertex)
    , worldPosition{this, &LightInformationBlock::get_worldPosition}
    , direction{this, &LightInformationBlock::get_direction}
    , color{this, &LightInformationBlock::get_color}
    , intensity{this, &LightInformationBlock::get_intensity}
{
  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Vertex);
  registerOutput("direction", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("color", NodeMaterialBlockConnectionPointTypes::Color3);
  registerOutput("intensity", NodeMaterialBlockConnectionPointTypes::Float);
}

LightInformationBlock::~LightInformationBlock() = default;

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

NodeMaterialConnectionPointPtr& LightInformationBlock::get_intensity()
{
  return _outputs[2];
}

void LightInformationBlock::bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial,
                                 Mesh* mesh, SubMesh* /*subMes*/)
{
  if (!mesh) {
    return;
  }

  if (light && light->isDisposed()) {
    light = nullptr;
  }

  auto scene = nodeMaterial->getScene();

  if (!light && !scene->lights.empty()) {
    light = scene->lights[0];
  }

  if (!light || !light->isEnabled()) {
    effect->setFloat3(_lightDataUniformName, 0.f, 0.f, 0.f);
    effect->setFloat4(_lightColorUniformName, 0.f, 0.f, 0.f, 0.f);
    return;
  }

  light->transferToNodeMaterialEffect(effect, _lightDataUniformName);

  effect->setColor4(_lightColorUniformName, light->diffuse, light->intensity);
}

void LightInformationBlock::prepareDefines(AbstractMesh* /*mesh*/,
                                           const NodeMaterialPtr& /*nodeMaterial*/,
                                           NodeMaterialDefines& defines, bool /*useInstances*/,
                                           const SubMeshPtr& /*subMesh*/)
{
  if (!defines._areLightsDirty) {
    return;
  }

  defines.setValue(_lightTypeDefineName,
                   light && light->getClassName() == "PointLight" ? true : false);
}

LightInformationBlock& LightInformationBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  state.sharedData->bindableBlocks.emplace_back(shared_from_this());
  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());

  const auto& _direction = direction();
  const auto& _color     = color();
  const auto& _intensity = intensity();

  _lightDataUniformName  = state._getFreeVariableName("lightData");
  _lightColorUniformName = state._getFreeVariableName("lightColor");
  _lightTypeDefineName   = state._getFreeDefineName("LIGHTPOINTTYPE");

  state._emitUniformFromString(_lightDataUniformName, "vec3");
  state._emitUniformFromString(_lightColorUniformName, "vec4");

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _lightTypeDefineName.c_str());
  state.compilationString
    += _declareOutput(_direction, state)
       + StringTools::printf(" = normalize(%s - %s.xyz - %s);\r\n", _lightDataUniformName.c_str(),
                             worldPosition()->associatedVariableName().c_str(),
                             _lightDataUniformName.c_str());
  state.compilationString += "#else\r\n";
  state.compilationString += _declareOutput(_direction, state)
                             + StringTools::printf(" = %s;\r\n", _lightDataUniformName.c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString
    += _declareOutput(_color, state)
       + StringTools::printf(" = %s.rgb;\r\n", _lightColorUniformName.c_str());
  state.compilationString += _declareOutput(_intensity, state)
                             + StringTools::printf(" = %s.a;\r\n", _lightColorUniformName.c_str());

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
