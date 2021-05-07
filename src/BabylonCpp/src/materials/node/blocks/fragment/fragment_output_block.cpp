#include <babylon/materials/node/blocks/fragment/fragment_output_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

FragmentOutputBlock::FragmentOutputBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment, true}
    , convertToGammaSpace{false}
    , convertToLinearSpace{false}
    , rgba{this, &FragmentOutputBlock::get_rgba}
    , rgb{this, &FragmentOutputBlock::get_rgb}
    , a{this, &FragmentOutputBlock::get_a}
{
  registerInput("rgba", NodeMaterialBlockConnectionPointTypes::Color4, true);
  registerInput("rgb", NodeMaterialBlockConnectionPointTypes::Color3, true);
  registerInput("a", NodeMaterialBlockConnectionPointTypes::Float, true);

  rgb()->acceptedConnectionPointTypes.emplace_back(NodeMaterialBlockConnectionPointTypes::Float);
}

FragmentOutputBlock::~FragmentOutputBlock() = default;

std::string FragmentOutputBlock::getClassName() const
{
  return "FragmentOutputBlock";
}

NodeMaterialConnectionPointPtr& FragmentOutputBlock::get_rgba()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& FragmentOutputBlock::get_rgb()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& FragmentOutputBlock::get_a()
{
  return _inputs[2];
}

void FragmentOutputBlock::prepareDefines(AbstractMesh* /*mesh*/,
                                         const NodeMaterialPtr& /*nodeMaterial*/,
                                         NodeMaterialDefines& defines, bool /*useInstances*/,
                                         SubMesh* /*subMesh*/)
{
  defines.setValue(_linearDefineName, convertToLinearSpace, true);
  defines.setValue(_gammaDefineName, convertToGammaSpace, true);
}

FragmentOutputBlock& FragmentOutputBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& _rgba = rgba();
  const auto& _rgb  = rgb();
  const auto& _a    = a();

  state.sharedData->hints.needAlphaBlending = _rgba->isConnected() || _a->isConnected();
  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());

  _linearDefineName = state._getFreeDefineName("CONVERTTOLINEAR");
  _gammaDefineName  = state._getFreeDefineName("CONVERTTOGAMMA");

  const auto iComments = StringTools::printf("//%s", name().c_str());
  state._emitFunctionFromInclude("helperFunctions", iComments);

  if (_rgba->connectedPoint()) {
    if (_a->isConnected()) {
      state.compilationString += StringTools::printf("gl_FragColor = vec4(%s.rgb, %s);\r\n",
                                                     _rgba->associatedVariableName().c_str(),
                                                     _a->associatedVariableName().c_str());
    }
    else {
      state.compilationString
        += StringTools::printf("gl_FragColor = %s;\r\n", _rgba->associatedVariableName().c_str());
    }
  }
  else if (_rgb->connectedPoint()) {
    std::string aValue = "1.0";

    if (_a->connectedPoint()) {
      aValue = _a->associatedVariableName();
    }

    if (_rgb->connectedPoint()->type() == NodeMaterialBlockConnectionPointTypes::Float) {
      state.compilationString += StringTools::printf(
        "gl_FragColor = vec4(%s, %s, %s, %s);\r\n", _rgb->associatedVariableName().c_str(),
        _rgb->associatedVariableName().c_str(), _rgb->associatedVariableName().c_str(),
        aValue.c_str());
    }
    else {
      state.compilationString += StringTools::printf(
        "gl_FragColor = vec4(%s, %s);\r\n", _rgb->associatedVariableName().c_str(), aValue.c_str());
    }
  }
  else {
    state.sharedData->checks.notConnectedNonOptionalInputs.emplace_back(_rgba);
  }

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _linearDefineName.c_str());
  state.compilationString += "gl_FragColor = toLinearSpace(gl_FragColor);\r\n";
  state.compilationString += "#endif\r\n";

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _gammaDefineName.c_str());
  state.compilationString += "gl_FragColor = toGammaSpace(gl_FragColor);\r\n";
  state.compilationString += "#endif\r\n";

  return *this;
}

std::string FragmentOutputBlock::_dumpPropertiesCode()
{
  std::string codeString = NodeMaterialBlock::_dumpPropertiesCode();
  codeString += StringTools::printf("%s.convertToGammaSpace = %s;\r\n", _codeVariableName.c_str(),
                                    convertToGammaSpace ? "true" : "false");
  codeString += StringTools::printf("%s.convertToLinearSpace = %s;\r\n", _codeVariableName.c_str(),
                                    convertToLinearSpace ? "true" : "false");

  return codeString;
}

json FragmentOutputBlock::serialize() const
{
  return nullptr;
}

void FragmentOutputBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                       const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
