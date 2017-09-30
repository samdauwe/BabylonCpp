#include <babylon/mesh/lines_mesh.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/shader_material.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

LinesMesh::LinesMesh(const string_t& iName, Scene* scene, Node* iParent,
                     LinesMesh* source, bool doNotCloneChildren,
                     bool useVertexColor)
    : Mesh(iName, scene, iParent, source, doNotCloneChildren)
    , dashSize{0.f}
    , gapSize{0.f}
    , color{Color3(1.f, 1.f, 1.f)}
    , alpha{1.f}
    , _intersectionThreshold{0.1f}
{
  if (source) {
    color          = source->color;
    alpha          = source->alpha;
    useVertexColor = source->useVertexColor;
  }

  ShaderMaterialOptions options;
  options.attributes        = {VertexBuffer::PositionKindChars};
  options.uniforms          = {"world", "viewProjection"};
  options.needAlphaBlending = false;

  if (!useVertexColor) {
    options.uniforms.emplace_back("color");
    options.needAlphaBlending = true;
  }

  _colorShader = ShaderMaterial::New("colorShader", scene, "color", options);
}

LinesMesh::~LinesMesh()
{
}

const char* LinesMesh::getClassName() const
{
  return "LinesMesh";
}

IReflect::Type LinesMesh::type() const
{
  return IReflect::Type::LINESMESH;
}

float LinesMesh::intersectionThreshold() const
{
  return _intersectionThreshold;
}

void LinesMesh::setIntersectionThreshold(float value)
{
  if (stl_util::almost_equal(_intersectionThreshold, value)) {
    return;
  }

  _intersectionThreshold = value;
  if (geometry()) {
    geometry()->setBoundingBias(Vector2(0.f, value));
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

InstancedMesh* LinesMesh::createInstance(const string_t& /*name*/)
{
  BABYLON_LOG_WARN("InstancedMesh",
                   "LinesMeshes do not support createInstance.");
  return nullptr;
}

void LinesMesh::_bind(SubMesh* /*subMesh*/, Effect* /*effect*/,
                      unsigned int /*fillMode*/)
{
  // VBOs
  _geometry->_bind(_colorShader->getEffect());

  // Color
  if (!useVertexColor) {
    _colorShader->setColor4("color", color.toColor4(alpha));
  }
}

void LinesMesh::_draw(SubMesh* subMesh, int /*fillMode*/,
                      size_t /*instancesCount*/, bool /*alternate*/)
{
  if (!_geometry || _geometry->getVertexBuffers().empty()
      || !_geometry->getIndexBuffer()) {
    return;
  }

  auto engine = getScene()->getEngine();

  // Draw order
  engine->draw(false, subMesh->indexStart,
               static_cast<int>(subMesh->indexCount));
}

PickingInfo LinesMesh::intersects(const Ray& /*ray*/, bool /*fastCheck*/)
{
  return PickingInfo();
}

void LinesMesh::dispose(bool /*doNotRecurse*/)
{
  _colorShader->dispose();

  Mesh::dispose();
}

LinesMesh* LinesMesh::clone(const string_t& iName, Node* newParent,
                            bool doNotCloneChildren)
{
  return LinesMesh::New(iName, getScene(), newParent, this, doNotCloneChildren);
}

} // end of namespace BABYLON
