#include <gtest/gtest.h>

#include <babylon/misc/tools.h>

TEST(TestTools, ExponentOfTwo)
{
  using namespace BABYLON;

  // Should be expoent of two
  {
    auto result = Tools::IsExponentOfTwo(2);
    EXPECT_TRUE(result);

    result = Tools::IsExponentOfTwo(4);
    EXPECT_TRUE(result);

    result = Tools::IsExponentOfTwo(8);
    EXPECT_TRUE(result);
  }

  // Should not be exponent of two
  {
    auto result = Tools::IsExponentOfTwo(3);
    EXPECT_FALSE(result);

    result = Tools::IsExponentOfTwo(6);
    EXPECT_FALSE(result);

    result = Tools::IsExponentOfTwo(12);
    EXPECT_FALSE(result);
  }
}
