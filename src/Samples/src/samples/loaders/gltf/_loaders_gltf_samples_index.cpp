#include <babylon/samples/loaders/gltf/_loaders_gtlf_samples_index.h>

// Loaders plugins
#ifdef WITH_LOADERS
// -- Simple models for testing individual features --
#include <babylon/samples/loaders/gltf/simplemodels/animated_morph_cube_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/animated_morph_sphere_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/animated_triangle_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/cameras_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/interpolation_test_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_material_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_meshes_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_morph_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_skin_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_sparse_accessor_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_texture_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/triangle_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/triangle_without_indices_scene.h>

// -- More complex models --
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_interleaved_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_textured_non_power_of_two_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_textured_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_vertex_colors_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/duck_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/reciprocating_saw_scene.h>

// -- PBR models --
#include <babylon/samples/loaders/gltf/pbrmodels/avocado_scene.h>
#include <babylon/samples/loaders/gltf/pbrmodels/barramundi_fish_scene.h>
#include <babylon/samples/loaders/gltf/pbrmodels/boom_box_scene.h>
#include <babylon/samples/loaders/gltf/pbrmodels/corset_scene.h>
#include <babylon/samples/loaders/gltf/pbrmodels/damaged_helmet_scene.h>
#include <babylon/samples/loaders/gltf/pbrmodels/flight_helmet_scene.h>
#include <babylon/samples/loaders/gltf/pbrmodels/lantern_scene.h>
#include <babylon/samples/loaders/gltf/pbrmodels/water_bottle_scene.h>

// -- Further PBR models --
#include <babylon/samples/loaders/gltf/furtherpbrmodels/animated_cube_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/antique_camera_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/cube_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/sci_fi_helmet_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/suzanne_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/two_sided_plane_scene.h>

// -- Feature Test Models --
#include <babylon/samples/loaders/gltf/featuretestmodels/alpha_blend_mode_test_scene.h>

#endif

namespace BABYLON {
namespace Samples {

_LoadersGLTFSamplesIndex::_LoadersGLTFSamplesIndex()
{

#ifdef WITH_LOADERS
  // -- Simple models for testing individual features --

  // Animated Morph Cube Scene (glTF)
  _samples["AnimatedMorphCubeGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<AnimatedMorphCubeScene>(iCanvas);
  };
  // Animated Morph Sphere Scene (glTF)
  _samples["AnimatedMorphSphereGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<AnimatedMorphSphereScene>(iCanvas);
  };
  // Animated Triangle Scene (glTF)
  _samples["AnimatedTriangleGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<AnimatedTriangleScene>(iCanvas);
  };
  // Cameras Scene (glTF)
  _samples["CamerasGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<CamerasGLTFScene>(iCanvas);
  };
  // Interpolation Test Scene (glTF)
  _samples["InterpolationTestGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<InterpolationTestScene>(iCanvas);
  };
  // Simple Material Scene (glTF)
  _samples["SimpleMaterialGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleMaterialGLTFScene>(iCanvas);
  };
  // Simple Meshes Scene (glTF)
  _samples["SimpleMeshesGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleMeshesScene>(iCanvas);
  };
  // Simple Morph Scene (glTF)
  _samples["SimpleMorphGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleMorphScene>(iCanvas);
  };
  // Simple Skin Scene (glTF)
  _samples["SimpleSkinGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleSkinScene>(iCanvas);
  };
  // Simple Sparse Accessor Scene (glTF)
  _samples["SimpleSparseAccessorGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleSparseAccessorScene>(iCanvas);
  };
  // Simple Texture Scene (glTF)
  _samples["SimpleTextureGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleTextureScene>(iCanvas);
  };
  // Triangle Scene (glTF)
  _samples["TriangleGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<TriangleScene>(iCanvas); };
  // Triangle Without Indices Scene (glTF)
  _samples["TriangleWithoutIndicesGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<TriangleWithoutIndicesScene>(iCanvas);
  };

  // -- More complex models --
  // Box Interleaved Scene (glTF)
  _samples["BoxInterleavedGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<BoxInterleavedScene>(iCanvas);
  };
  // Box Scene (glTF)
  _samples["BoxGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<BoxScene>(iCanvas); };
  // Box Textured Non Power Of Two Scene (glTF)
  _samples["BoxTexturedNonPowerOfTwoGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<BoxTexturedNonPowerOfTwoScene>(iCanvas);
  };
  // Box Textured Scene (glTF)
  _samples["BoxTexturedGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<BoxTexturedScene>(iCanvas);
  };
  // Box Vertex Colors Scene (glTF)
  _samples["BoxVertexColorsGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<BoxVertexColorsScene>(iCanvas);
  };
  // Duck Scene (glTF)
  _samples["DuckGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<DuckScene>(iCanvas); };
  // Reciprocating Saw Scene (glTF)
  _samples["ReciprocatingSawGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ReciprocatingSawScene>(iCanvas);
  };

  // -- PBR models --

  // Avocado Scene (glTF)
  _samples["AvocadoGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<AvocadoScene>(iCanvas); };
  // Barramundi Fish Scene (glTF)
  _samples["BarramundiFishGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<BarramundiFishScene>(iCanvas);
  };
  // Boom Box Scene (glTF)
  _samples["BoomBoxGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<BoomBoxScene>(iCanvas); };
  // Corset Scene (glTF)
  _samples["CorsetGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<CorsetScene>(iCanvas); };
  // Damaged Helmet Scene (glTF)
  _samples["DamagedHelmetGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<DamagedHelmetScene>(iCanvas);
  };
  // Flight Helmet Scene (glTF)
  _samples["FlightHelmetGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<FlightHelmetScene>(iCanvas);
  };
  // Lantern Scene (glTF)
  _samples["LanternGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<LanternScene>(iCanvas); };
  // Water Bottle Scene (glTF)
  _samples["WaterBottleGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<WaterBottleScene>(iCanvas);
  };

  // -- Further PBR models --

  // Animated Cube Scene (glTF)
  _samples["AnimatedCubeGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<AnimatedCubeScene>(iCanvas);
  };
  // Antique Camera Scene (glTF)
  _samples["AntiqueCameraGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<AntiqueCameraScene>(iCanvas);
  };
  // Cube Scene (glTF)
  _samples["CubeGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<CubeScene>(iCanvas); };
  // Sci Fi Helmet Scene (glTF)
  _samples["SciFiHelmetGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SciFiHelmetScene>(iCanvas);
  };
  // Suzanne Scene (glTF)
  _samples["SuzanneGLTFScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<SuzanneScene>(iCanvas); };
  // Two Sided Plane Scene (glTF)
  _samples["TwoSidedPlaneGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<TwoSidedPlaneScene>(iCanvas);
  };

  // -- Feature Test Models --

  // Alpha Blend Mode Test Scene (glTF)
  _samples["AlphaBlendModeTestGLTFScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<AlphaBlendModeTestScene>(iCanvas);
  };
#endif
}

} // end of namespace Samples
} // end of namespace BABYLON
