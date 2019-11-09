#include <babylon/samples/samples_index.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct Block {
  float r;
  float g;
  float b;
  float a;
}; // end of struct Block

struct MergedMeshesScene : public IRenderableScene {

  // json representation of the world
  static const char* world_json;

  MergedMeshesScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
  }
  ~MergedMeshesScene()
  {
  }

  const char* getName() override
  {
    return "Merged Meshes Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a Camera
    auto camera = FreeCamera::New("camera1", Vector3(-100, 0, 100), scene);
  
    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());
  
    // Attach the camera to the canvas
    camera->attachControl(canvas, true);
  
    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);
  
    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.8f;
  
    // Parse the json representation of the world
    auto world = json::parse(world_json);
  
    // Create map with default block name to color mapping
    std::unordered_map<std::string, Block> defaultBlocks;
    for (auto& defaultBlock :
         json_util::get_array<json>(world, "defaultBlocks")) {
      defaultBlocks[json_util::get_string(defaultBlock, "terrain")]
        = {json_util::get_number(defaultBlock, "r", 0.f),
           json_util::get_number(defaultBlock, "g", 0.f),
           json_util::get_number(defaultBlock, "b", 0.f),
           json_util::get_number(defaultBlock, "a", 0.f)};
    }
  
    // Block factory method
    const auto createBlock
      = [&scene](const std::string& name, const Block& block) {
          // Set the face colors
          std::array<Color4, 6> faceColors{{
            Color4(),                                   // Face 1
            Color4(),                                   // Face 2
            Color4(),                                   // Face 3
            Color4(),                                   // Face 4
            Color4(block.r, block.g, block.b, block.a), // Face 5
            Color4(),                                   // Face 6
          }};
          // Create the box with height, width and depth of 5
          BoxOptions options;
          options.size       = 5.f;
          options.faceColors = std::move(faceColors);
          return MeshBuilder::CreateBox(name, options, scene);
        };
  
    // Create map
    std::vector<MeshPtr> arrayOfMeshes;
    unsigned int blockNr = 0;
    for (auto& blockOfMap : json_util::get_array<json>(world, "map")) {
      const auto blockType = json_util::get_string(blockOfMap, "terrain");
      const auto blockName = String::concat("Block_", blockNr++);
      const float y        = json_util::get_number(blockOfMap, "y", 0.f);
      const float x        = json_util::get_number(blockOfMap, "x", 0.f);
      const float z        = json_util::get_number(blockOfMap, "z", 0.f);
      auto block           = createBlock(blockName, defaultBlocks[blockType]);
      block->position().y  = y * 5.f;
      block->position().x  = x * 5.f;
      block->position().z  = z * 5.f;
      arrayOfMeshes.emplace_back(block);
    }
  
    // Merge meshes
    auto mergedMesh = Mesh::MergeMeshes(arrayOfMeshes);
    mergedMesh->enableEdgesRendering();
    mergedMesh->edgesWidth = 0.5f;
    mergedMesh->edgesColor = Color4(0.f, 0.f, 0.f, 0.f);
    mergedMesh->position().y += 30.f;
    mergedMesh->position().x -= 45.f;
    mergedMesh->rotation().y -= Math::PI / 4.f;
    mergedMesh->rotation().x += Math::PI / 3.f;
  }

}; // end of struct MergedMeshesScene


