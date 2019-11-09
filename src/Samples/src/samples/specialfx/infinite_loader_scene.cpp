#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>
#include <vector>

namespace BABYLON {
namespace Samples {

struct Range {
  float min;
  float max;
}; // end of struct Range

struct Ring {
  Ring() : center{nullptr}, angle{0.f}
  {
  }

  Ring(const Ring& other) : center{other.center}, tubes{other.tubes}, angle{other.angle}
  {
  }

  void addTube(const std::vector<Vector3>& points)
  {
    auto tube = Mesh::CreateTube("tube", points, 1, 3, nullptr, Mesh::CAP_ALL, center->getScene(),
                                 false, Mesh::FRONTSIDE);
    tube->parent = center.get();
    tubes.emplace_back(tube);
  }

  MeshPtr center;
  std::vector<MeshPtr> tubes;
  float angle;
}; // end of struct Ring

/**
 * @brief Hypnotizing infinite loader scene. Example demonstrating how to create visually apealing
 * loading animation.
 * @see https://www.babylonjs-playground.com/#VUJG1#1
 */
class InfiniteLoaderScene : public IRenderableScene {

public:
  InfiniteLoaderScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~InfiniteLoaderScene() override = default;

  const char* getName() override
  {
    return "Infinite Loader Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // camera
    auto camera = ArcRotateCamera::New("camera1", -Math::PI_2, 0.f, 70.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    auto light       = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 1.f;

    // Returns the points in range
    const auto getPointsInRange = [](unsigned int nb, float width, const Range& range) {
      std::vector<Vector3> points;
      float alpha = 0;

      for (unsigned int i = 0; i < nb; ++i) {
        if (alpha >= range.min && alpha <= range.max) {
          points.emplace_back(Vector3(width * std::cos(alpha), 0, width * std::sin(alpha)));
        }
        alpha += Math::PI2 / static_cast<float>(nb);
      }
      return points;
    };

    // Create a ring
    const auto createRing = [&](unsigned int nb, float width, const std::vector<Range>& ranges) {
      Ring ring;
      ring.center = Mesh::CreateBox("center", 1.f, scene);

      for (auto& range : ranges) {
        ring.addTube(getPointsInRange(nb, width, range));
      }
      return ring;
    };

    unsigned int nbRings = 10;
    float w              = 5.f;
    float angle          = 0.005f;

    std::array<StandardMaterialPtr, 2> materials;

    auto mat1          = StandardMaterial::New("mat1", scene);
    mat1->diffuseColor = Color3::White();
    materials[0]       = mat1;

    auto mat2           = StandardMaterial::New("mat2", scene);
    mat2->diffuseColor  = Color3::Black();
    mat2->specularColor = Color3::Black();
    materials[1]        = mat2;

    for (unsigned int c = 1; c <= nbRings; ++c) {
      float cf = static_cast<float>(c);
      std::vector<Range> ranges{{Math::PI / 4.f, 3.f * Math::PI / 4.f},
                                {5.f * Math::PI / 4.f, 7.f * Math::PI / 4.f}};
      Ring ring  = createRing(30 * c, w + 1.5f * cf, ranges);
      ring.angle = angle * cf;
      for (auto& tube : ring.tubes) {
        tube->material = materials[c % 2];
      }
      _rings.emplace_back(ring);
    }

    _scene->registerBeforeRender([this](Scene*, EventState&) {
      for (auto& ring : _rings) {
        ring.center->rotation().y += ring.angle;
      }
    });
  }

private:
  std::vector<Ring> _rings;

}; // end of class InfiniteLoaderScene

BABYLON_REGISTER_SAMPLE("Special FX", InfiniteLoaderScene)

} // end of namespace Samples
} // end of namespace BABYLON
