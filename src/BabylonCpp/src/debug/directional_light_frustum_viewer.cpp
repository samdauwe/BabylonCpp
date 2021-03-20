#include <babylon/debug/directional_light_frustum_viewer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/lights/directional_light.h>
#include <babylon/materials/material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/builders/lines_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

DirectionalLightFrustumViewer::DirectionalLightFrustumViewer(const DirectionalLightPtr& light,
                                                             const CameraPtr& camera)
    : transparency{this, &DirectionalLightFrustumViewer::get_transparency,
                   &DirectionalLightFrustumViewer::set_transparency}
    , showLines{this, &DirectionalLightFrustumViewer::get_showLines,
                &DirectionalLightFrustumViewer::set_showLines}
    , showPlanes{this, &DirectionalLightFrustumViewer::get_showPlanes,
                 &DirectionalLightFrustumViewer::set_showPlanes}
    , _rootNode{nullptr}
    , _oldPosition{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(),
                   std::numeric_limits<float>::quiet_NaN()}
    , _oldDirection{std::numeric_limits<float>::quiet_NaN(),
                    std::numeric_limits<float>::quiet_NaN(),
                    std::numeric_limits<float>::quiet_NaN()}
    , _transparency{0.3f}
    , _showLines{true}
    , _showPlanes{true}
{
  _scene                    = light->getScene();
  _light                    = light;
  _camera                   = camera;
  _inverseViewMatrix        = Matrix::Identity();
  _lightHelperFrustumMeshes = {};
  _createGeometry();
  show();
  update();
}

DirectionalLightFrustumViewer::~DirectionalLightFrustumViewer() = default;

float DirectionalLightFrustumViewer::get_transparency() const
{
  return _transparency;
}

void DirectionalLightFrustumViewer::set_transparency(float alpha)
{
  _transparency = alpha;
  for (size_t i = 6; i < 12; ++i) {
    if (_lightHelperFrustumMeshes[i]->material()) {
      _lightHelperFrustumMeshes[i]->material()->alpha = alpha;
    }
  }
}

bool DirectionalLightFrustumViewer::get_showLines() const
{
  return _showLines;
}

void DirectionalLightFrustumViewer::set_showLines(bool show)
{
  if (_showLines == show) {
    return;
  }
  _showLines = show;
  for (size_t i = 0; i < 6; ++i) {
    _lightHelperFrustumMeshes[i]->setEnabled(show);
  }
}

bool DirectionalLightFrustumViewer::get_showPlanes() const
{
  return _showPlanes;
}

void DirectionalLightFrustumViewer::set_showPlanes(bool show)
{
  if (_showPlanes == show) {
    return;
  }
  _showPlanes = show;
  for (size_t i = 6; i < 12; ++i) {
    _lightHelperFrustumMeshes[i]->setEnabled(show);
  }
}

void DirectionalLightFrustumViewer::show()
{
  for (size_t index = 0; index < _lightHelperFrustumMeshes.size(); ++index) {
    const auto& mesh = _lightHelperFrustumMeshes[index];
    mesh->setEnabled((index < 6 && _showLines) || (index >= 6 && _showPlanes));
  }
  _oldPosition.set(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(),
                   std::numeric_limits<float>::quiet_NaN());
  _visible = true;
}

void DirectionalLightFrustumViewer::hide()
{
  for (const auto& mesh : _lightHelperFrustumMeshes) {
    mesh->setEnabled(false);
  }
  _visible = false;
}

