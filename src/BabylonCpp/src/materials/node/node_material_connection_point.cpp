#include <babylon/materials/node/node_material_connection_point.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

NodeMaterialConnectionPoint::NodeMaterialConnectionPoint(
  const std::string& iName, const NodeMaterialBlockPtr& ownerBlock,
  const NodeMaterialConnectionPointDirection& direction)
    : _ownerBlock{nullptr}
    , _connectedPoint{nullptr}
    , _typeConnectionSource{nullptr}
    , _linkedConnectionSource{nullptr}
    , _enforceAssociatedVariableName{false}
    , direction{this, &NodeMaterialConnectionPoint::get_direction}
    , associatedVariableName{this, &NodeMaterialConnectionPoint::get_associatedVariableName,
                             &NodeMaterialConnectionPoint::set_associatedVariableName}
    , innerType{this, &NodeMaterialConnectionPoint::get_innerType}
    , type{this, &NodeMaterialConnectionPoint::get_type, &NodeMaterialConnectionPoint::set_type}
    , isOptional{false}
    , _prioritizeVertex{false}
    , target{this, &NodeMaterialConnectionPoint::get_target,
             &NodeMaterialConnectionPoint::set_target}
    , isConnected{this, &NodeMaterialConnectionPoint::get_isConnected}
    , isConnectedToInputBlock{this, &NodeMaterialConnectionPoint::get_isConnectedToInputBlock}
    , connectInputBlock{this, &NodeMaterialConnectionPoint::get_connectInputBlock}
    , connectedPoint{this, &NodeMaterialConnectionPoint::get_connectedPoint}
    , ownerBlock{this, &NodeMaterialConnectionPoint::get_ownerBlock}
    , sourceBlock{this, &NodeMaterialConnectionPoint::get_sourceBlock}
    , connectedBlocks{this, &NodeMaterialConnectionPoint::get_connectedBlocks}
    , endpoints{this, &NodeMaterialConnectionPoint::get_endpoints}
    , hasEndpoints{this, &NodeMaterialConnectionPoint::get_hasEndpoints}
    , isConnectedInVertexShader{this, &NodeMaterialConnectionPoint::get_isConnectedInVertexShader}
    , isConnectedInFragmentShader{this,
                                  &NodeMaterialConnectionPoint::get_isConnectedInFragmentShader}
    , _target{NodeMaterialBlockTargets::VertexAndFragment}
    , _type{NodeMaterialBlockConnectionPointTypes::Float}
    , _connectInputBlock{nullptr}
    , _sourceBlock{nullptr}
{
  _ownerBlock = ownerBlock;
  name        = iName;
  _direction  = direction;
}

NodeMaterialConnectionPointDirection& NodeMaterialConnectionPoint::get_direction()
{
  return _direction;
}

std::string NodeMaterialConnectionPoint::get_associatedVariableName() const
{
  if (_ownerBlock->isInput()) {
    auto inputBlock = std::static_pointer_cast<InputBlock>(_ownerBlock);
    if (inputBlock) {
      return inputBlock->associatedVariableName();
    }
  }

  if ((!_enforceAssociatedVariableName || _associatedVariableName.empty()) && _connectedPoint) {
    return _connectedPoint->associatedVariableName();
  }

  return _associatedVariableName;
}

void NodeMaterialConnectionPoint::set_associatedVariableName(std::string value)
{
  _associatedVariableName = value;
}

NodeMaterialBlockConnectionPointTypes& NodeMaterialConnectionPoint::get_innerType()
{
  return _type;
}

NodeMaterialBlockConnectionPointTypes& NodeMaterialConnectionPoint::get_type()
{
  if (_type == NodeMaterialBlockConnectionPointTypes::AutoDetect) {
    if (_ownerBlock->isInput()) {
      auto inputBlock = std::static_pointer_cast<InputBlock>(_ownerBlock);
      if (inputBlock) {
        return inputBlock->type();
      }
    }

    if (_connectedPoint) {
      return _connectedPoint->type();
    }

    if (_linkedConnectionSource && _linkedConnectionSource->isConnected()) {
      return _linkedConnectionSource->type();
    }
  }

  if (_type == NodeMaterialBlockConnectionPointTypes::BasedOnInput && _typeConnectionSource) {
    return _typeConnectionSource->type();
  }

  return _type;
}

void NodeMaterialConnectionPoint::set_type(const NodeMaterialBlockConnectionPointTypes& value)
{
  _type = value;
}

