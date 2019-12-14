#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene_with_hud.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/vector4.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/samples/babylon_register_sample.h>
#include <imgui.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Extrude Polygon Scene. Example demonstrating how to use MeshBuilder to generate geometry
 * from extruded data.
 * @see https://www.babylonjs-playground.com/#TFLTJJ#0
 * @see https://doc.babylonjs.com/api/classes/babylon.meshbuilder#extrudepolygon
 */
class ExtrudePolygonScene : public IRenderableSceneWithHud {

public:
  ExtrudePolygonScene(ICanvas* iCanvas)
      : IRenderableSceneWithHud(iCanvas)
      // Roof
      , _roof{nullptr}
      , _ceiling{nullptr} // Front
      , _frontWall{nullptr}
      , _windowFBL{nullptr}
      , _windowFBR{nullptr}
      , _windowFTL{nullptr}
      , _windowFTR{nullptr}
      , _windowFTM{nullptr}
      , _frontDoor{nullptr} // Back
      , _rearWallnb1{nullptr}
      , _rearWallnb2{nullptr}
      , _windowRBL{nullptr}
      , _windowRBR{nullptr}
      , _windowRTL{nullptr}
      , _windowRTR{nullptr}
      , _windowR1BL{nullptr}
      , _windowR1TL{nullptr}
      , _windowR1TR{nullptr} // Left Side
      , _sideWallnb1{nullptr}
      , _sideWallnb3{nullptr}
      , _backDoor{nullptr} // Right Side
      , _sideWallnb2{nullptr}
  {
  }

  ~ExtrudePolygonScene() override = default;

  const char* getName() override
  {
    return "Extrude Polygon Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {

    // Camera
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI / 3.f, 25.f, Vector3::Zero(), scene);
    camera->target = Vector3(0, 0.f, 4.5f);
    camera->attachControl(canvas, true);

    auto light       = HemisphericLight::New("hemiLight", Vector3(10, 10, 0), scene);
    light->intensity = 0.98f;

    auto wallmat            = StandardMaterial::New("wallmaterial", scene);
    wallmat->diffuseTexture = Texture::New("textures/wallMaterial.jpg", scene);

    auto innerwallmat          = StandardMaterial::New("innerwallmaterial", scene);
    innerwallmat->diffuseColor = Color3(240.f / 255.f, 223.f / 255.f, 203.f / 255.f);

    // Front wall polygon shape in XoZ plane
    std::vector<Vector3> frontWallData{
      Vector3(-5.5f, 0.f, -3.f),   //
      Vector3(-0.5f, 0.f, -3.f),   //
      Vector3(-0.5f, 0.f, -0.75f), //
      Vector3(0.5f, 0.f, -0.75f),  //
      Vector3(0.5f, 0.f, -3.f),    //
      Vector3(5.5f, 0.f, -3.f),    //
      Vector3(5.5f, 0.f, 3.f),     //
      Vector3(-5.5f, 0.f, 3.f)     //
    };

    // Holes in XoZ plane
    std::vector<std::vector<Vector3>> frontWindowHoles{// frontWindowHoles[0]
                                                       {
                                                         Vector3(-4.78f, 0.f, -2.3f), //
                                                         Vector3(-1.58f, 0.f, -2.3f), //
                                                         Vector3(-1.58f, 0.f, -0.3f), //
                                                         Vector3(-4.78f, 0.f, -0.3f)  //
                                                       },
                                                       // frontWindowHoles[1]
                                                       {
                                                         Vector3(1.58f, 0.f, -2.3f), //
                                                         Vector3(4.78f, 0.f, -2.3f), //
                                                         Vector3(4.78f, 0.f, -0.3f), //
                                                         Vector3(1.58f, 0.f, -0.3f)  //
                                                       },
                                                       // frontWindowHoles[2]
                                                       {
                                                         Vector3(-4.03f, 0.f, 0.75f), //
                                                         Vector3(-2.13f, 0.f, 0.75f), //
                                                         Vector3(-2.13f, 0.f, 2.55f), //
                                                         Vector3(-4.03f, 0.f, 2.55f)  //
                                                       },
                                                       // frontWindowHoles[3]
                                                       {
                                                         Vector3(-0.65f, 0.f, 0.75f), //
                                                         Vector3(0.65f, 0.f, 0.75f),  //
                                                         Vector3(0.65f, 0.f, 2.55f),  //
                                                         Vector3(-0.65f, 0.f, 2.55f)  //
                                                       },
                                                       // frontWindowHoles[4]
                                                       {
                                                         Vector3(2.13f, 0.f, 0.75f), //
                                                         Vector3(4.03f, 0.f, 0.75f), //
                                                         Vector3(4.03f, 0.f, 2.55f), //
                                                         Vector3(2.13f, 0.f, 2.55f)  //
                                                       }};

