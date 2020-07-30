#include <babylon/materials/node/blocks/transform_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

TransformBlock::TransformBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , complementW{1.f}
    , complementZ{0.f}
    , vector{this, &TransformBlock::get_vector}
    , output{this, &TransformBlock::get_output}
    , xyz{this, &TransformBlock::get_xyz}
    , transform{this, &TransformBlock::get_transform}
{
  registerInput("vector", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("transform", NodeMaterialBlockConnectionPointTypes::Matrix);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerOutput("xyz", NodeMaterialBlockConnectionPointTypes::Vector3);

  _inputs[0]->onConnectionObservable.add(
    [this](NodeMaterialConnectionPoint* other, EventState & /*es*/) -> void {
      if (other && other->ownerBlock()->isInput()) {
        const auto otherAsInput = std::static_pointer_cast<InputBlock>(other->ownerBlock());

        if (otherAsInput->name == "normal" || otherAsInput->name == "tangent") {
          complementW = 0.f;
        }
      }
    });
}

TransformBlock::~TransformBlock() = default;

std::string TransformBlock::getClassName() const
{
  return "TransformBlock";
}

NodeMaterialConnectionPointPtr& TransformBlock::get_vector()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& TransformBlock::get_output()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& TransformBlock::get_xyz()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& TransformBlock::get_transform()
{
  return _inputs[1];
}

TransformBlock& TransformBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput    = output();
  const auto& iVector    = vector();
  const auto& iTransform = transform();
  const auto& iXyz       = xyz();

  if (iVector->connectedPoint()) {

    // None uniform scaling case.
    if (complementW == 0.f) {
      auto iComments = StringTools::printf("//%s", name.c_str());
      state._emitFunctionFromInclude("helperFunctions", iComments);
      state.sharedData->blocksWithDefines.emplace_back(shared_from_this());

      const auto transformName = state._getFreeVariableName(
        StringTools::printf("%s_NUS", iTransform->associatedVariableName().c_str()));
      state.compilationString
        += StringTools::printf("mat3 %s = mat3(%s);\r\n", transformName.c_str(),
                               iTransform->associatedVariableName().c_str());
      state.compilationString += "#ifdef NONUNIFORMSCALING\r\n";
      state.compilationString += StringTools::printf("%s = transposeMat3(inverseMat3(%s));\r\n",
                                                     transformName.c_str(), transformName.c_str());
      state.compilationString += "#endif\r\n";
      switch (iVector->connectedPoint()->type()) {
        case NodeMaterialBlockConnectionPointTypes::Vector2:
          state.compilationString
            += _declareOutput(iOutput, state)
               + StringTools::printf(" = vec4(%s * vec3(%s, %s), %s);\r\n", transformName.c_str(),
                                     iVector->associatedVariableName().c_str(),
                                     _writeFloat(complementZ).c_str(),
                                     _writeFloat(complementW).c_str());
          break;
        case NodeMaterialBlockConnectionPointTypes::Vector3:
        case NodeMaterialBlockConnectionPointTypes::Color3:
          state.compilationString
            += _declareOutput(iOutput, state)
               + StringTools::printf(" = vec4(%s * %s, %s);\r\n", transformName.c_str(),
                                     iVector->associatedVariableName().c_str(),
                                     _writeFloat(complementW).c_str());
          break;
        default:
          state.compilationString
            += _declareOutput(iOutput, state)
               + StringTools::printf(" = vec4(%s * %s.xyz, %s);\r\n", transformName.c_str(),
                                     iVector->associatedVariableName().c_str(),
                                     _writeFloat(complementW).c_str());
          break;
      }
    }
    else {
      const auto transformName = iTransform->associatedVariableName();
      switch (iVector->connectedPoint()->type()) {
        case NodeMaterialBlockConnectionPointTypes::Vector2:
          state.compilationString
            += _declareOutput(output, state)
               + StringTools::printf(" = %s * vec4(%s, %s, %s);\r\n", transformName.c_str(),
                                     iVector->associatedVariableName().c_str(),
                                     _writeFloat(complementZ).c_str(),
                                     _writeFloat(complementW).c_str());
          break;
        case NodeMaterialBlockConnectionPointTypes::Vector3:
        case NodeMaterialBlockConnectionPointTypes::Color3:
          state.compilationString
            += _declareOutput(output, state)
               + StringTools::printf(" = %s * vec4(%s, %s);\r\n", transformName.c_str(),
                                     iVector->associatedVariableName().c_str(),
                                     _writeFloat(complementW).c_str());
          break;
        default:
          state.compilationString
            += _declareOutput(output, state)
               + StringTools::printf(" = %s * %s;\r\n", transformName.c_str(),
                                     iVector->associatedVariableName().c_str());
          break;
      }
    }

    if (iXyz->hasEndpoints()) {
      state.compilationString
        += _declareOutput(iXyz, state)
           + StringTools::printf(" = %s.xyz;\r\n", iOutput->associatedVariableName().c_str());
    }
  }

  return *this;
}

void TransformBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& /*nodeMaterial*/,
                                    NodeMaterialDefines& defines, bool /*useInstances*/,
                                    const SubMeshPtr& /*subMesh*/)
{
  // Do nothing
  if (mesh->nonUniformScaling()) {
    defines.setValue("NONUNIFORMSCALING", true);
  }
}

json TransformBlock::serialize() const
{
  return nullptr;
}

void TransformBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
}

std::string TransformBlock::_dumpPropertiesCode()
{
  auto codeString
    = StringTools::printf("%s.complementZ = %f;\r\n", _codeVariableName.c_str(), complementZ);

  codeString
    += StringTools::printf("%s.complementW = %f;\r\n", _codeVariableName.c_str(), complementW);

  return codeString;
}

} // end of namespace BABYLON
