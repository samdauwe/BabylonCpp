#include <babylon/samples/meshes/house_from_floorplan.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/polygonmesh/polygon_mesh_builder.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {
namespace Samples {

HouseFromFloorplanScene::HouseFromFloorplanScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _alpha{0.01f}, _house{nullptr}
{
}

HouseFromFloorplanScene::~HouseFromFloorplanScene()
{
}

const char* HouseFromFloorplanScene::getName()
{
  return "House from a Floorplan Scene";
}

void HouseFromFloorplanScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a camera
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI / 3.f,
                                     25.f, Vector3(0.f, 0.f, 4.5f), scene);
  camera->setPosition(Vector3(0, 5, 12));
  camera->attachControl(canvas, true);

  // Create a light
  auto light
    = HemisphericLight::New("hemiLight", Vector3(5.f, 10.f, 0.f), scene);
  light->intensity = 0.95f;

  // Create the house from a floor plan
  Float32Array baseData{-3.f, -2.f, -1.f, -4.f, 1.f, -4.f, 3.f, -2.f, 5.f,
                        -2.f, 5.f,  1.f,  2.f,  1.f, 2.f,  3.f, -3.f, 3.f};

  const auto createCorner = [](float x, float y) { return Vector3(x, 0, y); };

  std::vector<Vector3> corners;
  for (size_t b = 0; b < baseData.size() / 2; ++b) {
    corners.emplace_back(createCorner(baseData[2 * b], baseData[2 * b + 1]));
  }

  Door door{1.f, 1.8f, 0.f};
  DoorSpace doorSpace{door, 1.f};

  Window window0{1.2f, 2.4f, 0.f, 0.f};
  Window window1{2.f, 2.4f, 0.f, 0.f};

  WindowSpace windowSpace02{window0, 0.814f, 0.8f, 0.45f};
  WindowSpace windowSpace1{window0, 0.4f, 0.4f, 0.45f};
  WindowSpace windowSpace78{window1, 1.5, 0.6f, 0.45f};

  std::vector<Wall> walls;
  for (size_t c = 0; c < corners.size(); ++c) {
    walls.emplace_back(Wall{corners[c], {}, {}});
  }

  walls[0].windowSpaces = {windowSpace02};
  walls[1].windowSpaces = {windowSpace1};
  walls[2].windowSpaces = {windowSpace02};
  walls[7].windowSpaces = {windowSpace78};
  walls[8].windowSpaces = {windowSpace78};

  walls[5].doorSpaces = {doorSpace};

  auto ply    = 0.3f;
  auto height = 3.2f;

  BuildFromPlanOptions buildOptions;
  buildOptions.interiorUV = Vector4(0.167f, 0.f, 1.f, 1.f);
  buildOptions.exteriorUV = Vector4(0.f, 0.f, 0.16f, 1.f);
  _house = buildFromPlan(walls, ply, height, buildOptions, scene);
  _house->position().y -= 4.f;

  // Animations
  _scene->registerBeforeRender([this](Scene*, EventState&) {
    _house->rotation().y += _alpha * getScene()->getAnimationRatio();
  });
}

