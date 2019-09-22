#include <babylon/materials/node/blocks/input/input_block.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_system_values.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

InputBlock::InputBlock(const std::string& iName,
                       NodeMaterialBlockTargets iTarget,
                       NodeMaterialBlockConnectionPointTypes type)
    : NodeMaterialBlock(iName, iTarget, false, true)
    , _systemValue{std::nullopt}
    , visibleInInspector{false}
    , type{this, &InputBlock::get_type}
    , output{this, &InputBlock::get_output}
    , associatedVariableName{this, &InputBlock::get_associatedVariableName,
                             &InputBlock::set_associatedVariableName}
    , animationType{this, &InputBlock::get_animationType,
                    &InputBlock::set_animationType}
    , isUndefined{this, &InputBlock::get_isUndefined}
    , isUniform{this, &InputBlock::get_isUniform, &InputBlock::set_isUniform}
    , isAttribute{this, &InputBlock::get_isAttribute,
                  &InputBlock::set_isAttribute}
    , isVarying{this, &InputBlock::get_isVarying, &InputBlock::set_isVarying}
    , isSystemValue{this, &InputBlock::get_isSystemValue}
    , systemValue{this, &InputBlock::get_systemValue,
                  &InputBlock::set_systemValue}
    , _mode{NodeMaterialBlockConnectionPointMode::Undefined}
    , _animationType{AnimatedInputBlockTypes::None}
{
  _type = type;

  setDefaultValue();

  registerOutput("output", type);
}

InputBlock::~InputBlock()
{
}

NodeMaterialBlockConnectionPointTypes& InputBlock::get_type()
{
  if (_type == NodeMaterialBlockConnectionPointTypes::AutoDetect) {
    if (isUniform() && value != nullptr) {
      if (!isNaN(value)) {
        _type = NodeMaterialBlockConnectionPointTypes::Float;
        return _type;
      }

      switch (value.getClassName()) {
        case "Vector2":
          _type = NodeMaterialBlockConnectionPointTypes::Vector2;
          return _type;
        case "Vector3":
          _type = NodeMaterialBlockConnectionPointTypes::Vector3;
          return _type;
        case "Vector4":
          _type = NodeMaterialBlockConnectionPointTypes::Vector4;
          return _type;
        case "Color3":
          _type = NodeMaterialBlockConnectionPointTypes::Color3;
          return _type;
        case "Color4":
          _type = NodeMaterialBlockConnectionPointTypes::Color4;
          return _type;
      }
    }

    if (isAttribute) {
      switch (name) {
        case "position":
        case "normal":
        case "tangent":
          _type = NodeMaterialBlockConnectionPointTypes::Vector3;
          return _type;
        case "uv":
        case "uv2":
          _type = NodeMaterialBlockConnectionPointTypes::Vector2;
          return _type;
        case "matricesIndices":
        case "matricesWeights":
        case "world0":
        case "world1":
        case "world2":
        case "world3":
          _type = NodeMaterialBlockConnectionPointTypes::Vector4;
          return _type;
        case "color":
          _type = NodeMaterialBlockConnectionPointTypes::Color4;
          return _type;
      }
    }

    if (isSystemValue) {
      switch (_systemValue) {
        case NodeMaterialSystemValues::World:
        case NodeMaterialSystemValues::WorldView:
        case NodeMaterialSystemValues::WorldViewProjection:
        case NodeMaterialSystemValues::View:
        case NodeMaterialSystemValues::ViewProjection:
        case NodeMaterialSystemValues::Projection:
          _type = NodeMaterialBlockConnectionPointTypes::Matrix;
          return _type;
        case NodeMaterialSystemValues::CameraPosition:
          _type = NodeMaterialBlockConnectionPointTypes::Vector3;
          return _type;
        case NodeMaterialSystemValues::FogColor:
          _type = NodeMaterialBlockConnectionPointTypes::Color3;
          return _type;
      }
    }
  }

  return _type;
}

NodeMaterialConnectionPointPtr& InputBlock::get_output()
{
  return _outputs[0];
}

InputBlock& InputBlock::setAsAttribute(const std::string& attributeName)
{
  if (!attributeName.empty()) {
    name = attributeName;
  }
  _mode = NodeMaterialBlockConnectionPointMode::Attribute;
  return *this;
}

InputBlock& InputBlock::setAsSystemValue(
  const std::optional<NodeMaterialSystemValues>& value)
{
  systemValue = value;
  return *this;
}

