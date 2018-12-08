#include <babylon/samples/meshes/airplane_model_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

AirplaneModelScene::AirplaneModelScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

AirplaneModelScene::~AirplaneModelScene()
{
}

const char* AirplaneModelScene::getName()
{
  return "Airplane Model Scene";
}

void AirplaneModelScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 12,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);
  camera->setPosition(Vector3(0.f, 0.f, -200.f));

  auto light1 = HemisphericLight::New("light1", Vector3(1.f, 0.5f, 0.f), scene);
  light1->intensity = 0.7f;

  auto light2         = HemisphericLight::New("hemi", Vector3(0, 1, 0), scene);
  light2->groundColor = Color3(0.2f, 0.2f, 0.2f);
  light2->intensity   = 0.2f;

  auto spot       = SpotLight::New("spot", Vector3(25.f, 15.f, -10.f),
                             Vector3(-1.f, -0.8f, 1.f), 15, 1, scene);
  spot->diffuse   = Color3(1.f, 1.f, 1.f);
  spot->specular  = Color3(0.f, 0.f, 0.f);
  spot->intensity = 0.1f;

  const auto createSphereOptions1
    = [](float diameter, float slice, unsigned int segments, bool updatable,
         unsigned int sideOrientation) -> SphereOptions {
    SphereOptions options(diameter);
    options.setSlice(slice);
    options.segments        = segments;
    options.updatable       = updatable;
    options.sideOrientation = sideOrientation;
    return options;
  };

  const auto createSphereOptions2
    = [](float diameter, unsigned int segments, bool updatable,
         unsigned int sideOrientation) -> SphereOptions {
    SphereOptions options(diameter);
    options.segments        = segments;
    options.updatable       = updatable;
    options.sideOrientation = sideOrientation;
    return options;
  };

  const auto createCylinderOptions1
    = [](float diameter, float diameterTop, float height,
         unsigned int tessellation) -> CylinderOptions {
    CylinderOptions options(diameter);
    options.diameterTop  = diameterTop;
    options.height       = height;
    options.tessellation = tessellation;
    return options;
  };

  const auto createCylinderOptions2
    = [](float diameter, float height,
         unsigned int tessellation) -> CylinderOptions {
    CylinderOptions options(diameter);
    options.height       = height;
    options.tessellation = tessellation;
    return options;
  };

  auto radius      = 5.f;
  auto diameter    = radius + radius;
  auto fuselageXer = 7.f;
  auto fuselength  = radius * fuselageXer;
  auto noseOptions
    = createSphereOptions1(diameter, 0.5f, 16u, true, Mesh::FRONTSIDE());
  auto nose = MeshBuilder::CreateSphere("nose", noseOptions, scene);
  auto noseconeOptions
    = createCylinderOptions1(radius * 2.02f, radius, diameter, 32);
  auto nosecone
    = MeshBuilder::CreateCylinder("nosecone", noseconeOptions, scene);
  auto taperOptions
    = createSphereOptions1(radius * 2.05f, 0.45f, 16u, true, Mesh::FRONTSIDE());
  auto taper = MeshBuilder::CreateSphere("taper", taperOptions, scene);
  auto flightdeckOptions
    = createSphereOptions2(diameter, 16u, true, Mesh::FRONTSIDE());
  auto flightdeck
    = MeshBuilder::CreateSphere("flightdeck", flightdeckOptions, scene);
  auto flightdecktaperOptions
    = createCylinderOptions2(radius * 1.5f, fuselength * 2.f, 32);
  auto flightdecktaper = MeshBuilder::CreateCylinder(
    "flightdecktaper", flightdecktaperOptions, scene);
  auto fuselageOptions = createCylinderOptions2(diameter, fuselength * 2.f, 32);
  auto fuselage
    = MeshBuilder::CreateCylinder("fuselage", fuselageOptions, scene);
  auto leftwing         = Mesh::New("leftwing", scene);
  auto rightwing        = Mesh::New("leftwing", scene);
  auto vstabilizer      = Mesh::New("vstab", scene);
  auto lefthstabilizer  = Mesh::New("lefthstab", scene);
  auto righthstabilizer = Mesh::New("righthstab", scene);
  auto formWing  = _makeFormAppendage(2.f, 1.f, 40.f, 1.25f, 0.95f, 20.f, 30.f);
  auto formVStab = _makeFormAppendage(1.f, 1.f, 12.f, 6.75f, 0.95f, 20.f, 20.f);
  auto formHStab = _makeFormAppendage(1.f, 1.f, 12.f, 6.75f, 0.65f, 10.f, 10.f);

  nose->rotation().z = 133.f * Math::PI / 256.f;
  nose->scaling      = Vector3(0.5f, 0.65f, 0.5f);
  nose->position
    = Vector3((0.16f - 1.60f - fuselageXer) * radius, -0.090f * radius, 0.f);

  nosecone->rotation().z = 133.f * Math::PI / 256.f;
  nosecone->scaling      = Vector3(1.f, 0.8f, 1.f);
  nosecone->position
    = Vector3((0.16f - 0.90f - fuselageXer) * radius, -0.055f * radius, 0.f);

  flightdeck->scaling = Vector3(2.f, 1.f, 0.8f);
  flightdeck->position
    = Vector3((1.04f - fuselageXer) * radius, 0.4f * radius, 0.f);

  flightdecktaper->position
    = Vector3(0.2f * radius * fuselageXer, 0.36f * radius, 0.f);
  flightdecktaper->rotation().z
    = (128.f - (16.f / fuselageXer)) * Math::PI / 256.f;
  flightdecktaper->scaling = Vector3(1.f, 1.f, 1.f);

  fuselage->rotation().z = Math::PI_2;
  fuselage->scaling      = Vector3(1.f, 1.f, 1.f);

  taper->rotation().z = 0.f - 7.f * Math::PI / 16.f;
  taper->scaling      = Vector3(1.f, 4.f, 1.f);
  taper->position
    = Vector3((fuselageXer - 0.85f) * radius, 0.f - 0.14f * radius, 0.f);

  leftwing                = formWing(leftwing, radius, fuselageXer, false);
  leftwing->rotation().x  = Math::PI_2;
  rightwing               = formWing(rightwing, radius, fuselageXer, true);
  rightwing->rotation().x = Math::PI_2;
  vstabilizer             = formVStab(vstabilizer, radius, fuselageXer, true);
  lefthstabilizer = formHStab(lefthstabilizer, radius, fuselageXer, false);
  lefthstabilizer->rotation().x = Math::PI_2;
  righthstabilizer = formHStab(righthstabilizer, radius, fuselageXer, true);
  righthstabilizer->rotation().x = Math::PI_2;

  _reBake(nose);
  _reBake(nosecone);
  _reBake(taper);
  _reBake(flightdeck);
  _reBake(fuselage);

  auto am1             = StandardMaterial::New("am1", scene);
  am1->diffuseColor    = Color3(0.35f, 0.35f, 0.35f);
  am1->specularColor   = Color3(0.15f, 0.15f, 0.15f);
  am1->ambientColor    = Color3(0.15f, 0.15f, 0.15f);
  am1->emissiveColor   = Color3(0.30f, 0.65f, 0.85f);
  am1->alpha           = 1.f;
  am1->backFaceCulling = false;

  auto airplane = Mesh::MergeMeshes(
    {nose, nosecone, flightdeck, flightdecktaper, fuselage, taper, leftwing,
     rightwing, vstabilizer, lefthstabilizer, righthstabilizer},
    true);
  airplane->rotation().y = Math::PI_2;
  _reBake(airplane);
  airplane->material = am1;
}

