#include <babylon/rendering/bounding_box_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/shader_material_options.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {

BoundingBoxRenderer::BoundingBoxRenderer(Scene* scene)
    : frontColor{Color3(1.f, 1.f, 1.f)}
    , backColor{Color3(0.1f, 0.1f, 0.1f)}
    , showBackLines{true}
    , _scene{scene}
    , _colorShader{nullptr}
    , _indexBuffer{nullptr}
{
  renderList.reserve(32);
}

BoundingBoxRenderer::~BoundingBoxRenderer()
{
}

void BoundingBoxRenderer::_prepareResources()
{
  if (_colorShader) {
    return;
  }

  ShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes = {VertexBuffer::PositionKindChars};
  shaderMaterialOptions.uniforms   = {"world", "viewProjection", "color"};

  _colorShader = ShaderMaterial::New("colorShader", _scene, "color",
                                     shaderMaterialOptions);

  auto engine = _scene->getEngine();
  BoxOptions options(1.f);
  auto boxdata = VertexData::CreateBox(options);
  _vertexBuffers.resize(VertexBuffer::PositionKind + 1);
  _vertexBuffers[VertexBuffer::PositionKind] = ::std::make_unique<VertexBuffer>(
    engine, ToVariant<Float32Array, Buffer*>(boxdata->positions),
    VertexBuffer::PositionKind, false);
  _vertexBuffersMap[VertexBuffer::PositionKindChars]
    = _vertexBuffers[VertexBuffer::PositionKind].get();
  _createIndexBuffer();
}

void BoundingBoxRenderer::_createIndexBuffer()
{
  auto engine = _scene->getEngine();
  const Uint32Array indices{0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                            6, 7, 7, 4, 0, 7, 1, 6, 2, 5, 3, 4};
  _indexBuffer = engine->createIndexBuffer(indices);
}

void BoundingBoxRenderer::_rebuild()
{
  if (VertexBuffer::PositionKind < _vertexBuffers.size()) {
    auto& vb = _vertexBuffers[VertexBuffer::PositionKind];
    if (vb) {
      vb->_rebuild();
    }
  }
  _createIndexBuffer();
}

void BoundingBoxRenderer::reset()
{
  renderList.clear();
}

void BoundingBoxRenderer::render()
{
  if (renderList.empty()) {
    return;
  }

  _prepareResources();

  if (!_colorShader->isReady()) {
    return;
  }

  auto engine = _scene->getEngine();
  engine->setDepthWrite(false);
  _colorShader->_preBind();
  for (auto& boundingBox : renderList) {
    const auto& min = boundingBox.minimum;
    const auto& max = boundingBox.maximum;
    auto diff       = max.subtract(min);
    auto median     = min.add(diff.scale(0.5f));

    auto worldMatrix
      = Matrix::Scaling(diff.x, diff.y, diff.z)
          .multiply(Matrix::Translation(median.x, median.y, median.z))
          .multiply(boundingBox.getWorldMatrix());

    // VBOs
    engine->bindBuffers(_vertexBuffersMap, _indexBuffer.get(),
                        _colorShader->getEffect());

    if (showBackLines) {
      // Back
      engine->setDepthFunctionToGreaterOrEqual();
      _scene->resetCachedMaterial();
      _colorShader->setColor4("color", backColor.toColor4());
      _colorShader->bind(&worldMatrix, nullptr);

      // Draw order
      engine->drawElementsType(Material::LineListDrawMode(), 0, 24);
    }

    // Front
    engine->setDepthFunctionToLess();
    _scene->resetCachedMaterial();
    _colorShader->setColor4("color", frontColor.toColor4());
    _colorShader->bind(&worldMatrix);

    // Draw order
    engine->drawElementsType(Material::LineListDrawMode(), 0, 24);
  }
  _colorShader->unbind();
  engine->setDepthFunctionToLessOrEqual();
  engine->setDepthWrite(true);
}

void BoundingBoxRenderer::renderOcclusionBoundingBox(AbstractMesh* mesh)
{
  _prepareResources();

  if (!_colorShader->isReady() || !mesh->_boundingInfo) {
    return;
  }

  auto engine = _scene->getEngine();
  engine->setDepthWrite(false);
  engine->setColorWrite(false);
  _colorShader->_preBind();

  auto& boundingBox = mesh->_boundingInfo->boundingBox;
  auto min          = boundingBox.minimum;
  auto max          = boundingBox.maximum;
  auto diff         = max.subtract(min);
  auto median       = min.add(diff.scale(0.5f));

  auto worldMatrix
    = Matrix::Scaling(diff.x, diff.y, diff.z)
        .multiply(Matrix::Translation(median.x, median.y, median.z))
        .multiply(boundingBox.getWorldMatrix());

  engine->bindBuffers(_vertexBuffersMap, _indexBuffer.get(),
                      _colorShader->getEffect());

  engine->setDepthFunctionToLess();
  _scene->resetCachedMaterial();
  _colorShader->bind(&worldMatrix);

  engine->drawElementsType(Material::LineListDrawMode(), 0, 24);

  _colorShader->unbind();
  engine->setDepthFunctionToLessOrEqual();
  engine->setDepthWrite(true);
  engine->setColorWrite(true);
}

void BoundingBoxRenderer::dispose()
{
  if (!_colorShader) {
    return;
  }

  renderList.clear();

  _colorShader->dispose();

  if (stl_util::contains(_vertexBuffersMap, VertexBuffer::PositionKindChars)) {
    auto& buffer = _vertexBuffers[VertexBuffer::PositionKind];
    if (buffer) {
      buffer->dispose();
      buffer.reset(nullptr);
      _vertexBuffersMap.erase(VertexBuffer::PositionKindChars);
    }
  }

  _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
}

} // end of namespace BABYLON
