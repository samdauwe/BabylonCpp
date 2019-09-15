#include <babylon/materials/node/blocks/fragment/image_processing_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

ImageProcessingBlock::ImageProcessingBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , color{this, &ImageProcessingBlock::get_color}
    , output{this, &ImageProcessingBlock::get_output}
{
  registerInput("color", NodeMaterialBlockConnectionPointTypes::Color4);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Color4);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color3);
}

ImageProcessingBlock::~ImageProcessingBlock()
{
}

const std::string ImageProcessingBlock::getClassName() const
{
  return "ImageProcessingBlock";
}

NodeMaterialConnectionPointPtr& ImageProcessingBlock::get_color()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ImageProcessingBlock::get_output()
{
  return _outputs[1];
}

void ImageProcessingBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("exposureLinear");
  state._excludeVariableName("contrast");
  state._excludeVariableName("vInverseScreenSize");
  state._excludeVariableName("vignetteSettings1");
  state._excludeVariableName("vignetteSettings2");
  state._excludeVariableName("vCameraColorCurveNegative");
  state._excludeVariableName("vCameraColorCurveNeutral");
  state._excludeVariableName("vCameraColorCurvePositive");
  state._excludeVariableName("txColorTransform");
  state._excludeVariableName("colorTransformSettings");
}

bool ImageProcessingBlock::isReady(AbstractMesh* mesh,
                                   const NodeMaterialPtr& nodeMaterial,
                                   const NodeMaterialDefines& defines,
                                   bool useInstances)
{
  if (defines._areImageProcessingDirty
      && nodeMaterial.imageProcessingConfiguration) {
    if (!nodeMaterial.imageProcessingConfiguration.isReady()) {
      return false;
    }
  }
  return true;
}

void ImageProcessingBlock::prepareDefines(AbstractMesh* mesh,
                                          const NodeMaterialPtr& nodeMaterial,
                                          const NodeMaterialDefines& defines,
                                          bool useInstances)
{
  if (defines._areImageProcessingDirty
      && nodeMaterial.imageProcessingConfiguration) {
    nodeMaterial.imageProcessingConfiguration.prepareDefines(defines);
  }
}

void ImageProcessingBlock::bind(Effect* effect,
                                const NodeMaterialPtr& nodeMaterial, Mesh* mesh)
{
  if (!mesh) {
    return;
  }

  if (!nodeMaterial.imageProcessingConfiguration) {
    return;
  }

  nodeMaterial.imageProcessingConfiguration.bind(effect);
}

ImageProcessingBlock&
ImageProcessingBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  // Register for defines
  state.sharedData->blocksWithDefines.emplace_back(this);

  // Register for blocking
  state.sharedData->blockingBlocks.emplace_back(this);

  // Register for binding
  state.sharedData->bindableBlocks.emplace_back(this);

  // Uniforms
  state.uniforms.emplace_back("exposureLinear");
  state.uniforms.emplace_back("contrast");
  state.uniforms.emplace_back("vInverseScreenSize");
  state.uniforms.emplace_back("vignetteSettings1");
  state.uniforms.emplace_back("vignetteSettings2");
  state.uniforms.emplace_back("vCameraColorCurveNegative");
  state.uniforms.emplace_back("vCameraColorCurveNeutral");
  state.uniforms.emplace_back("vCameraColorCurvePositive");
  state.uniforms.emplace_back("txColorTransform");
  state.uniforms.emplace_back("colorTransformSettings");

  // Emit code
  const auto& _color  = color();
  const auto& output  = _outputs[0];
  const auto comments = String::printf("//%s", name.c_str());

  state._emitFunctionFromInclude("helperFunctions", comments);
  state._emitFunctionFromInclude("imageProcessingDeclaration", comments);
  state._emitFunctionFromInclude("imageProcessingFunctions", comments);

  if (_color->connectedPoint()->type
      == NodeMaterialBlockConnectionPointTypes::Color4) {
    state.compilationString
      += String::printf("%s = %s;\r\n", _declareOutput(output, state).c_str(),
                        _color->associatedVariableName().c_str());
  }
  else {
    state.compilationString += String::printf(
      "%s = vec4(%s, 1.0);\r\n", _declareOutput(output, state).c_str(),
      _color->associatedVariableName().c_str());
  }
  state.compilationString += "#ifdef IMAGEPROCESSINGPOSTPROCESS\r\n";
  state.compilationString
    += String::printf("%s.rgb = toLinearSpace(%s.rgb);\r\n",
                      output->associatedVariableName().c_str(),
                      _color->associatedVariableName().c_str());
  state.compilationString += "#else\r\n";
  state.compilationString += "#ifdef IMAGEPROCESSING\r\n";
  state.compilationString
    += String::printf("%s.rgb = toLinearSpace(%s.rgb);\r\n",
                      output->associatedVariableName().c_str(),
                      _color->associatedVariableName().c_str());
  state.compilationString
    += String::printf("%s = applyImageProcessing(%s);\r\n",
                      output->associatedVariableName().c_str(),
                      output->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";
  state.compilationString += "#endif\r\n";

  return *this;
}

} // end of namespace BABYLON
