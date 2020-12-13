#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../test_utils.h"

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/vertex_buffer.h>

TEST(TestGeometry, TestGetVerticesData_Vec3FloatColor)
{
  using namespace BABYLON;
  // vec3 float color tightly packed
  auto subject = createSubject();
  auto scene   = Scene::New(subject.get());
  Float32Array data{0.4f, 0.4f, 0.4f, 0.6f, 0.6f, 0.6f, 0.8f, 0.8f, 0.8f, 1.f, 1.f, 1.f};
  auto buffer = std::make_shared<Buffer>(subject.get(), data, false);
  auto vertexBuffer
    = std::make_shared<VertexBuffer>(subject.get(), buffer, VertexBuffer::ColorKind, false,
                                     std::nullopt, std::nullopt, std::nullopt, std::nullopt, 3);

  auto geometry = Geometry::New("geometry1", scene.get());
  geometry->setVerticesBuffer(vertexBuffer);
  IndicesArray indices{0u, 1u, 2u, 3u};
  geometry->setIndices(indices, 4);

  auto result = geometry->getVerticesData(VertexBuffer::ColorKind);
  EXPECT_THAT(result, ::testing::ContainerEq(data));
}
