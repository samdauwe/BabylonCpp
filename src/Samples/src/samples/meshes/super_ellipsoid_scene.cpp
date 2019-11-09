#include <babylon/samples/samples_index.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/color3.h>
#include <babylon/math/scalar.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class SuperEllipsoidScene : public IRenderableScene {

public:
  SuperEllipsoidScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
  }
  ~SuperEllipsoidScene()
  {
  }

  const char* getName() override
  {
    return "Super Ellipsoid Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a rotating camera
    auto camera = ArcRotateCamera::New("Camera", 0.f, Math::PI_2, 12.f,
                                       Vector3::Zero(), scene);
    camera->setPosition(Vector3(-5.f, 5.f, 5.f));
  
    // Attach it to handle user inputs (keyboard, mouse, touch)
    camera->attachControl(canvas, false);
  
    // Add a light
    auto light = HemisphericLight::New("hemi", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.95f;
  
    // Create super ellipsoid and material
    auto mat              = StandardMaterial::New("mat", scene);
    mat->diffuseColor     = Color3::Purple();
    mat->backFaceCulling  = false;
    auto mat2             = StandardMaterial::New("mat2", scene);
    mat2->diffuseColor    = Color3::Red();
    mat2->backFaceCulling = false;
    auto mat3             = StandardMaterial::New("mat3", scene);
    mat3->diffuseColor    = Color3::Green();
    mat3->backFaceCulling = false;
    auto mat4             = StandardMaterial::New("mat4", scene);
    mat4->diffuseColor    = Color3::Blue();
    mat4->backFaceCulling = false;
    auto mat5             = StandardMaterial::New("mat5", scene);
    mat5->diffuseColor    = Color3::Yellow();
    mat5->backFaceCulling = false;
    auto superello1       = _createSuperEllipsoid(48, 0.2f, 0.2f, 1, 1, 1, scene);
    superello1->material  = mat2;
    auto superello2       = _createSuperEllipsoid(48, 0.2f, 0.2f, 1, 1, 1, scene);
    superello2->position().x += 2.5f;
    superello2->material              = mat;
    superello2->material()->wireframe = true;
    auto superello3 = _createSuperEllipsoid(48, 1.8f, 0.2f, 1, 1, 1, scene);
    superello3->position().x -= 2.5f;
    superello3->material = mat3;
    auto superello4      = _createSuperEllipsoid(48, 1.8f, 0.2f, 1, 1, 1, scene);
    superello4->position().z += 2.5f;
    superello4->material              = mat4;
    superello4->material()->wireframe = true;
    auto superello5 = _createSuperEllipsoid(48, 0.2f, 2.9f, 1, 1, 1, scene);
    superello5->position().z -= 2.5f;
    superello5->material = mat5;
  }

private:
  Vector3 _sampleSuperEllipsoid(float phi, float beta, float n1, float n2,
                                float scaleX, float scaleY, float scaleZ) const;
  Vector3 _calculateNormal(float phi, float beta, float n1, float n2,
                           float scaleX, float scaleY, float scaleZ) const;
  MeshPtr _createSuperEllipsoid(size_t samples, float n1, float n2,
                                float scalex, float scaley, float scalez,
                                Scene* scene);

}; // end of class SuperEllipsoidScene


Vector3 SuperEllipsoidScene::_sampleSuperEllipsoid(float phi, float beta,
                                                   float n1, float n2,
                                                   float scaleX, float scaleY,
                                                   float scaleZ) const
{
  Vector3 vertex;
  float cosPhi  = std::cos(phi);
  float cosBeta = std::cos(beta);
  float sinPhi  = std::sin(phi);
  float sinBeta = std::sin(beta);
  vertex.x = scaleX * Scalar::Signf(cosPhi) * std::pow(std::abs(cosPhi), n1)
             * Scalar::Signf(cosBeta) * std::pow(std::abs(cosBeta), n2);
  vertex.z = scaleY * Scalar::Signf(cosPhi) * std::pow(std::abs(cosPhi), n1)
             * Scalar::Signf(sinBeta) * std::pow(std::abs(sinBeta), n2);
  vertex.y = scaleZ * Scalar::Signf(sinPhi) * std::pow(std::abs(sinPhi), n1);
  return vertex;
}

