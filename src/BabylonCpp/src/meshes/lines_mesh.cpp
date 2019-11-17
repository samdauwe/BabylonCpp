#include <babylon/meshes/lines_mesh.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/culling/ray.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/instanced_lines_mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/rendering/line_edges_renderer.h>

namespace BABYLON {

LinesMesh::LinesMesh(const std::string& iName, Scene* scene, Node* iParent,
                     LinesMesh* iSource, bool doNotCloneChildren,
                     bool iUseVertexColor, bool iUseVertexAlpha)
    : Mesh(iName, scene, iParent, iSource, doNotCloneChildren)
    , dashSize{0.f}
    , gapSize{0.f}
    , color{Color3(1.f, 1.f, 1.f)}
    , alpha{1.f}
    , _colorShaderMaterial{nullptr}
{
  if (_source) {
    color          = iSource->color;
    alpha          = iSource->alpha;
    useVertexColor = iSource->useVertexColor;
    useVertexAlpha = iSource->useVertexAlpha;
  }

  intersectionThreshold = 0.1f;

  std::vector<std::string> defines;
  IShaderMaterialOptions options;
  options.attributes = {VertexBuffer::PositionKind, VertexBuffer::World0Kind,
                        VertexBuffer::World1Kind, VertexBuffer::World2Kind,
                        VertexBuffer::World3Kind};
  options.uniforms   = {"vClipPlane",  "vClipPlane2", "vClipPlane3",
                      "vClipPlane4", "world",       "viewProjection"};
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
    options.attributes.emplace_back(VertexBuffer::ColorKind);
  }

  _colorShader
    = ShaderMaterial::New("colorShader", getScene(), "color", options);
}

LinesMesh::~LinesMesh() = default;

void LinesMesh::_addClipPlaneDefine(const std::string& label)
{
  const auto define = "#define " + label;
  const auto hasDefine
    = stl_util::contains(_colorShader->options().defines, define);

  if (hasDefine) {
    return;
  }

  _colorShader->options().defines.emplace_back(define);
}

void LinesMesh::_removeClipPlaneDefine(const std::string& label)
{
  const auto define = "#define " + label;
  const auto index  = std::find(_colorShader->options().defines.begin(),
                               _colorShader->options().defines.end(), define);

  if (index == _colorShader->options().defines.end()) {
    return;
  }

  _colorShader->options().defines.erase(index);
}

bool LinesMesh::isReady(bool /*completeCheck*/, bool /*forceInstanceSupport*/)
{
  const auto& scene = *getScene();

  // Clip planes
  scene.clipPlane ? _addClipPlaneDefine("CLIPPLANE") :
                    _removeClipPlaneDefine("CLIPPLANE");
  scene.clipPlane2 ? _addClipPlaneDefine("CLIPPLANE2") :
                     _removeClipPlaneDefine("CLIPPLANE2");
  scene.clipPlane3 ? _addClipPlaneDefine("CLIPPLANE3") :
                     _removeClipPlaneDefine("CLIPPLANE3");
  scene.clipPlane4 ? _addClipPlaneDefine("CLIPPLANE4") :
                     _removeClipPlaneDefine("CLIPPLANE4");

  if (!_colorShader->isReady()) {
    return false;
  }

  return Mesh::isReady();
}

std::string LinesMesh::getClassName() const
{
  return "LinesMesh";
}

Type LinesMesh::type() const
{
  return Type::LINESMESH;
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

void LinesMesh::_bind(SubMesh* /*subMesh*/, const EffectPtr& /*effect*/,
                      unsigned int /*fillMode*/)
{
  if (!_geometry) {
    return;
  }
  const auto& colorEffect = _colorShader->getEffect();

  // VBOs
  const auto indexToBind = isUnIndexed ? nullptr : _geometry->getIndexBuffer();
  _geometry->_bind(colorEffect, indexToBind);

  // Color
  if (!useVertexColor) {
    color4.set(color.r, color.g, color.b, alpha);
    _colorShader->setColor4("color", color4);
  }

  // Clip planes
  MaterialHelper::BindClipPlane(colorEffect, getScene());
}

void LinesMesh::_draw(SubMesh* subMesh, int /*fillMode*/, size_t instancesCount,
                      bool /*alternate*/)
{
  if (!_geometry || _geometry->getVertexBuffers().empty()
      || (!_unIndexed && !_geometry->getIndexBuffer())) {
    return;
  }

  auto& engine = *getScene()->getEngine();

  // Draw order
  if (_unIndexed) {
    engine.drawArraysType(Material::LineListDrawMode,
                          static_cast<int>(subMesh->verticesStart),
                          static_cast<int>(subMesh->verticesCount),
                          static_cast<int>(instancesCount));
  }
  else {
    engine.drawElementsType(
      Material::LineListDrawMode, static_cast<int>(subMesh->indexStart),
      static_cast<int>(subMesh->indexCount), static_cast<int>(instancesCount));
  }
}

PickingInfo
LinesMesh::intersects(Ray& /*ray*/, bool /*fastCheck*/,
                      const TrianglePickingPredicate& /*trianglePredicate*/)
{
  return PickingInfo();
}

void LinesMesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _colorShader->dispose(false, false, true);

  Mesh::dispose(doNotRecurse, disposeMaterialAndTextures);
}

LinesMeshPtr LinesMesh::clone(const std::string& iName, Node* newParent,
                              bool doNotCloneChildren)
{
  return LinesMesh::New(iName, getScene(), newParent, this, doNotCloneChildren);
}

InstancedLinesMeshPtr LinesMesh::createInstance(const std::string& iName)
{
  return InstancedLinesMesh::New(iName, shared_from_base<LinesMesh>());
}

AbstractMeshPtr
LinesMesh::enableEdgesRendering(const AbstractMeshPtr& iSource, float epsilon,
                                bool checkVerticesInsteadOfIndices)
{
  iSource->disableEdgesRendering();
  iSource->_edgesRenderer = std::make_shared<LineEdgesRenderer>(
    iSource, epsilon, checkVerticesInsteadOfIndices);
  return iSource;
}

} // end of namespace BABYLON
