#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/math/matrix.h>

TEST(TestMatrix, Constructor)
{
  using namespace BABYLON;

  // Constructor
  Matrix matrix;
  for (const auto& el : matrix.m) {
    EXPECT_FLOAT_EQ(0.f, el);
  }

  EXPECT_FLOAT_EQ(0.f, matrix.determinant());
}

TEST(TestMatrix, FromValues)
{
  using namespace BABYLON;

  ::std::array<float, 16> m{{0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f,
                             10.f, 11.f, 12.f, 13.f, 14.f, 15.f}};
  Matrix matrix = Matrix::FromValues(m[0], m[1], m[2], m[3],   //
                                     m[4], m[5], m[6], m[7],   //
                                     m[8], m[9], m[10], m[11], //
                                     m[12], m[13], m[14], m[15]);
  for (unsigned int i = 0; i < m.size(); ++i) {
    EXPECT_FLOAT_EQ(m[i], matrix.m[i]);
  }

  Matrix::FromValuesToRef(m[0] * 2, m[1] * 2, m[2] * 2, m[3] * 2,   //
                          m[4] * 2, m[5] * 2, m[6] * 2, m[7] * 2,   //
                          m[8] * 2, m[9] * 2, m[10] * 2, m[11] * 2, //
                          m[12] * 2, m[13] * 2, m[14] * 2, m[15] * 2, matrix);
  for (unsigned int i = 0; i < m.size(); ++i) {
    EXPECT_FLOAT_EQ(m[i] * 2, matrix.m[i]);
  }
}

TEST(TestMatrix, Determinant)
{
  using namespace BABYLON;

  Matrix a = Matrix::Identity();
  EXPECT_FLOAT_EQ(1.f, a.determinant());

  a.m[0] = 2.f;
  EXPECT_FLOAT_EQ(2.f, a.determinant());

  a.m[0] = 0.f;
  EXPECT_FLOAT_EQ(0.f, a.determinant());

  // calculated via
  // http://www.euclideanspace.com/maths/algebra/matrix/functions/determinant/fourD/index.htm
  ::std::array<float, 16> m{{2.f, 3.f, 4.f, 5.f, -1.f, -21.f, -3.f, -4.f, 6.f,
                             7.f, 8.f, 10.f, -8.f, -9.f, -10.f, -12.f}};
  Matrix::FromValuesToRef(m[0], m[1], m[2], m[3],   //
                          m[4], m[5], m[6], m[7],   //
                          m[8], m[9], m[10], m[11], //
                          m[12], m[13], m[14], m[15], a);
  EXPECT_FLOAT_EQ(76.f, a.determinant());
}

TEST(TestMatrix, Transpose)
{
  using namespace BABYLON;

  Matrix a = Matrix::Identity();
  Matrix b = Matrix::Transpose(a);
  EXPECT_THAT(a.m, ::testing::ContainerEq(b.m));

  ::std::array<float, 16> m{{0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f,
                             10.f, 11.f, 12.f, 13.f, 14.f, 15.f}};
  Matrix::FromValuesToRef(m[0], m[1], m[2], m[3],   //
                          m[4], m[5], m[6], m[7],   //
                          m[8], m[9], m[10], m[11], //
                          m[12], m[13], m[14], m[15], b);
  Matrix c = Matrix::Transpose(b);
  EXPECT_FALSE(b.equals(c));
  c = Matrix::Transpose(c);
  EXPECT_THAT(b.m, ::testing::ContainerEq(c.m));
}

TEST(TestMatrix, Clone)
{
  using namespace BABYLON;

  ::std::array<float, 16> m{{0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f,
                             10.f, 11.f, 12.f, 13.f, 14.f, 15.f}};
  Matrix a = Matrix::FromValues(m[0], m[1], m[2], m[3],   //
                                m[4], m[5], m[6], m[7],   //
                                m[8], m[9], m[10], m[11], //
                                m[12], m[13], m[14], m[15]);
  Matrix b = a.copy();

  EXPECT_THAT(a.m, ::testing::ContainerEq(b.m));

  // ensure that it is a true copy
  a.m[0] = 2.f;
  EXPECT_FALSE(a.equals(b));
}
