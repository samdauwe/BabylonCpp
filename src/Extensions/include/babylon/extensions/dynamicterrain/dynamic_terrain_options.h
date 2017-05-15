#ifndef BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_OPTIONS_H
#define BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct DynamicTerrainOptions {
  // terrainSub the wanted terrain number of subdivisions : integer, multiple of
  // 2.
  int terrainSub;
  // mapData the array of the map 3D data: x, y, z successive float values
  Float32Array mapData;
  // mapSubX the data map number of x subdivisions
  int mapSubX;
  // mapSubZ the data map number of z subdivisions
  int mapSubZ;
  // mapUVs the array of the map UV data: u,v successive values, each between 0
  // and 1.
  Float32Array mapUVs;
  // mapColors the array of the map Color data: x, y, z successive float values.
  Float32Array mapColors;
  // mapNormals the array of the map normal data (optional) : r,g,b successive
  // values, each between 0 and 1.
  Float32Array mapNormals;
  // invertSide boolean, to invert the terrain mesh upside down
  bool invertSide = false;
  // camera the camera to link the terrain to. Optional, by default the scene
  // active camera
  Camera* camera = nullptr;
}; // end of class DynamicTerrain

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_OPTIONS_H