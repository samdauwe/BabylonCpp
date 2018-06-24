#include <babylon/samples/meshes/_meshes_samples_index.h>

#include <babylon/samples/meshes/basic_elements_scene.h>
#include <babylon/samples/meshes/basic_scene.h>
#include <babylon/samples/meshes/circle_curves_from_beziers.h>
#include <babylon/samples/meshes/cuboid_scene.h>
#include <babylon/samples/meshes/curved_helix_meshes_scene.h>
#include <babylon/samples/meshes/extruded_polygon_scene.h>
#include <babylon/samples/meshes/extrusion_scene.h>
#include <babylon/samples/meshes/height_map_scene.h>
#include <babylon/samples/meshes/house_from_floorplan.h>
#include <babylon/samples/meshes/ico_sphere_scene.h>
#include <babylon/samples/meshes/lathe_scene.h>
#include <babylon/samples/meshes/lines_mesh_spiral_scene.h>
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

  // Basic Elements Scene
  _samples["BasicElementsScene"] = ::std::make_tuple(
    true,                                                     //
    [](ICanvas* iCanvas) {                                    //
      return ::std::make_unique<BasicElementsScene>(iCanvas); //
    });                                                       //
  // Basic Scene
  _samples["BasicScene"]
    = ::std::make_tuple(true,                                             //
                        [](ICanvas* iCanvas) {                            //
                          return ::std::make_unique<BasicScene>(iCanvas); //
                        });                                               //

  // Circle Curves From Beziers Scene
  _samples["CircleCurvesFromBeziersScene"] = ::std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return ::std::make_unique<CircleCurvesFromBeziersScene>(iCanvas); //
    });                                                                 //
  // Cuboid Scene
  _samples["CuboidScene"]
    = ::std::make_tuple(true,                                              //
                        [](ICanvas* iCanvas) {                             //
                          return ::std::make_unique<CuboidScene>(iCanvas); //
                        });                                                //
  // Curved Helix Meshes Scene
  _samples["CurvedHelixMeshesScene"] = ::std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return ::std::make_unique<CurvedHelixMeshesScene>(iCanvas); //
    });                                                           //
  // Extruded Polygon Scene
  _samples["ExtrudedPolygonScene"] = ::std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<ExtrudedPolygonScene>(iCanvas); //
    });                                                         //
  // Extrusion Scene
  _samples["ExtrusionScene"]
    = ::std::make_tuple(true,                                                 //
                        [](ICanvas* iCanvas) {                                //
                          return ::std::make_unique<ExtrusionScene>(iCanvas); //
                        });                                                   //
  // Height Map Scene
  _samples["HeightMapScene"]
    = ::std::make_tuple(true,                                                 //
                        [](ICanvas* iCanvas) {                                //
                          return ::std::make_unique<HeightMapScene>(iCanvas); //
                        });                                                   //
  // House from a Floorplan Scene
  _samples["HouseFromFloorplanScene"] = ::std::make_tuple(
    true,                                                          //
    [](ICanvas* iCanvas) {                                         //
      return ::std::make_unique<HouseFromFloorplanScene>(iCanvas); //
    });                                                            //
  // Icosphere Scene
  _samples["IcoSphereScene"]
    = ::std::make_tuple(true,                                                 //
                        [](ICanvas* iCanvas) {                                //
                          return ::std::make_unique<IcoSphereScene>(iCanvas); //
                        });                                                   //
  // Lathe Scene
  _samples["LatheScene"]
    = ::std::make_tuple(true,                                             //
                        [](ICanvas* iCanvas) {                            //
                          return ::std::make_unique<LatheScene>(iCanvas); //
                        });                                               //

  // Lines Mesh Spiral Scene
  _samples["LinesMeshSpiralScene"] = ::std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<LinesMeshSpiralScene>(iCanvas); //
    });                                                         //
  // Lorenz Attractor Scene
  _samples["LorenzAttractorScene"] = ::std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<LorenzAttractorScene>(iCanvas); //
    });                                                         //
  // Mandelbrot Fractal Scene
  _samples["MandelbrotFractalScene"] = ::std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return ::std::make_unique<MandelbrotFractalScene>(iCanvas); //
    });                                                           //
  // Merged Meshes Scene
  _samples["MergedMeshesScene"] = ::std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return ::std::make_unique<MergedMeshesScene>(iCanvas); //
    });                                                      //
  // Mesh Normals Scene
  _samples["MeshNormalsScene"] = ::std::make_tuple(
    false,                                                  //
    [](ICanvas* iCanvas) {                                  //
      return ::std::make_unique<MeshNormalsScene>(iCanvas); //
    });                                                     //
  // Polyhedron Scene
  _samples["PolyhedronScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                 //
      return ::std::make_unique<PolyhedronScene>(iCanvas); //
    });                                                    //
  // Ribbon Scene
  _samples["RibbonScene"]
    = ::std::make_tuple(true,                                              //
                        [](ICanvas* iCanvas) {                             //
                          return ::std::make_unique<RibbonScene>(iCanvas); //
                        });                                                //
  // Rotation and Scaling Scene
  _samples["RotationAndScalingScene"] = ::std::make_tuple(
    true,                                                          //
    [](ICanvas* iCanvas) {                                         //
      return ::std::make_unique<RotationAndScalingScene>(iCanvas); //
    });                                                            //
  // Simple Car Following Path Scene
  _samples["SimpleCarFollowingPathScene"] = ::std::make_tuple(
    false,
    [](ICanvas* iCanvas) {                                             //
      return ::std::make_unique<SimpleCarFollowingPathScene>(iCanvas); //
    });                                                                //
  // Super Ellipsoid Scene
  _samples["SuperEllipsoidScene"] = ::std::make_tuple(
    true,                                                      //
    [](ICanvas* iCanvas) {                                     //
      return ::std::make_unique<SuperEllipsoidScene>(iCanvas); //
    });                                                        //
  // Tiled Ground Scene
  _samples["TiledGroundScene"] = ::std::make_tuple(
    false,                                                  //
    [](ICanvas* iCanvas) {                                  //
      return ::std::make_unique<TiledGroundScene>(iCanvas); //
    });                                                     //
  // Tube Scene
  _samples["TubeScene"]
    = ::std::make_tuple(true,                                            //
                        [](ICanvas* iCanvas) {                           //
                          return ::std::make_unique<TubeScene>(iCanvas); //
                        });                                              //

  // -- Polygon Mesh examples --

  // Polygon Mesh Bird Scene
  _samples["PolygonMeshBirdScene"] = ::std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<PolygonMeshBirdScene>(iCanvas); //
    });                                                         //
  // Polygon Mesh Monkey Scene
  _samples["PolygonMeshMonkeyScene"] = ::std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return ::std::make_unique<PolygonMeshMonkeyScene>(iCanvas); //
    });                                                           //
  // Polygon Mesh Puzzle Piece Scene
  _samples["PolygonMeshPuzzlePieceScene"] = ::std::make_tuple(
    true,                                                              //
    [](ICanvas* iCanvas) {                                             //
      return ::std::make_unique<PolygonMeshPuzzlePieceScene>(iCanvas); //
    });                                                                //
  // Polygon Mesh Stars Scene
  _samples["PolygonMeshStarsScene"] = ::std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return ::std::make_unique<PolygonMeshStarsScene>(iCanvas); //
    });                                                          //
  // Polygon Mesh Stars With Holes Scene
  _samples["PolygonMeshStarsWithHolesScene"] = ::std::make_tuple(
    true,                                                                 //
    [](ICanvas* iCanvas) {                                                //
      return ::std::make_unique<PolygonMeshStarsWithHolesScene>(iCanvas); //
    });                                                                   //
}

_MeshesSamplesIndex::~_MeshesSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