void DirectionalLightFrustumViewer::update()
{
  if (!_visible) {
    return;
  }

  if (_oldPosition.equals(_light->position())           //
      && _oldDirection.equals(_light->direction())      //
      && _oldAutoCalc == _light->autoCalcShadowZBounds  //
      && _oldMinZ == _light->shadowMinZ().value_or(0.f) //
      && _oldMaxZ == _light->shadowMaxZ().value_or(0.f) //
  ) {
    return;
  }

  _oldPosition.copyFrom(_light->position());
  _oldDirection.copyFrom(_light->direction());
  _oldAutoCalc = _light->autoCalcShadowZBounds;
  _oldMinZ     = _light->shadowMinZ().value_or(0.f);
  _oldMaxZ     = _light->shadowMaxZ().value_or(0.f);

  TmpVectors::Vector3Array[0].set(
    _light->orthoLeft(), _light->orthoBottom(),
    _light->shadowMinZ().has_value() ? *_light->shadowMinZ() : _camera->minZ); // min light extents
  TmpVectors::Vector3Array[1].set(
    _light->orthoRight(), _light->orthoTop(),
    _light->shadowMaxZ().has_value() ? *_light->shadowMaxZ() : _camera->maxZ); // max light extents

  const auto invLightView = _getInvertViewMatrix();

  TmpVectors::Vector3Array[2].copyFromFloats(TmpVectors::Vector3Array[1].x,
                                             TmpVectors::Vector3Array[1].y,
                                             TmpVectors::Vector3Array[0].z); // n1
  TmpVectors::Vector3Array[3].copyFromFloats(TmpVectors::Vector3Array[1].x,
                                             TmpVectors::Vector3Array[0].y,
                                             TmpVectors::Vector3Array[0].z); // n2
  TmpVectors::Vector3Array[4].copyFromFloats(TmpVectors::Vector3Array[0].x,
                                             TmpVectors::Vector3Array[0].y,
                                             TmpVectors::Vector3Array[0].z); // n3
  TmpVectors::Vector3Array[5].copyFromFloats(TmpVectors::Vector3Array[0].x,
                                             TmpVectors::Vector3Array[1].y,
                                             TmpVectors::Vector3Array[0].z); // n4

  Vector3::TransformCoordinatesToRef(TmpVectors::Vector3Array[2], invLightView,
                                     TmpVectors::Vector3Array[2]); // near1
  Vector3::TransformCoordinatesToRef(TmpVectors::Vector3Array[3], invLightView,
                                     TmpVectors::Vector3Array[3]); // near2
  Vector3::TransformCoordinatesToRef(TmpVectors::Vector3Array[4], invLightView,
                                     TmpVectors::Vector3Array[4]); // near3
  Vector3::TransformCoordinatesToRef(TmpVectors::Vector3Array[5], invLightView,
                                     TmpVectors::Vector3Array[5]); // near4

  TmpVectors::Vector3Array[6].copyFromFloats(TmpVectors::Vector3Array[1].x,
                                             TmpVectors::Vector3Array[1].y,
                                             TmpVectors::Vector3Array[1].z); // f1
  TmpVectors::Vector3Array[7].copyFromFloats(TmpVectors::Vector3Array[1].x,
                                             TmpVectors::Vector3Array[0].y,
                                             TmpVectors::Vector3Array[1].z); // f2
  TmpVectors::Vector3Array[8].copyFromFloats(TmpVectors::Vector3Array[0].x,
                                             TmpVectors::Vector3Array[0].y,
                                             TmpVectors::Vector3Array[1].z); // f3
  TmpVectors::Vector3Array[9].copyFromFloats(TmpVectors::Vector3Array[0].x,
                                             TmpVectors::Vector3Array[1].y,
                                             TmpVectors::Vector3Array[1].z); // f4

  Vector3::TransformCoordinatesToRef(TmpVectors::Vector3Array[6], invLightView,
                                     TmpVectors::Vector3Array[6]); // far1
  Vector3::TransformCoordinatesToRef(TmpVectors::Vector3Array[7], invLightView,
                                     TmpVectors::Vector3Array[7]); // far2
  Vector3::TransformCoordinatesToRef(TmpVectors::Vector3Array[8], invLightView,
                                     TmpVectors::Vector3Array[8]); // far3
  Vector3::TransformCoordinatesToRef(TmpVectors::Vector3Array[9], invLightView,
                                     TmpVectors::Vector3Array[9]); // far4

  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _nearLinesPoints;
    options.instance  = std::static_pointer_cast<LinesMesh>(_lightHelperFrustumMeshes[0]);
    LinesBuilder::CreateLines("nearlines", options, _scene);
  }

  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _farLinesPoints;
    options.instance  = std::static_pointer_cast<LinesMesh>(_lightHelperFrustumMeshes[1]);
    LinesBuilder::CreateLines("farlines", options, _scene);
  }

  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _trLinesPoints;
    options.instance  = std::static_pointer_cast<LinesMesh>(_lightHelperFrustumMeshes[2]);
    LinesBuilder::CreateLines("trlines", options, _scene);
  }

  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _brLinesPoints;
    options.instance  = std::static_pointer_cast<LinesMesh>(_lightHelperFrustumMeshes[3]);
    LinesBuilder::CreateLines("brlines", options, _scene);
  }

  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _tlLinesPoints;
    options.instance  = std::static_pointer_cast<LinesMesh>(_lightHelperFrustumMeshes[4]);
    LinesBuilder::CreateLines("tllines", options, _scene);
  }

  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _blLinesPoints;
    options.instance  = std::static_pointer_cast<LinesMesh>(_lightHelperFrustumMeshes[5]);
    LinesBuilder::CreateLines("bllines", options, _scene);
  }

  TmpVectors::Vector3Array[2].toArray(_nearPlaneVertices, 0);
  TmpVectors::Vector3Array[3].toArray(_nearPlaneVertices, 3);
  TmpVectors::Vector3Array[4].toArray(_nearPlaneVertices, 6);
  TmpVectors::Vector3Array[5].toArray(_nearPlaneVertices, 9);
  if (_lightHelperFrustumMeshes[6]->geometry()) {
    _lightHelperFrustumMeshes[6]->geometry()->updateVerticesDataDirectly("position",
                                                                         _nearPlaneVertices, 0);
  }

  TmpVectors::Vector3Array[6].toArray(_farPlaneVertices, 0);
  TmpVectors::Vector3Array[7].toArray(_farPlaneVertices, 3);
  TmpVectors::Vector3Array[8].toArray(_farPlaneVertices, 6);
  TmpVectors::Vector3Array[9].toArray(_farPlaneVertices, 9);
  if (_lightHelperFrustumMeshes[7]->geometry()) {
    _lightHelperFrustumMeshes[7]->geometry()->updateVerticesDataDirectly("position",
                                                                         _farPlaneVertices, 0);
  }

  TmpVectors::Vector3Array[2].toArray(_rightPlaneVertices, 0);
  TmpVectors::Vector3Array[6].toArray(_rightPlaneVertices, 3);
  TmpVectors::Vector3Array[7].toArray(_rightPlaneVertices, 6);
  TmpVectors::Vector3Array[3].toArray(_rightPlaneVertices, 9);
  if (_lightHelperFrustumMeshes[8]->geometry()) {
    _lightHelperFrustumMeshes[8]->geometry()->updateVerticesDataDirectly("position",
                                                                         _rightPlaneVertices, 0);
  }

  TmpVectors::Vector3Array[5].toArray(_leftPlaneVertices, 0);
  TmpVectors::Vector3Array[9].toArray(_leftPlaneVertices, 3);
  TmpVectors::Vector3Array[8].toArray(_leftPlaneVertices, 6);
  TmpVectors::Vector3Array[4].toArray(_leftPlaneVertices, 9);
  if (_lightHelperFrustumMeshes[9]->geometry()) {
    _lightHelperFrustumMeshes[9]->geometry()->updateVerticesDataDirectly("position",
                                                                         _leftPlaneVertices, 0);
  }

  TmpVectors::Vector3Array[2].toArray(_topPlaneVertices, 0);
  TmpVectors::Vector3Array[6].toArray(_topPlaneVertices, 3);
  TmpVectors::Vector3Array[9].toArray(_topPlaneVertices, 6);
  TmpVectors::Vector3Array[5].toArray(_topPlaneVertices, 9);
  if (_lightHelperFrustumMeshes[10]->geometry()) {
    _lightHelperFrustumMeshes[10]->geometry()->updateVerticesDataDirectly("position",
                                                                          _topPlaneVertices, 0);
  }

  TmpVectors::Vector3Array[3].toArray(_bottomPlaneVertices, 0);
  TmpVectors::Vector3Array[7].toArray(_bottomPlaneVertices, 3);
  TmpVectors::Vector3Array[8].toArray(_bottomPlaneVertices, 6);
  TmpVectors::Vector3Array[4].toArray(_bottomPlaneVertices, 9);
  if (_lightHelperFrustumMeshes[11]->geometry()) {
    _lightHelperFrustumMeshes[11]->geometry()->updateVerticesDataDirectly("position",
                                                                          _bottomPlaneVertices, 0);
  }
}