Vector3 SuperEllipsoidScene::_calculateNormal(float phi, float beta, float n1,
                                              float n2, float scaleX,
                                              float scaleY, float scaleZ) const
{
  Vector3 normal;
  float cosPhi  = std::cos(phi);
  float cosBeta = std::cos(beta);
  float sinPhi  = std::sin(phi);
  float sinBeta = std::sin(beta);
  normal.x      = Scalar::Signf(cosPhi) * std::pow(std::abs(cosPhi), 2 - n1)
             * Scalar::Signf(cosBeta) * std::pow(std::abs(cosBeta), 2 - n2)
             / scaleX;
  normal.z = Scalar::Signf(cosPhi) * std::pow(std::abs(cosPhi), 2 - n1)
             * Scalar::Signf(sinBeta) * std::pow(std::abs(sinBeta), 2 - n2)
             / scaleY;
  normal.y
    = Scalar::Signf(sinPhi) * std::pow(std::abs(sinPhi), 2 - n1) / scaleZ;
  normal.normalize();
  return normal;
}

MeshPtr SuperEllipsoidScene::_createSuperEllipsoid(size_t samples, float n1,
                                                   float n2, float scalex,
                                                   float scaley, float scalez,
                                                   Scene* scene)
{
  auto superello = Mesh::New("superello", scene);
  float phi = 0.f, beta = 0.f;
  float dB = Math::PI2 / static_cast<float>(samples);
  float dP = Math::PI2 / static_cast<float>(samples);
  phi      = -Math::PI_2;
  std::vector<Vector3> vertices;
  std::vector<Vector3> normals;
  for (size_t j = 0; j <= samples / 2; j++) {
    beta = -Math::PI;
    for (size_t i = 0; i <= samples; i++) {
      // Triangle #1
      vertices.emplace_back(
        _sampleSuperEllipsoid(phi, beta, n1, n2, scalex, scaley, scalez));
      normals.emplace_back(
        _calculateNormal(phi, beta, n1, n2, scalex, scaley, scalez));
      vertices.emplace_back(
        _sampleSuperEllipsoid(phi + dP, beta, n1, n2, scalex, scaley, scalez));
      normals.emplace_back(
        _calculateNormal(phi + dP, beta, n1, n2, scalex, scaley, scalez));
      vertices.emplace_back(_sampleSuperEllipsoid(phi + dP, beta + dB, n1, n2,
                                                  scalex, scaley, scalez));
      normals.emplace_back(
        _calculateNormal(phi + dP, beta + dB, n1, n2, scalex, scaley, scalez));
      // Triangle #2
      vertices.emplace_back(
        _sampleSuperEllipsoid(phi, beta, n1, n2, scalex, scaley, scalez));
      normals.emplace_back(
        _calculateNormal(phi, beta, n1, n2, scalex, scaley, scalez));
      vertices.emplace_back(_sampleSuperEllipsoid(phi + dP, beta + dB, n1, n2,
                                                  scalex, scaley, scalez));
      normals.emplace_back(
        _calculateNormal(phi + dP, beta + dB, n1, n2, scalex, scaley, scalez));
      vertices.emplace_back(
        _sampleSuperEllipsoid(phi, beta + dB, n1, n2, scalex, scaley, scalez));
      normals.emplace_back(
        _calculateNormal(phi, beta + dB, n1, n2, scalex, scaley, scalez));
      beta += dB;
    }
    phi += dP;
  }
  auto shapeReturned = std::make_unique<VertexData>();
  shapeReturned->positions.clear();
  shapeReturned->normals.clear();
  shapeReturned->indices.clear();
  shapeReturned->uvs.clear();
  int32_t indice = 0;

  for (size_t i = 0; i < vertices.size(); ++i) {
    shapeReturned->indices.emplace_back(indice++);
    shapeReturned->positions.emplace_back(vertices[i].x);
    shapeReturned->positions.emplace_back(vertices[i].y);
    shapeReturned->positions.emplace_back(vertices[i].z);
    shapeReturned->normals.emplace_back(normals[i].x);
    shapeReturned->normals.emplace_back(normals[i].y);
    shapeReturned->normals.emplace_back(normals[i].z);
  }
  shapeReturned->applyToMesh(*superello);
  return superello;
}

BABYLON_REGISTER_SAMPLE("Meshes", SuperEllipsoidScene)
} // end of namespace Samples
} // end of namespace BABYLON