NodeMaterialBlockTargets& NodeMaterialConnectionPoint::get_target()
{
  if (!_prioritizeVertex || !_ownerBlock) {
    return _target;
  }

  if (_target != NodeMaterialBlockTargets::VertexAndFragment) {
    return _target;
  }

  if (_ownerBlock->target() == NodeMaterialBlockTargets::Fragment) {
    _tmpTarget = NodeMaterialBlockTargets::Fragment;
    return _tmpTarget;
  }

  _tmpTarget = NodeMaterialBlockTargets::Vertex;
  return _tmpTarget;
}

void NodeMaterialConnectionPoint::set_target(const NodeMaterialBlockTargets& value)
{
  _target = value;
}

bool NodeMaterialConnectionPoint::get_isConnected() const
{
  return connectedPoint() != nullptr;
}

bool NodeMaterialConnectionPoint::get_isConnectedToInputBlock() const
{
  return connectedPoint() != nullptr && connectedPoint()->ownerBlock()->isInput();
}

InputBlockPtr& NodeMaterialConnectionPoint::get_connectInputBlock()
{
  if (!isConnectedToInputBlock()) {
    _connectInputBlock = nullptr;
    return _connectInputBlock;
  }

  _connectInputBlock = std::static_pointer_cast<InputBlock>(connectedPoint()->ownerBlock());
  return _connectInputBlock;
}

NodeMaterialConnectionPointPtr& NodeMaterialConnectionPoint::get_connectedPoint()
{
  return _connectedPoint;
}

NodeMaterialBlockPtr& NodeMaterialConnectionPoint::get_ownerBlock()
{
  return _ownerBlock;
}

NodeMaterialBlockPtr& NodeMaterialConnectionPoint::get_sourceBlock()
{
  if (!_connectedPoint) {
    _sourceBlock = nullptr;
    return _sourceBlock;
  }

  _sourceBlock = _connectedPoint->ownerBlock();
  return _sourceBlock;
}

std::vector<NodeMaterialBlockPtr>& NodeMaterialConnectionPoint::get_connectedBlocks()
{
  _connectedBlocks.clear();
  if (_endpoints.empty()) {
    return _connectedBlocks;
  }

  for (const auto& e : _endpoints) {
    _connectedBlocks.emplace_back(e->ownerBlock());
  }

  return _connectedBlocks;
}

std::vector<NodeMaterialConnectionPointPtr>& NodeMaterialConnectionPoint::get_endpoints()
{
  return _endpoints;
}

bool NodeMaterialConnectionPoint::get_hasEndpoints() const
{
  return !_endpoints.empty();
}

bool NodeMaterialConnectionPoint::get_isConnectedInVertexShader() const
{
  if (target() == NodeMaterialBlockTargets::Vertex) {
    return true;
  }

  if (!hasEndpoints()) {
    return false;
  }

  for (const auto& endpoint : _endpoints) {
    if (endpoint->ownerBlock()->target() == NodeMaterialBlockTargets::Vertex) {
      return true;
    }

    if (endpoint->target() == NodeMaterialBlockTargets::Vertex) {
      return true;
    }

    if (endpoint->ownerBlock()->target() == NodeMaterialBlockTargets::Neutral
        || endpoint->ownerBlock()->target() == NodeMaterialBlockTargets::VertexAndFragment) {
      for (const auto& o : endpoint->ownerBlock()->outputs()) {
        if (o->isConnectedInVertexShader()) {
          return true;
        }
      }
    }
  }

  return false;
}

bool NodeMaterialConnectionPoint::get_isConnectedInFragmentShader() const
{
  if (target() == NodeMaterialBlockTargets::Fragment) {
    return true;
  }

  if (!hasEndpoints()) {
    return false;
  }

  for (const auto& endpoint : _endpoints) {
    if (endpoint->ownerBlock()->target() == NodeMaterialBlockTargets::Fragment) {
      return true;
    }

    if (endpoint->ownerBlock()->target() == NodeMaterialBlockTargets::Neutral
        || endpoint->ownerBlock()->target() == NodeMaterialBlockTargets::VertexAndFragment) {
      for (const auto& o : endpoint->ownerBlock()->outputs()) {
        if (o->isConnectedInFragmentShader()) {
          return true;
        }
      }
    }
  }

  return false;
}

std::string NodeMaterialConnectionPoint::getClassName() const
{
  return "NodeMaterialConnectionPoint";
}