MeshPtr HouseFromFloorplanScene::buildFromPlan(std::vector<Wall>& walls,
                                               float ply, float height,
                                               BuildFromPlanOptions& options,
                                               Scene* scene)
{
  // Arrays for vertex positions and indices
  Float32Array positions;
  Uint32Array indices;
  Float32Array normals;
  Float32Array uvs;
  Float32Array colors;

  const auto& interiorUV    = options.interiorUV;
  const auto& exteriorUV    = options.exteriorUV;
  const auto& interiorColor = options.interiorColor;
  const auto& exteriorColor = options.exteriorColor;

  size_t interiorIndex = 0;

  // Arrays to hold wall corner data
  std::vector<Vector3> innerBaseCorners;
  std::vector<Vector3> outerBaseCorners;
  std::vector<Vector3> innerTopCorners;
  std::vector<Vector3> outerTopCorners;
  std::vector<std::vector<std::vector<Vector3>>> innerDoorCorners;
  std::vector<std::vector<std::vector<Vector3>>> outerDoorCorners;
  std::vector<std::vector<std::vector<Vector3>>> innerWindowCorners;
  std::vector<std::vector<std::vector<Vector3>>> outerWindowCorners;

  std::vector<Vector3> outerData;
  auto angle     = 0.f;
  auto direction = 0.f;
  auto line      = Vector3::Zero();
  walls[1].corner.subtractToRef(walls[0].corner, line);
  auto nextLine = Vector3::Zero();
  walls[2].corner.subtractToRef(walls[1].corner, nextLine);

  auto nbWalls = walls.size();
  outerData.resize(nbWalls);
  for (size_t w = 0; w < nbWalls; ++w) {
    angle = Math::PI
            - std::acos(Vector3::Dot(line, nextLine)
                        / (line.length() * nextLine.length()));
    direction       = Vector3::Cross(nextLine, line).normalize().y;
    auto lineNormal = Vector3(line.z, 0.f, -1.f * line.x).normalize();
    line.normalize();
    outerData[(w + 1) % nbWalls]
      = walls[(w + 1) % nbWalls]
          .corner.add(lineNormal.scale(ply))
          .add(line.scale(direction * ply / std::tan(angle / 2.f)));
    line = nextLine;
    walls[(w + 3) % nbWalls].corner.subtractToRef(
      walls[(w + 2) % nbWalls].corner, nextLine);
  }

  for (size_t w = 0; w < nbWalls; ++w) {
    innerBaseCorners.emplace_back(walls[w].corner); // inner corners base
  }

  for (size_t w = 0; w < nbWalls; ++w) {
    outerBaseCorners.emplace_back(outerData[w]); // outer corners base
  }

  // Inner and outer top corners
  for (size_t w = 0; w < nbWalls; w++) {
    innerTopCorners.emplace_back(
      Vector3(innerBaseCorners[w].x, height, innerBaseCorners[w].z));
    outerTopCorners.emplace_back(
      Vector3(outerBaseCorners[w].x, height, outerBaseCorners[w].z));
  }

  auto maxL = 0.f;
  for (size_t w = 0; w < nbWalls; ++w) {
    maxL = std::max(innerBaseCorners[(w + 1) % nbWalls]
                      .subtract(innerBaseCorners[w])
                      .length(),
                    maxL);
  }

  auto maxH = height; // for when gables introduced

  /**************** House Mesh Construction ***********************************/

  // Wall Construction
  std::vector<Vector2> polygonCorners;
  WallData wallData;
  auto wallDirection      = Vector3::Zero();
  auto wallNormal         = Vector3::Zero();
  auto wallLength         = 0.f;
  auto exteriorWallLength = 0.f;
  Float32Array doorData;
  Float32Array windowData;
  auto uvx = 0.f, uvy = 0.f;
  auto wallDiff = 0.f;

  const auto compareLeft
    = [](const DoorSpace& a, const DoorSpace& b) { return a.left - b.left; };

  for (size_t w = 0; w < nbWalls; w++) {
    walls[(w + 1) % nbWalls].corner.subtractToRef(walls[w].corner,
                                                  wallDirection);
    wallLength = wallDirection.length();
    wallDirection.normalize();
    wallNormal.x       = wallDirection.z;
    wallNormal.z       = -1.f * wallDirection.x;
    exteriorWallLength = outerBaseCorners[(w + 1) % nbWalls]
                           .subtract(outerBaseCorners[w])
                           .length();
    wallDiff = exteriorWallLength - wallLength;

    // Doors
    if (!walls[w].doorSpaces.empty()) {
      std::sort(walls[w].doorSpaces.begin(), walls[w].doorSpaces.end(),
                compareLeft);
    }
    auto doors = walls[w].doorSpaces.size();

    // Construct INNER wall polygon starting from (0, 0) using wall length and
    // height and door data
    polygonCorners.clear();
    polygonCorners.emplace_back(Vector2::Zero());

    for (size_t d = 0; d < doors; d++) {
      polygonCorners.emplace_back(Vector2(walls[w].doorSpaces[d].left, 0.f));
      polygonCorners.emplace_back(Vector2(walls[w].doorSpaces[d].left,
                                          walls[w].doorSpaces[d].door.height));
      polygonCorners.emplace_back(
        Vector2(walls[w].doorSpaces[d].left + walls[w].doorSpaces[d].door.width,
                walls[w].doorSpaces[d].door.height));
      polygonCorners.emplace_back(Vector2(
        walls[w].doorSpaces[d].left + walls[w].doorSpaces[d].door.width, 0.f));
    }

    polygonCorners.emplace_back(Vector2(wallLength, 0));
    polygonCorners.emplace_back(Vector2(wallLength, height));
    polygonCorners.emplace_back(Vector2(0, height));

    // Construct triangulation of polygon using its corners
    PolygonMeshBuilder polygonTriangulation
      = PolygonMeshBuilder("", polygonCorners, scene);

    // Windows
    // Construct holes and add to polygon from window data
    auto windows = walls[w].windowSpaces.size();
    std::vector<std::vector<Vector2>> holes;
    for (size_t ws = 0; ws < windows; ++ws) {
      std::vector<Vector2> holeData;
      holeData.emplace_back<Vector2>(
        Vector2(walls[w].windowSpaces[ws].left,
                height - walls[w].windowSpaces[ws].top
                  - walls[w].windowSpaces[ws].window.height));
      holeData.emplace_back<Vector2>(Vector2(
        walls[w].windowSpaces[ws].left + walls[w].windowSpaces[ws].window.width,
        height - walls[w].windowSpaces[ws].top
          - walls[w].windowSpaces[ws].window.height));
      holeData.emplace_back<Vector2>(Vector2(
        walls[w].windowSpaces[ws].left + walls[w].windowSpaces[ws].window.width,
        height - walls[w].windowSpaces[ws].top));
      holeData.emplace_back<Vector2>(
        Vector2(walls[w].windowSpaces[ws].left,
                height - walls[w].windowSpaces[ws].top));
      holes.emplace_back(holeData);
    }

    for (size_t h = 0; h < holes.size(); ++h) {
      polygonTriangulation.addHole(holes[h]);
    }

    // WallBuilder produces wall vertex positions array and indices using the
    // current and next wall to rotate and translate vertex positions to correct
    // place
    std::tie(wallData.positions, wallData.indices)
      = polygonTriangulation.buildWall(walls[w].corner,
                                       walls[(w + 1) % nbWalls].corner);

    auto nbIndices = positions.size() / 3; // current number of indices

    const auto& polyTriPointsElements = polygonTriangulation.points().elements;
    for (const auto& p : polyTriPointsElements) {
      uvx = interiorUV.x + p.x * (interiorUV.z - interiorUV.x) / maxL;
      uvy = interiorUV.y + p.y * (interiorUV.w - interiorUV.y) / height;
      stl_util::concat(uvs, {uvx, uvy});
      stl_util::concat(colors, {interiorColor.r, interiorColor.g,
                                interiorColor.b, interiorColor.a});
    }

    // Add inner wall positions (repeated for flat shaded mesh)
    positions     = stl_util::concat(positions, wallData.positions);
    interiorIndex = positions.size() / 3;
    for (auto& idx : wallData.indices) {
      indices.emplace_back(static_cast<uint32_t>(idx + nbIndices));
    }

    // WallData has format for inner wall [base left, 0 or more doors, base
    // right, top right, top left, windows]
    // extract door and wall data
    // WindowData: 4 entries per door + 4 entries for wall corners, each entry
    // has 3 data points
    windowData
      = stl_util::slice(wallData.positions, static_cast<int>(12 * (doors + 1)));
    doorData = stl_util::slice(wallData.positions, 3,
                               static_cast<int>(3 * (4 * doors + 1)));

    // For each inner door save corner as an array of four Vector3s, base left,
    // top left, top right, base right
    // Extend door data outwards by ply and save outer door corners
    std::vector<std::vector<Vector3>> doorCornersIn;
    std::vector<std::vector<Vector3>> doorCornersOut;
    for (size_t p = 0; p < doorData.size() / 12; ++p) {
      std::vector<Vector3> doorsIn;
      std::vector<Vector3> doorsOut;
      for (size_t d = 0; d < 4; ++d) {
        doorsIn.emplace_back(Vector3(doorData[3 * d + 12 * p],
                                     doorData[3 * d + 12 * p + 1],
                                     doorData[3 * d + 12 * p + 2]));
        doorData[3 * d + 12 * p] += ply * wallNormal.x;
        doorData[3 * d + 12 * p + 2] += ply * wallNormal.z;
        doorsOut.emplace_back(Vector3(doorData[3 * d + 12 * p],
                                      doorData[3 * d + 12 * p + 1],
                                      doorData[3 * d + 12 * p + 2]));
      }
      doorCornersIn.emplace_back(doorsIn);
      doorCornersOut.emplace_back(doorsOut);
    }
    innerDoorCorners.emplace_back(doorCornersIn);
    outerDoorCorners.emplace_back(doorCornersOut);

    // For each inner window save corner as an array of four Vector3s, base
    // left, top left, top right, base right
    // Extend window data outwards by ply and save outer window corners
    std::vector<std::vector<Vector3>> windowCornersIn;
    std::vector<std::vector<Vector3>> windowCornersOut;
    for (size_t p = 0; p < windowData.size() / 12; ++p) {
      std::vector<Vector3> windowsIn;
      std::vector<Vector3> windowsOut;
      for (size_t d = 0; d < 4; ++d) {
        windowsIn.emplace_back(Vector3(windowData[3 * d + 12 * p],
                                       windowData[3 * d + 12 * p + 1],
                                       windowData[3 * d + 12 * p + 2]));
        windowData[3 * d + 12 * p] += ply * wallNormal.x;
        windowData[3 * d + 12 * p + 2] += ply * wallNormal.z;
        windowsOut.emplace_back(Vector3(windowData[3 * d + 12 * p],
                                        windowData[3 * d + 12 * p + 1],
                                        windowData[3 * d + 12 * p + 2]));
      }
      windowCornersIn.emplace_back(windowsIn);
      windowCornersOut.emplace_back(windowsOut);
    }
    innerWindowCorners.emplace_back(windowCornersIn);
    outerWindowCorners.emplace_back(windowCornersOut);

    // Construct OUTER wall facet positions from inner wall
    // Add outer wall corner positions back to wallData positions
    wallData.positions.clear();
    stl_util::concat(
      wallData.positions,
      {outerBaseCorners[w].x, outerBaseCorners[w].y, outerBaseCorners[w].z});
    wallData.positions = stl_util::concat(wallData.positions, doorData);
    stl_util::concat(wallData.positions,
                     {outerBaseCorners[(w + 1) % nbWalls].x,
                      outerBaseCorners[(w + 1) % nbWalls].y,
                      outerBaseCorners[(w + 1) % nbWalls].z});
    stl_util::concat(wallData.positions,
                     {outerTopCorners[(w + 1) % nbWalls].x,
                      outerTopCorners[(w + 1) % nbWalls].y,
                      outerTopCorners[(w + 1) % nbWalls].z});
    stl_util::concat(
      wallData.positions,
      {outerTopCorners[w].x, outerTopCorners[w].y, outerTopCorners[w].z});
    wallData.positions = stl_util::concat(wallData.positions, windowData);

    // Calculate exterior wall uvs
    for (const auto& p : polyTriPointsElements) {
      if (p.x == 0.f) {
        uvx = exteriorUV.x;
      }
      else if (wallLength - p.x < 0.000001f) {
        uvx = exteriorUV.x
              + (wallDiff + p.x) * (exteriorUV.z - exteriorUV.x)
                  / (maxL + wallDiff);
      }
      else {
        uvx = exteriorUV.x
              + (0.5f * wallDiff + p.x) * (exteriorUV.z - exteriorUV.x)
                  / (maxL + wallDiff);
      }
      uvy = exteriorUV.y + p.y * (exteriorUV.w - exteriorUV.y) / height;
      stl_util::concat(uvs, {uvx, uvy});
    }

    nbIndices = positions.size() / 3; // current number of indices

    // Add outer wall positions, uvs and colors (repeated for flat shaded mesh)
    positions = stl_util::concat(positions, wallData.positions);

    // Reverse indices for correct normals
    std::reverse(wallData.indices.begin(), wallData.indices.end());
    for (const auto& idx : wallData.indices) {
      indices.emplace_back(static_cast<uint32_t>(idx + nbIndices));
    }

    // Construct facets for base and door top and door sides, repeating
    // positions for flatshaded mesh
    int doorsRemaining = static_cast<int>(doors);
    auto doorNb        = 0ul;

    if (doorsRemaining > 0) {
      // Base
      nbIndices       = positions.size() / 3; // current number of indices
      auto _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerBaseCorners[w].x, innerBaseCorners[w].y,
                                   innerBaseCorners[w].z}); // tl
      stl_util::concat(positions, {outerBaseCorners[w].x, outerBaseCorners[w].y,
                                   outerBaseCorners[w].z}); // bl
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][0].x,
                                   innerDoorCorners[w][doorNb][0].y,
                                   innerDoorCorners[w][doorNb][0].z}); // tr
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][0].x,
                                   outerDoorCorners[w][doorNb][0].y,
                                   outerDoorCorners[w][doorNb][0].z}); // br

      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y
                                             + (exteriorUV.w - exteriorUV.y)
                                                 * ply / maxH}); // top Left
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y});       // base Left
      stl_util::concat(
        uvs, {exteriorUV.x
                + (exteriorUV.z - exteriorUV.x)
                    * walls[w].doorSpaces[doorNb].left / maxL,
              exteriorUV.y
                + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right
      stl_util::concat(uvs, {exteriorUV.x
                               + (exteriorUV.z - exteriorUV.x)
                                   * walls[w].doorSpaces[doorNb].left / maxL,
                             exteriorUV.y}); // base right

      stl_util::concat(indices, {_nbIndices, _nbIndices + 2, _nbIndices + 3,
                                 _nbIndices + 3, _nbIndices + 1, _nbIndices});

      // Left side
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerDoorCorners[w][doorNb][0].x,
                                   innerDoorCorners[w][doorNb][0].y,
                                   innerDoorCorners[w][doorNb][0].z}); // br
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][1].x,
                                   innerDoorCorners[w][doorNb][1].y,
                                   innerDoorCorners[w][doorNb][1].z}); // tr
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][0].x,
                                   outerDoorCorners[w][doorNb][0].y,
                                   outerDoorCorners[w][doorNb][0].z}); // bl
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][1].x,
                                   outerDoorCorners[w][doorNb][1].y,
                                   outerDoorCorners[w][doorNb][1].z}); // tl

      stl_util::concat(
        uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
              exteriorUV.y}); // base right
      stl_util::concat(
        uvs,
        {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
         exteriorUV.y
           + (exteriorUV.w - exteriorUV.y)
               * walls[w].doorSpaces[doorNb].door.height / maxH}); // top right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y});         // base Left
      stl_util::concat(
        uvs, {exteriorUV.x, exteriorUV.y
                              + (exteriorUV.w - exteriorUV.y)
                                  * walls[w].doorSpaces[doorNb].door.height
                                  / maxH}); // top Left

      stl_util::concat(indices, {_nbIndices, _nbIndices + 1, _nbIndices + 3,
                                 _nbIndices, _nbIndices + 3, _nbIndices + 2});

      // Top
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerDoorCorners[w][doorNb][1].x,
                                   innerDoorCorners[w][doorNb][1].y,
                                   innerDoorCorners[w][doorNb][1].z}); // bl
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][2].x,
                                   innerDoorCorners[w][doorNb][2].y,
                                   innerDoorCorners[w][doorNb][2].z}); // br
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][1].x,
                                   outerDoorCorners[w][doorNb][1].y,
                                   outerDoorCorners[w][doorNb][1].z}); // tl
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][2].x,
                                   outerDoorCorners[w][doorNb][2].y,
                                   outerDoorCorners[w][doorNb][2].z}); // tr

      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(uvs,
                       {exteriorUV.x
                          + (exteriorUV.z - exteriorUV.x)
                              * walls[w].doorSpaces[doorNb].door.width / maxL,
                        exteriorUV.y}); // base right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y
                                             + (exteriorUV.w - exteriorUV.y)
                                                 * ply / maxH}); // top Left
      stl_util::concat(
        uvs, {exteriorUV.x
                + (exteriorUV.z - exteriorUV.x)
                    * walls[w].doorSpaces[doorNb].door.width / maxL,
              exteriorUV.y
                + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right

      stl_util::concat(indices, {_nbIndices + 2, _nbIndices + 1, _nbIndices + 3,
                                 _nbIndices + 2, _nbIndices, _nbIndices + 1});

      // Right side
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerDoorCorners[w][doorNb][2].x,
                                   innerDoorCorners[w][doorNb][2].y,
                                   innerDoorCorners[w][doorNb][2].z}); // tl
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][3].x,
                                   innerDoorCorners[w][doorNb][3].y,
                                   innerDoorCorners[w][doorNb][3].z}); // bl
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][2].x,
                                   outerDoorCorners[w][doorNb][2].y,
                                   outerDoorCorners[w][doorNb][2].z}); // tr
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][3].x,
                                   outerDoorCorners[w][doorNb][3].y,
                                   outerDoorCorners[w][doorNb][3].z}); // br

      stl_util::concat(
        uvs, {exteriorUV.x, exteriorUV.y
                              + (exteriorUV.w - exteriorUV.y)
                                  * walls[w].doorSpaces[doorNb].door.height
                                  / maxH});                // top Left
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(
        uvs,
        {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
         exteriorUV.y
           + (exteriorUV.w - exteriorUV.y)
               * walls[w].doorSpaces[doorNb].door.height / maxH}); // top right
      stl_util::concat(
        uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
              exteriorUV.y}); // base right

      stl_util::concat(indices, {_nbIndices, _nbIndices + 3, _nbIndices + 2,
                                 _nbIndices, _nbIndices + 1, _nbIndices + 3});
    }
    --doorsRemaining;
    ++doorNb;

    while (doorsRemaining > 0) {
      // Base
      nbIndices       = positions.size() / 3; // current number of indices
      auto _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerDoorCorners[w][doorNb - 1][3].x,
                                   innerDoorCorners[w][doorNb - 1][3].y,
                                   innerDoorCorners[w][doorNb - 1][3].z}); // bl
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][0].x,
                                   innerDoorCorners[w][doorNb][0].y,
                                   innerDoorCorners[w][doorNb][0].z}); // br
      stl_util::concat(positions, {outerDoorCorners[w][doorNb - 1][3].x,
                                   outerDoorCorners[w][doorNb - 1][3].y,
                                   outerDoorCorners[w][doorNb - 1][3].z}); // tl
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][0].x,
                                   outerDoorCorners[w][doorNb][0].y,
                                   outerDoorCorners[w][doorNb][0].z}); // tr

      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(
        uvs, {exteriorUV.x
                + (exteriorUV.z - exteriorUV.x)
                    * (walls[w].doorSpaces[doorNb].left
                       - (walls[w].doorSpaces[doorNb - 1].left
                          + walls[w].doorSpaces[doorNb - 1].door.width))
                    / maxL / maxL,
              exteriorUV.y}); // base right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y
                                             + (exteriorUV.w - exteriorUV.y)
                                                 * ply / maxH}); // top Left
      stl_util::concat(
        uvs, {exteriorUV.x
                + (exteriorUV.z - exteriorUV.x)
                    * (walls[w].doorSpaces[doorNb].left
                       - (walls[w].doorSpaces[doorNb - 1].left
                          + walls[w].doorSpaces[doorNb - 1].door.width))
                    / maxL,
              exteriorUV.y
                + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right

      stl_util::concat(indices, {_nbIndices, _nbIndices + 1, _nbIndices + 3,
                                 _nbIndices + 3, _nbIndices + 2, _nbIndices});

      // Left side
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerDoorCorners[w][doorNb][0].x,
                                   innerDoorCorners[w][doorNb][0].y,
                                   innerDoorCorners[w][doorNb][0].z}); // br
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][1].x,
                                   innerDoorCorners[w][doorNb][1].y,
                                   innerDoorCorners[w][doorNb][1].z}); // tr
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][0].x,
                                   outerDoorCorners[w][doorNb][0].y,
                                   outerDoorCorners[w][doorNb][0].z}); // bl
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][1].x,
                                   outerDoorCorners[w][doorNb][1].y,
                                   outerDoorCorners[w][doorNb][1].z}); // tl

      stl_util::concat(
        uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
              exteriorUV.y}); // base right
      stl_util::concat(
        uvs,
        {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
         exteriorUV.y
           + (exteriorUV.w - exteriorUV.y)
               * walls[w].doorSpaces[doorNb].door.height / maxH}); // top right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y});         // base Left
      stl_util::concat(
        uvs, {exteriorUV.x, exteriorUV.y
                              + (exteriorUV.w - exteriorUV.y)
                                  * walls[w].doorSpaces[doorNb].door.height
                                  / maxH}); // top Left

      stl_util::concat(indices, {_nbIndices, _nbIndices + 1, _nbIndices + 3,
                                 _nbIndices, _nbIndices + 3, _nbIndices + 2});

      // Top
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerDoorCorners[w][doorNb][1].x,
                                   innerDoorCorners[w][doorNb][1].y,
                                   innerDoorCorners[w][doorNb][1].z}); // bl
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][2].x,
                                   innerDoorCorners[w][doorNb][2].y,
                                   innerDoorCorners[w][doorNb][2].z}); // br
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][1].x,
                                   outerDoorCorners[w][doorNb][1].y,
                                   outerDoorCorners[w][doorNb][1].z}); // tl
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][2].x,
                                   outerDoorCorners[w][doorNb][2].y,
                                   outerDoorCorners[w][doorNb][2].z}); // tr

      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(uvs,
                       {exteriorUV.x
                          + (exteriorUV.z - exteriorUV.x)
                              * walls[w].doorSpaces[doorNb].door.width / maxL,
                        exteriorUV.y}); // base right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y
                                             + (exteriorUV.w - exteriorUV.y)
                                                 * ply / maxH}); // top Left
      stl_util::concat(
        uvs, {exteriorUV.x
                + (exteriorUV.z - exteriorUV.x)
                    * walls[w].doorSpaces[doorNb].door.width / maxL,
              exteriorUV.y
                + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right

      stl_util::concat(indices, {_nbIndices + 2, _nbIndices + 1, _nbIndices + 3,
                                 _nbIndices + 2, _nbIndices, _nbIndices + 1});

      // Right side
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerDoorCorners[w][doorNb][2].x,
                                   innerDoorCorners[w][doorNb][2].y,
                                   innerDoorCorners[w][doorNb][2].z}); // tl
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][3].x,
                                   innerDoorCorners[w][doorNb][3].y,
                                   innerDoorCorners[w][doorNb][3].z}); // bl
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][2].x,
                                   outerDoorCorners[w][doorNb][2].y,
                                   outerDoorCorners[w][doorNb][2].z}); // tr
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][3].x,
                                   outerDoorCorners[w][doorNb][3].y,
                                   outerDoorCorners[w][doorNb][3].z}); // br

      stl_util::concat(
        uvs, {exteriorUV.x, exteriorUV.y
                              + (exteriorUV.w - exteriorUV.y)
                                  * walls[w].doorSpaces[doorNb].door.height
                                  / maxH});                // top Left
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(
        uvs,
        {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
         exteriorUV.y
           + (exteriorUV.w - exteriorUV.y)
               * walls[w].doorSpaces[doorNb].door.height / maxH}); // top right
      stl_util::concat(
        uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
              exteriorUV.y}); // base right

      stl_util::concat(indices, {_nbIndices, _nbIndices + 3, _nbIndices + 2,
                                 _nbIndices, _nbIndices + 1, _nbIndices + 3});

      --doorsRemaining;
      ++doorNb;
    }

    --doorNb;
    nbIndices       = positions.size() / 3; // current number of indices
    auto _nbIndices = static_cast<uint32_t>(nbIndices);

    // Final base
    if (doors > 0) {
      stl_util::concat(positions, {innerDoorCorners[w][doorNb][3].x,
                                   innerDoorCorners[w][doorNb][3].y,
                                   innerDoorCorners[w][doorNb][3].z}); // bl
      stl_util::concat(positions,
                       {innerBaseCorners[(w + 1) % nbWalls].x,
                        innerBaseCorners[(w + 1) % nbWalls].y,
                        innerBaseCorners[(w + 1) % nbWalls].z}); // br
      stl_util::concat(positions, {outerDoorCorners[w][doorNb][3].x,
                                   outerDoorCorners[w][doorNb][3].y,
                                   outerDoorCorners[w][doorNb][3].z}); // tl
      stl_util::concat(positions,
                       {outerBaseCorners[(w + 1) % nbWalls].x,
                        outerBaseCorners[(w + 1) % nbWalls].y,
                        outerBaseCorners[(w + 1) % nbWalls].z}); // tr

      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(uvs,
                       {exteriorUV.x
                          + (exteriorUV.z - exteriorUV.x)
                              * (wallLength
                                 - (walls[w].doorSpaces[doorNb].left
                                    + walls[w].doorSpaces[doorNb].door.width))
                              / maxL,
                        exteriorUV.y}); // base right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y
                                             + (exteriorUV.w - exteriorUV.y)
                                                 * ply / maxH}); // top Left
      stl_util::concat(
        uvs, {exteriorUV.x
                + (exteriorUV.z - exteriorUV.x)
                    * (wallLength
                       - (walls[w].doorSpaces[doorNb].left
                          + walls[w].doorSpaces[doorNb].door.width))
                    / maxL,
              exteriorUV.y
                + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right
    }
    else {
      stl_util::concat(positions, {innerBaseCorners[w].x, innerBaseCorners[w].y,
                                   innerBaseCorners[w].z}); // bl
      stl_util::concat(positions,
                       {innerBaseCorners[(w + 1) % nbWalls].x,
                        innerBaseCorners[(w + 1) % nbWalls].y,
                        innerBaseCorners[(w + 1) % nbWalls].z}); // br
      stl_util::concat(positions, {outerBaseCorners[w].x, outerBaseCorners[w].y,
                                   outerBaseCorners[w].z}); // tl
      stl_util::concat(positions,
                       {outerBaseCorners[(w + 1) % nbWalls].x,
                        outerBaseCorners[(w + 1) % nbWalls].y,
                        outerBaseCorners[(w + 1) % nbWalls].z}); // tr

      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(
        uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * wallLength / maxL,
              exteriorUV.y}); // base right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y
                                             + (exteriorUV.w - exteriorUV.y)
                                                 * ply / maxH}); // top Left
      stl_util::concat(
        uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * wallLength / maxL,
              exteriorUV.y
                + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right
    }
    stl_util::concat(indices, {_nbIndices, _nbIndices + 1, _nbIndices + 3,
                               _nbIndices + 3, _nbIndices + 2, _nbIndices});

    // Construct facets for window base, top and sides, repeating positions for
    // flatshaded mesh
    for (size_t ww = 0; ww < innerWindowCorners[w].size(); ++ww) {
      // Left side
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerWindowCorners[w][ww][3].x,
                                   innerWindowCorners[w][ww][3].y,
                                   innerWindowCorners[w][ww][3].z}); // tr
      stl_util::concat(positions, {innerWindowCorners[w][ww][0].x,
                                   innerWindowCorners[w][ww][0].y,
                                   innerWindowCorners[w][ww][0].z}); // br
      stl_util::concat(positions, {outerWindowCorners[w][ww][3].x,
                                   outerWindowCorners[w][ww][3].y,
                                   outerWindowCorners[w][ww][3].z}); // tl
      stl_util::concat(positions, {outerWindowCorners[w][ww][0].x,
                                   outerWindowCorners[w][ww][0].y,
                                   outerWindowCorners[w][ww][0].z}); // bl

      stl_util::concat(
        uvs,
        {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
         exteriorUV.y
           + (exteriorUV.w - exteriorUV.y)
               * walls[w].windowSpaces[ww].window.height / maxH}); // top right
      stl_util::concat(
        uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
              exteriorUV.y}); // base right
      stl_util::concat(
        uvs, {exteriorUV.x, exteriorUV.y
                              + (exteriorUV.w - exteriorUV.y)
                                  * walls[w].windowSpaces[ww].window.height
                                  / maxH});                // top Left
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left

      stl_util::concat(indices, {_nbIndices + 1, _nbIndices, _nbIndices + 3,
                                 _nbIndices + 2, _nbIndices + 3, _nbIndices});

      // Base
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerWindowCorners[w][ww][0].x,
                                   innerWindowCorners[w][ww][0].y,
                                   innerWindowCorners[w][ww][0].z}); // tl
      stl_util::concat(positions, {innerWindowCorners[w][ww][1].x,
                                   innerWindowCorners[w][ww][1].y,
                                   innerWindowCorners[w][ww][1].z}); // tr
      stl_util::concat(positions, {outerWindowCorners[w][ww][0].x,
                                   outerWindowCorners[w][ww][0].y,
                                   outerWindowCorners[w][ww][0].z}); // bl
      stl_util::concat(positions, {outerWindowCorners[w][ww][1].x,
                                   outerWindowCorners[w][ww][1].y,
                                   outerWindowCorners[w][ww][1].z}); // br

      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y
                                             + (exteriorUV.w - exteriorUV.y)
                                                 * ply / maxH}); // top Left
      stl_util::concat(
        uvs, {exteriorUV.x
                + (exteriorUV.z - exteriorUV.x)
                    * walls[w].windowSpaces[ww].window.width / maxL,
              exteriorUV.y
                + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y});      // base Left
      stl_util::concat(uvs,
                       {exteriorUV.x
                          + (exteriorUV.z - exteriorUV.x)
                              * walls[w].windowSpaces[ww].window.width / maxL,
                        exteriorUV.y}); // base right

      stl_util::concat(indices, {_nbIndices + 1, _nbIndices, _nbIndices + 3,
                                 _nbIndices + 3, _nbIndices, _nbIndices + 2});

      // Right side
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerWindowCorners[w][ww][1].x,
                                   innerWindowCorners[w][ww][1].y,
                                   innerWindowCorners[w][ww][1].z}); // bl
      stl_util::concat(positions, {innerWindowCorners[w][ww][2].x,
                                   innerWindowCorners[w][ww][2].y,
                                   innerWindowCorners[w][ww][2].z}); // tl
      stl_util::concat(positions, {outerWindowCorners[w][ww][1].x,
                                   outerWindowCorners[w][ww][1].y,
                                   outerWindowCorners[w][ww][1].z}); // br
      stl_util::concat(positions, {outerWindowCorners[w][ww][2].x,
                                   outerWindowCorners[w][ww][2].y,
                                   outerWindowCorners[w][ww][2].z}); // tr

      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(
        uvs, {exteriorUV.x, exteriorUV.y
                              + (exteriorUV.w - exteriorUV.y)
                                  * walls[w].windowSpaces[ww].window.height
                                  / maxH}); // top Left
      stl_util::concat(
        uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x) * ply / maxL,
              exteriorUV.y}); // base right
      stl_util::concat(uvs, {exteriorUV.x + (exteriorUV.z - exteriorUV.x),
                             exteriorUV.y
                               + (exteriorUV.w - exteriorUV.y)
                                   * walls[w].windowSpaces[ww].window.height
                                   / maxH}); // top right

      stl_util::concat(indices, {_nbIndices + 1, _nbIndices + 2, _nbIndices + 3,
                                 _nbIndices, _nbIndices + 2, _nbIndices + 1});

      // Top
      nbIndices  = positions.size() / 3; // current number of indices
      _nbIndices = static_cast<uint32_t>(nbIndices);

      stl_util::concat(positions, {innerWindowCorners[w][ww][2].x,
                                   innerWindowCorners[w][ww][2].y,
                                   innerWindowCorners[w][ww][2].z}); // br
      stl_util::concat(positions, {innerWindowCorners[w][ww][3].x,
                                   innerWindowCorners[w][ww][3].y,
                                   innerWindowCorners[w][ww][3].z}); // bl
      stl_util::concat(positions, {outerWindowCorners[w][ww][2].x,
                                   outerWindowCorners[w][ww][2].y,
                                   outerWindowCorners[w][ww][2].z}); // tr
      stl_util::concat(positions, {outerWindowCorners[w][ww][3].x,
                                   outerWindowCorners[w][ww][3].y,
                                   outerWindowCorners[w][ww][3].z}); // tl

      stl_util::concat(uvs,
                       {exteriorUV.x
                          + (exteriorUV.z - exteriorUV.x)
                              * walls[w].windowSpaces[ww].window.width / maxL,
                        exteriorUV.y});                    // base right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
      stl_util::concat(
        uvs, {exteriorUV.x
                + (exteriorUV.z - exteriorUV.x)
                    * walls[w].windowSpaces[ww].window.width / maxL,
              exteriorUV.y
                + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right
      stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y
                                             + (exteriorUV.w - exteriorUV.y)
                                                 * ply / maxH}); // top Left

      stl_util::concat(indices, {_nbIndices + 3, _nbIndices, _nbIndices + 2,
                                 _nbIndices + 1, _nbIndices, _nbIndices + 3});
    }

    // Construction of top of wall facets
    nbIndices  = positions.size() / 3; // current number of indices
    _nbIndices = static_cast<uint32_t>(nbIndices);

    stl_util::concat(positions, {innerTopCorners[w].x, innerTopCorners[w].y,
                                 innerTopCorners[w].z}); // tl
    stl_util::concat(positions, {innerTopCorners[(w + 1) % nbWalls].x,
                                 innerTopCorners[(w + 1) % nbWalls].y,
                                 innerTopCorners[(w + 1) % nbWalls].z}); // tr
    stl_util::concat(positions, {outerTopCorners[w].x, outerTopCorners[w].y,
                                 outerTopCorners[w].z}); // bl
    stl_util::concat(positions, {outerTopCorners[(w + 1) % nbWalls].x,
                                 outerTopCorners[(w + 1) % nbWalls].y,
                                 outerTopCorners[(w + 1) % nbWalls].z}); // br

    uvx = exteriorUV.x + 0.5f * wallDiff * (exteriorUV.z - exteriorUV.x) / maxL;
    stl_util::concat(
      uvs, {uvx, exteriorUV.y
                   + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top Left

    uvx
      = exteriorUV.x
        + (0.5f * wallDiff + wallLength) * (exteriorUV.z - exteriorUV.x) / maxL;
    stl_util::concat(
      uvs, {uvx, exteriorUV.y
                   + (exteriorUV.w - exteriorUV.y) * ply / maxH}); // top right

    stl_util::concat(uvs, {exteriorUV.x, exteriorUV.y}); // base Left
    stl_util::concat(uvs, {exteriorUV.x
                             + (exteriorUV.z - exteriorUV.x)
                                 * exteriorWallLength / (maxL + wallDiff),
                           exteriorUV.y}); // base right

    stl_util::concat(indices, {_nbIndices + 1, _nbIndices, _nbIndices + 3,
                               _nbIndices + 2, _nbIndices + 3, _nbIndices});

    for (size_t p = interiorIndex; p < positions.size() / 3; ++p) {
      stl_util::concat(colors, {exteriorColor.r, exteriorColor.g,
                                exteriorColor.b, exteriorColor.a});
    }
  }

  VertexData::ComputeNormals(positions, indices, normals);
  VertexData::_ComputeSides(Mesh::FRONTSIDE, positions, indices, normals, uvs);

  // Create a custom mesh
  auto customMesh = Mesh::New("custom", scene);

  // Result
  auto vertexData = std::make_unique<VertexData>();

  // Assign positions and indices to vertexData
  vertexData->positions = std::move(positions);
  vertexData->indices   = std::move(indices);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);
  vertexData->colors    = std::move(colors);

  // Apply vertexData to custom mesh
  vertexData->applyToMesh(*customMesh);

  return customMesh;
}

} // end of namespace Samples
} // end of namespace BABYLON