    // Front wall faceUVs
    std::vector<Vector4> frontWallFaceUV{
      Vector4(0.f, 0.f, 7.f / 15.f, 1.f),        //
      Vector4(14.f / 15.f, 0.f, 1.f, 1.f),       //
      Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f) //
    };

    // Front Wall
    PolygonOptions frontWallOptions;
    frontWallOptions.shape   = frontWallData;
    frontWallOptions.depth   = 0.15f;
    frontWallOptions.holes   = frontWindowHoles;
    frontWallOptions.faceUV  = frontWallFaceUV;
    _frontWall               = MeshBuilder::ExtrudePolygon("wall", frontWallOptions, scene);
    _frontWall->rotation().x = -Math::PI_2;
    _frontWall->material     = wallmat;

    // Real wall polygon shape in XoZ plane
    std::vector<Vector3> rearWallnb1Data{
      Vector3(1.4f, 0.f, -3.f), //
      Vector3(5.5f, 0.f, -3.f), //
      Vector3(5.5f, 0.f, 3.f),  //
      Vector3(1.4f, 0.f, 3.f)   //
    };

    // Holes in XoZ plane
    std::vector<std::vector<Vector3>> rear1WindowHoles{// rear1WindowHoles[0]
                                                       {
                                                         Vector3(3.7f, 0.f, -1.8f), //
                                                         Vector3(4.5f, 0.f, -1.8f), //
                                                         Vector3(4.5f, 0.f, -0.3f), //
                                                         Vector3(3.7f, 0.f, -0.3f)  //
                                                       },
                                                       // rear1WindowHoles[1]
                                                       {
                                                         Vector3(1.9f, 0.f, 0.75f), //
                                                         Vector3(2.7f, 0.f, 0.75f), //
                                                         Vector3(2.7f, 0.f, 2.55f), //
                                                         Vector3(1.9f, 0.f, 2.55f)  //
                                                       },
                                                       // rear1WindowHoles[2]
                                                       {
                                                         Vector3(4.2f, 0.f, 0.75f), //
                                                         Vector3(5.f, 0.f, 0.75f),  //
                                                         Vector3(5.f, 0.f, 2.55f),  //
                                                         Vector3(4.2f, 0.f, 2.55f)  //
                                                       }};

    // Rear Wall faceUVs
    std::vector<Vector4> rearFaceUV{
      Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f), //
      Vector4(14.f / 15.f, 0.f, 1.f, 1.f),        //
      Vector4(0.f, 0.f, 7.f / 15.f, 1.f)          //
    };

    // Rear wall 1
    PolygonOptions rearWallnb1Options;
    rearWallnb1Options.shape  = rearWallnb1Data;
    rearWallnb1Options.depth  = 0.1f;
    rearWallnb1Options.holes  = rear1WindowHoles;
    rearWallnb1Options.faceUV = rearFaceUV;
    _rearWallnb1 = MeshBuilder::ExtrudePolygon("rearWallnb1", rearWallnb1Options, scene);
    _rearWallnb1->rotation().x = -Math::PI_2;
    _rearWallnb1->position().z = 6.15f;
    _rearWallnb1->material     = wallmat;

    // Real wall nb2 polygon shape in XoZ plane
    std::vector<Vector3> rearWallnb2Data{
      Vector3(-5.6f, 0.f, -3.f),   //
      Vector3(1.45f, 0.f, -3.f),   //
      Vector3(1.45f, 0.f, 3.f),    //
      Vector3(-2.075f, 0.f, 5.5f), //
      Vector3(-5.6f, 0.f, 3.f)     //
    };

    // Holes in XoZ plane
    std::vector<std::vector<Vector3>> rear2WindowHoles{// rear2WindowHoles[0]
                                                       {
                                                         Vector3(-5.f, 0.f, -1.8f),   //
                                                         Vector3(-1.85f, 0.f, -1.8f), //
                                                         Vector3(-1.85f, 0.f, -0.3f), //
                                                         Vector3(-5.f, 0.f, -0.3f)    //
                                                       },
                                                       // rear2WindowHoles[1]
                                                       {
                                                         Vector3(-0.8f, 0.f, -1.8f), //
                                                         Vector3(0.9f, 0.f, -1.8f),  //
                                                         Vector3(0.9f, 0.f, -0.3f),  //
                                                         Vector3(-0.8f, 0.f, -0.3f)  //
                                                       },
                                                       // rear2WindowHoles[2]
                                                       {
                                                         Vector3(-5.f, 0.f, 0.75f),   //
                                                         Vector3(-1.85f, 0.f, 0.75f), //
                                                         Vector3(-1.85f, 0.f, 2.55f), //
                                                         Vector3(-5.f, 0.f, 2.55f)    //
                                                       },
                                                       // rear2WindowHoles[3]
                                                       {
                                                         Vector3(-0.6f, 0.f, 1.75f), //
                                                         Vector3(0.7f, 0.f, 1.75f),  //
                                                         Vector3(0.7f, 0.f, 2.55f),  //
                                                         Vector3(-0.6f, 0.f, 2.55f)  //
                                                       }};

    // Rear wall 2
    PolygonOptions rearWallnb2Options;
    rearWallnb2Options.shape  = rearWallnb2Data;
    rearWallnb2Options.depth  = 0.1f;
    rearWallnb2Options.holes  = rear2WindowHoles;
    rearWallnb2Options.faceUV = rearFaceUV;
    _rearWallnb2 = MeshBuilder::ExtrudePolygon("rearWallnb2", rearWallnb2Options, scene);
    _rearWallnb2->rotation().x = -Math::PI_2;
    _rearWallnb2->position().z = 9.15f;
    _rearWallnb2->material     = wallmat;

    // Side wall 1 polygon shape in XoZ plane
    std::vector<Vector3> sideWallnb1Data{
      Vector3(-3.15f, 0.f, -3.f), //
      Vector3(3.1f, 0.f, -3.f),   //
      Vector3(3.1f, 0.f, 3.f),    //
      Vector3(0.f, 0.f, 5.5f),    //
      Vector3(-3.15f, 0.f, 3.f)   //
    };

    // Side wall 1 faceUVs
    std::vector<Vector4> side1FaceUV{
      Vector4(0.f, 0.f, 7.f / 15.f, 1.f),        //
      Vector4(14.f / 15.f, 0.f, 1.f, 1.f),       //
      Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f) //
    };

    // Side wall 1
    PolygonOptions sideWallnb1Options;
    sideWallnb1Options.shape  = sideWallnb1Data;
    sideWallnb1Options.depth  = 0.1f;
    sideWallnb1Options.faceUV = side1FaceUV;
    _sideWallnb1 = MeshBuilder::ExtrudePolygon("sideWallnb1", sideWallnb1Options, scene);
    _sideWallnb1->rotation().z = -Math::PI_2;
    _sideWallnb1->rotation().x = -Math::PI_2;
    _sideWallnb1->position().x = 5.6f;
    _sideWallnb1->position().z = 3.15f;
    _sideWallnb1->material     = wallmat;

    // Side wall nb2 polygon shape in XoZ plane
    std::vector<Vector3> sideWallnb2Data{
      Vector3(-3.15f, 0.f, -3.f), //
      Vector3(6.f, 0.f, -3.f),    //
      Vector3(6.f, 0.f, 3.f),     //
      Vector3(3.1f, 0.f, 3.f),    //
      Vector3(0.f, 0.f, 5.5f),    //
      Vector3(-3.15f, 0.f, 3.f)   //
    };

    // Side wall 2 faceUVs
    std::vector<Vector4> side2FaceUV{
      Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f), //
      Vector4(14.f / 15.f, 0.f, 1.f, 1.f),        //
      Vector4(0.f, 0.f, 7.f / 15.f, 1.f)          //
    };

    // Side wall 2
    PolygonOptions sideWallnb2Options;
    sideWallnb2Options.shape  = sideWallnb2Data;
    sideWallnb2Options.depth  = 0.1f;
    sideWallnb2Options.faceUV = side2FaceUV;
    _sideWallnb2 = MeshBuilder::ExtrudePolygon("sideWallnb2", sideWallnb2Options, scene);
    _sideWallnb2->rotation().z = -Math::PI_2;
    _sideWallnb2->rotation().x = -Math::PI_2;
    _sideWallnb2->position().x = -5.5f;
    _sideWallnb2->position().z = 3.15f;
    _sideWallnb2->material     = wallmat;

    // Side wall 3
    std::vector<Vector3> sideWallnb3Data{
      Vector3(3.1f, 0.f, -3.f),   //
      Vector3(4.5f, 0.f, -3.f),   //
      Vector3(4.5f, 0.f, -0.75f), //
      Vector3(5.5f, 0.f, -0.75f), //
      Vector3(5.5f, 0.f, -3.f),   //
      Vector3(6.f, 0.f, -3.f),    //
      Vector3(6.f, 0.f, 3.f),     //
      Vector3(3.1f, 0.f, 3.f)     //
    };

    // Side wall 3 faceUVs
    std::vector<Vector4> side3FaceUV{
      Vector4(0.f, 0.f, 7.f / 15.f, 1.f),        //
      Vector4(14.f / 15.f, 0.f, 1.f, 1.f),       //
      Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f) //
    };

    // Side wall 3
    PolygonOptions sideWallnb3Options;
    sideWallnb3Options.shape  = sideWallnb3Data;
    sideWallnb3Options.depth  = 0.1f;
    sideWallnb3Options.faceUV = side3FaceUV;
    _sideWallnb3 = MeshBuilder::ExtrudePolygon("sideWallnb3", sideWallnb3Options, scene);
    _sideWallnb3->rotation().z = -Math::PI_2;
    _sideWallnb3->rotation().x = -Math::PI_2;
    _sideWallnb3->position().x = 1.45f;
    _sideWallnb3->position().z = 3.15f;
    _sideWallnb3->material     = wallmat;

    // Roof material
    auto roofmat            = StandardMaterial::New("roofmat", scene);
    roofmat->diffuseTexture = Texture::New("textures/roofMaterial.jpg", scene);

    // Roof 1 polygon shape in XoZ plane
    std::vector<Vector3> roof1Data{
      Vector3(-0.05f, 0.f, 0.f), //
      Vector3(0.1f, 0.f, 0.f),   //
      Vector3(3.3f, 0.f, 2.65f), //
      Vector3(6.5f, 0.f, 0.f),   //
      Vector3(6.6f, 0.f, 0.f),   //
      Vector3(3.3f, 0.f, 2.8f)   //
    };

    // Roof 1
    PolygonOptions roof1Options;
    roof1Options.shape  = roof1Data;
    roof1Options.depth  = 11.5f;
    auto roof1          = MeshBuilder::ExtrudePolygon("roof1", roof1Options, scene);
    roof1->rotation().z = -Math::PI_2;
    roof1->rotation().x = -Math::PI_2;
    roof1->position().x = 5.7f;
    roof1->position().y = 2.9f;
    roof1->position().z = -0.15f;
    roof1->material     = roofmat;

    // Roof 2 polygon shape in XoZ plane
    std::vector<Vector3> roof2Data{
      Vector3(0.f, 0.f, 0.f),     //
      Vector3(0.142f, 0.f, 0.f),  //
      Vector3(3.834f, 0.f, 2.6f), //
      Vector3(7.476f, 0.f, 0.f),  //
      Vector3(7.618f, 0.f, 0.f),  //
      Vector3(3.834f, 0.f, 2.7f)  //
    };

    // Roof 2
    PolygonOptions roof2Options;
    roof2Options.shape  = roof2Data;
    roof2Options.depth  = 3.2f;
    auto roof2          = MeshBuilder::ExtrudePolygon("roof2", roof2Options, scene);
    roof2->rotation().x = -Math::PI_2;
    roof2->position().x = -5.9f;
    roof2->position().y = 2.9f;
    roof2->position().z = 6.3f;
    roof2->material     = roofmat;

    // Roof 3 polygon shape in XoZ plane
    std::vector<Vector3> roof3Data{
      Vector3(0.3f, 0.f, 0.2f),   //
      Vector3(0.442f, 0.f, 0.2f), //
      Vector3(3.834f, 0.f, 2.6f), //
      Vector3(7.476f, 0.f, 0.f),  //
      Vector3(7.618f, 0.f, 0.f),  //
      Vector3(3.834f, 0.f, 2.7f)  //
    };

    // Roof 3
    PolygonOptions roof3Options;
    roof3Options.shape  = roof3Data;
    roof3Options.depth  = 3.2f;
    auto roof3          = MeshBuilder::ExtrudePolygon("roof3", roof3Options, scene);
    roof3->rotation().x = -Math::PI_2;
    roof3->position().x = -5.9f;
    roof3->position().y = 2.9f;
    roof3->position().z = 3.1f;
    roof3->material     = roofmat;

    // Roof
    _roof = Mesh::MergeMeshes({roof1, roof2, roof3}, true);

    // Staircase
    auto stairsDepth     = 2.f;
    auto stairsHeight    = 3.f;
    auto stairsThickness = 0.05f;
    auto nBStairs        = 12;
    std::vector<Vector3> stairs;
    auto x = 0.f;
    auto z = 0.f;

    // Up
    stairs.emplace_back(Vector3(x, 0.f, z));
    z += stairsHeight / nBStairs - stairsThickness;
    stairs.emplace_back(Vector3(x, 0.f, z));
    for (int i = 0; i < nBStairs; ++i) {
      x += stairsDepth / nBStairs;
      stairs.emplace_back(Vector3(x, 0.f, z));
      z += stairsHeight / nBStairs;
      stairs.emplace_back(Vector3(x, 0.f, z));
    }
    x += stairsDepth / nBStairs - stairsThickness;
    stairs.emplace_back(Vector3(x, 0.f, z));

    // Down
    for (int i = 0; i <= nBStairs; i++) {
      x -= stairsDepth / nBStairs;
      stairs.emplace_back(Vector3(x, 0.f, z));
      z -= stairsHeight / nBStairs;
      stairs.emplace_back(Vector3(x, 0.f, z));
    }

    std::vector<Color4> faceColors{
      Color4(0.f, 0.f, 0.f, 1.f),                              //
      Color4(190.f / 255.f, 139.f / 255.f, 94.f / 255.f, 1.f), //
      Color4(0.f, 0.f, 0.f, 1.f),                              //
    };

    // Stairs
    auto stairsWidth = 1.f;
    PolygonOptions stairsOptions;
    stairsOptions.shape      = stairs;
    stairsOptions.depth      = stairsWidth;
    stairsOptions.faceColors = faceColors;
    auto stairCase           = MeshBuilder::ExtrudePolygon("stairs", stairsOptions, scene);
    stairCase->position().x  = 1.37f;
    stairCase->position().y  = -3.f;
    stairCase->position().z  = 2.51f;
    stairCase->rotation().x  = -Math::PI_2;
    stairCase->rotation().y  = -Math::PI_2;

    // Floor material
    auto floormat            = StandardMaterial::New("floormaterial", scene);
    floormat->diffuseTexture = Texture::New("textures/floorMaterial.jpg", scene);

    // Floor polygon shape in XoZ plane
    std::vector<Vector3> floorData{
      Vector3(-5.5f, 0.f, 0),  //
      Vector3(5.5f, 0.f, 0),   //
      Vector3(5.5f, 0.f, 6),   //
      Vector3(1.345f, 0.f, 6), //
      Vector3(1.345f, 0.f, 9), //
      Vector3(-5.5f, 0.f, 9)   //
    };

    // Stair space
    std::vector<std::vector<Vector3>> stairSpace{// stairSpace[0]
                                                 {
                                                   Vector3(0.27f, 0.f, 2.5f), //
                                                   Vector3(0.27f, 0.f, 4.5f), //
                                                   Vector3(1.37f, 0.f, 4.5f), //
                                                   Vector3(1.37f, 0.f, 2.5f), //
                                                 }};

    // Rear Wall faceUVs
    std::vector<Vector4> floorFaceUV{
      Vector4(0.f, 0.f, 0.5f, 1.f), //
      Vector4(0.f, 0.f, 0.f, 0.f),  //
      Vector4(0.5f, 0.f, 1.f, 1.f)  //
    };

    // Floor
    PolygonOptions floorOptions;
    floorOptions.shape  = floorData;
    floorOptions.holes  = stairSpace;
    floorOptions.depth  = 0.1f;
    floorOptions.faceUV = floorFaceUV;
    auto floor          = MeshBuilder::ExtrudePolygon("floor", floorOptions, scene);
    floor->position().y = 0.21f;
    floor->position().z = 0.15f;
    floor->material     = floormat;

    // Ground floor polygon shape in XoZ plane
    std::vector<Vector3> groundFloorData{
      Vector3(-5.6f, 0.f, -0.1f), //
      Vector3(5.6f, 0.f, -0.1f),  //
      Vector3(5.6f, 0.f, 6.1f),   //
      Vector3(1.36f, 0.f, 6.1f),  //
      Vector3(1.36f, 0.f, 9.1f),  //
      Vector3(-5.6f, 0.f, 9.1f)   //
    };

    // Ground floor faceUVs
    std::vector<Vector4> groundFloorFaceUV{
      Vector4(0.f, 0.f, 0.5f, 1.f), //
      Vector4(0.f, 0.f, 0.f, 0.f),  //
      Vector4(0.5f, 0.f, 1.f, 1.f)  //
    };

    // Ground floor
    PolygonOptions groundFloorOptions;
    groundFloorOptions.shape  = groundFloorData;
    groundFloorOptions.depth  = 0.04f;
    groundFloorOptions.faceUV = groundFloorFaceUV;
    auto groundFloor = MeshBuilder::ExtrudePolygon("groundFloor", groundFloorOptions, scene);
    groundFloor->position().y = -3.f;
    groundFloor->position().z = 0.15f;
    groundFloor->material     = floormat;

    // Ceiling polygon shape in XoZ plane
    std::vector<Vector3> ceilingData{
      Vector3(-5.5f, 0.f, 0.f),  //
      Vector3(5.5f, 0.f, 0.f),   //
      Vector3(5.5f, 0.f, 6.f),   //
      Vector3(1.345f, 0.f, 6.f), //
      Vector3(1.345f, 0.f, 9.f), //
      Vector3(-5.5f, 0.f, 9.f)   //
    };

    // Ceiling
    PolygonOptions ceilingOptions;
    ceilingOptions.shape   = ceilingData;
    ceilingOptions.depth   = 0.1f;
    _ceiling               = MeshBuilder::ExtrudePolygon("ceiling", ceilingOptions, scene);
    _ceiling->position().y = 2.8f;
    _ceiling->position().z = 0.15f;
    _ceiling->material     = innerwallmat;

    // Inner wall 1 polygon shape in XoZ plane
    std::vector<Vector3> innerWallnb1Data{
      Vector3(-3.f, 0.f, 0.6f), //
      Vector3(-3.f, 0.f, 0.f),  //
      Vector3(3.f, 0.f, 0.f),   //
      Vector3(3.f, 0.f, 6.1f),  //
      Vector3(-3.f, 0.f, 6.1f), //
      Vector3(-3.f, 0.f, 1.6f), //
      Vector3(-1.f, 0.f, 1.6f), //
      Vector3(-1.f, 0.f, 0.6f), //
    };

    // Door space 1
    std::vector<std::vector<Vector3>> doorSpace1{// doorSpace1[0]
                                                 {
                                                   Vector3(0.1f, 0.f, 1.6f), //
                                                   Vector3(0.1f, 0.f, 0.6f), //
                                                   Vector3(2.f, 0.f, 0.6f),  //
                                                   Vector3(2.f, 0.f, 1.6f)   //
                                                 }};

    // Inner wall 1
    PolygonOptions innerWallnb1Options;
    innerWallnb1Options.shape = innerWallnb1Data;
    innerWallnb1Options.holes = doorSpace1;
    innerWallnb1Options.depth = 0.1f;
    auto innerWallnb1 = MeshBuilder::ExtrudePolygon("innerWallnb1", innerWallnb1Options, scene);
    innerWallnb1->rotation().z = Math::PI_2;
    innerWallnb1->position().x = 1.35f;
    innerWallnb1->position().z = 0.15f;
    innerWallnb1->material     = innerwallmat;

    // Inner wall 1 polygon shape in XoZ plane
    std::vector<Vector3> innerWallnb2Data{
      Vector3(-3.f, 0.f, 0.f),  //
      Vector3(3.f, 0.f, 0.f),   //
      Vector3(3.f, 0.f, 9.f),   //
      Vector3(-3.f, 0.f, 9.f),  //
      Vector3(-3.f, 0.f, 7.6f), //
      Vector3(-1.f, 0.f, 7.6f), //
      Vector3(-1.f, 0.f, 6.6f), //
      Vector3(-3.f, 0.f, 6.6f), //
      Vector3(-3.f, 0.f, 1.6f), //
      Vector3(-1.f, 0.f, 1.6f), //
      Vector3(-1.f, 0.f, 0.6f), //
      Vector3(-3.f, 0.f, 0.6f)  //
    };

    // Door space 2
    std::vector<std::vector<Vector3>> doorSpace2{// doorSpace2[0]
                                                 {
                                                   Vector3(0.1f, 0.f, 0.6f), //
                                                   Vector3(2.f, 0.f, 0.6f),  //
                                                   Vector3(2.f, 0.f, 1.6f),  //
                                                   Vector3(0.1f, 0.f, 1.6f)  //
                                                 },
                                                 // doorSpace2[1]
                                                 {
                                                   Vector3(0.1f, 0.f, 4.6f), //
                                                   Vector3(2.f, 0.f, 4.6f),  //
                                                   Vector3(2.f, 0.f, 5.6f),  //
                                                   Vector3(0.1f, 0.f, 5.6f)  //
                                                 }};

    // Inner wall 2
    PolygonOptions innerWallnb2options;
    innerWallnb2options.shape = innerWallnb2Data;
    innerWallnb2options.holes = doorSpace2;
    innerWallnb2options.depth = 0.1f;
    auto innerWallnb2 = MeshBuilder::ExtrudePolygon("innerWallnb2", innerWallnb2options, scene);
    innerWallnb2->rotation().z = Math::PI_2;
    innerWallnb2->position().x = 1.35f;
    innerWallnb2->position().z = 0.15f;
    innerWallnb2->position().x = -1.4f;
    innerWallnb2->material     = innerwallmat;

    // Bathroom wall polygon shape in XoZ plane
    std::vector<Vector3> bathroomWallData{
      Vector3(-1.4f, 0.f, 0), //
      Vector3(-0.5f, 0.f, 0), //
      Vector3(-0.5f, 0.f, 2), //
      Vector3(0.5f, 0.f, 2),  //
      Vector3(0.5f, 0.f, 0),  //
      Vector3(1.4f, 0.f, 0),  //
      Vector3(1.4f, 0.f, 6),  //
      Vector3(-1.4f, 0.f, 6)  //
    };

    // Door space 3
    std::vector<std::vector<Vector3>> doorSpace3{// doorSpace3[0]
                                                 {
                                                   Vector3(-0.5f, 0.f, 3.2f), //
                                                   Vector3(-0.5f, 0.f, 5.2f), //
                                                   Vector3(0.5f, 0.f, 5.2f),  //
                                                   Vector3(0.5f, 0.f, 3.2f)   //
                                                 }};

    // Bathroom wall
    PolygonOptions bathroomOptions;
    bathroomOptions.shape = bathroomWallData;
    bathroomOptions.depth = 0.1f;
    bathroomOptions.holes = doorSpace3;
    auto bathroomWall     = MeshBuilder::ExtrudePolygon("bathroomWall", bathroomOptions, scene);
    bathroomWall->rotation().x = -Math::PI_2;
    bathroomWall->position().y = -3.f;
    bathroomWall->position().z = 6.f;
    bathroomWall->material     = innerwallmat;

    // Bedroom 1 wall shape in XoZ plane
    std::vector<Vector3> bedroom1WallData{
      Vector3(-5.5f, 0.f, 0.f), //
      Vector3(-2.9f, 0.f, 0.f), //
      Vector3(-2.9f, 0.f, 2.f), //
      Vector3(-1.9f, 0.f, 2.f), //
      Vector3(-1.9f, 0.f, 0.f), //
      Vector3(-1.4f, 0.f, 0.f), //
      Vector3(-1.4f, 0.f, 6.f), //
      Vector3(-5.5f, 0.f, 6.f)  //
    };

    // Bedroom 1 wall
    PolygonOptions bedroom1WallOptions;
    bedroom1WallOptions.shape = bedroom1WallData;
    bedroom1WallOptions.depth = 0.1f;
    auto bedroom1Wall = MeshBuilder::ExtrudePolygon("bedroom1Wall", bedroom1WallOptions, scene);
    bedroom1Wall->rotation().x = -Math::PI_2;
    bedroom1Wall->position().y = -3;
    bedroom1Wall->position().z = 4.5;
    bedroom1Wall->material     = innerwallmat;

    // Bannister wall shape in XoZ plane
    std::vector<Vector3> bannisterWallData{
      Vector3(0.f, 0.f, 0.f),    //
      Vector3(1.f, 0.f, 0.f),    //
      Vector3(1.f, 0.f, 1.4f),   //
      Vector3(1.75f, 0.f, 1.4f), //
      Vector3(1.75f, 0.f, 0.f),  //
      Vector3(3.5f, 0.f, 0.f),   //
      Vector3(3.5f, 0.f, 3.2f),  //
      Vector3(1.5f, 0.f, 3.2f),  //
      Vector3(0.f, 0.f, 0.75f)   //
    };
    auto spindleThickness = 0.05f;
    auto spindles         = 12;
    auto railGap          = (1.5f - spindles * spindleThickness) / (spindles - 1.f);
    std::vector<std::vector<Vector3>> rail;
    auto ac = spindleThickness;
    for (auto s = 0; s < spindles - 1; s++) {
      std::vector<Vector3> rails;
      rails.emplace_back(Vector3(ac, 0.f, 0.1f + 1.6f * ac));
      rails.emplace_back(Vector3(ac, 0.f, (0.75f - spindleThickness) + 1.6f * ac));
      rails.emplace_back(
        Vector3(ac + railGap, 0.f, (0.75f - spindleThickness) + 1.6f * (ac + railGap)));
      rails.emplace_back(Vector3(ac + railGap, 0.f, 1.6f * (ac + railGap)));
      rail.emplace_back(rails);
      ac += spindleThickness + railGap;
    }

    // Bannister wall
    PolygonOptions bannisterWallOptions;
    bannisterWallOptions.shape = bannisterWallData;
    bannisterWallOptions.holes = rail;
    bannisterWallOptions.depth = 0.1f;
    auto bannisterWall = MeshBuilder::ExtrudePolygon("bannisterWall", bannisterWallOptions, scene);
    bannisterWall->rotation().x = -Math::PI_2;
    bannisterWall->rotation().z = -Math::PI_2;
    bannisterWall->position().x = 0.4f;
    bannisterWall->position().y = -3.f;
    bannisterWall->position().z = 2.51f;

    // Bannister 1 shape in XoZ plane
    std::vector<Vector3> bannister1Data{
      Vector3(0, 0.f, 0),
      Vector3(2, 0.f, 0),
      Vector3(2, 0.f, 0.75),
      Vector3(0, 0.f, 0.75),
    };
    auto spindle1Thickness = 0.05f;
    auto spindles1         = 12;
    auto rail1Gap          = (2.f - spindles1 * spindle1Thickness) / (spindles1 - 1.f);
    std::vector<std::vector<Vector3>> rail1;
    auto ac1 = spindle1Thickness;
    for (auto s = 0; s < spindles1 - 1; s++) {
      std::vector<Vector3> rail1s;
      rail1s.emplace_back(Vector3(ac1, 0.f, spindle1Thickness));
      rail1s.emplace_back(Vector3(ac1, 0.f, 0.75f - spindle1Thickness));
      rail1s.emplace_back(Vector3(ac1 + rail1Gap, 0.f, 0.75f - spindle1Thickness));
      rail1s.emplace_back(Vector3(ac1 + rail1Gap, 0.f, spindle1Thickness));
      rail1.emplace_back(rail1s);
      ac1 += spindle1Thickness + rail1Gap;
    }

    // Bannister 1
    PolygonOptions bannister1Options;
    bannister1Options.shape  = bannister1Data;
    bannister1Options.holes  = rail1;
    bannister1Options.depth  = 0.1f;
    auto bannister1          = MeshBuilder::ExtrudePolygon("bannister1", bannister1Options, scene);
    bannister1->rotation().x = -Math::PI_2;
    bannister1->rotation().z = -Math::PI_2;
    bannister1->position().x = 0.3f;
    bannister1->position().y = 0.2f;
    bannister1->position().z = 2.61f;

    // Bannister 2 shape in XoZ plane
    std::vector<Vector3> bannister2Data{
      Vector3(0, 0.f, 0),
      Vector3(1, 0.f, 0),
      Vector3(1, 0.f, 0.75),
      Vector3(0, 0.f, 0.75),
    };
    auto spindle2Thickness = 0.05f;
    auto spindles2         = 6;
    auto rail2Gap          = (1.f - spindles2 * spindle2Thickness) / (spindles2 - 1.f);
    std::vector<std::vector<Vector3>> rail2;
    auto ac2 = spindle2Thickness;
    for (auto s = 0; s < spindles2 - 1; s++) {
      std::vector<Vector3> rail2s;
      rail2s.emplace_back(Vector3(ac2, 0.f, spindle2Thickness));
      rail2s.emplace_back(Vector3(ac2, 0.f, 0.75f - spindle2Thickness));
      rail2s.emplace_back(Vector3(ac2 + rail2Gap, 0.f, 0.75f - spindle2Thickness));
      rail2s.emplace_back(Vector3(ac2 + rail2Gap, 0.f, spindle2Thickness));
      rail2.emplace_back(rail2s);
      ac2 += spindle2Thickness + rail2Gap;
    }

    // Bannister 2
    PolygonOptions bannister2Options;
    bannister2Options.shape  = bannister2Data;
    bannister2Options.holes  = rail2;
    bannister2Options.depth  = 0.1f;
    auto bannister2          = MeshBuilder::ExtrudePolygon("bannister2", bannister2Options, scene);
    bannister2->rotation().x = -Math::PI_2;
    bannister2->position().x = 0.3f;
    bannister2->position().y = 0.2f;
    bannister2->position().z = 2.61f;

    // Windo maker function
    const auto windowMaker = [scene](float width, float height, float frames, float frameDepth,
                                     float frameThickness) {
      std::vector<Vector3> windowShape{
        Vector3(0.f, 0.f, 0.f),      //
        Vector3(width, 0.f, 0.f),    //
        Vector3(width, 0.f, height), //
        Vector3(0.f, 0.f, height)    //
      };
      auto glassWidth     = (width - (frames + 1.f) * frameThickness) / frames;
      auto glassTopHeight = height / 3.f - frameThickness;
      auto glassBotHeight = 2.f * glassTopHeight;
      std::vector<std::vector<Vector3>> glass;
      auto acf = frameThickness;
      for (auto f = 0; f < frames; ++f) {
        std::vector<Vector3> glass2f;
        glass2f.emplace_back(Vector3(acf, 0.f, 2.f * frameThickness + glassBotHeight));
        glass2f.emplace_back(Vector3(acf + glassWidth, 0.f, 2.f * frameThickness + glassBotHeight));
        glass2f.emplace_back(
          Vector3(acf + glassWidth, 0.f, 2.f * frameThickness + glassBotHeight + glassTopHeight));
        glass2f.emplace_back(
          Vector3(acf, 0.f, 2.f * frameThickness + glassBotHeight + glassTopHeight));
        glass.emplace_back(glass2f);
        std::vector<Vector3> glass2f1;
        glass2f1.emplace_back(Vector3(acf, 0.f, frameThickness));
        glass2f1.emplace_back(Vector3(acf + glassWidth, 0.f, frameThickness));
        glass2f1.emplace_back(Vector3(acf + glassWidth, 0.f, frameThickness + glassBotHeight));
        glass2f1.emplace_back(Vector3(acf, 0.f, frameThickness + glassBotHeight));
        glass.emplace_back(glass2f1);
        acf += frameThickness + glassWidth;
      }
      PolygonOptions windowOptions;
      windowOptions.shape  = windowShape;
      windowOptions.holes  = glass;
      windowOptions.depth  = frameDepth;
      auto window          = MeshBuilder::ExtrudePolygon("window", windowOptions, scene);
      window->rotation().x = -Math::PI_2;
      return window;
    };

    _windowFBL               = windowMaker(3.2f, 2.f, 4.f, 0.15f, 0.1f);
    _windowFBL->position().x = -4.78f;
    _windowFBL->position().y = -2.3f;
    _windowFBL->position().z = 0.1f;

    _windowFBR               = windowMaker(3.2f, 2.f, 4.f, 0.15f, 0.1f);
    _windowFBR->position().x = 1.58f;
    _windowFBR->position().y = -2.3f;
    _windowFBR->position().z = 0.1f;

    _windowFTL               = windowMaker(1.9f, 1.8f, 2.f, 0.15f, 0.1f);
    _windowFTL->position().x = -4.03f;
    _windowFTL->position().y = 0.75f;
    _windowFTL->position().z = 0.1f;

    _windowFTR               = windowMaker(1.9f, 1.8f, 2.f, 0.15f, 0.1f);
    _windowFTR->position().x = 2.13f;
    _windowFTR->position().y = 0.75f;
    _windowFTR->position().z = 0.1f;

    _windowFTM               = windowMaker(1.3f, 1.8f, 2.f, 0.15f, 0.1f);
    _windowFTM->position().x = -0.65f;
    _windowFTM->position().y = 0.75f;
    _windowFTM->position().z = 0.1f;

    _windowRBL               = windowMaker(3.15f, 1.5f, 4.f, 0.15f, 0.1f);
    _windowRBL->position().x = -5.f;
    _windowRBL->position().y = -1.8f;
    _windowRBL->position().z = 9.f;

    _windowRBR               = windowMaker(1.7f, 1.5f, 2.f, 0.15f, 0.1f);
    _windowRBR->position().x = -0.8f;
    _windowRBR->position().y = -1.8f;
    _windowRBR->position().z = 9.f;

    _windowRTL               = windowMaker(3.15f, 1.8f, 4.f, 0.15f, 0.1f);
    _windowRTL->position().x = -5.f;
    _windowRTL->position().y = 0.75f;
    _windowRTL->position().z = 9.f;

    _windowRTR               = windowMaker(1.3f, 0.8f, 1.f, 0.15f, 0.1f);
    _windowRTR->position().x = -0.6f;
    _windowRTR->position().y = 1.75f;
    _windowRTR->position().z = 9.f;

    _windowR1BL               = windowMaker(0.8f, 1.5f, 1.f, 0.15f, 0.1f);
    _windowR1BL->position().x = 3.7f;
    _windowR1BL->position().y = -1.8f;
    _windowR1BL->position().z = 6.f;

    _windowR1TL               = windowMaker(0.8f, 1.8f, 1.f, 0.15f, 0.1f);
    _windowR1TL->position().x = 1.9f;
    _windowR1TL->position().y = 0.75f;
    _windowR1TL->position().z = 6.f;

    _windowR1TR               = windowMaker(0.8f, 1.8f, 1.f, 0.15f, 0.1f);
    _windowR1TR->position().x = 4.2f;
    _windowR1TR->position().y = 0.75f;
    _windowR1TR->position().z = 6.f;

    // Door maker function
    const auto doorMaker = [scene](float width, float height, float depth) {
      std::vector<Vector3> doorShape{
        Vector3(0.f, 0.f, 0.f),      //
        Vector3(width, 0.f, 0.f),    //
        Vector3(width, 0.f, height), //
        Vector3(0.f, 0.f, height)    //
      };
      auto edgeThickness  = width / 8.f;
      auto panelWidth     = width - 2.f * edgeThickness;
      auto panelBotHeight = (height - 3.f * edgeThickness) / 1.75f;
      auto panelTopHeight = 0.75f * panelBotHeight;
      std::vector<std::vector<Vector3>> panel;
      std::vector<Vector3> panel0;
      panel0.emplace_back(Vector3(edgeThickness, 0.f, 2.f * edgeThickness + panelBotHeight));
      panel0.emplace_back(
        Vector3(edgeThickness + panelWidth, 0.f, 2.f * edgeThickness + panelBotHeight));
      panel0.emplace_back(Vector3(edgeThickness + panelWidth, 0.f,
                                  2.f * edgeThickness + panelBotHeight + panelTopHeight));
      panel0.emplace_back(
        Vector3(edgeThickness, 0.f, 2.f * edgeThickness + panelBotHeight + panelTopHeight));
      panel.emplace_back(panel0);
      std::vector<Vector3> panel1;
      panel1.emplace_back(Vector3(edgeThickness, 0.f, edgeThickness));
      panel1.emplace_back(Vector3(edgeThickness + panelWidth, 0.f, edgeThickness));
      panel1.emplace_back(Vector3(edgeThickness + panelWidth, 0.f, edgeThickness + panelBotHeight));
      panel1.emplace_back(Vector3(edgeThickness, 0.f, edgeThickness + panelBotHeight));
      panel.emplace_back(panel1);
      // Door
      PolygonOptions doorOptions;
      doorOptions.shape  = doorShape;
      doorOptions.holes  = panel;
      doorOptions.depth  = depth;
      auto door          = MeshBuilder::ExtrudePolygon("door", doorOptions, scene);
      door->rotation().x = -Math::PI_2;
      // Bottom panel
      BoxOptions panelBOptions;
      panelBOptions.width  = panelWidth;
      panelBOptions.height = panelBotHeight;
      panelBOptions.depth  = depth / 2.f;
      auto panelB          = MeshBuilder::CreateBox("p1b", panelBOptions, scene);
      panelB->position().x = edgeThickness + panelWidth / 2.f;
      panelB->position().y = edgeThickness + panelBotHeight / 2.f;
      panelB->position().z = depth / 2.f;
      // Top panel
      BoxOptions panelTOptions;
      panelTOptions.width  = panelWidth;
      panelTOptions.height = panelTopHeight;
      panelTOptions.depth  = depth / 2.f;
      auto panelT          = MeshBuilder::CreateBox("p1t", panelTOptions, scene);
      panelT->position().x = edgeThickness + panelWidth / 2;
      panelT->position().y = 2.f * edgeThickness + panelBotHeight + panelTopHeight / 2.f;
      panelT->position().z = depth / 2.f;

      return Mesh::MergeMeshes({door, panelB, panelT}, true);
    };

    auto doormat          = StandardMaterial::New("door", scene);
    doormat->diffuseColor = Color3(82.f / 255.f, 172.f / 255.f, 106.f / 255.f);

    // Front door
    _frontDoor               = doorMaker(1.f, 2.25f, 0.1f);
    _frontDoor->position().x = -0.5f;
    _frontDoor->position().y = -3.f;
    _frontDoor->position().z = 0.1f;
    _frontDoor->material     = doormat;

    // Back door
    _backDoor               = doorMaker(1, 2.25f, 0.1f);
    _backDoor->rotation().y = Math::PI_2;
    _backDoor->position().x = 1.3f;
    _backDoor->position().y = -3.f;
    _backDoor->position().z = 8.65f;
    _backDoor->material     = doormat;

    hudGui = [=]() {
      // Checkbox helper
      const auto renderCheckBox = [](const char* label, bool isSelected) -> bool {
        bool origValue = isSelected;
        ImGui::Checkbox(label, &isSelected);
        return origValue != isSelected;
      };

      // Header
      ImGui::TextWrapped("%s", "Toggle Visibility");

      // Toggle Roof Visibility
      if (renderCheckBox("Roof", _roof->isVisible)) {
        _roof->isVisible    = !_roof->isVisible;
        _ceiling->isVisible = !_ceiling->isVisible;
      }
      // Toggle Front Visibility
      if (renderCheckBox("Front", _frontWall->isVisible)) {
        _frontWall->isVisible = !_frontWall->isVisible;
        _windowFBL->isVisible = !_windowFBL->isVisible;
        _windowFBR->isVisible = !_windowFBR->isVisible;
        _windowFTL->isVisible = !_windowFTL->isVisible;
        _windowFTR->isVisible = !_windowFTR->isVisible;
        _windowFTM->isVisible = !_windowFTM->isVisible;
        _frontDoor->isVisible = !_frontDoor->isVisible;
      }
      // Toggle Back Visibility
      if (renderCheckBox("Back", _rearWallnb1->isVisible)) {
        _rearWallnb1->isVisible = !_rearWallnb1->isVisible;
        _rearWallnb2->isVisible = !_rearWallnb2->isVisible;
        _windowRBL->isVisible   = !_windowRBL->isVisible;
        _windowRBR->isVisible   = !_windowRBR->isVisible;
        _windowRTL->isVisible   = !_windowRTL->isVisible;
        _windowRTR->isVisible   = !_windowRTR->isVisible;
        _windowR1BL->isVisible  = !_windowR1BL->isVisible;
        _windowR1TL->isVisible  = !_windowR1TL->isVisible;
        _windowR1TR->isVisible  = !_windowR1TR->isVisible;
      }
      // Toggle Left Side Visibility
      if (renderCheckBox("Left Side", _sideWallnb1->isVisible)) {
        _sideWallnb1->isVisible = !_sideWallnb1->isVisible;
        _sideWallnb3->isVisible = !_sideWallnb3->isVisible;
        _backDoor->isVisible    = !_backDoor->isVisible;
      }
      // Toggle Right Side Visibility
      if (renderCheckBox("Right Side", _sideWallnb2->isVisible)) {
        _sideWallnb2->isVisible = !_sideWallnb2->isVisible;
      }
    };
  }

private:
  // Roof
  MeshPtr _roof, _ceiling;
  // Front
  MeshPtr _frontWall, _windowFBL, _windowFBR, _windowFTL, _windowFTR, _windowFTM, _frontDoor;
  // Back
  MeshPtr _rearWallnb1, _rearWallnb2, _windowRBL, _windowRBR, _windowRTL, _windowRTR, _windowR1BL,
    _windowR1TL, _windowR1TR;
  // Left Side
  MeshPtr _sideWallnb1, _sideWallnb3, _backDoor;
  // Right Side
  MeshPtr _sideWallnb2;

}; // end of class ExtrudePolygonScene

BABYLON_REGISTER_SAMPLE("Meshes", ExtrudePolygonScene)

} // end of namespace Samples
} // end of namespace BABYLON
