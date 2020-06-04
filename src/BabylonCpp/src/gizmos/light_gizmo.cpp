#include <babylon/gizmos/light_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadow_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/hemisphere_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

float LightGizmo::_Scale = 0.007f;

LightGizmo::LightGizmo(const UtilityLayerRendererPtr& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , light{this, &LightGizmo::get_light, &LightGizmo::set_light}
    , material{this, &LightGizmo::get_material}
    , _lightMesh{nullptr}
    , _material{nullptr}
    , _cachedForward{Vector3(0.f, 0.f, 1.f)}
    , _attachedMeshParent{nullptr}
    , _light{nullptr}
{
  attachedMesh        = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  _attachedMeshParent = TransformNode::New("parent", gizmoLayer->utilityLayerScene.get());

  attachedMesh()->parent   = _attachedMeshParent.get();
  _material                = StandardMaterial::New("light", gizmoLayer->utilityLayerScene.get());
  _material->diffuseColor  = Color3(0.5f, 0.5f, 0.5f);
  _material->specularColor = Color3(0.1f, 0.1f, 0.1f);
}

LightGizmo::~LightGizmo() = default;

void LightGizmo::set_light(const LightPtr& iLight)
{
  _light = iLight;
  if (iLight) {
    // Create the mesh for the given light type
    if (_lightMesh) {
      _lightMesh->dispose();
    }

    if (iLight->type() == Type::HEMISPHERICLIGHT) {
      _lightMesh = LightGizmo::_CreateHemisphericLightMesh(gizmoLayer->utilityLayerScene.get());
    }
    else if (iLight->type() == Type::DIRECTIONALLIGHT) {
      _lightMesh = LightGizmo::_CreateDirectionalLightMesh(gizmoLayer->utilityLayerScene.get());
    }
    else if (iLight->type() == Type::SPOTLIGHT) {
      _lightMesh = LightGizmo::_CreateSpotLightMesh(gizmoLayer->utilityLayerScene.get());
    }
    else {
      _lightMesh = LightGizmo::_CreatePointLightMesh(gizmoLayer->utilityLayerScene.get());
    }
    for (const auto& m : _lightMesh->getChildMeshes(false)) {
      m->material = _material;
    }
    _lightMesh->parent = _rootMesh.get();

    // Add lighting to the light gizmo
    const auto& gizmoLight = gizmoLayer->_getSharedGizmoLight();
    gizmoLight->includedOnlyMeshes
      = stl_util::concat(gizmoLight->includedOnlyMeshes(), _lightMesh->getChildMeshes(false));

    _lightMesh->rotationQuaternion = Quaternion();

    if (iLight->parent()) {
      _attachedMeshParent->freezeWorldMatrix(iLight->parent()->getWorldMatrix());
    }

    // Get update position and direction if the light has it
    auto shadowLight = std::static_pointer_cast<ShadowLight>(_light);
    if (shadowLight && attachedMesh()) {
      // Position
      attachedMesh()->position().copyFrom(shadowLight->position());
      attachedMesh()->computeWorldMatrix(true);
      _cachedPosition.copyFrom(attachedMesh()->position());
      // Direction
      attachedMesh()->setDirection(shadowLight->direction());
      attachedMesh()->computeWorldMatrix(true);
      _cachedForward.copyFrom(attachedMesh()->forward());
    }

    _update();
  }
}

LightPtr& LightGizmo::get_light()
{
  return _light;
}

StandardMaterialPtr& LightGizmo::get_material()
{
  return _material;
}

void LightGizmo::_update()
{
  Gizmo::_update();
  if (!_light) {
    return;
  }

  if (_light->parent()) {
    _attachedMeshParent->freezeWorldMatrix(_light->parent()->getWorldMatrix());
  }

  auto shadowLight = std::static_pointer_cast<ShadowLight>(_light);
  if (shadowLight) {
    // If the gizmo is moved update the light otherwise update the gizmo to match the light
    if (!attachedMesh()->position().equals(_cachedPosition)) {
      // update light to match gizmo
      shadowLight->position().copyFrom(attachedMesh()->position());
      _cachedPosition.copyFrom(attachedMesh()->position());
    }
    else {
      // update gizmo to match light
      attachedMesh()->position().copyFrom(shadowLight->position());
      attachedMesh()->computeWorldMatrix(true);
      _cachedPosition.copyFrom(attachedMesh()->position());
    }
  }
  if (shadowLight) {
    // If the gizmo is moved update the light otherwise update the gizmo to match the light
    if (Vector3::DistanceSquared(attachedMesh()->forward(), _cachedForward) > 0.0001f) {
      // update light to match gizmo
      shadowLight->direction().copyFrom(attachedMesh()->forward);
      _cachedForward.copyFrom(attachedMesh()->forward);
    }
    else if (Vector3::DistanceSquared(attachedMesh()->forward, shadowLight->direction())
             > 0.0001f) {
      // update gizmo to match light
      attachedMesh()->setDirection(shadowLight->direction());
      attachedMesh()->computeWorldMatrix(true);
      _cachedForward.copyFrom(attachedMesh()->forward);
    }
  }
  if (!_light->isEnabled()) {
    _material->diffuseColor.set(0.f, 0.f, 0.f);
  }
  else {
    _material->diffuseColor.set(_light->diffuse.r / 3.f, _light->diffuse.g / 3.f,
                                _light->diffuse.b / 3.f);
  }
}

MeshPtr LightGizmo::_CreateLightLines(float levels, Scene* scene)
{
  auto distFromSphere = 1.2f;

  auto root          = Mesh::New("root", scene);
  root->rotation().x = Math::PI_2;

  // Create the top line, this will be cloned for all other lines
  auto linePivot     = Mesh::New("linePivot", scene);
  linePivot->parent  = root.get();
  auto line          = Mesh::CreateCylinder("line", 2.f, 0.2f, 0.3f, 6, 1, scene);
  line->position().y = line->scaling().y / 2.f + distFromSphere;
  line->parent       = linePivot.get();

  if (levels < 2.f) {
    return linePivot;
  }
  for (unsigned i = 0u; i < 4; i++) {
    auto l          = linePivot->clone("lineParentClone");
    l->rotation().z = Math::PI_4;
    l->rotation().y = (Math::PI_2) + (Math::PI_2 * i);

    l->getChildMeshes()[0]->scaling().y = 0.5f;
    l->getChildMeshes()[0]->scaling().x = l->getChildMeshes()[0]->scaling().z = 0.8f;
    l->getChildMeshes()[0]->position().y
      = l->getChildMeshes()[0]->scaling().y / 2.f + distFromSphere;
  }

  if (levels < 3.f) {
    return root;
  }
  for (unsigned i = 0u; i < 4; i++) {
    auto l          = linePivot->clone("linePivotClone");
    l->rotation().z = Math::PI_2;
    l->rotation().y = (Math::PI_2 * i);
  }

  if (levels < 4.f) {
    return root;
  }
  for (unsigned i = 0u; i < 4; i++) {
    auto l          = linePivot->clone("linePivotClone");
    l->rotation().z = Math::PI + (Math::PI_4);
    l->rotation().y = (Math::PI_2) + (Math::PI_2 * i);

    l->getChildMeshes()[0]->scaling().y = 0.5;
    l->getChildMeshes()[0]->scaling().x = l->getChildMeshes()[0]->scaling().z = 0.8f;
    l->getChildMeshes()[0]->position().y
      = l->getChildMeshes()[0]->scaling().y / 2.f + distFromSphere;
  }

  if (levels < 5.f) {
    return root;
  }
  auto l          = linePivot->clone("linePivotClone");
  l->rotation().z = Math::PI;

  return root;
}

void LightGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _material->dispose();
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
  _attachedMeshParent->dispose();
}