void AirplaneModelScene::_reBake(const MeshPtr& mesh)
{
  mesh->bakeCurrentTransformIntoVertices();
  mesh->updateFacetData();
  auto myVertexData = VertexData::ExtractFromMesh(mesh.get(), true);
  myVertexData->transform(mesh->getWorldMatrix());
  myVertexData->applyToMesh(*mesh);
}

AirplaneModelScene::MakeWingFunction AirplaneModelScene::_makeFormAppendage(
  float frontBaseV, float backBaseV, float ySize, float xOrigin, float yOrigin,
  float baseXsize, float xSweepback)
{

  const auto makeWing
    = [=](const MeshPtr& mesh, float radius, float fuselageXer, bool invertY) {
        auto yMultiplier      = invertY == true ? -1.f : 1.f;
        auto frontBaseVtop    = frontBaseV;
        auto frontBaseVbottom = 0.f - frontBaseV;
        auto backBaseVtop     = backBaseV;
        auto backBaseVbottom  = 0.f - backBaseV;
        auto wingYsize        = ySize * yMultiplier;
        auto wingXorigin      = xOrigin * fuselageXer;
        auto wingYorigin
          = (0.f - yOrigin) * radius * yMultiplier; // radius of fuselage
        auto wingtipXsize   = 3.f;
        auto wingbaseXsize  = baseXsize;
        auto wingXsweepback = xSweepback;

        auto a = wingXorigin - wingbaseXsize + wingXsweepback;
        auto b = wingYorigin - wingYsize;
        auto c = 0.f;
        auto d = a + wingtipXsize;
        auto e = b;
        auto f = c;
        auto g = wingXorigin;
        auto h = wingYorigin;
        auto i = backBaseVbottom;
        auto j = wingXorigin - wingbaseXsize;
        auto k = wingYorigin;
        auto l = frontBaseVbottom;
        auto m = wingXorigin - wingbaseXsize;
        auto n = wingYorigin;
        auto o = frontBaseVtop;
        auto p = wingXorigin;
        auto q = wingYorigin;
        auto r = backBaseVtop;

        mesh->position = Vector3::Zero();

        Float32Array positions{
          a,
          b,
          c, // 0
          d,
          e,
          f, // 1
          g,
          h,
          i, // 2
          j,
          k,
          l, // 3
          m,
          n,
          o, // 4
          p,
          q,
          r, // 5
          (a + j + m) / 3.f,
          (b + k + n) / 3.f,
          (c + l + o) / 3.f, // 6
          (d + g + p) / 3.f,
          (e + h + q) / 3.f,
          (f + i + r) / 3.f, // 7
        };

        // connect the triangle dots ... counter clockwise
        IndicesArray indices{
          0, 1, 4, // connect vertices 0->1->4
          1, 5, 4, // connect vertices 1->5->4
          4, 3, 6, // connect vertices 4->3->6
          4, 6, 0, // connect vertices 4->6->0
          3, 0, 6, // connect vertices 3->0->6
          1, 7, 5, // connect vertices 1->7->5
          2, 5, 7, // connect vertices 2->5->7
          2, 7, 1, // connect vertices 2->7->1
          2, 4, 5, // connect vertices 2->4->5
          2, 3, 4, // connect vertices 2->3->4
          3, 1, 0, // connect vertices 3->1->0
          3, 2, 1, // connect vertices 3->2->1
        };

        // light bounce directions
        Float32Array normals{
          0, 1, 0, // 0
          0, 1, 0, // 1
          0, 1, 0, // 2
          0, 1, 0, // 3
          0, 1, 0, // 4
          0, 1, 0, // 5
          0, 0, 1, // 6
          0, 0, 1, // 7
        };

        // texture skinning directions, maybe.
        Float32Array uvs{
          1.f, 0.f, // 0
          1.f, 0.f, // 1
          1.f, 0.f, // 2
          1.f, 0.f, // 3
          1.f, 0.f, // 4
          1.f, 0.f, // 5
          1.f, 0.f, // 6
          1.f, 0.f, // 7
        };

        // Make a mesh shaper device.
        auto vertexData = std::make_unique<VertexData>();

        // stuff its buffers with your stuff
        vertexData->positions = positions;
        vertexData->indices   = indices;
        vertexData->normals   = normals;
        vertexData->uvs       = uvs;

        // Use the vertexData object.. to shape-ify blankmesh
        vertexData->applyToMesh(*mesh, true);
        return mesh;
      };
  return makeWing;
}

} // end of namespace Samples
} // end of namespace BABYLON