std::string InputBlock::get_associatedVariableName() const
{
  return _associatedVariableName;
}

void InputBlock::set_associatedVariableName(std::string value)
{
  _associatedVariableName = std::move(value);
}

AnimatedInputBlockTypes& InputBlock::get_animationType()
{
  return _animationType;
}

void InputBlock::set_animationType(const AnimatedInputBlockTypes& value)
{
  _animationType = value;
}

bool InputBlock::get_isUndefined() const
{
  return _mode == NodeMaterialBlockConnectionPointMode::Undefined;
}

bool InputBlock::get_isUniform() const
{
  return _mode == NodeMaterialBlockConnectionPointMode::Uniform;
}

void InputBlock::set_isUniform(bool value)
{
  _mode = value ? NodeMaterialBlockConnectionPointMode::Uniform :
                  NodeMaterialBlockConnectionPointMode::Undefined;
  associatedVariableName = "";
}

bool InputBlock::get_isAttribute() const
{
  return _mode == NodeMaterialBlockConnectionPointMode::Attribute;
}

void InputBlock::set_isAttribute(bool value)
{
  _mode = value ? NodeMaterialBlockConnectionPointMode::Attribute :
                  NodeMaterialBlockConnectionPointMode::Undefined;
  associatedVariableName = "";
}

bool InputBlock::get_isVarying() const
{
  return _mode == NodeMaterialBlockConnectionPointMode::Varying;
}

void InputBlock::set_isVarying(bool value)
{
  _mode = value ? NodeMaterialBlockConnectionPointMode::Varying :
                  NodeMaterialBlockConnectionPointMode::Undefined;
  associatedVariableName = "";
}

bool InputBlock::get_isSystemValue() const
{
  return _systemValue.has_value();
}

std::optional<NodeMaterialSystemValues>& InputBlock::get_systemValue()
{
  return _systemValue;
}

void InputBlock::set_systemValue(
  const std::optional<NodeMaterialSystemValues>& value)
{
  _mode                  = NodeMaterialBlockConnectionPointMode::Uniform;
  associatedVariableName = "";
  _systemValue           = value;
}

const std::string InputBlock::getClassName() const
{
  return "InputBlock";
}

void InputBlock::animate(Scene* scene)
{
  switch (_animationType) {
    case AnimatedInputBlockTypes::Time: {
      if (type == NodeMaterialBlockConnectionPointTypes::Float) {
        value += scene->getAnimationRatio() * 0.01f;
      }
      break;
    }
    default:
      break;
  }
}

std::string InputBlock::_emitDefine(const std::string& define) const
{
  if (!define.empty() && define[0] == '!') {
    return String::printf("#ifndef %s\r\n", define.substr(1).c_str());
  }

  return String::printf("#ifdef %s\r\n", define.c_str());
}

void InputBlock::setDefaultValue()
{
  switch (type) {
    case NodeMaterialBlockConnectionPointTypes::Float:
      value = 0.f;
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector2:
      value = Vector2::Zero();
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector3:
      value = Vector3::Zero();
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector4:
      value = Vector4::Zero();
      break;
    case NodeMaterialBlockConnectionPointTypes::Color3:
      value = Color3::White();
      break;
    case NodeMaterialBlockConnectionPointTypes::Color4:
      value = Color4(1.f, 1.f, 1.f, 1.f);
      break;
    case NodeMaterialBlockConnectionPointTypes::Matrix:
      value = Matrix::Identity();
      break;
    default:
      break;
  }
}

std::string InputBlock::_dumpPropertiesCode()
{
  // TODO
  return "";
}

