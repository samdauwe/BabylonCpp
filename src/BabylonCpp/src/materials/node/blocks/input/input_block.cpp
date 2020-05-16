#include <babylon/materials/node/blocks/input/input_block.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/blocks/input/input_value.h>
#include <babylon/materials/node/enums/node_material_system_values.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

InputBlock::InputBlock(const std::string& iName, NodeMaterialBlockTargets iTarget,
                       NodeMaterialBlockConnectionPointTypes type)
    : NodeMaterialBlock(iName, iTarget, false, true)
    , min{0.f}
    , max{0.f}
    , isBoolean{false}
    , matrixMode{0}
    , _systemValue{std::nullopt}
    , visibleInInspector{false}
    , isConstant{false}
    , type{this, &InputBlock::get_type}
    , output{this, &InputBlock::get_output}
    , value{this, &InputBlock::get_value, &InputBlock::set_value}
    , valueCallback{this, &InputBlock::get_valueCallback, &InputBlock::set_valueCallback}
    , associatedVariableName{this, &InputBlock::get_associatedVariableName,
                             &InputBlock::set_associatedVariableName}
    , animationType{this, &InputBlock::get_animationType, &InputBlock::set_animationType}
    , isUndefined{this, &InputBlock::get_isUndefined}
    , isUniform{this, &InputBlock::get_isUniform, &InputBlock::set_isUniform}
    , isAttribute{this, &InputBlock::get_isAttribute, &InputBlock::set_isAttribute}
    , isVarying{this, &InputBlock::get_isVarying, &InputBlock::set_isVarying}
    , isSystemValue{this, &InputBlock::get_isSystemValue}
    , systemValue{this, &InputBlock::get_systemValue, &InputBlock::set_systemValue}
    , _mode{NodeMaterialBlockConnectionPointMode::Undefined}
    , _storedValue{nullptr}
    , _valueCallback{nullptr}
    , _animationType{AnimatedInputBlockTypes::None}
{
  _type = type;

  setDefaultValue();

  registerOutput("output", type);
}

InputBlock::~InputBlock() = default;

