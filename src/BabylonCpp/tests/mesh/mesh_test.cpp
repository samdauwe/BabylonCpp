#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../test_utils.h"

#include <babylon/engines/scene.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>

TEST(Mesh, clone)
{
  using namespace BABYLON;
  {
    auto engine = createSubject();
    auto scene  = BABYLON::Scene::New(engine.get());
    SphereOptions sphereOptions;
    sphereOptions.diameter = 0.2f;
    auto sphere            = MeshBuilder::CreateSphere("sphere", sphereOptions, scene.get());
    auto sphereClone       = sphere->clone("sphereClone");
    EXPECT_EQ(sphere->name, "sphere");
    EXPECT_EQ(sphereClone->name, "sphereClone");
    EXPECT_NE(sphere->uniqueId, sphereClone->uniqueId);
  }
}
