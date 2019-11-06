#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/null_engine.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/vertex_buffer.h>

std::unique_ptr<BABYLON::Engine> createSubject()
{
  using namespace BABYLON;
  NullEngineOptions options;
  options.renderHeight          = 256;
  options.renderWidth           = 256;
  options.textureSize           = 256;
  options.deterministicLockstep = false;
  options.lockstepMaxSteps      = 1;
  return NullEngine::New(options);
}

TEST(TestGeometry, TestVec3FloatColor)
{
  using namespace BABYLON;
  // vec3 float color tightly packed
  auto subject = createSubject();
  auto scene   = Scene::New(subject.get());
  Float32Array data{0.4f, 0.4f, 0.4f, 0.6f, 0.6f, 0.6f,
                    0.8f, 0.8f, 0.8f, 1.f,  1.f,  1.f};
  auto buffer       = std::make_unique<Buffer>(subject.get(), data, false);
  auto vertexBuffer = std::make_shared<VertexBuffer>(
    subject.get(), buffer.get(), VertexBuffer::ColorKind, false, std::nullopt,
    std::nullopt, std::nullopt, std::nullopt, 3);

  auto geometry = Geometry::New("geometry1", scene.get());
  geometry->setVerticesBuffer(vertexBuffer);
  IndicesArray indices{0u, 1u, 2u, 3u};
  geometry->setIndices(indices, 4);

  auto result = geometry->getVerticesData(VertexBuffer::ColorKind);
  EXPECT_THAT(result, ::testing::ContainerEq(data));
}
