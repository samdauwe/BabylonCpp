#include <babylon/tools/packed_rect.h>

#include <babylon/core/logging.h>
#include <babylon/tools/rect_packing_map.h>

namespace BABYLON {

Size PackedRect::TpsSize{Size::Zero()};

PackedRect::PackedRect(PackedRect* root, PackedRect* parent, const Vector2& pos,
                       const Size& size)
    : _root{root}
    , _parent{parent}
    , _size{size}
    , _contentSize{nullptr}
    , _initialSize{nullptr}
    , _leftNode{nullptr}
    , _rightNode{nullptr}
    , _bottomNode{nullptr}
    , _pos{pos}
{
}

PackedRect::~PackedRect()
{
}

const Vector2& PackedRect::pos() const
{
  return _pos;
}

Size* PackedRect::contentSize() const
{
  return _contentSize.get();
}

void PackedRect::getInnerPosToRef(Vector2& res)
{
  const auto root = static_cast<RectPackingMap*>(_root);
  const auto& m   = root->_margin;
  res.x           = _pos.x + m;
  res.y           = _pos.y + m;
}

void PackedRect::getInnerSizeToRef(Size& res)
{
  const auto root = static_cast<RectPackingMap*>(_root);
  const auto& m   = root->_margin;
  res.width       = static_cast<int>(_contentSize->width - (m * 2.f));
  res.height      = static_cast<int>(_contentSize->height - (m * 2.f));
}

std::vector<Vector2> PackedRect::UVs()
{
  if (!_contentSize) {
    BABYLON_LOG_ERROR(
      "PackedRect",
      "Can't compute UVs for this object because it's nor allocated");
    return std::vector<Vector2>();
  }

  return getUVsForCustomSize(*_contentSize);
}

std::vector<Vector2> PackedRect::getUVsForCustomSize(const Size& customSize)
{
  const float mainWidth  = static_cast<float>(_root->_size.width);
  const float mainHeight = static_cast<float>(_root->_size.height);
  const auto root        = static_cast<RectPackingMap*>(_root);
  const float margin     = root->_margin;

  Vector2 topLeft((_pos.x + margin) / mainWidth,
                  (_pos.y + margin) / mainHeight);
  Vector2 rightBottom(
    (_pos.x + static_cast<float>(customSize.width) + margin - 1.f) / mainWidth,
    (_pos.y + static_cast<float>(customSize.height) + margin - 1.f)
      / mainHeight);
  std::vector<Vector2> uvs;
  uvs.emplace_back(topLeft);
  uvs.emplace_back(Vector2(rightBottom.x, topLeft.y));
  uvs.emplace_back(rightBottom);
  uvs.emplace_back(Vector2(topLeft.x, rightBottom.y));

  return uvs;
}

void PackedRect::freeContent()
{
  if (!contentSize()) {
    return;
  }

  _contentSize.reset(nullptr);

  // If everything below is also free, reset the whole node, and attempt to
  // reset parents if they also become free
  attemptDefrag();
}

bool PackedRect::isUsed() const
{
  return _contentSize != nullptr || _leftNode != nullptr;
}

PackedRect* PackedRect::findAndSplitNode(const Size& contentSize)
{
  auto node = findNode(contentSize);

  // Not enough space...
  if (!node) {
    return nullptr;
  }

  node->splitNode(contentSize);
  return node;
}

PackedRect* PackedRect::findNode(const Size& size)
{
  PackedRect* resNode = nullptr;
  const auto root     = static_cast<RectPackingMap*>(_root);
  float margin        = root->_margin * 2.f;

  // If this node is used, recurse to each of his subNodes to find an available
  // one in its branch
  if (isUsed()) {
    if (_leftNode) {
      resNode = _leftNode->findNode(size);
    }
    if (!resNode && _rightNode) {
      resNode = _rightNode->findNode(size);
    }
    if (!resNode && _bottomNode) {
      resNode = _bottomNode->findNode(size);
    }
  }

  // The node is free, but was previously allocated (_initialSize is set), rely
  // on initialSize to make the test as it's the space we have
  else if (_initialSize) {
    if (((size.width + margin) <= _initialSize->width)
        && ((size.height + margin) <= _initialSize->height)) {
      resNode = this;
    }
    else {
      return nullptr;
    }
  }

  // The node is free and empty, rely on its size for the test
  else if (((size.width + margin) <= _size.width)
           && ((size.height + margin) <= _size.height)) {
    resNode = this;
  }
  return resNode;
}

PackedRect* PackedRect::splitNode(const Size& contentSize)
{
  auto& cs        = PackedRect::TpsSize;
  const auto root = static_cast<RectPackingMap*>(_root);
  float margin    = root->_margin * 2.f;
  cs.copyFrom(contentSize);
  cs.width += margin;
  cs.height += margin;

  // If there's no contentSize but an initialSize it means this node were
  // previously allocated, but freed, we need to create a _leftNode as subNode
  // and use to allocate the space we need (and this node will have a
  // right/bottom subNode for the space left as _initialSize may be greater than
  // contentSize)
  if (!_contentSize && _initialSize) {
    _contentSize = cs.clone();
    _leftNode    = std::make_unique<PackedRect>(
      _root, this, Vector2(_pos.x, _pos.y),
      Size(_initialSize->width, _initialSize->height));
    return _leftNode->splitNode(contentSize);
  }
  else {
    _contentSize = cs.clone();
    _initialSize = cs.clone();

    if (cs.width != _size.width) {
      _rightNode = std::make_unique<PackedRect>(
        _root, this, Vector2(_pos.x + cs.width, _pos.y),
        Size(_size.width - cs.width, cs.height));
    }

    if (cs.height != _size.height) {
      _bottomNode = std::make_unique<PackedRect>(
        _root, this, Vector2(_pos.x, _pos.y + cs.height),
        Size(_size.width, _size.height - cs.height));
    }
    return this;
  }
}

void PackedRect::attemptDefrag()
{
  if (!isUsed() && isRecursiveFree()) {
    clearNode();

    if (_parent) {
      _parent->attemptDefrag();
    }
  }
}

void PackedRect::clearNode()
{
  _initialSize.reset(nullptr);
  _rightNode.reset(nullptr);
  _bottomNode.reset(nullptr);
}

bool PackedRect::isRecursiveFree() const
{
  return !contentSize() && (!_leftNode || _leftNode->isRecursiveFree())
         && (!_rightNode || _rightNode->isRecursiveFree())
         && (!_bottomNode || _bottomNode->isRecursiveFree());
}

size_t PackedRect::evalFreeSize(const size_t& size)
{
  size_t levelSize = 0;

  if (!isUsed()) {
    const auto root = static_cast<RectPackingMap*>(_root);
    float margin    = root->_margin;

    if (_initialSize) {
      levelSize = static_cast<size_t>(_initialSize->surface()
                                      - (_initialSize->width * margin)
                                      - (_initialSize->height * margin));
    }
    else {
      levelSize = static_cast<size_t>(_size.surface() - (_size.width * margin)
                                      - (_size.height * margin));
    }
  }

  if (_rightNode) {
    levelSize += _rightNode->evalFreeSize(0);
  }

  if (_bottomNode) {
    levelSize += _bottomNode->evalFreeSize(0);
  }

  return levelSize + size;
}

} // end of namespace BABYLON
