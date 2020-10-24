#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/vertex_data.h>

TEST(TestVertexData, CreateBox)
{
  using namespace BABYLON;
  // Create test data
  const float size = 3.f;
  BoxOptions options;
  options.size = size;
  auto box     = VertexData::CreateBox(options);
  // Set expected results
  Uint32Array expectedIndices{0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,
                              8,  9,  10, 8,  10, 11, 12, 13, 14, 12, 14, 15,
                              16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};
  Float32Array expectedPositions{
    1.5f,  -1.5f, 1.5f,  -1.5f, -1.5f, 1.5f,  -1.5f, 1.5f,  1.5f,  1.5f,  1.5f,  1.5f,
    1.5f,  1.5f,  -1.5f, -1.5f, 1.5f,  -1.5f, -1.5f, -1.5f, -1.5f, 1.5f,  -1.5f, -1.5f,
    1.5f,  1.5f,  -1.5f, 1.5f,  -1.5f, -1.5f, 1.5f,  -1.5f, 1.5f,  1.5f,  1.5f,  1.5f,
    -1.5f, 1.5f,  1.5f,  -1.5f, -1.5f, 1.5f,  -1.5f, -1.5f, -1.5f, -1.5f, 1.5f,  -1.5f,
    -1.5f, 1.5f,  1.5f,  -1.5f, 1.5f,  -1.5f, 1.5f,  1.5f,  -1.5f, 1.5f,  1.5f,  1.5f,
    1.5f,  -1.5f, 1.5f,  1.5f,  -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, 1.5f};
  Float32Array expectedNormals{
    0.f,  0.f,  1.f,  0.f, 0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f, 0.f,  0.f, -1.f,
    0.f,  0.f,  -1.f, 0.f, 0.f,  -1.f, 0.f,  0.f,  -1.f, 1.f,  0.f,  0.f, 1.f,  0.f, 0.f,
    1.f,  0.f,  0.f,  1.f, 0.f,  0.f,  -1.f, 0.f,  0.f,  -1.f, 0.f,  0.f, -1.f, 0.f, 0.f,
    -1.f, 0.f,  0.f,  0.f, 1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f, 0.f,  1.f, 0.f,
    0.f,  -1.f, 0.f,  0.f, -1.f, 0.f,  0.f,  -1.f, 0.f,  0.f,  -1.f, 0.f};
  Float32Array expectedUVs{1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f,
                           0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
                           1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f,
                           0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f};

  // Perform comparison
  EXPECT_THAT(box->indices, ::testing::ContainerEq(expectedIndices));
  EXPECT_THAT(box->positions, ::testing::ContainerEq(expectedPositions));
  EXPECT_THAT(box->normals, ::testing::ContainerEq(expectedNormals));
  EXPECT_THAT(box->uvs, ::testing::ContainerEq(expectedUVs));
}

TEST(TestVertexData, CreateGround)
{
  using namespace BABYLON;
  // Create test data
  const unsigned int width        = 5;
  const unsigned int height       = 5;
  const unsigned int subdivisions = 2;
  GroundOptions options;
  options.subdivisions = subdivisions;
  options.width        = width;
  options.height       = height;
  auto ground          = VertexData::CreateGround(options);
  // Set expected results
  Uint32Array expectedIndices{4, 1, 0, 3, 4, 0, 5, 2, 1, 4, 5, 1,
                              7, 4, 3, 6, 7, 3, 8, 5, 4, 7, 8, 4};
  Float32Array expectedPositions{-2.5f, 0.f, 2.5f,  0.f, 0.f, 2.5f,  2.5f, 0.f, 2.5f,
                                 -2.5f, 0.f, 0.f,   0.f, 0.f, 0.f,   2.5f, 0.f, 0.f,
                                 -2.5f, 0.f, -2.5f, 0.f, 0.f, -2.5f, 2.5f, 0.f, -2.5f};
  Float32Array expectedNormals{0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f,
                               0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f};
  Float32Array expectedUVs{0.f,  1.f, 0.5f, 1.f, 1.f, 1.f,  0.f, 0.5f, 0.5f,
                           0.5f, 1.f, 0.5f, 0.f, 0.f, 0.5f, 0.f, 1.f,  0.f};
  // Perform comparison
  EXPECT_THAT(ground->indices, ::testing::ContainerEq(expectedIndices));
  EXPECT_THAT(ground->positions, ::testing::ContainerEq(expectedPositions));
  EXPECT_THAT(ground->normals, ::testing::ContainerEq(expectedNormals));
  EXPECT_THAT(ground->uvs, ::testing::ContainerEq(expectedUVs));
}

