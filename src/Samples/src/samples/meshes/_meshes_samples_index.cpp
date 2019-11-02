#include <babylon/samples/meshes/_meshes_samples_index.h>

#include <babylon/samples/meshes/airplane_model_scene.h>
#include <babylon/samples/meshes/basic_elements_scene.h>
#include <babylon/samples/meshes/basic_scene.h>
#include <babylon/samples/meshes/circle_curves_from_beziers.h>
#include <babylon/samples/meshes/colored_cube_faces_scene.h>
#include <babylon/samples/meshes/colored_ribbon_scene.h>
#include <babylon/samples/meshes/colored_triangle_scene.h>
#include <babylon/samples/meshes/constructive_solid_geometries_scene.h>
#include <babylon/samples/meshes/cubes_scene.h>
#include <babylon/samples/meshes/cuboid_scene.h>
#include <babylon/samples/meshes/curved_helix_meshes_scene.h>
#include <babylon/samples/meshes/decals_scene.h>
#include <babylon/samples/meshes/displacement_map_cpu_scene.h>
#include <babylon/samples/meshes/edges_renderer_scene.h>
#include <babylon/samples/meshes/extrude_polygon_scene.h>
#include <babylon/samples/meshes/extruded_polygon_scene.h>
#include <babylon/samples/meshes/extrusion_scene.h>
#include <babylon/samples/meshes/height_map_scene.h>
#include <babylon/samples/meshes/house_from_floorplan.h>
#include <babylon/samples/meshes/ico_sphere_scene.h>
#include <babylon/samples/meshes/inner_mesh_points_scene.h>
#include <babylon/samples/meshes/is_point_inside_mesh_scene.h>
#include <babylon/samples/meshes/lathe_scene.h>
#include <babylon/samples/meshes/lines_mesh_spiral_scene.h>
#include <babylon/samples/meshes/look_at_scene.h>
#include <babylon/samples/meshes/lorenz_attractor_scene.h>
#include <babylon/samples/meshes/mandelbrot_fractal_scene.h>
#include <babylon/samples/meshes/merged_meshes_scene.h>
#include <babylon/samples/meshes/mesh_normals_scene.h>
#include <babylon/samples/meshes/polygonmesh/polygon_mesh_bird_scene.h>
#include <babylon/samples/meshes/polygonmesh/polygon_mesh_monkey_scene.h>
#include <babylon/samples/meshes/polygonmesh/polygon_mesh_puzzle_piece_scene.h>
#include <babylon/samples/meshes/polygonmesh/polygon_mesh_stars_scene.h>
#include <babylon/samples/meshes/polygonmesh/polygon_mesh_stars_with_holes_scene.h>
#include <babylon/samples/meshes/polyhedron_scene.h>
#include <babylon/samples/meshes/ray_helper_scene.h>
#include <babylon/samples/meshes/raycast_on_height_map_scene.h>
#include <babylon/samples/meshes/ribbon_scene.h>
#include <babylon/samples/meshes/rotation_and_scaling_scene.h>
#include <babylon/samples/meshes/simple_car_following_path_scene.h>
#include <babylon/samples/meshes/super_ellipsoid_scene.h>
#include <babylon/samples/meshes/tiled_ground_scene.h>
#include <babylon/samples/meshes/tube_scene.h>

