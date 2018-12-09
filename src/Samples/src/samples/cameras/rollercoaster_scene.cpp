#include <babylon/samples/cameras/rollercoaster_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>

#include <babylon/cameras/follow_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/curve3.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

RollercoasterScene::RollercoasterScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

RollercoasterScene::~RollercoasterScene()
{
}

const char* RollercoasterScene::getName()
{
  return "Rollercoaster Scene";
}

void RollercoasterScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Image resources for textures
  std::string urlRoof   = "textures/rooftile2.jpg";
  std::string urlWall   = "textures/stonewall.jpg";
  std::string urlGrass  = "textures/grass2.jpg";
  std::string urlSkyBox = "textures/skybox/skybox";

  // Let's create a scene with an hemispheric light
  scene->clearColor = Color3(0.4f, 0.6f, 1.f);
  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.2f), scene);
  light->intensity = 0.7f;

  // Then the sky with a Skybox
  auto skybox
    = Mesh::CreateBox("skyBox", 2000.f, scene, false, Mesh::BACKSIDE());
  auto skyboxMaterial                = StandardMaterial::New("skyBox", scene);
  auto reflectionTexture             = CubeTexture::New(urlSkyBox, scene);
  reflectionTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
  skyboxMaterial->diffuseColor       = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->specularColor      = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->reflectionTexture  = reflectionTexture;
  skybox->material                   = skyboxMaterial;

  // Then the ground with a textured big disc
  auto ground               = Mesh::CreateDisc("ground", 800, 60, scene);
  ground->rotation().x      = Math::PI_2;
  auto matGround            = StandardMaterial::New("mg", scene);
  ground->material          = matGround;
  auto groundTexture        = Texture::New(urlGrass, scene);
  groundTexture->uScale     = 100.f;
  groundTexture->vScale     = 100.f;
  matGround->diffuseTexture = groundTexture;
  matGround->specularColor  = Color3::Black();

  // Now we will draw the roller coaster rails
  // We set a closed curve by concatenating 5 different curves :
  // 1 - a set of three chained loops
  // 2 - a  left turn
  // 3 - a hill
  // 4 - a  right turn
  // 5 - a tiny hermite spline to close the curve
  // Curve computation ============================================
  Curve3 continued;
  float x = 0.f, y = 0.f, z = 0.f;

  // Loops : cosinus and sinus
  std::vector<Vector3> loops;
  size_t nbpt = 200;
  float nbptf = static_cast<float>(nbpt);
  float pi6   = Math::PI * 6.f;
  for (float lp = 0; lp < nbpt; ++lp) {
    x = lp - nbptf / 2.f
        + (nbptf / 5.f - lp / 8.f) * std::cos(pi6 * lp / nbptf);
    y = nbptf / 5.f + (nbptf / 5.f - lp / 8.f) * std::sin(pi6 * lp / nbptf);
    z = lp - nbptf;
    loops.emplace_back(Vector3(x, y, z));
  }

  // First turn : cosinus and sinus
  std::vector<Vector3> turn1;
  float ang = Math::PI;
  nbpt      = 40;
  nbptf     = static_cast<float>(nbpt);
  for (float tp = 0; tp < nbpt; ++tp) {
    x = 30.f * std::cos(ang * tp / nbptf - 0.8f);
    y = 30.f * std::sin(ang * tp / nbptf);
    z = 30.f * std::sin(ang * tp / nbptf - 0.8f);
    turn1.emplace_back(Vector3(x, y, z));
  }

  // Hill : exponential
  std::vector<Vector3> hill;
  nbpt  = 80;
  nbptf = static_cast<float>(nbpt);
  for (float hp = 0; hp < nbpt; ++hp) {
    x = -hp * 2.f;
    y = 80.f
        * std::exp(-(hp - nbptf / 3.f) * (hp - nbptf / 3.f) / (nbptf * 5.f));
    z = -hp * 3.2f;
    hill.emplace_back(Vector3(x, y, z));
  }

  // Second turn : cosinus and sinus
  std::vector<Vector3> turn2;
  nbpt  = 60;
  nbptf = static_cast<float>(nbpt);
  ang   = -Math::PI * 3.f / 2.f;
  for (float tp = 0; tp < nbpt; ++tp) {
    x = 30.f * std::cos(ang * tp / nbptf - 0.6f);
    y = -tp / 2.5f;
    z = 30.f * std::sin(ang * tp / nbptf - 0.6f);
    turn2.emplace_back(Vector3(x, y, z));
  }

  // Close rails with hermite
  Curve3 curloops(loops);
  Curve3 curturn1(turn1);
  Curve3 curhill(hill);
  Curve3 curturn2(turn2);
  continued = (curloops)
                .continueCurve3(curturn1)
                .continueCurve3(curhill)
                .continueCurve3(curturn2);
  _points = continued.getPoints();

  auto p1      = _points[_points.size() - 1];
  auto t1      = (p1.subtract(_points[_points.size() - 2])).scale(10.f);
  auto p2      = _points[0];
  auto t2      = (_points[1].subtract(p2)).scale(30.f);
  auto hermite = Curve3::CreateHermiteSpline(p1, t1, p2, t2, 15);

  // Full final curve
  continued = hermite.continueCurve3(continued);
  _points   = continued.getPoints();
  // Curve computed ===========================================

  // Let's know define a simple triangular shape for the rails
  std::vector<Vector3> shape{
    Vector3(0.f, 2.f, 0.f),  //
    Vector3(-1.f, 0.f, 0.f), //
    Vector3(0.f, -2.f, 0.f), //
  };
  shape.emplace_back(shape[0]);

  // Let's now extrude this shape along the curve to draw the whole roller
  // coaster
  auto origin = Vector3::Zero();
  auto rollerCoaster
    = Mesh::ExtrudeShape("rc", shape, _points, 2, 0, Mesh::NO_CAP(), scene);
  auto mat                = StandardMaterial::New("mat1", scene);
  mat->diffuseColor       = Color3(1.f, 0.8f, 1.f);
  mat->wireframe          = true;
  rollerCoaster->material = mat;
  rollerCoaster->position = origin;

  // As the first and final extruded shape may not fit along a closed curve, we
  // hide this junction with a little house
  auto house          = Mesh::CreateBox("house", 10, scene);
  house->scaling().x  = 2.f;
  house->scaling().z  = 1.5f;
  house->rotation().y = 0.6f;
  house->position     = _points[0];
  auto roof           = Mesh::CreateCylinder("roof", 22, 20, 20, 3, 1, scene);
  roof->rotation().y  = house->rotation().y;
  roof->rotation().z  = Math::PI_2;
  roof->position().x  = house->position().x;
  roof->position().y  = 12;
  roof->position().z  = house->position().z;
  auto wallTexture    = Texture::New(urlWall, scene);
  auto houseMat       = StandardMaterial::New("housemat", scene);
  houseMat->diffuseTexture = wallTexture;
  house->material          = houseMat;
  auto roofTexture         = Texture::New(urlRoof, scene);
  roofTexture->uScale      = 10.f;
  roofTexture->vScale      = 5.f;
  auto roofMat             = StandardMaterial::New("roofmat", scene);
  roofMat->specularColor   = Color3::Black();
  roofMat->diffuseTexture  = roofTexture;
  roof->material           = roofMat;

  // All immobile meshes can be now frozen to avoid un-needed matrix
  // computations
  skybox->freezeWorldMatrix();
  ground->freezeWorldMatrix();
  house->freezeWorldMatrix();
  roof->freezeWorldMatrix();

  // We've got the rails, we need now a wagon !
  _wagon                 = Mesh::CreateBox("wagon", 4, scene);
  _wagon->scaling().x    = 2.f;
  auto matWagon          = StandardMaterial::New("mw", scene);
  matWagon->diffuseColor = Color3(1.f, 0.f, 0.8f);
  _wagon->material       = matWagon;
  _wagon->position       = origin;

  // Just in case you want to position the roller coaster somewhere else
  // translate the relative curve points to the same origin than the mesh
  _lg = _points.size();
  for (size_t pt = 0; pt < _lg; ++pt) {
    _points[pt].addInPlace(origin);
  }

  // Let's add a Follow Camera.
  // It is set initially very far from the roller-coaster in order to get an
  // "approach"" effect on start
  _followCam = FollowCamera::New("fcam", Vector3(20.f, 200.f, -800.f), scene);
  _followCam->lockedTarget = _wagon;
  _followCam->attachControl(canvas, true);

  // followCam.target = wagon;
  _followCam->radius         = 50.f;
  _followCam->maxCameraSpeed = 1.f;
  scene->activeCamera        = _followCam;

  // All meshes and cam are set, so now let's handle the wagon trajectory
  // Let's create a Path3D object with the curve
  auto path3d = Path3D(_points);
  _tgts       = path3d.getTangents();
  _norms      = path3d.getNormals();
  _binorms    = path3d.getBinormals();

  // let's scale the normals, where the wagon will be attached, to be a little
  // farther from the rails
  float shift = 2.f;
  for (size_t n = 0; n < _lg; n++) {
    _norms[n].scaleInPlace(shift);
  }

  // Let's initialize now some variables for updating the wagon position each
  // frame
  _i       = 0;
  _j       = 1;
  _curvect = _points[_j].subtract(_points[_i]);
  _rot     = Vector3::RotationFromAxis(_tgts[_i], _norms[_i], _binorms[_i]);
  _pos     = Vector3::Zero();
  _scaled  = Vector3::Zero();
  _step    = 3;
  _k       = 0;

  // Animation
  // the wagon slides many steps on each curve segment
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    _curvect.scaleToRef(static_cast<float>(_k) / static_cast<float>(_step),
                        _scaled);
    _points[_i].addToRef(_scaled, _pos);
    _pos.addInPlace(_norms[_i]);
    _wagon->position = _pos;
    _wagon->rotation = _rot;
    ++_k;
    // next segment
    if (_k == _step) {
      _i   = (_i + 1) % _lg;
      _j   = (_i + 1) % _lg;
      _rot = Vector3::RotationFromAxis(_tgts[_i], _norms[_i], _binorms[_i]);
      _points[_j].subtractToRef(_points[_i], _curvect);
      _k = 0;
      if (_i == 0) {
        // followCam->radius = 50.f + 200.f * Math::random();
      }
    }
    _followCam->position.y = 50.f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
