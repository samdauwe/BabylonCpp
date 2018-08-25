#include <babylon/extensions/manipulationhelpers/radix.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_data.h>

namespace BABYLON {
namespace Extensions {

float Radix::pc = 0.6f;
float Radix::sc = 0.2f;

Radix::Radix(Scene* scene, RadixFeatures features, float arrowLength,
             float coneLength, float coneRadius, float planeSelectionLength)
{
  _scene                = scene;
  _arrowLength          = (arrowLength > 0.f) ? arrowLength : 1.f;
  _coneLength           = (coneLength > 0.f) ? coneLength : 0.2f;
  _coneRadius           = (coneRadius > 0.f) ? coneRadius : 0.1f;
  _planeSelectionLength = (planeSelectionLength > 0.f) ? planeSelectionLength :
                                                         (_arrowLength / 5.f);

  _wireSelectionThreshold = 0.05f;

  _light1 = PointLight::New("ManipulatorLight", Vector3(50, 50, 70), _scene);
  _light1->id = "***SceneManipulatorLight***";
  _light2 = PointLight::New("ManipulatorLight", Vector3(-50, -50, -70), _scene);
  _light2->id = "***SceneManipulatorLight***";

  _xArrowColor = Color3(Radix::pc, Radix::sc / 2.f, Radix::sc);
  _yArrowColor = Color3(Radix::sc, Radix::pc, Radix::sc / 2.f);
  _zArrowColor = Color3(Radix::sc / 2.f, Radix::sc, Radix::pc);

  _xyPlaneSelectionColor
    = Color3(Radix::pc / 1.1f, Radix::pc / 1.3f, Radix::sc);
  _xzPlaneSelectionColor
    = Color3(Radix::pc / 1.1f, Radix::sc, Radix::pc / 1.3f);
  _yzPlaneSelectionColor
    = Color3(Radix::sc, Radix::pc / 1.3f, Radix::pc / 1.1f);

  using MaterialDef = std::tuple<std::string, Color3>;
  std::vector<MaterialDef> materials;
  materials.emplace_back(MaterialDef{"arrowX", xArrowColor()});
  materials.emplace_back(MaterialDef{"arrowY", yArrowColor()});
  materials.emplace_back(MaterialDef{"arrowZ", zArrowColor()});
  materials.emplace_back(MaterialDef{"planeXY", xyPlaneSelectionColor()});
  materials.emplace_back(MaterialDef{"planeXZ", xzPlaneSelectionColor()});
  materials.emplace_back(MaterialDef{"planeYZ", yzPlaneSelectionColor()});
  materials.emplace_back(MaterialDef{"rotationX", xArrowColor()});
  materials.emplace_back(MaterialDef{"rotationY", yArrowColor()});
  materials.emplace_back(MaterialDef{"rotationZ", zArrowColor()});

  _materials.clear();
  for (auto& matData : materials) {
    std::string materialNme;
    Color3 materialColor;
    std::tie(materialNme, materialColor) = matData;
    auto mtl = StandardMaterial::New(materialNme + "RadixMaterial", _scene);
    mtl->diffuseColor = materialColor;

    _materials[materialNme] = mtl;
  }

  _features                   = features;
  _rootMesh                   = Mesh::New("radixRoot", _scene);
  _rootMesh->renderingGroupId = 1;

  constructGraphicalObjects();
}

Radix::~Radix()
{
}

RadixFeatures Radix::intersect(const Vector2& pos)
{
  auto hit     = RadixFeatures::None;
  auto closest = numeric_limits_t<float>::max();

  // Arrows
  if (hasFeature(RadixFeatures::ArrowX)) {
    auto dist = intersectMeshes(pos, "arrowX", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::ArrowX;
    }
  }

  if (hasFeature(RadixFeatures::ArrowY)) {
    auto dist = intersectMeshes(pos, "arrowY", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::ArrowY;
    }
  }

  if (hasFeature(RadixFeatures::ArrowZ)) {
    auto dist = intersectMeshes(pos, "arrowZ", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::ArrowZ;
    }
  }

  // Planes
  if (hasFeature(RadixFeatures::PlaneSelectionXY)) {
    auto dist = intersectMeshes(pos, "planeXY", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::PlaneSelectionXY;
    }
  }

  if (hasFeature(RadixFeatures::PlaneSelectionXZ)) {
    auto dist = intersectMeshes(pos, "planeXZ", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::PlaneSelectionXZ;
    }
  }

  if (hasFeature(RadixFeatures::PlaneSelectionYZ)) {
    auto dist = intersectMeshes(pos, "planeYZ", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::PlaneSelectionYZ;
    }
  }

  // Rotation
  if (hasFeature(RadixFeatures::RotationX)) {
    auto dist = intersectMeshes(pos, "rotationX", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::RotationX;
    }
  }

  if (hasFeature(RadixFeatures::RotationY)) {
    auto dist = intersectMeshes(pos, "rotationY", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::RotationY;
    }
  }

  if (hasFeature(RadixFeatures::RotationZ)) {
    auto dist = intersectMeshes(pos, "rotationZ", closest);
    if (dist < closest) {
      closest = dist;
      hit     = RadixFeatures::RotationZ;
    }
  }
  return hit;
}

void Radix::setWorld(const Vector3& position, const Quaternion& rotation,
                     const Vector3& scale)
{
  _rootMesh->position           = position;
  _rootMesh->rotationQuaternion = rotation;
  _rootMesh->scaling            = scale;
}

void Radix::show()
{
  setVisibleState(_rootMesh, true);
}

void Radix::hide()
{
  setVisibleState(_rootMesh, false);
}

void Radix::setVisibleState(const AbstractMeshPtr& mesh, bool state)
{
  mesh->isVisible = state;
  for (auto& childMesh : mesh->getChildMeshes(true)) {
    setVisibleState(childMesh, state);
  }
}

float Radix::intersectMeshes(const Vector2& pos, const std::string& startName,
                             float currentClosest)
{
  auto meshes
    = _rootMesh->getChildMeshes(true, [&startName](const NodePtr& node) {
        return String::startsWith(node->name, startName);
      });
  for (auto& mesh : meshes) {
    auto ray = _scene->createPickingRay(
      static_cast<int>(pos.x), static_cast<int>(pos.y), mesh->getWorldMatrix(),
      _scene->activeCamera);
    auto pi = mesh->intersects(ray, false);
    if (pi.hit && pi.distance < currentClosest) {
      currentClosest = pi.distance;
    }
  }
  return currentClosest;
}

void Radix::constructGraphicalObjects()
{
  const auto hp = Math::PI_2;
  if (hasFeature(RadixFeatures::ArrowX)) {
    constructArrow(RadixFeatures::ArrowX, "arrowX", Matrix::RotationZ(-hp));
  }
  if (hasFeature(RadixFeatures::ArrowY)) {
    constructArrow(RadixFeatures::ArrowY, "arrowY", Matrix::Identity());
  }
  if (hasFeature(RadixFeatures::ArrowZ)) {
    constructArrow(RadixFeatures::ArrowZ, "arrowZ", Matrix::RotationX(hp));
  }

  if (hasFeature(RadixFeatures::PlaneSelectionXY)) {
    constructPlaneSelection(RadixFeatures::PlaneSelectionXY, "planeXY",
                            Matrix::Identity());
  }

  if (hasFeature(RadixFeatures::PlaneSelectionXZ)) {
    constructPlaneSelection(RadixFeatures::PlaneSelectionXZ, "planeXZ",
                            Matrix::RotationX(hp));
  }

  if (hasFeature(RadixFeatures::PlaneSelectionYZ)) {
    constructPlaneSelection(RadixFeatures::PlaneSelectionYZ, "planeYZ",
                            Matrix::RotationY(-hp));
  }

  if (hasFeature(RadixFeatures::RotationX)) {
    constructRotation(RadixFeatures::RotationX, "rotationX",
                      Matrix::RotationZ(-hp));
  }

  if (hasFeature(RadixFeatures::RotationY)) {
    constructRotation(RadixFeatures::RotationY, "rotationY",
                      Matrix::Identity());
  }

  if (hasFeature(RadixFeatures::RotationZ)) {
    constructRotation(RadixFeatures::RotationZ, "rotationZ",
                      Matrix::RotationX(hp));
  }
}

void Radix::constructArrow(RadixFeatures feature, const std::string& name,
                           const Matrix& transform)
{
  auto mtl    = getMaterial(name);
  bool hasRot = false;

  switch (feature) {
    case RadixFeatures::ArrowX:
      hasRot = hasFeature(RadixFeatures::RotationX);
      break;
    case RadixFeatures::ArrowY:
      hasRot = hasFeature(RadixFeatures::RotationY);
      break;
    case RadixFeatures::ArrowZ:
      hasRot = hasFeature(RadixFeatures::RotationZ);
      break;
    default:
      hasRot = false;
  }

  auto rotation = Quaternion::FromRotationMatrix(transform);

  Float32Array points;
  stl_util::concat(points, {0.f, hasRot ? _coneLength : 0.f, 0.f});
  stl_util::concat(points, {0.f, _arrowLength - _coneLength, 0.f});

  auto wireMesh                = LinesMesh::New(name + "Wire", _scene);
  wireMesh->rotationQuaternion = rotation;
  wireMesh->setParent(_rootMesh.get());
  wireMesh->color                 = mtl->diffuseColor;
  wireMesh->renderingGroupId      = 1;
  wireMesh->intersectionThreshold = wireSelectionThreshold();
  wireMesh->isPickable            = false;

  auto vd       = std::make_unique<VertexData>();
  vd->positions = points;
  vd->indices   = {0, 1};
  vd->applyToMesh(*wireMesh);

  auto arrow = Mesh::CreateCylinder(name + "Cone", _coneLength, 0, _coneRadius,
                                    18, 1, _scene, false);
  arrow->position = Vector3::TransformCoordinates(
    Vector3(0, _arrowLength - (_coneLength / 2.f), 0.f), transform);
  arrow->rotationQuaternion = rotation;
  arrow->material           = mtl;
  arrow->setParent(_rootMesh.get());
  arrow->renderingGroupId = 1;
  arrow->isPickable       = false;
  addSymbolicMeshToLit(arrow);
}

void Radix::constructPlaneSelection(RadixFeatures /*feature*/,
                                    const std::string& name,
                                    const Matrix& transform)
{
  auto mtl = getMaterial(name);

  std::vector<Vector3> points;

  points.emplace_back(
    Vector3(_arrowLength - _planeSelectionLength, _arrowLength, 0));
  points.emplace_back(Vector3(_arrowLength, _arrowLength, 0));
  points.emplace_back(
    Vector3(_arrowLength, _arrowLength - _planeSelectionLength, 0));

  auto wireMesh = Mesh::CreateLines(name + "Plane", points, _scene);
  wireMesh->setParent(_rootMesh.get());
  wireMesh->color                 = mtl->diffuseColor;
  wireMesh->rotationQuaternion    = Quaternion::FromRotationMatrix(transform);
  wireMesh->renderingGroupId      = 1;
  wireMesh->intersectionThreshold = wireSelectionThreshold();
  wireMesh->isPickable            = false;
}

void Radix::constructRotation(RadixFeatures /*feature*/,
                              const std::string& name, const Matrix& transform)
{
  auto mtl = getMaterial(name);

  auto rotCyl
    = Mesh::CreateCylinder(name + "Cylinder", _coneLength, _coneRadius,
                           _coneRadius, 18, 1, _scene, false);
  rotCyl->material = mtl;
  rotCyl->position = Vector3::TransformCoordinates(
    Vector3(0.f, _coneLength / 2.f, 0.f), transform);
  rotCyl->rotationQuaternion = Quaternion::FromRotationMatrix(transform);
  rotCyl->setParent(_rootMesh.get());
  rotCyl->renderingGroupId = 1;
  rotCyl->isPickable       = false;
  addSymbolicMeshToLit(rotCyl);
}

void Radix::addSymbolicMeshToLit(const AbstractMeshPtr& mesh)
{
  _light1->includedOnlyMeshes().emplace_back(mesh.get());
  _light2->includedOnlyMeshes().emplace_back(mesh.get());
  for (auto& l : _scene->lights) {
    if ((l != _light1) && (l != _light2)) {
      l->excludedMeshes().emplace_back(mesh.get());
    }
  }
}

bool Radix::hasFeature(RadixFeatures value)
{
  return (_features & value) != RadixFeatures::None;
}

bool Radix::hasHighlightedFeature(RadixFeatures value)
{
  return (_highlighted & value) != RadixFeatures::None;
}

void Radix::updateMaterial(const std::string& name, const Color3& color)
{
  auto mtl = getMaterial(name);
  if (mtl) {
    mtl->diffuseColor = color;
  }
}

void Radix::updateMaterialFromHighlighted(RadixFeatures feature,
                                          RadixFeatures highlighted,
                                          const std::string& name)
{
  if (!hasFeature(feature)) {
    return;
  }

  if ((_highlighted & feature) != (highlighted & feature)) {
    auto mtl = getMaterial(name);
    if ((highlighted & feature) != RadixFeatures::None) {
      mtl->diffuseColor.r *= 1.8f;
      mtl->diffuseColor.g *= 1.8f;
      mtl->diffuseColor.b *= 1.8f;
    }
    else {
      mtl->diffuseColor.r /= 1.8f;
      mtl->diffuseColor.g /= 1.8f;
      mtl->diffuseColor.b /= 1.8f;
    }
  }
}

StandardMaterialPtr Radix::getMaterial(const std::string& name)
{
  if (!stl_util::contains(_materials, name)) {
    return nullptr;
  }

  return ::std::static_pointer_cast<StandardMaterial>(_materials[name]);
}

/** Getters / Setters **/

float Radix::wireSelectionThreshold()
{
  return _wireSelectionThreshold;
}

void Radix::setWireSelectionThreshold(float value)
{
  _wireSelectionThreshold = value;

  auto meshes = _rootMesh->getChildMeshes(true, [](const NodePtr& node) {
    return node->type() == IReflect::Type::LINESMESH;
  });
  for (auto mesh : meshes) {
    auto lm = ::std::static_pointer_cast<LinesMesh>(mesh);
    if (lm) {
      lm->intersectionThreshold = value;
    }
  }
}

Color3& Radix::xArrowColor()
{
  return _xArrowColor;
}

void Radix::setXArrowColor(const Color3& value)
{
  _xArrowColor = value;
  updateMaterial("arrowX", value);
  updateMaterial("rotationX", value);
}

Color3& Radix::yArrowColor()
{
  return _yArrowColor;
}

void Radix::setYArrowColor(const Color3& value)
{
  _yArrowColor = value;
  updateMaterial("arrowY", value);
  updateMaterial("rotationY", value);
}

Color3& Radix::zArrowColor()
{
  return _zArrowColor;
}

void Radix::setZArrowColor(const Color3& value)
{
  _zArrowColor = value;
  updateMaterial("arrowZ", value);
  updateMaterial("rotationZ", value);
}

Color3& Radix::xyPlaneSelectionColor()
{
  return _xyPlaneSelectionColor;
}

void Radix::setXYPlaneSelectionColor(const Color3& value)
{
  _xyPlaneSelectionColor = value;
}

Color3& Radix::xzPlaneSelectionColor()
{
  return _xzPlaneSelectionColor;
}

void Radix::setXZPlaneSelectionColor(const Color3& value)
{
  _xzPlaneSelectionColor = value;
}

Color3& Radix::yzPlaneSelectionColor()
{
  return _yzPlaneSelectionColor;
}

void Radix::setYZPlaneSelectionColor(const Color3& value)
{
  _yzPlaneSelectionColor = value;
}

RadixFeatures Radix::highlighted() const
{
  return _highlighted;
}

void Radix::setHighlighted(RadixFeatures value)
{
  updateMaterialFromHighlighted(RadixFeatures::ArrowX, value, "arrowX");
  updateMaterialFromHighlighted(RadixFeatures::ArrowY, value, "arrowY");
  updateMaterialFromHighlighted(RadixFeatures::ArrowZ, value, "arrowZ");
  updateMaterialFromHighlighted(RadixFeatures::PlaneSelectionXY, value,
                                "planeXY");
  updateMaterialFromHighlighted(RadixFeatures::PlaneSelectionXZ, value,
                                "planeXZ");
  updateMaterialFromHighlighted(RadixFeatures::PlaneSelectionYZ, value,
                                "planeYZ");
  updateMaterialFromHighlighted(RadixFeatures::RotationX, value, "rotationX");
  updateMaterialFromHighlighted(RadixFeatures::RotationY, value, "rotationY");
  updateMaterialFromHighlighted(RadixFeatures::RotationZ, value, "rotationZ");

  _highlighted = value;
}

RadixFeatures Radix::features() const
{
  return _features;
}

} // end of namespace Extensions
} // end of namespace BABYLON
