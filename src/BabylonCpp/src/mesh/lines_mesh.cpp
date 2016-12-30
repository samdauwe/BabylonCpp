#include <babylon/mesh/lines_mesh.h>

#include <babylon/core/logging.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/shader_material.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

LinesMesh::LinesMesh(const std::string& iName, Scene* scene, Node* iParent,
                     LinesMesh* source, bool doNotCloneChildren)
    : Mesh(iName, scene, iParent, source, doNotCloneChildren)
    , dashSize{0.f}
    , gapSize{0.f}
    , color{Color3(1, 1, 1)}
    , alpha{1.f}
    , _intersectionThreshold{0.1f}
{
  if (source) {
    color = source->color;
    alpha = source->alpha;
  }

  ShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes        = {VertexBuffer::PositionKindChars};
  shaderMaterialOptions.uniforms          = {"worldViewProjection", "color"};
  shaderMaterialOptions.needAlphaBlending = true;

  _colorShader
    = ShaderMaterial::New("colorShader", scene, "color", shaderMaterialOptions);
}

LinesMesh::~LinesMesh()
{
}

Node::Type LinesMesh::type() const
{
  return Node::Type::LINESMESH;
}

float LinesMesh::intersectionThreshold() const
{
  return _intersectionThreshold;
}

void LinesMesh::setIntersectionThreshold(float value)
{
  if (std_util::almost_equal(_intersectionThreshold, value)) {
    return;
  }

  _intersectionThreshold = value;
  if (geometry()) {
    geometry()->setBoundingBias(Vector2(0, value));
  }
}

Material* LinesMesh::getMaterial()
{
  return _colorShader;
}

bool LinesMesh::checkCollisions()
{
  return false;
}

InstancedMesh* LinesMesh::createInstance(const std::string& /*name*/)
{
  BABYLON_LOG_WARN("InstancedMesh",
                   "LinesMeshes do not support createInstance.");
  return nullptr;
}

void LinesMesh::_bind(SubMesh* /*subMesh*/, Effect* /*effect*/,
                      unsigned int /*fillMode*/)
{
  auto engine = getScene()->getEngine();

  _positionBuffer[VertexBuffer::PositionKindChars]
    = _geometry->getVertexBuffer(VertexBuffer::PositionKind);

  // VBOs
  engine->bindBuffers(_positionBuffer, _geometry->getIndexBuffer(),
                      _colorShader->getEffect());

  // Color
  _colorShader->setColor4("color", color.toColor4(alpha));
}

void LinesMesh::_draw(SubMesh* subMesh, int /*fillMode*/,
                      size_t /*instancesCount*/)
{
  if (!_geometry || _geometry->getVertexBuffers().empty()
      || !_geometry->getIndexBuffer()) {
    return;
  }

  auto engine = getScene()->getEngine();

  // Draw order
  engine->draw(false, subMesh->indexStart, subMesh->indexCount);
}

PickingInfo* LinesMesh::intersects(const Ray& /*ray*/, bool /*fastCheck*/)
{
  return nullptr;
}

void LinesMesh::dispose(bool /*doNotRecurse*/)
{
  _colorShader->dispose();

  Mesh::dispose();
}

LinesMesh* LinesMesh::clone(const std::string& iName, Node* newParent,
                            bool doNotCloneChildren)
{
  return new LinesMesh(iName, getScene(), newParent, this, doNotCloneChildren);
}

} // end of namespace BABYLON