bool NodeMaterialConnectionPoint::canConnectTo(const NodeMaterialConnectionPoint& connectionPoint)
{
  return checkCompatibilityState(connectionPoint)
         == NodeMaterialConnectionPointCompatibilityStates::Compatible;
}

NodeMaterialConnectionPointCompatibilityStates NodeMaterialConnectionPoint::checkCompatibilityState(
  const NodeMaterialConnectionPoint& connectionPoint)
{
  const auto& iOwnerBlock = _ownerBlock;

  if (iOwnerBlock->target() == NodeMaterialBlockTargets::Fragment) {
    // Let's check we are not going reverse
    const auto& otherBlock = connectionPoint.ownerBlock();

    if (otherBlock->target() == NodeMaterialBlockTargets::Vertex) {
      return NodeMaterialConnectionPointCompatibilityStates::TargetIncompatible;
    }

    for (const auto& output : otherBlock->outputs()) {
      if (output->isConnectedInVertexShader()) {
        return NodeMaterialConnectionPointCompatibilityStates::TargetIncompatible;
      }
    }
  }

  if (type != connectionPoint.type
      && connectionPoint.innerType() != NodeMaterialBlockConnectionPointTypes::AutoDetect) {
    // Equivalents
    switch (type) {
      case NodeMaterialBlockConnectionPointTypes::Vector3: {
        if (connectionPoint.type() == NodeMaterialBlockConnectionPointTypes::Color3) {
          return NodeMaterialConnectionPointCompatibilityStates::Compatible;
        }
      } break;
      case NodeMaterialBlockConnectionPointTypes::Vector4: {
        if (connectionPoint.type() == NodeMaterialBlockConnectionPointTypes::Color4) {
          return NodeMaterialConnectionPointCompatibilityStates::Compatible;
        }
      } break;
      case NodeMaterialBlockConnectionPointTypes::Color3: {
        if (connectionPoint.type() == NodeMaterialBlockConnectionPointTypes::Vector3) {
          return NodeMaterialConnectionPointCompatibilityStates::Compatible;
        }
      } break;
      case NodeMaterialBlockConnectionPointTypes::Color4: {
        if (connectionPoint.type() == NodeMaterialBlockConnectionPointTypes::Vector4) {
          return NodeMaterialConnectionPointCompatibilityStates::Compatible;
        }
      } break;
      default:
        break;
    }

    // Accepted types
    if (!connectionPoint.acceptedConnectionPointTypes.empty()
        && stl_util::contains(connectionPoint.acceptedConnectionPointTypes, type)) {
      return NodeMaterialConnectionPointCompatibilityStates::Compatible;
    }
    else {
      return NodeMaterialConnectionPointCompatibilityStates::TypeIncompatible;
    }
  }

  // Excluded
  if ((!connectionPoint.excludedConnectionPointTypes.empty()
       && stl_util::contains(connectionPoint.excludedConnectionPointTypes, type))) {
    return NodeMaterialConnectionPointCompatibilityStates::TypeIncompatible;
  }

  return NodeMaterialConnectionPointCompatibilityStates::Compatible;
}

NodeMaterialConnectionPoint&
NodeMaterialConnectionPoint::connectTo(const NodeMaterialConnectionPointPtr& connectionPoint,
                                       bool ignoreConstraints)
{
  if (!ignoreConstraints && !canConnectTo(*connectionPoint)) {
    throw std::runtime_error("Cannot connect these two connectors.");
  }

  _endpoints.emplace_back(connectionPoint);
  connectionPoint->_connectedPoint = shared_from_this();

  _enforceAssociatedVariableName = false;

  onConnectionObservable.notifyObservers(connectionPoint.get());
  connectionPoint->onConnectionObservable.notifyObservers(this);

  return *this;
}

NodeMaterialConnectionPoint&
NodeMaterialConnectionPoint::disconnectFrom(const NodeMaterialConnectionPointPtr& endpoint)
{
  auto index = stl_util::index_of(_endpoints, endpoint);

  if (index == -1) {
    return *this;
  }

  stl_util::splice(_endpoints, index, 1);
  endpoint->_connectedPoint                = nullptr;
  _enforceAssociatedVariableName           = false;
  endpoint->_enforceAssociatedVariableName = false;
  return *this;
}

json NodeMaterialConnectionPoint::serialize() const
{
  return nullptr;
}

void NodeMaterialConnectionPoint::dispose()
{
  onConnectionObservable.clear();
}

} // end of namespace BABYLON