MeshPtr LightGizmo::_CreateHemisphericLightMesh(Scene* scene)
{
  auto root = Mesh::New("hemisphereLight", scene);
  HemisphereOptions hemisphereOptions;
  hemisphereOptions.diameter = 1.f;
  hemisphereOptions.segments = 10;
  auto hemisphere = HemisphereBuilder::CreateHemisphere(root->name, hemisphereOptions, scene);
  hemisphere->position().z = -0.15f;
  hemisphere->rotation().x = Math::PI_2;
  hemisphere->parent       = root.get();

  auto lines          = _CreateLightLines(3.f, scene);
  lines->parent       = root.get();
  lines->position().z = 0.15f;

  root->scaling().scaleInPlace(LightGizmo::_Scale);
  root->rotation().x = Math::PI_2;

  return root;
}

MeshPtr LightGizmo::_CreatePointLightMesh(Scene* scene)
{
  auto root = Mesh::New("pointLight", scene);
  SphereOptions sphereOptions;
  sphereOptions.diameter = 1.f;
  sphereOptions.segments = 10;
  auto sphere            = SphereBuilder::CreateSphere(root->name, sphereOptions, scene);
  sphere->rotation().x   = Math::PI_2;
  sphere->parent         = root.get();

  auto lines    = _CreateLightLines(5.f, scene);
  lines->parent = root.get();
  root->scaling().scaleInPlace(LightGizmo::_Scale);
  root->rotation().x = Math::PI_2;

  return root;
}