const char* MergedMeshesScene::world_json
  = R"someToken({"defaultBlocks":[{"terrain":"mountain","r":1,"g":1,"b":1,"a":1},{"terrain":"hill","r":0.7,"g":0.5,"b":0.4,"a":0.1},{"terrain":"water","r":0,"g":0,"b":1,"a":1},{"terrain":"grass","r":0,"g":1,"b":0,"a":1}],"map":[{"terrain":"mountain","x":0,"z":0,"y":0},{"terrain":"mountain","x":0,"z":0,"y":1},{"terrain":"mountain","x":0,"z":0,"y":2},{"terrain":"mountain","x":0,"z":1,"y":0},{"terrain":"mountain","x":0,"z":1,"y":1},{"terrain":"mountain","x":0,"z":1,"y":2},{"terrain":"water","x":0,"z":2,"y":0},{"terrain":"water","x":0,"z":3,"y":0},{"terrain":"mountain","x":0,"z":4,"y":0},{"terrain":"mountain","x":0,"z":4,"y":1},{"terrain":"mountain","x":0,"z":4,"y":2},{"terrain":"mountain","x":0,"z":5,"y":0},{"terrain":"mountain","x":0,"z":5,"y":1},{"terrain":"mountain","x":0,"z":5,"y":2},{"terrain":"mountain","x":0,"z":6,"y":0},{"terrain":"mountain","x":0,"z":6,"y":1},{"terrain":"mountain","x":0,"z":6,"y":2},{"terrain":"hill","x":0,"z":7,"y":0},{"terrain":"hill","x":0,"z":7,"y":1},{"terrain":"hill","x":0,"z":8,"y":0},{"terrain":"hill","x":0,"z":8,"y":1},{"terrain":"grass","x":0,"z":9,"y":0},{"terrain":"grass","x":0,"z":10,"y":0},{"terrain":"grass","x":0,"z":11,"y":0},{"terrain":"grass","x":0,"z":12,"y":0},{"terrain":"grass","x":0,"z":13,"y":0},{"terrain":"mountain","x":1,"z":0,"y":0},{"terrain":"mountain","x":1,"z":0,"y":1},{"terrain":"mountain","x":1,"z":0,"y":2},{"terrain":"mountain","x":1,"z":1,"y":0},{"terrain":"mountain","x":1,"z":1,"y":1},{"terrain":"mountain","x":1,"z":1,"y":2},{"terrain":"hill","x":1,"z":2,"y":0},{"terrain":"hill","x":1,"z":2,"y":1},{"terrain":"water","x":1,"z":3,"y":0},{"terrain":"grass","x":1,"z":4,"y":0},{"terrain":"hill","x":1,"z":5,"y":0},{"terrain":"hill","x":1,"z":5,"y":1},{"terrain":"hill","x":1,"z":6,"y":0},{"terrain":"hill","x":1,"z":6,"y":1},{"terrain":"hill","x":1,"z":7,"y":0},{"terrain":"hill","x":1,"z":7,"y":1},{"terrain":"grass","x":1,"z":8,"y":0},{"terrain":"grass","x":1,"z":9,"y":0},{"terrain":"grass","x":1,"z":10,"y":0},{"terrain":"grass","x":1,"z":11,"y":0},{"terrain":"grass","x":1,"z":12,"y":0},{"terrain":"grass","x":1,"z":13,"y":0},{"terrain":"mountain","x":2,"z":0,"y":0},{"terrain":"mountain","x":2,"z":0,"y":1},{"terrain":"mountain","x":2,"z":0,"y":2},{"terrain":"hill","x":2,"z":1,"y":0},{"terrain":"hill","x":2,"z":1,"y":1},{"terrain":"grass","x":2,"z":2,"y":0},{"terrain":"water","x":2,"z":3,"y":0},{"terrain":"grass","x":2,"z":4,"y":0},{"terrain":"grass","x":2,"z":5,"y":0},{"terrain":"grass","x":2,"z":6,"y":0},{"terrain":"grass","x":2,"z":7,"y":0},{"terrain":"grass","x":2,"z":8,"y":0},{"terrain":"grass","x":2,"z":9,"y":0},{"terrain":"grass","x":2,"z":10,"y":0},{"terrain":"grass","x":2,"z":11,"y":0},{"terrain":"grass","x":2,"z":12,"y":0},{"terrain":"grass","x":2,"z":13,"y":0},{"terrain":"mountain","x":3,"z":0,"y":0},{"terrain":"mountain","x":3,"z":0,"y":1},{"terrain":"mountain","x":3,"z":0,"y":2},{"terrain":"grass","x":3,"z":1,"y":0},{"terrain":"grass","x":3,"z":2,"y":0},{"terrain":"water","x":3,"z":3,"y":0},{"terrain":"water","x":3,"z":4,"y":0},{"terrain":"water","x":3,"z":5,"y":0},{"terrain":"water","x":3,"z":6,"y":0},{"terrain":"water","x":3,"z":7,"y":0},{"terrain":"water","x":3,"z":8,"y":0},{"terrain":"grass","x":3,"z":9,"y":0},{"terrain":"grass","x":3,"z":10,"y":0},{"terrain":"grass","x":3,"z":11,"y":0},{"terrain":"grass","x":3,"z":12,"y":0},{"terrain":"grass","x":3,"z":13,"y":0},{"terrain":"hill","x":4,"z":0,"y":0},{"terrain":"hill","x":4,"z":0,"y":1},{"terrain":"grass","x":4,"z":1,"y":0},{"terrain":"grass","x":4,"z":2,"y":0},{"terrain":"grass","x":4,"z":3,"y":0},{"terrain":"grass","x":4,"z":4,"y":0},{"terrain":"grass","x":4,"z":5,"y":0},{"terrain":"grass","x":4,"z":6,"y":0},{"terrain":"grass","x":4,"z":7,"y":0},{"terrain":"water","x":4,"z":8,"y":0},{"terrain":"grass","x":4,"z":9,"y":0},{"terrain":"grass","x":4,"z":10,"y":0},{"terrain":"grass","x":4,"z":11,"y":0},{"terrain":"grass","x":4,"z":12,"y":0},{"terrain":"grass","x":4,"z":13,"y":0},{"terrain":"hill","x":5,"z":0,"y":0},{"terrain":"hill","x":5,"z":0,"y":1},{"terrain":"grass","x":5,"z":1,"y":0},{"terrain":"grass","x":5,"z":2,"y":0},{"terrain":"grass","x":5,"z":3,"y":0},{"terrain":"grass","x":5,"z":4,"y":0},{"terrain":"grass","x":5,"z":5,"y":0},{"terrain":"grass","x":5,"z":6,"y":0},{"terrain":"grass","x":5,"z":7,"y":0},{"terrain":"water","x":5,"z":8,"y":0},{"terrain":"grass","x":5,"z":9,"y":0},{"terrain":"grass","x":5,"z":10,"y":0},{"terrain":"grass","x":5,"z":11,"y":0},{"terrain":"grass","x":5,"z":12,"y":0},{"terrain":"grass","x":5,"z":13,"y":0},{"terrain":"hill","x":6,"z":0,"y":0},{"terrain":"hill","x":6,"z":0,"y":1},{"terrain":"grass","x":6,"z":1,"y":0},{"terrain":"grass","x":6,"z":2,"y":0},{"terrain":"grass","x":6,"z":3,"y":0},{"terrain":"grass","x":6,"z":4,"y":0},{"terrain":"grass","x":6,"z":5,"y":0},{"terrain":"grass","x":6,"z":6,"y":0},{"terrain":"grass","x":6,"z":7,"y":0},{"terrain":"water","x":6,"z":8,"y":0},{"terrain":"grass","x":6,"z":9,"y":0},{"terrain":"grass","x":6,"z":10,"y":0},{"terrain":"grass","x":6,"z":11,"y":0},{"terrain":"grass","x":6,"z":12,"y":0},{"terrain":"grass","x":6,"z":13,"y":0},{"terrain":"hill","x":7,"z":0,"y":0},{"terrain":"hill","x":7,"z":0,"y":1},{"terrain":"grass","x":7,"z":1,"y":0},{"terrain":"grass","x":7,"z":2,"y":0},{"terrain":"grass","x":7,"z":3,"y":0},{"terrain":"grass","x":7,"z":4,"y":0},{"terrain":"grass","x":7,"z":5,"y":0},{"terrain":"grass","x":7,"z":6,"y":0},{"terrain":"grass","x":7,"z":7,"y":0},{"terrain":"water","x":7,"z":8,"y":0},{"terrain":"grass","x":7,"z":9,"y":0},{"terrain":"grass","x":7,"z":10,"y":0},{"terrain":"grass","x":7,"z":11,"y":0},{"terrain":"grass","x":7,"z":12,"y":0},{"terrain":"water","x":7,"z":13,"y":0},{"terrain":"hill","x":8,"z":0,"y":0},{"terrain":"hill","x":8,"z":0,"y":1},{"terrain":"grass","x":8,"z":1,"y":0},{"terrain":"grass","x":8,"z":2,"y":0},{"terrain":"grass","x":8,"z":3,"y":0},{"terrain":"grass","x":8,"z":4,"y":0},{"terrain":"grass","x":8,"z":5,"y":0},{"terrain":"grass","x":8,"z":6,"y":0},{"terrain":"grass","x":8,"z":7,"y":0},{"terrain":"water","x":8,"z":8,"y":0},{"terrain":"water","x":8,"z":9,"y":0},{"terrain":"water","x":8,"z":10,"y":0},{"terrain":"water","x":8,"z":11,"y":0},{"terrain":"water","x":8,"z":12,"y":0},{"terrain":"water","x":8,"z":13,"y":0},{"terrain":"hill","x":9,"z":0,"y":0},{"terrain":"hill","x":9,"z":0,"y":1},{"terrain":"grass","x":9,"z":1,"y":0},{"terrain":"grass","x":9,"z":2,"y":0},{"terrain":"grass","x":9,"z":3,"y":0},{"terrain":"grass","x":9,"z":4,"y":0},{"terrain":"grass","x":9,"z":5,"y":0},{"terrain":"grass","x":9,"z":6,"y":0},{"terrain":"grass","x":9,"z":7,"y":0},{"terrain":"grass","x":9,"z":8,"y":0},{"terrain":"water","x":9,"z":9,"y":0},{"terrain":"water","x":9,"z":10,"y":0},{"terrain":"water","x":9,"z":11,"y":0},{"terrain":"water","x":9,"z":12,"y":0},{"terrain":"water","x":9,"z":13,"y":0},{"terrain":"hill","x":10,"z":0,"y":0},{"terrain":"hill","x":10,"z":0,"y":1},{"terrain":"grass","x":10,"z":1,"y":0},{"terrain":"grass","x":10,"z":2,"y":0},{"terrain":"grass","x":10,"z":3,"y":0},{"terrain":"grass","x":10,"z":4,"y":0},{"terrain":"grass","x":10,"z":5,"y":0},{"terrain":"grass","x":10,"z":6,"y":0},{"terrain":"grass","x":10,"z":7,"y":0},{"terrain":"grass","x":10,"z":8,"y":0},{"terrain":"water","x":10,"z":9,"y":0},{"terrain":"water","x":10,"z":10,"y":0},{"terrain":"water","x":10,"z":11,"y":0},{"terrain":"water","x":10,"z":12,"y":0},{"terrain":"water","x":10,"z":13,"y":0},{"terrain":"hill","x":11,"z":0,"y":0},{"terrain":"hill","x":11,"z":0,"y":1},{"terrain":"grass","x":11,"z":1,"y":0},{"terrain":"grass","x":11,"z":2,"y":0},{"terrain":"grass","x":11,"z":3,"y":0},{"terrain":"grass","x":11,"z":4,"y":0},{"terrain":"grass","x":11,"z":5,"y":0},{"terrain":"grass","x":11,"z":6,"y":0},{"terrain":"water","x":11,"z":7,"y":0},{"terrain":"water","x":11,"z":8,"y":0},{"terrain":"water","x":11,"z":9,"y":0},{"terrain":"water","x":11,"z":10,"y":0},{"terrain":"water","x":11,"z":11,"y":0},{"terrain":"water","x":11,"z":12,"y":0},{"terrain":"water","x":11,"z":13,"y":0},{"terrain":"hill","x":12,"z":0,"y":0},{"terrain":"hill","x":12,"z":0,"y":1},{"terrain":"grass","x":12,"z":1,"y":0},{"terrain":"grass","x":12,"z":2,"y":0},{"terrain":"grass","x":12,"z":3,"y":0},{"terrain":"grass","x":12,"z":4,"y":0},{"terrain":"grass","x":12,"z":5,"y":0},{"terrain":"grass","x":12,"z":6,"y":0},{"terrain":"grass","x":12,"z":7,"y":0},{"terrain":"water","x":12,"z":8,"y":0},{"terrain":"water","x":12,"z":9,"y":0},{"terrain":"water","x":12,"z":10,"y":0},{"terrain":"water","x":12,"z":11,"y":0},{"terrain":"water","x":12,"z":12,"y":0},{"terrain":"water","x":12,"z":13,"y":0},{"terrain":"hill","x":13,"z":0,"y":0},{"terrain":"hill","x":13,"z":0,"y":1},{"terrain":"grass","x":13,"z":1,"y":0},{"terrain":"grass","x":13,"z":2,"y":0},{"terrain":"grass","x":13,"z":3,"y":0},{"terrain":"grass","x":13,"z":4,"y":0},{"terrain":"grass","x":13,"z":5,"y":0},{"terrain":"water","x":13,"z":6,"y":0},{"terrain":"water","x":13,"z":7,"y":0},{"terrain":"water","x":13,"z":8,"y":0},{"terrain":"water","x":13,"z":9,"y":0},{"terrain":"water","x":13,"z":10,"y":0},{"terrain":"water","x":13,"z":11,"y":0},{"terrain":"water","x":13,"z":12,"y":0},{"terrain":"water","x":13,"z":13,"y":0}]})someToken";

BABYLON_REGISTER_SAMPLE("Meshes", MergedMeshesScene)
} // end of namespace Samples
} // end of namespace BABYLON