// TODO FIXME
/*TEST(TestVertexData, CreateLines)
{
  using namespace BABYLON;
  // Create test data
  std::vector<Vector3> coords{Vector3(-5, 0, 0), //
                              Vector3(5, 0, 0),  //
                              Vector3(0, 0, -5), //
                              Vector3(0, 0, 5)}; //
  auto lines = VertexData::CreateLines(coords);
  // Set expected results
  Uint32Array expectedIndices{0, 1, 1, 2, 2, 3};
  Float32Array expectedPositions{-5.f, 0.f, 0.f,  5.f, 0.f, 0.f,  //
                                 0.f,  0.f, -5.f, 0.f, 0.f, 5.f}; //
  // Perform comparison
  EXPECT_THAT(lines->indices, ::testing::ContainerEq(expectedIndices));
  EXPECT_THAT(lines->positions, ::testing::ContainerEq(expectedPositions));
}*/

TEST(TestVertexData, CreatePlane)
{
  using namespace BABYLON;
  // Create test data
  const float size = 5.f;
  PlaneOptions options;
  options.size = size;
  auto plane   = VertexData::CreatePlane(options);
  // Set expected results
  Uint32Array expectedIndices{0, 1, 2, 0, 2, 3};
  Float32Array expectedPositions{-2.5f, -2.5f, 0.f, 2.5f,  -2.5f, 0.f,  //
                                 2.5f,  2.5f,  0.f, -2.5f, 2.5f,  0.f}; //
  Float32Array expectedNormals{0.f, 0.f, -1.f, 0.f, 0.f, -1.f,          //
                               0.f, 0.f, -1.f, 0.f, 0.f, -1.f};         //
  Float32Array expectedUVs{0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};     //
  // Perform comparison
  EXPECT_THAT(plane->indices, ::testing::ContainerEq(expectedIndices));
  EXPECT_THAT(plane->positions, ::testing::ContainerEq(expectedPositions));
  EXPECT_THAT(plane->normals, ::testing::ContainerEq(expectedNormals));
  EXPECT_THAT(plane->uvs, ::testing::ContainerEq(expectedUVs));
}

