#include <babylon/materials/node/node_material_connection_point.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

NodeMaterialConnectionPoint::NodeMaterialConnectionPoint(const std::string& iName,
                                                         const NodeMaterialBlockPtr& ownerBlock)
    : _ownerBlock{nullptr}
    , _connectedPoint{nullptr}
    , _typeConnectionSource{nullptr}
    , _linkedConnectionSource{nullptr}
    , _enforceAssociatedVariableName{false}
    , associatedVariableName{this, &NodeMaterialConnectionPoint::get_associatedVariableName,
                             &NodeMaterialConnectionPoint::set_associatedVariableName}
    , type{this, &NodeMaterialConnectionPoint::get_type, &NodeMaterialConnectionPoint::set_type}
    , isOptional{false}
    , target{NodeMaterialBlockTargets::VertexAndFragment}
    , isConnected{this, &NodeMaterialConnectionPoint::get_isConnected}
    , isConnectedToInputBlock{this, &NodeMaterialConnectionPoint::get_isConnectedToInputBlock}
    , connectInputBlock{this, &NodeMaterialConnectionPoint::get_connectInputBlock}
    , connectedPoint{this, &NodeMaterialConnectionPoint::get_connectedPoint}
    , ownerBlock{this, &NodeMaterialConnectionPoint::get_ownerBlock}
    , sourceBlock{this, &NodeMaterialConnectionPoint::get_sourceBlock}
    , connectedBlocks{this, &NodeMaterialConnectionPoint::get_connectedBlocks}
    , endpoints{this, &NodeMaterialConnectionPoint::get_endpoints}
    , hasEndpoints{this, &NodeMaterialConnectionPoint::get_hasEndpoints}
    , _type{NodeMaterialBlockConnectionPointTypes::Float}
    , _connectInputBlock{nullptr}
    , _sourceBlock{nullptr}
{
  _ownerBlock = ownerBlock;
  name        = iName;
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
  return _endpoints.size() > 0;
}

std::string NodeMaterialConnectionPoint::getClassName() const
{
  return "NodeMaterialConnectionPoint";
}

bool NodeMaterialConnectionPoint::canConnectTo(const NodeMaterialConnectionPoint& connectionPoint)
{
  if (type != connectionPoint.type
      && connectionPoint.type != NodeMaterialBlockConnectionPointTypes::AutoDetect) {
    // Equivalents
    switch (type) {
      case NodeMaterialBlockConnectionPointTypes::Vector3: {
        if (connectionPoint.type == NodeMaterialBlockConnectionPointTypes::Color3) {
          return true;
        }
      } break;
      case NodeMaterialBlockConnectionPointTypes::Vector4: {
        if (connectionPoint.type == NodeMaterialBlockConnectionPointTypes::Color4) {
          return true;
        }
      } break;
      case NodeMaterialBlockConnectionPointTypes::Color3: {
        if (connectionPoint.type == NodeMaterialBlockConnectionPointTypes::Vector3) {
          return true;
        }
      } break;
      case NodeMaterialBlockConnectionPointTypes::Color4: {
        if (connectionPoint.type == NodeMaterialBlockConnectionPointTypes::Vector4) {
          return true;
        }
      } break;
      default:
        break;
    }

    // Accepted types
    return (!connectionPoint.acceptedConnectionPointTypes.empty()
            && stl_util::contains(connectionPoint.acceptedConnectionPointTypes, type));
  }

  return true;
}

NodeMaterialConnectionPoint&
NodeMaterialConnectionPoint::connectTo(NodeMaterialConnectionPointPtr& connectionPoint,
                                       bool ignoreConstraints)
{
  if (!ignoreConstraints && !canConnectTo(*connectionPoint)) {
    throw std::runtime_error("Cannot connect two different connection types.");
  }

  _endpoints.emplace_back(connectionPoint);
  connectionPoint->_connectedPoint = shared_from_this();

  _enforceAssociatedVariableName = false;
  return *this;
}

NodeMaterialConnectionPoint&
NodeMaterialConnectionPoint::disconnectFrom(NodeMaterialConnectionPoint& endpoint)
{
  auto index = stl_util::index_of(_endpoints, endpoint);

  if (index == -1) {
    return *this;
  }

  stl_util::splice(_endpoints, index, 1);
  endpoint._connectedPoint                = nullptr;
  _enforceAssociatedVariableName          = false;
  endpoint._enforceAssociatedVariableName = false;
  return *this;
}

json NodeMaterialConnectionPoint::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