namespace BABYLON {
namespace Samples {

_MeshesSamplesIndex::_MeshesSamplesIndex()
{
  // -- Mesh examples --

  // Airplane Model Scene
  _samples["AirplaneModelScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<AirplaneModelScene>(iCanvas);
  };
  // Basic Elements Scene
  _samples["BasicElementsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<BasicElementsScene>(iCanvas);
  };
  // Basic Scene
  _samples["BasicScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<BasicScene>(iCanvas); };

  // Circle Curves From Beziers Scene
  _samples["CircleCurvesFromBeziersScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<CircleCurvesFromBeziersScene>(iCanvas);
  };
  // Colored Cube Faces Scene
  _samples["ColoredCubeFacesScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ColoredCubeFacesScene>(iCanvas);
  };
  // Colored Ribbon Scene
  _samples["ColoredRibbonScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ColoredRibbonScene>(iCanvas);
  };
  // Colored Triangle Scene
  _samples["ColoredTriangleScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ColoredTriangleScene>(iCanvas);
  };
  // Constructive Solid Geometries Scene
  _samples["ConstructiveSolidGeometriesScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ConstructiveSolidGeometriesScene>(iCanvas);
  };
  // Cubes Scene
  _samples["CubesScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<CubesScene>(iCanvas); };
  // Cuboid Scene
  _samples["CuboidScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<CuboidScene>(iCanvas); };
  // Curved Helix Meshes Scene
  _samples["CurvedHelixMeshesScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<CurvedHelixMeshesScene>(iCanvas);
  };
  // Decals Scene
  _samples["DecalsScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<DecalsScene>(iCanvas); };
  // Displacement map (CPU) Scene
  _samples["DisplacementMapCPUScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<DisplacementMapCPUScene>(iCanvas);
  };
  // Edges Render Scene
  _samples["EdgesRenderScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<EdgesRenderScene>(iCanvas);
  };
  // Extrude Polygon Scene
  _samples["ExtrudePolygonScene"]
    = [](ICanvas* iCanvas) { return MakeExtrudePolygonScene(iCanvas); };
  // Extruded Polygon Scene
  _samples["ExtrudedPolygonScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ExtrudedPolygonScene>(iCanvas);
  };
  // Extrusion Scene
  _samples["ExtrusionScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ExtrusionScene>(iCanvas);
  };
  // Height Map Scene
  _samples["HeightMapScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<HeightMapScene>(iCanvas);
  };
  // House from a Floorplan Scene
  _samples["HouseFromFloorplanScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<HouseFromFloorplanScene>(iCanvas);
  };
  // Icosphere Scene
  _samples["IcoSphereScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<IcoSphereScene>(iCanvas);
  };
  // Inner MeshP oints Scene
  _samples["InnerMeshPointsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<InnerMeshPointsScene>(iCanvas);
  };
  // Is Point Inside Mesh Scene
  _samples["IsPointInsideMeshScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<IsPointInsideMeshScene>(iCanvas);
  };
  // Lathe Scene
  _samples["LatheScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<LatheScene>(iCanvas); };
  // Lines Mesh Spiral Scene
  _samples["LinesMeshSpiralScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<LinesMeshSpiralScene>(iCanvas);
  };
  // Look At Scene
  _samples["LookAtScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<LookAtScene>(iCanvas); };
  // Lorenz Attractor Scene
  _samples["LorenzAttractorScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<LorenzAttractorScene>(iCanvas);
  };
  // Mandelbrot Fractal Scene
  _samples["MandelbrotFractalScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<MandelbrotFractalScene>(iCanvas);
  };
  // Merged Meshes Scene
  _samples["MergedMeshesScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<MergedMeshesScene>(iCanvas);
  };
  // Mesh Normals Scene
  _samples["MeshNormalsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<MeshNormalsScene>(iCanvas);
  };
  // Polyhedron Scene
  _samples["PolyhedronScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PolyhedronScene>(iCanvas);
  };
  // Ray Helper Scene
  _samples["RayHelperScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<RayHelperScene>(iCanvas);
  };
  // Raycast On Height Map Scene
  _samples["RaycastOnHeightMapScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<RaycastOnHeightMapScene>(iCanvas);
  };
  // Ribbon Scene
  _samples["RibbonScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<RibbonScene>(iCanvas); };
  // Rotation and Scaling Scene
  _samples["RotationAndScalingScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<RotationAndScalingScene>(iCanvas);
  };
  // Simple Car Following Path Scene
  _samples["SimpleCarFollowingPathScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleCarFollowingPathScene>(iCanvas);
  };
  // Super Ellipsoid Scene
  _samples["SuperEllipsoidScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SuperEllipsoidScene>(iCanvas);
  };
  // Tiled Ground Scene
  _samples["TiledGroundScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<TiledGroundScene>(iCanvas);
  };
  // Tube Scene
  _samples["TubeScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<TubeScene>(iCanvas); };

  // -- Polygon Mesh examples --

  // Polygon Mesh Bird Scene
  _samples["PolygonMeshBirdScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PolygonMeshBirdScene>(iCanvas);
  };
  // Polygon Mesh Monkey Scene
  _samples["PolygonMeshMonkeyScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PolygonMeshMonkeyScene>(iCanvas);
  };
  // Polygon Mesh Puzzle Piece Scene
  _samples["PolygonMeshPuzzlePieceScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PolygonMeshPuzzlePieceScene>(iCanvas);
  };
  // Polygon Mesh Stars Scene
  _samples["PolygonMeshStarsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PolygonMeshStarsScene>(iCanvas);
  };
  // Polygon Mesh Stars With Holes Scene
  _samples["PolygonMeshStarsWithHolesScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PolygonMeshStarsWithHolesScene>(iCanvas);
  };
}

_MeshesSamplesIndex::~_MeshesSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