TEST(TestVertexData, CreateSphere)
{
  using namespace BABYLON;
  // Create test data
  const unsigned int segments = 1;
  const float diameter        = 0.01f;
  SphereOptions options;
  options.diameter = diameter;
  options.segments = segments;
  auto sphere      = VertexData::CreateSphere(options);
  // Set expected results
  Uint32Array expectedIndices{
    0,  1,  7,  7,  1,  8,  1,  2,  8,  8,  2,  9,  2,  3,  9,  9,  3,  10, 3,  4,  10, 10,
    4,  11, 4,  5,  11, 11, 5,  12, 5,  6,  12, 12, 6,  13, 7,  8,  14, 14, 8,  15, 8,  9,
    15, 15, 9,  16, 9,  10, 16, 16, 10, 17, 10, 11, 17, 17, 11, 18, 11, 12, 18, 18, 12, 19,
    12, 13, 19, 19, 13, 20, 14, 15, 21, 21, 15, 22, 15, 16, 22, 22, 16, 23, 16, 17, 23, 23,
    17, 24, 17, 18, 24, 24, 18, 25, 18, 19, 25, 25, 19, 26, 19, 20, 26, 26, 20, 27};
  Float32Array expectedPositions{
    0.000000f,  0.005000f,  0.000000f,  0.000000f,  0.005000f,  0.000000f,  0.000000f,  0.005000f,
    0.000000f,  0.000000f,  0.005000f,  0.000000f,  0.000000f,  0.005000f,  0.000000f,  0.000000f,
    0.005000f,  0.000000f,  0.000000f,  0.005000f,  0.000000f,  0.004330f,  0.002500f,  0.000000f,
    0.002165f,  0.002500f,  -0.003750f, -0.002165f, 0.002500f,  -0.003750f, -0.004330f, 0.002500f,
    -0.000000f, -0.002165f, 0.002500f,  0.003750f,  0.002165f,  0.002500f,  0.003750f,  0.004330f,
    0.002500f,  0.000000f,  0.004330f,  -0.002500f, 0.000000f,  0.002165f,  -0.002500f, -0.003750f,
    -0.002165f, -0.002500f, -0.003750f, -0.004330f, -0.002500f, -0.000000f, -0.002165f, -0.002500f,
    0.003750f,  0.002165f,  -0.002500f, 0.003750f,  0.004330f,  -0.002500f, 0.000000f,  0.000000f,
    -0.005000f, 0.000000f,  0.000000f,  -0.005000f, -0.000000f, -0.000000f, -0.005000f, -0.000000f,
    -0.000000f, -0.005000f, -0.000000f, -0.000000f, -0.005000f, 0.000000f,  0.000000f,  -0.005000f,
    0.000000f,  0.000000f,  -0.005000f, 0.000000f};
  Float32Array expectedNormals{
    0.000000f,  1.000000f,  0.000000f,  0.000000f,  1.000000f,  0.000000f,  0.000000f,  1.000000f,
    0.000000f,  0.000000f,  1.000000f,  0.000000f,  0.000000f,  1.000000f,  0.000000f,  0.000000f,
    1.000000f,  0.000000f,  0.000000f,  1.000000f,  0.000000f,  0.866025f,  0.500000f,  0.000000f,
    0.433013f,  0.500000f,  -0.750000f, -0.433013f, 0.500000f,  -0.750000f, -0.866025f, 0.500000f,
    -0.000000f, -0.433013f, 0.500000f,  0.750000f,  0.433013f,  0.500000f,  0.750000f,  0.866025f,
    0.500000f,  0.000000f,  0.866025f,  -0.500000f, 0.000000f,  0.433013f,  -0.500000f, -0.750000f,
    -0.433013f, -0.500000f, -0.750000f, -0.866025f, -0.500000f, -0.000000f, -0.433013f, -0.500000f,
    0.750000f,  0.433013f,  -0.500000f, 0.750000f,  0.866025f,  -0.500000f, 0.000000f,  0.000000f,
    -1.000000f, 0.000000f,  0.000000f,  -1.000000f, -0.000000f, -0.000000f, -1.000000f, -0.000000f,
    -0.000000f, -1.000000f, -0.000000f, -0.000000f, -1.000000f, 0.000000f,  0.000000f,  -1.000000f,
    0.000000f,  0.000000f,  -1.000000f, 0.000000f};
  Float32Array expectedUVs{
    0.000000f, 0.000000f, 0.166667f, 0.000000f, 0.333333f, 0.000000f, 0.500000f, 0.000000f,
    0.666667f, 0.000000f, 0.833333f, 0.000000f, 1.000000f, 0.000000f, 0.000000f, 0.333333f,
    0.166667f, 0.333333f, 0.333333f, 0.333333f, 0.500000f, 0.333333f, 0.666667f, 0.333333f,
    0.833333f, 0.333333f, 1.000000f, 0.333333f, 0.000000f, 0.666667f, 0.166667f, 0.666667f,
    0.333333f, 0.666667f, 0.500000f, 0.666667f, 0.666667f, 0.666667f, 0.833333f, 0.666667f,
    1.000000f, 0.666667f, 0.000000f, 1.000000f, 0.166667f, 1.000000f, 0.333333f, 1.000000f,
    0.500000f, 1.000000f, 0.666667f, 1.000000f, 0.833333f, 1.000000f, 1.000000f, 1.000000f};
  // Perform comparison
  EXPECT_THAT(sphere->indices, ::testing::ContainerEq(expectedIndices));
  EXPECT_EQ(sphere->positions.size(), expectedPositions.size());
  for (size_t i = 0; i < expectedPositions.size(); ++i) {
    EXPECT_NEAR(static_cast<double>(sphere->positions[i]),
                static_cast<double>(expectedPositions[i]), 1e-6);
  }
  EXPECT_EQ(sphere->normals.size(), expectedNormals.size());
  for (size_t i = 0; i < expectedNormals.size(); ++i) {
    EXPECT_NEAR(static_cast<double>(sphere->normals[i]), static_cast<double>(expectedNormals[i]),
                1e-6);
  }
  EXPECT_EQ(sphere->uvs.size(), expectedUVs.size());
  for (size_t i = 0; i < expectedUVs.size(); ++i) {
    EXPECT_NEAR(static_cast<double>(sphere->uvs[i]), static_cast<double>(expectedUVs[i]), 1e-6);
  }
}

