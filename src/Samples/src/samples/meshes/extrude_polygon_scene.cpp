#include <babylon/samples/meshes/extrude_polygon_scene.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/vector4.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

ExtrudePolygonScene::ExtrudePolygonScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ExtrudePolygonScene::~ExtrudePolygonScene()
{
}

const char* ExtrudePolygonScene::getName()
{
  return "Extrude Polygon Scene";
}

void ExtrudePolygonScene::initializeScene(ICanvas* canvas, Scene* scene)
{

  // camera
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI / 3.f, 25,
                                     Vector3(0, 0, 4.5f), scene);
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("hemiLight", Vector3(10, 10, 0), scene);

  auto wallmat = StandardMaterial::New("wallmaterial", scene);
  wallmat->diffuseTexture
    = Texture::New("http://i.imgur.com/2b1C7UH.jpg", scene);

  auto innerwallmat = StandardMaterial::New("innerwallmaterial", scene);
  innerwallmat->diffuseColor
    = Color3(240.f / 255.f, 223.f / 255.f, 203.f / 255.f);

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
  std::vector<std::vector<Vector3>> frontWindowHoles{
    // frontWindowHoles[0]
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
  frontWallOptions.shape  = frontWallData;
  frontWallOptions.depth  = 0.15f;
  frontWallOptions.holes  = frontWindowHoles;
  frontWallOptions.faceUV = frontWallFaceUV;
  auto frontWall = MeshBuilder::ExtrudePolygon("wall", frontWallOptions, scene);
  frontWall->rotation().x = -Math::PI_2;
  frontWall->material     = wallmat;

  // Real wall polygon shape in XoZ plane
  std::vector<Vector3> rearWallnb1Data{
    Vector3(1.4f, 0.f, -3.f), //
    Vector3(5.5f, 0.f, -3.f), //
    Vector3(5.5f, 0.f, 3.f),  //
    Vector3(1.4f, 0.f, 3.f)   //
  };

  // Holes in XoZ plane
  std::vector<std::vector<Vector3>> rear1WindowHoles{
    // rear1WindowHoles[0]
    {
      Vector3(3.7f, 0, -1.8f), //
      Vector3(4.5f, 0, -1.8f), //
      Vector3(4.5f, 0, -0.3f), //
      Vector3(3.7f, 0, -0.3f)  //
    },
    // rear1WindowHoles[1]
    {
      Vector3(1.9f, 0, 0.75f), //
      Vector3(2.7f, 0, 0.75f), //
      Vector3(2.7f, 0, 2.55f), //
      Vector3(1.9f, 0, 2.55f)  //
    },
    // rear1WindowHoles[2]
    {
      Vector3(4.2f, 0, 0.75f), //
      Vector3(5.f, 0, 0.75f),  //
      Vector3(5.f, 0, 2.55f),  //
      Vector3(4.2f, 0, 2.55f)  //
    }};

  // Rear Wall faceUVs
  std::vector<Vector4> rearFaceUV{
    Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f), //
    Vector4(14.f / 15.f, 0.f, 1.f, 1.f),        //
    Vector4(0.f, 0.f, 7.f / 15.f, 1.f)          //
  };

  // Rear wall nb1 polygon shape in XoZ plane
  PolygonOptions rearWallnb1Options;
  rearWallnb1Options.shape  = rearWallnb1Data;
  rearWallnb1Options.depth  = 0.1f;
  rearWallnb1Options.holes  = rear1WindowHoles;
  rearWallnb1Options.faceUV = rearFaceUV;
  auto rearWallnb1
    = MeshBuilder::ExtrudePolygon("rearWallnb1", rearWallnb1Options, scene);
  rearWallnb1->rotation().x = -Math::PI_2;
  rearWallnb1->position().z = 6.15f;
  rearWallnb1->material     = wallmat;

  // Real wall nb2 polygon shape in XoZ plane
  std::vector<Vector3> rearWallnb2Data{
    Vector3(-5.6f, 0, -3.f),   //
    Vector3(1.45f, 0, -3.f),   //
    Vector3(1.45f, 0, 3.f),    //
    Vector3(-2.075f, 0, 5.5f), //
    Vector3(-5.6f, 0, 3.f)     //
  };

  // Holes in XoZ plane
  std::vector<std::vector<Vector3>> rear2WindowHoles{
    // rear2WindowHoles[0]
    {
      Vector3(-5.f, 0, -1.8f),   //
      Vector3(-1.85f, 0, -1.8f), //
      Vector3(-1.85f, 0, -0.3f), //
      Vector3(-5.f, 0, -0.3f)    //
    },
    // rear2WindowHoles[1]
    {
      Vector3(-0.8f, 0, -1.8f), //
      Vector3(0.9f, 0, -1.8f),  //
      Vector3(0.9f, 0, -0.3f),  //
      Vector3(-0.8f, 0, -0.3f)  //
    },
    // rear2WindowHoles[2]
    {
      Vector3(-5.f, 0, 0.75f),   //
      Vector3(-1.85f, 0, 0.75f), //
      Vector3(-1.85f, 0, 2.55f), //
      Vector3(-5.f, 0, 2.55f)    //
    },
    // rear2WindowHoles[3]
    {
      Vector3(-0.6f, 0, 1.75f), //
      Vector3(0.7f, 0, 1.75f),  //
      Vector3(0.7f, 0, 2.55f),  //
      Vector3(-0.6f, 0, 2.55f)  //
    }};

  // Rear wall nb2 polygon shape in XoZ plane
  PolygonOptions rearWallnb2Options;
  rearWallnb2Options.shape  = rearWallnb2Data;
  rearWallnb2Options.depth  = 0.1f;
  rearWallnb2Options.holes  = rear2WindowHoles;
  rearWallnb2Options.faceUV = rearFaceUV;
  auto rearWallnb2
    = MeshBuilder::ExtrudePolygon("rearWallnb2", rearWallnb2Options, scene);
  rearWallnb2->rotation().x = -Math::PI_2;
  rearWallnb2->position().z = 9.15f;
  rearWallnb2->material     = wallmat;

  // Side wall nb1 polygon shape in XoZ plane
  std::vector<Vector3> sideWallnb1Data{
    Vector3(-3.15f, 0, -3.f), //
    Vector3(3.1f, 0, -3.f),   //
    Vector3(3.1f, 0, 3.f),    //
    Vector3(0.f, 0, 5.5f),    //
    Vector3(-3.15f, 0, 3.f)   //
  };

  // faceUVs
  std::vector<Vector4> side1FaceUV{
    Vector4(0.f, 0.f, 7.f / 15.f, 1.f),        //
    Vector4(14.f / 15.f, 0.f, 1.f, 1.f),       //
    Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f) //
  };

  // Side wall nb1
  PolygonOptions sideWallnb1Options;
  sideWallnb1Options.shape  = sideWallnb1Data;
  sideWallnb1Options.depth  = 0.1f;
  sideWallnb1Options.faceUV = side1FaceUV;
  auto sideWallnb1
    = MeshBuilder::ExtrudePolygon("sideWallnb1", sideWallnb1Options, scene);
  sideWallnb1->rotation().z = -Math::PI_2;
  sideWallnb1->rotation().x = -Math::PI_2;
  sideWallnb1->position().x = 5.6f;
  sideWallnb1->position().z = 3.15f;
  sideWallnb1->material     = wallmat;

  // Side wall nb2 polygon shape in XoZ plane
  std::vector<Vector3> sideWallnb2Data{
    Vector3(-3.15f, 0.f, -3.f), //
    Vector3(6.f, 0.f, -3.f),    //
    Vector3(6.f, 0.f, 3.f),     //
    Vector3(3.1f, 0.f, 3.f),    //
    Vector3(0.f, 0.f, 5.5f),    //
    Vector3(-3.15f, 0, 3.f)     //
  };

  // Side 2 faceUVs
  std::vector<Vector4> side2FaceUV{
    Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f), //
    Vector4(14.f / 15.f, 0.f, 1.f, 1.f),        //
    Vector4(0.f, 0.f, 7.f / 15.f, 1.f)          //
  };

  // Side wall nb2
  PolygonOptions sideWallnb2Options;
  sideWallnb2Options.shape  = sideWallnb2Data;
  sideWallnb2Options.depth  = 0.1f;
  sideWallnb2Options.faceUV = side2FaceUV;
  auto sideWallnb2
    = MeshBuilder::ExtrudePolygon("sideWallnb2", sideWallnb2Options, scene);
  sideWallnb2->rotation().z = -Math::PI_2;
  sideWallnb2->rotation().x = -Math::PI_2;
  sideWallnb2->position().x = -5.5f;
  sideWallnb2->position().z = 3.15f;
  sideWallnb2->material     = wallmat;

  // Side wall nb3 polygon shape in XoZ plane
  std::vector<Vector3> sideWallnb3Data{
    Vector3(3.1f, 0, -3.f),   //
    Vector3(4.5f, 0, -3.f),   //
    Vector3(4.5f, 0, -0.75f), //
    Vector3(5.5f, 0, -0.75f), //
    Vector3(5.5f, 0, -3.f),   //
    Vector3(6.f, 0, -3.f),    //
    Vector3(6.f, 0, 3.f),     //
    Vector3(3.1f, 0, 3.f)     //
  };

  // Side 3 faceUVs
  std::vector<Vector4> side3FaceUV{
    Vector4(0.f, 0.f, 7.f / 15.f, 1.f),        //
    Vector4(14.f / 15.f, 0.f, 1.f, 1.f),       //
    Vector4(7.f / 15.f, 0.f, 14.f / 15.f, 1.f) //
  };

  // Side wall nb3
  PolygonOptions sideWallnb3Options;
  sideWallnb3Options.shape  = sideWallnb3Data;
  sideWallnb3Options.depth  = 0.1f;
  sideWallnb3Options.faceUV = side3FaceUV;
  auto sideWallnb3
    = MeshBuilder::ExtrudePolygon("sideWallnb3", sideWallnb3Options, scene);
  sideWallnb3->rotation().z = -Math::PI_2;
  sideWallnb3->rotation().x = -Math::PI_2;
  sideWallnb3->position().x = 1.45f;
  sideWallnb3->position().z = 3.15f;
  sideWallnb3->material     = wallmat;

  // Roof material
  auto roofmat = StandardMaterial::New("roofmat", scene);
  roofmat->diffuseTexture
    = Texture::New("http://i.imgur.com/Vw4fzwq.jpg", scene);

  // Roof 1 polygon shape in XoZ plane
  std::vector<Vector3> roof1Data{
    Vector3(-0.05f, 0, 0.f), //
    Vector3(0.1f, 0, 0.f),   //
    Vector3(3.3f, 0, 2.65f), //
    Vector3(6.5f, 0, 0.f),   //
    Vector3(6.6f, 0, 0.f),   //
    Vector3(3.3f, 0, 2.8f)   //
  };

  // Roof 1
  PolygonOptions roof1Options;
  roof1Options.shape = roof1Data;
  roof1Options.depth = 11.5f;
  auto roof1 = MeshBuilder::ExtrudePolygon("roof1", roof1Options, scene);
  roof1->rotation().z = -Math::PI_2;
  roof1->rotation().x = -Math::PI_2;
  roof1->position().x = 5.7f;
  roof1->position().y = 2.9f;
  roof1->position().z = -0.15f;
  roof1->material     = roofmat;

  // Roof 2 polygon shape in XoZ plane
  std::vector<Vector3> roof2Data{
    Vector3(0.f, 0, 0.f),     //
    Vector3(0.142f, 0, 0.f),  //
    Vector3(3.834f, 0, 2.6f), //
    Vector3(7.476f, 0, 0.f),  //
    Vector3(7.618f, 0, 0.f),  //
    Vector3(3.834f, 0, 2.7f)  //
  };

  // Roof 2
  PolygonOptions roof2Options;
  roof2Options.shape = roof2Data;
  roof2Options.depth = 3.2f;
  auto roof2 = MeshBuilder::ExtrudePolygon("roof2", roof2Options, scene);
  roof2->rotation().x = -Math::PI_2;
  roof2->position().x = -5.9f;
  roof2->position().y = 2.9f;
  roof2->position().z = 6.3f;
  roof2->material     = roofmat;

  // Roof 3 polygon shape in XoZ plane
  std::vector<Vector3> roof3Data{
    Vector3(0.3f, 0, 0.2f),   //
    Vector3(0.442f, 0, 0.2f), //
    Vector3(3.834f, 0, 2.6f), //
    Vector3(7.476f, 0, 0.f),  //
    Vector3(7.618f, 0, 0.f),  //
    Vector3(3.834f, 0, 2.7f)  //
  };

  // Roof 3
  PolygonOptions roof3Options;
  roof3Options.shape = roof3Data;
  roof3Options.depth = 3.2f;
  auto roof3 = MeshBuilder::ExtrudePolygon("roof3", roof3Options, scene);
  roof3->rotation().x = -Math::PI_2;
  roof3->position().x = -5.9f;
  roof3->position().y = 2.9f;
  roof3->position().z = 3.1f;
  roof3->material     = roofmat;

  //////////////////////////////////////////////////////////////////////////////

  const auto windowMaker = [scene](float width, float height, float frames,
                                   float frameDepth, float frameThickness) {
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
      glass2f.emplace_back(
        Vector3(acf, 0.f, 2.f * frameThickness + glassBotHeight));
      glass2f.emplace_back(
        Vector3(acf + glassWidth, 0.f, 2.f * frameThickness + glassBotHeight));
      glass2f.emplace_back(
        Vector3(acf + glassWidth, 0.f,
                2.f * frameThickness + glassBotHeight + glassTopHeight));
      glass2f.emplace_back(Vector3(
        acf, 0.f, 2.f * frameThickness + glassBotHeight + glassTopHeight));
      glass.emplace_back(glass2f);
      std::vector<Vector3> glass2f1;
      glass2f1.emplace_back(Vector3(acf, 0.f, frameThickness));
      glass2f1.emplace_back(Vector3(acf + glassWidth, 0.f, frameThickness));
      glass2f1.emplace_back(
        Vector3(acf + glassWidth, 0.f, frameThickness + glassBotHeight));
      glass2f1.emplace_back(Vector3(acf, 0.f, frameThickness + glassBotHeight));
      glass.emplace_back(glass2f1);
      acf += frameThickness + glassWidth;
    }
    PolygonOptions windowOptions;
    windowOptions.shape = windowShape;
    windowOptions.holes = glass;
    windowOptions.depth = frameDepth;
    auto window = MeshBuilder::ExtrudePolygon("window", windowOptions, scene);
    window->rotation().x = -Math::PI_2;
    return window;
  };

  auto windowFBL          = windowMaker(3.2f, 2.f, 4.f, 0.15f, 0.1f);
  windowFBL->position().x = -4.78f;
  windowFBL->position().y = -2.3f;
  windowFBL->position().z = 0.1f;

  auto windowFBR          = windowMaker(3.2f, 2.f, 4.f, 0.15f, 0.1f);
  windowFBR->position().x = 1.58f;
  windowFBR->position().y = -2.3f;
  windowFBR->position().z = 0.1f;

  auto windowFTL          = windowMaker(1.9f, 1.8f, 2.f, 0.15f, 0.1f);
  windowFTL->position().x = -4.03f;
  windowFTL->position().y = 0.75f;
  windowFTL->position().z = 0.1f;

  auto windowFTR          = windowMaker(1.9f, 1.8f, 2.f, 0.15f, 0.1f);
  windowFTR->position().x = 2.13f;
  windowFTR->position().y = 0.75f;
  windowFTR->position().z = 0.1f;

  auto windowFTM          = windowMaker(1.3f, 1.8f, 2.f, 0.15f, 0.1f);
  windowFTM->position().x = -0.65f;
  windowFTM->position().y = 0.75f;
  windowFTM->position().z = 0.1f;

  auto windowRBL          = windowMaker(3.15f, 1.5f, 4.f, 0.15f, 0.1f);
  windowRBL->position().x = -5.f;
  windowRBL->position().y = -1.8f;
  windowRBL->position().z = 9.f;

  auto windowRBR          = windowMaker(1.7f, 1.5f, 2.f, 0.15f, 0.1f);
  windowRBR->position().x = -0.8f;
  windowRBR->position().y = -1.8f;
  windowRBR->position().z = 9.f;

  auto windowRTL          = windowMaker(3.15f, 1.8f, 4.f, 0.15f, 0.1f);
  windowRTL->position().x = -5.f;
  windowRTL->position().y = 0.75f;
  windowRTL->position().z = 9.f;

  auto windowRTR          = windowMaker(1.3f, 0.8f, 1.f, 0.15f, 0.1f);
  windowRTR->position().x = -0.6f;
  windowRTR->position().y = 1.75f;
  windowRTR->position().z = 9.f;

  auto windowR1BL          = windowMaker(0.8f, 1.5f, 1.f, 0.15f, 0.1f);
  windowR1BL->position().x = 3.7f;
  windowR1BL->position().y = -1.8f;
  windowR1BL->position().z = 6.f;

  auto windowR1TL          = windowMaker(0.8f, 1.8f, 1.f, 0.15f, 0.1f);
  windowR1TL->position().x = 1.9f;
  windowR1TL->position().y = 0.75f;
  windowR1TL->position().z = 6.f;

  auto windowR1TR          = windowMaker(0.8f, 1.8f, 1.f, 0.15f, 0.1f);
  windowR1TR->position().x = 4.2f;
  windowR1TR->position().y = 0.75f;
  windowR1TR->position().z = 6.f;

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
    panel0.emplace_back(
      Vector3(edgeThickness, 0.f, 2.f * edgeThickness + panelBotHeight));
    panel0.emplace_back(Vector3(edgeThickness + panelWidth, 0.f,
                                2.f * edgeThickness + panelBotHeight));
    panel0.emplace_back(
      Vector3(edgeThickness + panelWidth, 0.f,
              2.f * edgeThickness + panelBotHeight + panelTopHeight));
    panel0.emplace_back(
      Vector3(edgeThickness, 0.f,
              2.f * edgeThickness + panelBotHeight + panelTopHeight));
    panel.emplace_back(panel0);
    std::vector<Vector3> panel1;
    panel1.emplace_back(Vector3(edgeThickness, 0.f, edgeThickness));
    panel1.emplace_back(
      Vector3(edgeThickness + panelWidth, 0.f, edgeThickness));
    panel1.emplace_back(
      Vector3(edgeThickness + panelWidth, 0.f, edgeThickness + panelBotHeight));
    panel1.emplace_back(
      Vector3(edgeThickness, 0.f, edgeThickness + panelBotHeight));
    panel.emplace_back(panel1);
    // Door
    PolygonOptions doorOptions;
    doorOptions.shape = doorShape;
    doorOptions.holes = panel;
    doorOptions.depth = depth;
    auto door         = MeshBuilder::ExtrudePolygon("door", doorOptions, scene);
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
    panelT->position().y
      = 2.f * edgeThickness + panelBotHeight + panelTopHeight / 2.f;
    panelT->position().z = depth / 2.f;

    return Mesh::MergeMeshes({door, panelB, panelT}, true);
  };

  auto doormat          = StandardMaterial::New("door", scene);
  doormat->diffuseColor = Color3(82.f / 255.f, 172.f / 255.f, 106.f / 255.f);

  auto frontDoor          = doorMaker(1.f, 2.25f, 0.1f);
  frontDoor->position().x = -0.5f;
  frontDoor->position().y = -3.f;
  frontDoor->position().z = 0.1f;
  frontDoor->material     = doormat;

  auto backDoor          = doorMaker(1, 2.25f, 0.1f);
  backDoor->rotation().y = Math::PI_2;
  backDoor->position().x = 1.3f;
  backDoor->position().y = -3.f;
  backDoor->position().z = 8.65f;
  backDoor->material     = doormat;
}

} // end of namespace Samples
} // end of namespace BABYLON