MeshPtr LightGizmo::_CreateSpotLightMesh(Scene* scene)
{
  auto root = Mesh::New("spotLight", scene);
  SphereOptions sphereOptions;
  sphereOptions.diameter = 1.f;
  sphereOptions.segments = 10;
  auto sphere            = SphereBuilder::CreateSphere(root->name, sphereOptions, scene);
  sphere->parent         = root.get();

  HemisphereOptions hemisphereOptions;
  hemisphereOptions.diameter = 2.f;
  hemisphereOptions.segments = 10;
  auto hemisphere    = HemisphereBuilder::CreateHemisphere(root->name, hemisphereOptions, scene);
  hemisphere->parent = root.get();
  hemisphere->rotation().x = -Math::PI_2;

  auto lines    = _CreateLightLines(2.f, scene);
  lines->parent = root.get();
  root->scaling().scaleInPlace(LightGizmo::_Scale);
  root->rotation().x = Math::PI_2;

  return root;
}

MeshPtr LightGizmo::_CreateDirectionalLightMesh(Scene* scene)
{
  auto root = Mesh::New("directionalLight", scene);

  auto mesh    = Mesh::New(root->name, scene);
  mesh->parent = root.get();
  SphereOptions options;
  options.diameter = 1.2f;
  options.segments = 10;
  auto sphere      = SphereBuilder::CreateSphere(root->name, options, scene);
  sphere->parent   = mesh.get();

  auto line    = Mesh::CreateCylinder(root->name, 6.f, 0.3f, 0.3f, 6, 1, scene);
  line->parent = mesh.get();

  auto left         = line->clone(root->name);
  left->scaling().y = 0.5f;
  left->position().x += 1.25f;

  auto right         = line->clone(root->name);
  right->scaling().y = 0.5f;
  right->position().x += -1.25f;

  auto arrowHead = Mesh::CreateCylinder(root->name, 1.f, 0.f, 0.6f, 6, 1, scene);
  arrowHead->position().y += 3.f;
  arrowHead->parent = mesh.get();

  left               = arrowHead->clone(root->name);
  left->position().y = 1.5f;
  left->position().x += 1.25f;

  right               = arrowHead->clone(root->name);
  right->position().y = 1.5f;
  right->position().x += -1.25f;

  mesh->scaling().scaleInPlace(LightGizmo::_Scale);
  mesh->rotation().z = Math::PI_2;
  mesh->rotation().y = Math::PI_2;
  return root;
}

} // end of namespace BABYLON