void DirectionalLightFrustumViewer::dispose()
{
  for (const auto& mesh : _lightHelperFrustumMeshes) {
    if (mesh->material()) {
      mesh->material()->dispose();
    }
    mesh->dispose();
  }
  _lightHelperFrustumMeshes = {};
  _rootNode->dispose();
}

void DirectionalLightFrustumViewer::_createGeometry()
{
  _rootNode         = TransformNode::New("directionalLightHelperRoot_" + _light->name, _scene);
  _rootNode->parent = _light->parent();

  _nearLinesPoints = {Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly(),
                      Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly()};
  LinesMeshPtr nearLines = nullptr;
  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _nearLinesPoints;
    nearLines         = LinesBuilder::CreateLines("nearlines", options, _scene);
  }
  nearLines->parent                   = _rootNode.get();
  nearLines->alwaysSelectAsActiveMesh = true;

  _farLinesPoints = {Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly(),
                     Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly()};
  LinesMeshPtr farLines = nullptr;
  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _farLinesPoints;
    farLines          = LinesBuilder::CreateLines("farlines", options, _scene);
  }
  farLines->parent                   = _rootNode.get();
  farLines->alwaysSelectAsActiveMesh = true;

  _trLinesPoints       = {Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly()};
  LinesMeshPtr trLines = nullptr;
  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _trLinesPoints;
    trLines           = LinesBuilder::CreateLines("trlines", options, _scene);
  }
  trLines->parent                   = _rootNode.get();
  trLines->alwaysSelectAsActiveMesh = true;

  _brLinesPoints       = {Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly()};
  LinesMeshPtr brLines = nullptr;
  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _brLinesPoints;
    brLines           = LinesBuilder::CreateLines("brlines", options, _scene);
  }
  brLines->parent                   = _rootNode.get();
  brLines->alwaysSelectAsActiveMesh = true;

  _tlLinesPoints       = {Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly()};
  LinesMeshPtr tlLines = nullptr;
  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _tlLinesPoints;
    tlLines           = LinesBuilder::CreateLines("tllines", options, _scene);
  }
  tlLines->parent                   = _rootNode.get();
  tlLines->alwaysSelectAsActiveMesh = true;

  _blLinesPoints       = {Vector3::ZeroReadOnly(), Vector3::ZeroReadOnly()};
  LinesMeshPtr blLines = nullptr;
  {
    LinesOptions options;
    options.updatable = true;
    options.points    = _blLinesPoints;
    blLines           = LinesBuilder::CreateLines("bllines", options, _scene);
  }
  blLines->parent                   = _rootNode.get();
  blLines->alwaysSelectAsActiveMesh = true;

  stl_util::concat(_lightHelperFrustumMeshes,
                   {nearLines, farLines, trLines, brLines, tlLines, blLines});

  const auto makePlane
    = [this](const std::string& name, const Color3& color, const Float32Array& positions) -> void {
    const auto plane = Mesh::New(name + "plane", _scene);
    const auto mat   = StandardMaterial::New(name + "PlaneMat", _scene);

    plane->material                 = mat;
    plane->parent                   = _rootNode.get();
    plane->alwaysSelectAsActiveMesh = true;

    mat->emissiveColor   = color;
    mat->alpha           = transparency;
    mat->backFaceCulling = false;
    mat->disableLighting = true;

    const IndicesArray indices = {0, 1, 2, 0, 2, 3};

    const auto vertexData = std::make_unique<VertexData>();

    vertexData->positions = positions;
    vertexData->indices   = std::move(indices);

    vertexData->applyToMesh(*plane, true);

    _lightHelperFrustumMeshes.emplace_back(plane);
  };

  _nearPlaneVertices   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  _farPlaneVertices    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  _rightPlaneVertices  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  _leftPlaneVertices   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  _topPlaneVertices    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  _bottomPlaneVertices = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  makePlane("near", Color3(1.f, 0.f, 0.f), _nearPlaneVertices);
  makePlane("far", Color3(0.3f, 0.f, 0.f), _farPlaneVertices);
  makePlane("right", Color3(0.f, 1.f, 0.f), _rightPlaneVertices);
  makePlane("left", Color3(0.f, 0.3f, 0.f), _leftPlaneVertices);
  makePlane("top", Color3(0.f, 0.f, 1.f), _topPlaneVertices);
  makePlane("bottom", Color3(0.f, 0.f, 0.3f), _bottomPlaneVertices);

  _nearLinesPoints[0] = TmpVectors::Vector3Array[2];
  _nearLinesPoints[1] = TmpVectors::Vector3Array[3];
  _nearLinesPoints[2] = TmpVectors::Vector3Array[4];
  _nearLinesPoints[3] = TmpVectors::Vector3Array[5];
  _nearLinesPoints[4] = TmpVectors::Vector3Array[2];

  _farLinesPoints[0] = TmpVectors::Vector3Array[6];
  _farLinesPoints[1] = TmpVectors::Vector3Array[7];
  _farLinesPoints[2] = TmpVectors::Vector3Array[8];
  _farLinesPoints[3] = TmpVectors::Vector3Array[9];
  _farLinesPoints[4] = TmpVectors::Vector3Array[6];

  _trLinesPoints[0] = TmpVectors::Vector3Array[2];
  _trLinesPoints[1] = TmpVectors::Vector3Array[6];

  _brLinesPoints[0] = TmpVectors::Vector3Array[3];
  _brLinesPoints[1] = TmpVectors::Vector3Array[7];

  _tlLinesPoints[0] = TmpVectors::Vector3Array[4];
  _tlLinesPoints[1] = TmpVectors::Vector3Array[8];

  _blLinesPoints[0] = TmpVectors::Vector3Array[5];
  _blLinesPoints[1] = TmpVectors::Vector3Array[9];
}

Matrix& DirectionalLightFrustumViewer::_getInvertViewMatrix()
{
  Matrix::LookAtLHToRef(_light->position(), _light->position().add(_light->direction()),
                        Vector3::UpReadOnly(), _inverseViewMatrix);
  _inverseViewMatrix.invertToRef(_inverseViewMatrix);
  return _inverseViewMatrix;
}

} // end of namespace BABYLON