void InputBlock::_emit(NodeMaterialBuildState& state, const std::string& define)
{
  // Uniforms
  if (isUniform) {
    if (associatedVariableName().empty()) {
      associatedVariableName = state._getFreeVariableName("u_" + name);
    }

    if (stl_util::contains(state.uniforms, associatedVariableName())) {
      return;
    }

    state.uniforms.emplace_back(associatedVariableName);
    if (!define.empty()) {
      state._uniformDeclaration += _emitDefine(define);
    }
    state._uniformDeclaration
      += String::printf("uniform %s %s;\r\n", state._getGLType(type()).c_str(),
                        associatedVariableName().c_str());
    if (!define.empty()) {
      state._uniformDeclaration += "#endif\r\n";
    }

    // well known
    auto& hints = state.sharedData->hints;
    if (_systemValue.has_value()) {
      switch (*_systemValue) {
        case NodeMaterialSystemValues::WorldView:
          hints.needWorldViewMatrix = true;
          break;
        case NodeMaterialSystemValues::WorldViewProjection:
          hints.needWorldViewProjectionMatrix = true;
          break;
        default:
          break;
      }
    }
    else {
      if (_animationType != AnimatedInputBlockTypes::None) {
        state.sharedData->animatedInputs.emplace_back(this);
      }
    }

    return;
  }

  // Attribute
  if (isAttribute) {
    associatedVariableName = name;

    if (target == NodeMaterialBlockTargets::Vertex
        && state._vertexState) { // Attribute for fragment need to be carried
                                 // over by varyings
      _emit(*state._vertexState, define);
      return;
    }

    if (stl_util::contains(state.attributes, associatedVariableName())) {
      return;
    }

    state.attributes.emplace_back(associatedVariableName);
    if (!define.empty()) {
      state._attributeDeclaration += _emitDefine(define);
    }
    state._attributeDeclaration += String::printf(
      "attribute %s %s;\r\n", state._getGLType(type()).c_str(),
      associatedVariableName().c_str());
    if (!define.empty()) {
      state._attributeDeclaration += "#endif\r\n";
    }
  }
}

void InputBlock::_transmitWorld(Effect* effect, const Matrix& world,
                                const Matrix& worldView,
                                Matrix& worldViewProjection)
{
  if (!_systemValue.has_value()) {
    return;
  }

  const auto& variableName = associatedVariableName();
  switch (*_systemValue) {
    case NodeMaterialSystemValues::World:
      effect->setMatrix(variableName, world);
      break;
    case NodeMaterialSystemValues::WorldView:
      effect->setMatrix(variableName, worldView);
      break;
    case NodeMaterialSystemValues::WorldViewProjection:
      effect->setMatrix(variableName, worldViewProjection);
      break;
    default:
      break;
  }
}

void InputBlock::_transmit(Effect* effect, Scene* scene)
{
  if (isAttribute()) {
    return;
  }

  const auto& variableName = associatedVariableName();
  if (_systemValue.has_value()) {
    switch (*_systemValue) {
      case NodeMaterialSystemValues::World:
      case NodeMaterialSystemValues::WorldView:
      case NodeMaterialSystemValues::WorldViewProjection:
        return;
      case NodeMaterialSystemValues::View:
        effect->setMatrix(variableName, scene->getViewMatrix());
        break;
      case NodeMaterialSystemValues::Projection:
        effect->setMatrix(variableName, scene->getProjectionMatrix());
        break;
      case NodeMaterialSystemValues::ViewProjection:
        effect->setMatrix(variableName, scene->getTransformMatrix());
        break;
      case NodeMaterialSystemValues::CameraPosition:
        effect->setVector3(variableName,
                           scene->activeCamera()->globalPosition());
        break;
      case NodeMaterialSystemValues::FogColor:
                        effect->setColor3(variableName, scene->fogColor;
                        break;
    }
    return;
  }

  const auto& value = _valueCallback ? _valueCallback() : _storedValue;

  if (value == nullptr) {
    return;
  }

  switch (type()) {
    case NodeMaterialBlockConnectionPointTypes::Float:
      effect->setFloat(variableName, value);
      break;
    case NodeMaterialBlockConnectionPointTypes::Int:
      effect->setInt(variableName, value);
      break;
    case NodeMaterialBlockConnectionPointTypes::Color3:
      effect->setColor3(variableName, value);
      break;
    case NodeMaterialBlockConnectionPointTypes::Color4:
      effect->setDirectColor4(variableName, value);
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector2:
      effect->setVector2(variableName, value);
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector3:
      effect->setVector3(variableName, value);
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector4:
      effect->setVector4(variableName, value);
      break;
    case NodeMaterialBlockConnectionPointTypes::Matrix:
      effect->setMatrix(variableName, value);
      break;
    default:
      break;
  }
}

InputBlock& InputBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (isUniform() || isSystemValue()) {
    state.sharedData->inputBlocks.emplace_back(this);
  }

  _emit(state);
}

json InputBlock::serialize() const
{
  return nullptr;
}

void InputBlock::_deserialize(const json& /*serializationObject*/,
                              Scene* /*scene*/, const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