NodeMaterialBlockConnectionPointTypes& InputBlock::get_type()
{
  if (_type == NodeMaterialBlockConnectionPointTypes::AutoDetect) {
    if (isUniform() && value() != nullptr) {
      const auto valueType = value()->animationType();
      if (valueType.has_value()) {
        switch (*valueType) {
          case Animation::ANIMATIONTYPE_FLOAT:
            _type = NodeMaterialBlockConnectionPointTypes::Float;
            return _type;
          case Animation::ANIMATIONTYPE_INT:
            _type = NodeMaterialBlockConnectionPointTypes::Int;
            return _type;
          case Animation::ANIMATIONTYPE_VECTOR2:
            _type = NodeMaterialBlockConnectionPointTypes::Vector2;
            return _type;
          case Animation::ANIMATIONTYPE_VECTOR3:
            _type = NodeMaterialBlockConnectionPointTypes::Vector3;
            return _type;
          case Animation::ANIMATIONTYPE_VECTOR4:
            _type = NodeMaterialBlockConnectionPointTypes::Vector4;
            return _type;
          case Animation::ANIMATIONTYPE_COLOR3:
            _type = NodeMaterialBlockConnectionPointTypes::Color3;
            return _type;
          case Animation::ANIMATIONTYPE_COLOR4:
            _type = NodeMaterialBlockConnectionPointTypes::Color4;
            return _type;
          case Animation::ANIMATIONTYPE_MATRIX:
            _type = NodeMaterialBlockConnectionPointTypes::Matrix;
            return _type;
          default:
            break;
        }
      }
    }

    if (isAttribute()) {
      if (name == "position" || name == "normal" || name == "tangent") {
        _type = NodeMaterialBlockConnectionPointTypes::Vector3;
        return _type;
      }
      else if (name == "uv" || name == "uv2") {
        _type = NodeMaterialBlockConnectionPointTypes::Vector2;
        return _type;
      }
      else if (name == "matricesIndices" || name == "matricesWeights" || name == "world0"
               || name == "world1" || name == "world2" || name == "world3") {
        _type = NodeMaterialBlockConnectionPointTypes::Vector4;
        return _type;
      }
      else if (name == "color") {
        _type = NodeMaterialBlockConnectionPointTypes::Color4;
        return _type;
      }
    }

    if (isSystemValue()) {
      switch (*_systemValue) {
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
        case NodeMaterialSystemValues::DeltaTime:
          _type = NodeMaterialBlockConnectionPointTypes::Float;
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

InputBlock& InputBlock::setAsSystemValue(const std::optional<NodeMaterialSystemValues>& iValue)
{
  systemValue = iValue;
  return *this;
}

AnimationValuePtr& InputBlock::get_value()
{
  return _storedValue;
}

void InputBlock::set_value(const AnimationValuePtr& iValue)
{
  AnimationValuePtr newValue = iValue;
  if (type() == NodeMaterialBlockConnectionPointTypes::Float) {
    if (isBoolean) {
      newValue = std::make_shared<AnimationValue>(iValue->get<float>() != 0.f ? 1.f : 0.f);
    }
    else if (!stl_util::almost_equal(min, max)) {
      auto tmpValue = iValue->get<float>();
      tmpValue      = std::max(min, tmpValue);
      tmpValue      = std::min(max, tmpValue);
      newValue      = std::make_shared<AnimationValue>(tmpValue);
    }
  }

  _storedValue = newValue;
  _mode        = NodeMaterialBlockConnectionPointMode::Uniform;

  onValueChangedObservable.notifyObservers(this);
}

std::function<AnimationValuePtr()>& InputBlock::get_valueCallback()
{
  return _valueCallback;
}

void InputBlock::set_valueCallback(const std::function<AnimationValuePtr()>& iValue)
{
  _valueCallback = iValue;
  _mode          = NodeMaterialBlockConnectionPointMode::Uniform;
}

std::string InputBlock::get_associatedVariableName() const
{
  return _associatedVariableName;
}

void InputBlock::set_associatedVariableName(std::string iValue)
{
  _associatedVariableName = std::move(iValue);
}

AnimatedInputBlockTypes& InputBlock::get_animationType()
{
  return _animationType;
}

void InputBlock::set_animationType(const AnimatedInputBlockTypes& iValue)
{
  _animationType = iValue;
}

bool InputBlock::get_isUndefined() const
{
  return _mode == NodeMaterialBlockConnectionPointMode::Undefined;
}

bool InputBlock::get_isUniform() const
{
  return _mode == NodeMaterialBlockConnectionPointMode::Uniform;
}

void InputBlock::set_isUniform(bool iValue)
{
  _mode = iValue ? NodeMaterialBlockConnectionPointMode::Uniform :
                   NodeMaterialBlockConnectionPointMode::Undefined;
  associatedVariableName = "";
}

bool InputBlock::get_isAttribute() const
{
  return _mode == NodeMaterialBlockConnectionPointMode::Attribute;
}

void InputBlock::set_isAttribute(bool iValue)
{
  _mode = iValue ? NodeMaterialBlockConnectionPointMode::Attribute :
                   NodeMaterialBlockConnectionPointMode::Undefined;
  associatedVariableName = "";
}

bool InputBlock::get_isVarying() const
{
  return _mode == NodeMaterialBlockConnectionPointMode::Varying;
}

void InputBlock::set_isVarying(bool iValue)
{
  _mode = iValue ? NodeMaterialBlockConnectionPointMode::Varying :
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

void InputBlock::set_systemValue(const std::optional<NodeMaterialSystemValues>& iValue)
{
  _mode                  = NodeMaterialBlockConnectionPointMode::Uniform;
  associatedVariableName = "";
  _systemValue           = iValue;
}

std::string InputBlock::getClassName() const
{
  return "InputBlock";
}

void InputBlock::animate(Scene* scene)
{
  switch (_animationType) {
    case AnimatedInputBlockTypes::Time: {
      if (type() == NodeMaterialBlockConnectionPointTypes::Float) {
        value()->get<float>() += scene->getAnimationRatio() * 0.01f;
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
    return StringTools::printf("#ifndef %s\r\n", define.substr(1).c_str());
  }

  return StringTools::printf("#ifdef %s\r\n", define.c_str());
}

void InputBlock::initialize(NodeMaterialBuildState& /*state*/)
{
  associatedVariableName = "";
}

void InputBlock::setDefaultValue()
{
  switch (type) {
    case NodeMaterialBlockConnectionPointTypes::Float:
      value()->get<float>() = 0.f;
      break;
    case NodeMaterialBlockConnectionPointTypes::Int:
      value()->get<int>() = 0;
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector2:
      value()->get<Vector2>() = Vector2::Zero();
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector3:
      value()->get<Vector3>() = Vector3::Zero();
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector4:
      value()->get<Vector4>() = Vector4::Zero();
      break;
    case NodeMaterialBlockConnectionPointTypes::Color3:
      value()->get<Color3>() = Color3::White();
      break;
    case NodeMaterialBlockConnectionPointTypes::Color4:
      value()->get<Color4>() = Color4(1.f, 1.f, 1.f, 1.f);
      break;
    case NodeMaterialBlockConnectionPointTypes::Matrix:
      value()->get<Matrix>() = Matrix::Identity();
      break;
    default:
      break;
  }
}

std::string InputBlock::_emitConstant(NodeMaterialBuildState& state)
{
  switch (type()) {
    case NodeMaterialBlockConnectionPointTypes::Float: {
      const auto floatValue = value()->get<float>();
      return state._emitFloat(floatValue);
    }
    case NodeMaterialBlockConnectionPointTypes::Vector2: {
      const auto& vector2Value = value()->get<Vector2>();
      return StringTools::printf("vec2(%f, %f)", vector2Value.x, vector2Value.y);
    }
    case NodeMaterialBlockConnectionPointTypes::Vector3: {
      const auto& vector3Value = value()->get<Vector3>();
      return StringTools::printf("vec3(%f, %f, %f)", vector3Value.x, vector3Value.y,
                                 vector3Value.z);
    }
    case NodeMaterialBlockConnectionPointTypes::Vector4: {
      const auto& vector4Value = value()->get<Vector4>();
      return StringTools::printf("vec4(%f, %f, %f, %f)", vector4Value.x, vector4Value.y,
                                 vector4Value.z, vector4Value.w);
    }
    case NodeMaterialBlockConnectionPointTypes::Color3: {
      const auto& color3Value = value()->get<Color3>();
      return StringTools::printf("vec3(%f, %f, %f)", color3Value.r, color3Value.g, color3Value.b);
    }
    case NodeMaterialBlockConnectionPointTypes::Color4: {
      const auto& color4Value = value()->get<Color4>();
      return StringTools::printf("vec4(%f, %f, %f, %f)", color4Value.r, color4Value.g,
                                 color4Value.b, color4Value.a);
    }
    default:
      break;
  }

  return "";
}

void InputBlock::_emit(NodeMaterialBuildState& state, const std::string& define)
{
  // Uniforms
  if (isUniform) {
    if (associatedVariableName().empty()) {
      associatedVariableName = state._getFreeVariableName("u_" + name);
    }

    if (isConstant) {
      if (stl_util::contains(state.constants, associatedVariableName())) {
        return;
      }
      state.constants.emplace_back(associatedVariableName);
      state._constantDeclaration
        += _declareOutput(output, state)
           + StringTools::printf(" = %s;\r\n", _emitConstant(state).c_str());
      return;
    }

    if (stl_util::contains(state.uniforms, associatedVariableName())) {
      return;
    }

    state.uniforms.emplace_back(associatedVariableName);
    if (!define.empty()) {
      state._uniformDeclaration += _emitDefine(define);
    }
    state._uniformDeclaration += StringTools::printf(
      "uniform %s %s;\r\n", state._getGLType(type()).c_str(), associatedVariableName().c_str());
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

    if (target() == NodeMaterialBlockTargets::Vertex
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
    state._attributeDeclaration += StringTools::printf(
      "attribute %s %s;\r\n", state._getGLType(type()).c_str(), associatedVariableName().c_str());
    if (!define.empty()) {
      state._attributeDeclaration += "#endif\r\n";
    }
  }
}

void InputBlock::_transmitWorld(Effect* effect, const Matrix& world, const Matrix& worldView,
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
        effect->setVector3(variableName, scene->activeCamera()->globalPosition());
        break;
      case NodeMaterialSystemValues::FogColor:
        effect->setColor3(variableName, scene->fogColor);
        break;
      case NodeMaterialSystemValues::DeltaTime:
        effect->setFloat(variableName, scene->deltaTime / 1000.f);
    }
    return;
  }

  const auto& iValue = _valueCallback ? _valueCallback() : _storedValue;

  if (iValue == nullptr) {
    return;
  }

  switch (type()) {
    case NodeMaterialBlockConnectionPointTypes::Float:
      effect->setFloat(variableName, iValue->get<float>());
      break;
    case NodeMaterialBlockConnectionPointTypes::Int:
      effect->setInt(variableName, iValue->get<int>());
      break;
    case NodeMaterialBlockConnectionPointTypes::Color3:
      effect->setColor3(variableName, iValue->get<Color3>());
      break;
    case NodeMaterialBlockConnectionPointTypes::Color4:
      effect->setDirectColor4(variableName, iValue->get<Color4>());
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector2:
      effect->setVector2(variableName, iValue->get<Vector2>());
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector3:
      effect->setVector3(variableName, iValue->get<Vector3>());
      break;
    case NodeMaterialBlockConnectionPointTypes::Vector4:
      effect->setVector4(variableName, iValue->get<Vector4>());
      break;
    case NodeMaterialBlockConnectionPointTypes::Matrix:
      effect->setMatrix(variableName, iValue->get<Matrix>());
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

  return *this;
}

std::string InputBlock::_dumpPropertiesCode()
{
  const auto& variableName = _codeVariableName;

  if (isAttribute()) {
    return StringTools::printf("%s.setAsAttribute(\"%s\");\r\n", variableName.c_str(),
                               name.c_str());
  }
  if (isSystemValue()) {
    return StringTools::printf("%s.setAsSystemValue(NodeMaterialSystemValues(%u));\r\n",
                               variableName.c_str(), static_cast<unsigned int>(*_systemValue));
  }
  if (isUniform()) {
    std::vector<std::string> codes;

    std::string valueString = "";

    switch (type()) {
      case NodeMaterialBlockConnectionPointTypes::Float: {
        const auto floatValue = value()->get<float>();
        valueString           = StringTools::printf("%f", floatValue);
      } break;
      case NodeMaterialBlockConnectionPointTypes::Vector2: {
        const auto& vector2Value = value()->get<Vector2>();
        valueString = StringTools::printf("Vector2(%f, %f)", vector2Value.x, vector2Value.y);
      } break;
      case NodeMaterialBlockConnectionPointTypes::Vector3: {
        const auto& vector3Value = value()->get<Vector3>();
        valueString = StringTools::printf("Vector3(%f, %f, %f)", vector3Value.x, vector3Value.y,
                                          vector3Value.z);
      } break;
      case NodeMaterialBlockConnectionPointTypes::Vector4: {
        const auto& vector4Value = value()->get<Vector4>();
        valueString = StringTools::printf("Vector4(%f, %f, %f, %f)", vector4Value.x, vector4Value.y,
                                          vector4Value.z, vector4Value.x);
      } break;
      case NodeMaterialBlockConnectionPointTypes::Color3: {
        const auto& color3Value = value()->get<Color3>();
        valueString
          = StringTools::printf("Color3(%f, %f, %f)", color3Value.r, color3Value.g, color3Value.b);
      } break;
      case NodeMaterialBlockConnectionPointTypes::Color4: {
        const auto& color4Value = value()->get<Color4>();
        valueString = StringTools::printf("Color4(%f, %f, %f, %f)", color4Value.r, color4Value.g,
                                          color4Value.b, color4Value.a);
      } break;
      case NodeMaterialBlockConnectionPointTypes::Matrix: {
        // const auto& matrixValue = value()->get<Matrix>();
        // valueString = StringTools::printf("Matrix::FromArray(%s)", matrixValue.m());
      } break;
      default:
        break;
    }

    // Common Property "Value"
    codes.emplace_back(
      StringTools::printf("%s.value = %s", variableName.c_str(), valueString.c_str()));

    // Float-Value-Specific Properties
    if (type() == NodeMaterialBlockConnectionPointTypes::Float) {
      stl_util::concat(
        codes,
        {StringTools::printf("%s.min = %f;\r\n", variableName.c_str(), min),
         StringTools::printf("%s.max = %f;\r\n", variableName.c_str(), max),
         StringTools::printf("%s.isBoolean = %s;\r\n", variableName.c_str(),
                             isBoolean ? "true" : "false"),
         StringTools::printf("%s.matrixMode = %u;\r\n", variableName.c_str(), matrixMode),
         StringTools::printf("%s.animationType  = AnimatedInputBlockTypes(%u);\r\n",
                             variableName.c_str(), static_cast<unsigned int>(animationType()))});
    }

    // Common Property "Type"
    stl_util::concat(codes,
                     {StringTools::printf("%s.isConstant = %s;\r\n", variableName.c_str(),
                                          isConstant ? "true" : "false"),
                      StringTools::printf("%s.visibleInInspector = %s;\r\n", variableName.c_str(),
                                          visibleInInspector ? "true" : "false")});

    return StringTools::join(codes, ";\r\n");
  }
  return "";
}

void InputBlock::dispose()
{
  onValueChangedObservable.clear();

  NodeMaterialBlock::dispose();
}

json InputBlock::serialize() const
{
  return nullptr;
}

void InputBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                              const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
