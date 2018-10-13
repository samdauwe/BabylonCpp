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
#include <babylon/rendering/line_edges_renderer.h>

namespace BABYLON {

LinesMesh::LinesMesh(const std::string& iName, Scene* scene, Node* iParent,
                     LinesMesh* source, bool doNotCloneChildren,
                     bool iUseVertexColor, bool iUseVertexAlpha)
    : Mesh(iName, scene, iParent, source, doNotCloneChildren)
    , dashSize{0.f}
    , gapSize{0.f}
    , color{Color3(1.f, 1.f, 1.f)}
    , alpha{1.f}
    , intersectionThreshold{this, &LinesMesh::get_intersectionThreshold,
                            &LinesMesh::set_intersectionThreshold}
    , _colorShaderMaterial{nullptr}
{
  if (source) {
    color          = source->color;
    alpha          = source->alpha;
    useVertexColor = source->useVertexColor;
    useVertexAlpha = source->useVertexAlpha;
  }

  _intersectionThreshold = 0.1f;

  std::vector<std::string> defines;
  ShaderMaterialOptions options;
  options.attributes        = {VertexBuffer::PositionKindChars};
  options.uniforms          = {"world", "viewProjection"};
  options.needAlphaBlending = true;
  options.defines           = defines;

  if (!iUseVertexAlpha) {
    options.needAlphaBlending = false;
  }

  if (!iUseVertexColor) {
    options.uniforms.emplace_back("color");
  }
  else {
    options.defines.emplace_back("#define VERTEXCOLOR");
    options.attributes.emplace_back(VertexBuffer::ColorKindChars);
  }

  _colorShader
    = ShaderMaterial::New("colorShader", getScene(), "color", options);
}

LinesMesh::~LinesMesh()
{
}

const std::string LinesMesh::getClassName() const
{
  return "LinesMesh";
}

IReflect::Type LinesMesh::type() const
{
  return IReflect::Type::LINESMESH;
}

float LinesMesh::get_intersectionThreshold() const
{
  return _intersectionThreshold;
}

void LinesMesh::set_intersectionThreshold(float value)
{
  if (stl_util::almost_equal(_intersectionThreshold, value)) {
    return;
  }

  _intersectionThreshold = value;
  if (geometry()) {
    geometry()->boundingBias = Vector2(0.f, value);
  }
}

MaterialPtr& LinesMesh::get_material()
{
  _colorShaderMaterial = std::static_pointer_cast<Material>(_colorShader);
  return _colorShaderMaterial;
}

void LinesMesh::set_material(const MaterialPtr& /*material*/)
{
  // Do nothing
}

bool LinesMesh::get_checkCollisions() const
{
  return false;
}

InstancedMesh* LinesMesh::createInstance(const std::string& /*name*/)
{
  BABYLON_LOG_ERROR("InstancedMesh",
                    "LinesMeshes do not support createInstance.");
  return nullptr;
}

void LinesMesh::_bind(SubMesh* /*subMesh*/, Effect* /*effect*/,
                      unsigned int /*fillMode*/)
{
  if (!_geometry) {
    return;
  }
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
      || (!_unIndexed && !_geometry->getIndexBuffer())) {
    return;
  }

  auto engine = getScene()->getEngine();

  // Draw order
  engine->drawElementsType(Material::LineListDrawMode(),
                           static_cast<int>(subMesh->indexStart),
                           static_cast<int>(subMesh->indexCount));
}

PickingInfo LinesMesh::intersects(Ray& /*ray*/, bool /*fastCheck*/)
{
  return PickingInfo();
}

void LinesMesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _colorShader->dispose();

  Mesh::dispose(doNotRecurse, disposeMaterialAndTextures);
}

LinesMeshPtr LinesMesh::clone(const std::string& iName, Node* newParent,
                              bool doNotCloneChildren)
{
  return LinesMesh::New(iName, getScene(), newParent, this, doNotCloneChildren);
}

AbstractMesh&
LinesMesh::enableEdgesRendering(float epsilon,
                                bool checkVerticesInsteadOfIndices)
{
  disableEdgesRendering();
  _edgesRenderer = std::make_unique<LineEdgesRenderer>(
    shared_from_base<LinesMesh>(), epsilon, checkVerticesInsteadOfIndices);
  return *this;
}

} // end of namespace BABYLON