TEST(TestVertexData, CreateTiledGround)
{
  using namespace BABYLON;
  // Create test data
  float xmin = -5.f;
  float zmin = -5.f;
  float xmax = 5.f;
  float zmax = 5.f;
  ISize subdivisions{2, 2};
  ISize precision{2, 2};
  TiledGroundOptions options;
  options.xmin         = xmin;
  options.zmin         = zmin;
  options.xmax         = xmax;
  options.zmax         = zmax;
  options.subdivisions = subdivisions;
  options.precision    = precision;
  // Create the Tiled Ground
  auto tiledGround = VertexData::CreateTiledGround(options);
  // Set expected results
  Uint32Array expectedIndices{
    1,  4,  3,  0,  1,  3,  2,  5,  4,  1,  2,  4,  4,  7,  6,  3,  4,  6,  5,  8,  7,  4,  5,  7,
    10, 13, 12, 9,  10, 12, 11, 14, 13, 10, 11, 13, 13, 16, 15, 12, 13, 15, 14, 17, 16, 13, 14, 16,
    19, 22, 21, 18, 19, 21, 20, 23, 22, 19, 20, 22, 22, 25, 24, 21, 22, 24, 23, 26, 25, 22, 23, 25,
    28, 31, 30, 27, 28, 30, 29, 32, 31, 28, 29, 31, 31, 34, 33, 30, 31, 33, 32, 35, 34, 31, 32, 34};
  Float32Array expectedPositions{
    -5.f,  0.f,   -5.f,  -2.5f, 0.f,  -5.f,  0.f,  0.f,   -5.f, -5.f, 0.f,   -2.5f, -2.5f, 0.f,
    -2.5f, 0.f,   0.f,   -2.5f, -5.f, 0.f,   0.f,  -2.5f, 0.f,  0.f,  0.f,   0.f,   0.f,   0.f,
    0.f,   -5.f,  2.5f,  0.f,   -5.f, 5.f,   0.f,  -5.f,  0.f,  0.f,  -2.5f, 2.5f,  0.f,   -2.5f,
    5.f,   0.f,   -2.5f, 0.f,   0.f,  0.f,   2.5f, 0.f,   0.f,  5.f,  0.f,   0.f,   -5.f,  0.f,
    0.f,   -2.5f, 0.f,   0.f,   0.f,  0.f,   0.f,  -5.f,  0.f,  2.5f, -2.5f, 0.f,   2.5f,  0.f,
    0.f,   2.5f,  -5.f,  0.f,   5.f,  -2.5f, 0.f,  5.f,   0.f,  0.f,  5.f,   0.f,   0.f,   0.f,
    2.5f,  0.f,   0.f,   5.f,   0.f,  0.f,   0.f,  0.f,   2.5f, 2.5f, 0.f,   2.5f,  5.f,   0.f,
    2.5f,  0.f,   0.f,   5.f,   2.5f, 0.f,   5.f,  5.f,   0.f,  5.f};
  Float32Array expectedNormals{
    0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f};
  Float32Array expectedUVs{
    0.f, 0.f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.5f, 0.5f, 0.5f, 1.f, 0.5f, 0.f, 1.f, 0.5f, 1.f, 1.f, 1.f,
    0.f, 0.f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.5f, 0.5f, 0.5f, 1.f, 0.5f, 0.f, 1.f, 0.5f, 1.f, 1.f, 1.f,
    0.f, 0.f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.5f, 0.5f, 0.5f, 1.f, 0.5f, 0.f, 1.f, 0.5f, 1.f, 1.f, 1.f,
    0.f, 0.f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.5f, 0.5f, 0.5f, 1.f, 0.5f, 0.f, 1.f, 0.5f, 1.f, 1.f, 1.f};
  // Perform comparison
  EXPECT_THAT(tiledGround->indices, ::testing::ContainerEq(expectedIndices));
  EXPECT_THAT(tiledGround->positions, ::testing::ContainerEq(expectedPositions));
  EXPECT_THAT(tiledGround->normals, ::testing::ContainerEq(expectedNormals));
  EXPECT_THAT(tiledGround->uvs, ::testing::ContainerEq(expectedUVs));
}
