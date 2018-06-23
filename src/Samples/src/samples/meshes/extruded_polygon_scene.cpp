#include <babylon/samples/meshes/extruded_polygon_scene.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/vector4.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

ExtrudedPolygonScene::ExtrudedPolygonScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ExtrudedPolygonScene::~ExtrudedPolygonScene()
{
}

const char* ExtrudedPolygonScene::getName()
{
  return "Extruded Polygon Scene";
}

void ExtrudedPolygonScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Camera
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI / 3.f,
                                     12.f, Vector3::Zero(), scene);
  camera->alpha += 0.9f;
  camera->beta += -0.01f;

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("light1", Vector3(10.f, 10.f, 0.f), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.95f;

  /** Wall **/
  // Polygon shape in XoZ plane
  std::vector<Vector3> wallData{
    Vector3(-3.5f, 0.f, -2.5f), //
    Vector3(0.5f, 0.f, -2.5f),  //
    Vector3(0.5f, 0.f, -0.5f),  //
    Vector3(1.5f, 0.f, -0.5f),  //
    Vector3(1.5f, 0.f, -2.5f),  //
    Vector3(3.5f, 0.f, -2.5f),  //
    Vector3(3.5f, 0.f, 2.5f),   //
    Vector3(-3.5f, 0.f, 2.5f)   //
  };

  // Holes in XoZ plane
  std::vector<std::vector<Vector3>> holes{// holes[0]
                                          {
                                            Vector3(-2.5f, 0.f, -2.f),  //
                                            Vector3(-0.5f, 0.f, -2.f),  //
                                            Vector3(-0.5f, 0.f, -0.5f), //
                                            Vector3(-2.5f, 0.f, -0.5f)  //
                                          },
                                          // holes[1]
                                          {
                                            Vector3(-3.f, 0.f, 0.75f),  //
                                            Vector3(-1.5f, 0.f, 0.75f), //
                                            Vector3(-1.5f, 0.f, 1.75f), //
                                            Vector3(-3.f, 0.f, 1.75f)   //
                                          },
                                          // holes[2]
                                          {
                                            Vector3(0.f, 0.f, 0.75f),  //
                                            Vector3(1.5f, 0.f, 0.75f), //
                                            Vector3(1.5f, 0.f, 1.75f), //
                                            Vector3(0.f, 0.f, 1.75f)   //
                                          },
                                          // holes[3]
                                          {
                                            Vector3(2.f, 0.f, 0.75f), //
                                            Vector3(3.f, 0.f, 0.75f), //
                                            Vector3(3.f, 0.f, 1.75f), //
                                            Vector3(2.f, 0.f, 1.75f)  //
                                          }};

  // faceUVs
  std::vector<Vector4> faceUV{
    Vector4(0.f, 0.f, 7.f / 15.f, 1.f),        //
    Vector4(14.f / 15.f, 0.f, 1.f, 1.f),       //
    Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f) //
  };

  // Wall
  PolygonOptions wallOptions;
  wallOptions.shape  = wallData;
  wallOptions.depth  = 0.15f;
  wallOptions.holes  = holes;
  wallOptions.faceUV = faceUV;
  auto wall          = MeshBuilder::ExtrudePolygon("wall", wallOptions, scene);
  wall->rotation().x = -Math::PI_2;

  auto wallmat          = StandardMaterial::New("wallmaterial", scene);
  wallmat->diffuseColor = Color3::FromInts(203, 65, 84);
  wall->material        = wallmat;

  /** Stairs **/
  auto stairsDepth     = 2.f;
  auto stairsHeight    = 2.5f;
  auto stairsThickness = 0.05f;
  auto nBStairs        = 12u;
  std::vector<Vector3> stairs;
  auto x = 0.f;
  auto z = 0.f;

  // -- Up --
  stairs.emplace_back(Vector3(x, 0.f, z));
  z += stairsHeight / nBStairs - stairsThickness;
  stairs.emplace_back(Vector3(x, 0.f, z));
  for (unsigned int i = 0; i < nBStairs; ++i) {
    x += stairsDepth / nBStairs;
    stairs.emplace_back(Vector3(x, 0.f, z));
    z += stairsHeight / nBStairs;
    stairs.emplace_back(Vector3(x, 0.f, z));
  }
  x += stairsDepth / nBStairs - stairsThickness;
  stairs.emplace_back(Vector3(x, 0.f, z));
  z += stairsThickness;
  stairs.emplace_back(Vector3(x, 0.f, z));

  // -- Down --
  for (unsigned int i = 0; i <= nBStairs; ++i) {
    x -= stairsDepth / nBStairs;
    stairs.emplace_back(Vector3(x, 0.f, z));
    z -= stairsHeight / nBStairs;
    stairs.emplace_back(Vector3(x, 0.f, z));
  }

  std::vector<Color4> faceColors{
    Color4(0.f, 0.f, 0.f, 1.f), //
    Color4(1.f, 0.f, 0.f, 1.f), //
    Color4(0.f, 0.f, 0.f, 1.f)  //
  };

  auto stairsWidth = 1.5f;
  PolygonOptions stairsOptions;
  stairsOptions.shape      = stairs;
  stairsOptions.depth      = stairsWidth;
  stairsOptions.faceColors = faceColors;
  auto stairCase = MeshBuilder::ExtrudePolygon("stairs", stairsOptions, scene);
  stairCase->position().x = 3.5f;
  stairCase->position().y = -2.5f;
  stairCase->position().z = 1.f;
  stairCase->rotation().x = -Math::PI_2;
  stairCase->rotation().y = -Math::PI_2;

  /** Floor **/
  std::vector<Vector3> floorData{
    Vector3(-3.5f, 0.f, 0.f),  //
    Vector3(3.5f, 0.f, 0.f),   //
    Vector3(3.5f, 0.f, 1.5f),  //
    Vector3(2.f, 0.f, 1.5f),   //
    Vector3(2.f, 0.f, 2.85f),  //
    Vector3(3.5f, 0.f, 2.85f), //
    Vector3(3.5f, 0.f, 4.f),   //
    Vector3(-3.5f, 0.f, 4.f)   //
  };

  std::vector<Vector4> floorFaceUV{
    Vector4(0.f, 0.f, 0.5f, 1.f), //
    Vector4(0.5f, 0.f, 1.f, 1.f)  //
  };

  PolygonOptions floorOptions;
  floorOptions.shape  = floorData;
  floorOptions.depth  = 0.1f;
  floorOptions.faceUV = floorFaceUV;
  auto floor = MeshBuilder::ExtrudePolygon("floor", floorOptions, scene);
  floor->position().y = 0.21f;
  floor->position().z = 0.15f;

  auto floormat          = StandardMaterial::New("floormaterial", scene);
  floormat->diffuseColor = Color3::FromInts(168, 119, 90);
  floor->material        = floormat;
}

} // end of namespace Samples
} // end of namespace BABYLON
