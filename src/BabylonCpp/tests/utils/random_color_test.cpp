#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/utils/random_color.h>

TEST(TestRandomColor, HSVToRGB)
{
  using namespace BABYLON::randomcolor;
  // Black
  std::array<unsigned int, 3> rgb      = RandomColor::HSVToRGB(0.f, 0.f, 0.f);
  std::array<unsigned int, 3> expected = {{0, 0, 0}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // White
  rgb      = RandomColor::HSVToRGB(0.f, 0.f, 1.f);
  expected = {{255, 255, 255}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Red
  rgb      = RandomColor::HSVToRGB(0.f, 1.f, 1.f);
  expected = {{255, 0, 0}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Lime
  rgb      = RandomColor::HSVToRGB(120.f, 1.f, 1.f);
  expected = {{0, 255, 0}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Blue
  rgb      = RandomColor::HSVToRGB(240.f, 1.f, 1.f);
  expected = {{0, 0, 255}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Yellow
  rgb      = RandomColor::HSVToRGB(60.f, 1.f, 1.f);
  expected = {{255, 255, 0}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Cyan
  rgb      = RandomColor::HSVToRGB(180.f, 1.f, 1.f);
  expected = {{0, 255, 255}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Magenta
  rgb      = RandomColor::HSVToRGB(300.f, 1.f, 1.f);
  expected = {{255, 0, 255}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Silver
  rgb      = RandomColor::HSVToRGB(0.f, 0.f, 0.75f);
  expected = {{192, 192, 192}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Gray
  rgb      = RandomColor::HSVToRGB(0.f, 0.f, 0.5f);
  expected = {{128, 128, 128}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Maroon
  rgb      = RandomColor::HSVToRGB(0.f, 1.f, 0.5f);
  expected = {{128, 0, 0}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Olive
  rgb      = RandomColor::HSVToRGB(60.f, 1.f, 0.5f);
  expected = {{128, 128, 0}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Green
  rgb      = RandomColor::HSVToRGB(120.f, 1.f, 0.5f);
  expected = {{0, 128, 0}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Purple
  rgb      = RandomColor::HSVToRGB(300.f, 1.f, 0.5f);
  expected = {{128, 0, 128}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Teal
  rgb      = RandomColor::HSVToRGB(180.f, 1.f, 0.5f);
  expected = {{0, 128, 128}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
  // Navy
  rgb      = RandomColor::HSVToRGB(240.f, 1.f, 0.5f);
  expected = {{0, 0, 128}};
  EXPECT_THAT(rgb, ::testing::ContainerEq(expected));
}
