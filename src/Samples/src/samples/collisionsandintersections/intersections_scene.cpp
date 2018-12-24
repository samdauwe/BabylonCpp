#include <babylon/samples/collisionsandintersections/intersections_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

IntersectionsScene::IntersectionsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _balloon1Material{nullptr}
    , _balloon2Material{nullptr}
    , _balloon3Material{nullptr}
    , _plane1{nullptr}
    , _plane2{nullptr}
    , _balloon1{nullptr}
    , _balloon2{nullptr}
    , _balloon3{nullptr}
    , _alpha{Math::PI}
{
}

IntersectionsScene::~IntersectionsScene()
{
}

const char* IntersectionsScene::getName()
{
  return "Intersections Scene";
}

void IntersectionsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 1, 0.8f, 70, Vector3::Zero(), scene);
  camera->setTarget(Vector3(5.f, 0.f, 0.f));
  camera->attachControl(canvas, true);

  // Material
  auto matPlane             = StandardMaterial::New("matPlan1", scene);
  matPlane->backFaceCulling = false;
  matPlane->emissiveColor   = Color3(0.2f, 1.f, 0.2f);

  auto matBB           = StandardMaterial::New("matBB", scene);
  matBB->emissiveColor = Color3(1.f, 1.f, 1.f);
  matBB->wireframe     = true;

  // Intersection point
  _pointToIntersect = Vector3(-30.f, 0.f, 0.f);
  auto origin       = Mesh::CreateSphere("origin", 4, 0.3f, scene);
  origin->position  = _pointToIntersect;
  origin->material  = matPlane;

  // Create two planes
  _plane1               = Mesh::CreatePlane("plane1", 20, scene);
  _plane1->position     = Vector3(13.f, 0.f, 0.f);
  _plane1->rotation().x = -Math::PI_4;
  _plane1->material     = matPlane;

  _plane2               = Mesh::CreatePlane("plane2", 20, scene);
  _plane2->position     = Vector3(-13.f, 0.f, 0.f);
  _plane2->rotation().x = -Math::PI_4;
  _plane2->material     = matPlane;

  // AABB - Axis aligned bounding box
  auto planAABB      = Mesh::CreateBox("AABB", 20, scene);
  planAABB->material = matBB;
  planAABB->position = Vector3(13.f, 0.f, 0.f);
  planAABB->scaling  = Vector3(1, std::cos(Math::PI_4), std::cos(Math::PI_4));

  // OBB - Object boundind box
  auto planOBB      = Mesh::CreateBox("OBB", 20, scene);
  planOBB->scaling  = Vector3(1.f, 1.f, 0.05f);
  planOBB->parent   = _plane2.get();
  planOBB->material = matBB;

  // Balloons
  _balloon1 = Mesh::CreateSphere("balloon1", 10, 2.f, scene);
  _balloon2 = Mesh::CreateSphere("balloon2", 10, 2.f, scene);
  _balloon3 = Mesh::CreateSphere("balloon3", 10, 2.f, scene);

  _balloon1Material = StandardMaterial::New("matBallon1", scene);
  _balloon2Material = StandardMaterial::New("matBallon2", scene);
  _balloon3Material = StandardMaterial::New("matBallon3", scene);

  _balloon1->material = _balloon1Material;
  _balloon2->material = _balloon2Material;
  _balloon3->material = _balloon3Material;

  _balloon1->position = Vector3(6.f, 5.f, 0.f);
  _balloon2->position = Vector3(-6.f, 5.f, 0.f);
  _balloon3->position = Vector3(-30.f, 5.f, 0.f);

  // Animation
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    // Balloon 1 intersection -- Precise = false
    if (_balloon1->intersectsMesh(*_plane1, false)) {
      _balloon1Material->emissiveColor = Color3(1.f, 0.f, 0.f);
    }
    else {
      _balloon1Material->emissiveColor = Color3(1.f, 1.f, 1.f);
    }

    // Balloon 2 intersection -- Precise = true
    if (_balloon2->intersectsMesh(*_plane2, true)) {
      _balloon2Material->emissiveColor = Color3(1.f, 0.f, 0.f);
    }
    else {
      _balloon2Material->emissiveColor = Color3(1.f, 1.f, 1.f);
    }

    // Balloon 3 intersection on single point
    if (_balloon3->intersectsPoint(_pointToIntersect)) {
      _balloon3Material->emissiveColor = Color3(1.f, 0.f, 0.f);
    }
    else {
      _balloon3Material->emissiveColor = Color3(1.f, 1.f, 1.f);
    }

    _alpha += 0.01f;
    _balloon1->position().y += std::cos(_alpha) / 10.f;
    _balloon2->position().y = _balloon1->position().y;
    _balloon3->position().y = _balloon1->position().y;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
