#include <gtest/gtest.h>
#include <vector>
#include <babylon/babylon_stl_util.h>



TEST(splice, test)
{
  using VecInt = std::vector<int>;
  {
    int index = -2;
    int howmany = 2;

    VecInt v{ 0, 1, 2 };
    VecInt vItemsToAdd{};
    auto v2 = BABYLON::stl_util::splice(v, index, howmany, vItemsToAdd);
    EXPECT_EQ(v, VecInt({ 0 }));
    EXPECT_EQ(v2, VecInt({ 1, 2 }));
  }
  {
    int index = -2;
    int howmany = 2;

    VecInt v{ 0, 1, 2 };
    VecInt vItemsToAdd{3, 4};
    auto v2 = BABYLON::stl_util::splice(v, index, howmany, vItemsToAdd);
    EXPECT_EQ(v, VecInt({ 0, 3, 4 }));
    EXPECT_EQ(v2, VecInt({ 1, 2 }));
  }
  {
    int index = 0;
    int howmany = 2;

    VecInt v{ 0, 1, 2 };
    VecInt vItemsToAdd{ 3, 4 };
    auto v2 = BABYLON::stl_util::splice(v, index, howmany, vItemsToAdd);
    EXPECT_EQ(v, VecInt({ 2, 3, 4 }));
    EXPECT_EQ(v2, VecInt({ 0, 1 }));
  }
  {
    int index = 0;
    int howmany = 3;

    VecInt v{ 0, 1, 2 };
    VecInt vItemsToAdd{};
    auto v2 = BABYLON::stl_util::splice(v, index, howmany, vItemsToAdd);
    EXPECT_EQ(v, VecInt({}));
    EXPECT_EQ(v2, VecInt({ 0, 1, 2 }));
  }
  {
    int index = -3;
    int howmany = 3;

    VecInt v{ 0, 1, 2 };
    VecInt vItemsToAdd{};
    auto v2 = BABYLON::stl_util::splice(v, index, howmany, vItemsToAdd);
    EXPECT_EQ(v, VecInt({}));
    EXPECT_EQ(v2, VecInt({ 0, 1, 2 }));
  }
}
