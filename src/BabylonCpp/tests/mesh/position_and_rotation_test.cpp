#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../test_utils.h"

#include <babylon/engines/null_engine.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/abstract_mesh.h>

TEST(TestPositionAndRotation, TestConversionBetweenQuaternionsAndEuler)
{
  using namespace BABYLON;
  // Converting between quaternions/euler
  Vector3 originalRotation{0.1f, 0.2f, 0.3f};
  auto v = originalRotation.copy();
  auto q = Quaternion::FromEulerVector(v);
  q.toEulerAnglesToRef(v);
  EXPECT_TRUE(v.subtract(originalRotation).length() < 0.00001f);
}

TEST(TestPositionAndRotation, TestReordersVectorInPlace)
{
  using namespace BABYLON;
  // Converting between quaternions/euler
  Vector3 originalRotation{0.1f, 0.2f, 0.3f};
  auto v = originalRotation.copy();
  v.reorderInPlace("ZYX");
  EXPECT_TRUE(v.subtract(Vector3(0.3f, 0.2f, 0.1f)).length() < 0.00001f);
}

TEST(TestPositionAndRotation, TestParenting)
{
  using namespace BABYLON;
  // Parent child positions
  auto subject = createSubject();
  auto scene   = Scene::New(subject.get());
  auto child   = AbstractMesh::New("", scene.get());
  auto parent  = AbstractMesh::New("", scene.get());
  parent->position().set(0.f, 0.f, 1.f);
  child->position().set(0.f, 0.f, -1.f);
  child->parent = parent.get();
  child->computeWorldMatrix();
  EXPECT_TRUE(child->absolutePosition().equals(Vector3{0.f, 0.f, 0.f}));
  // Rotate parent around child
  parent->rotationQuaternion = Quaternion();
  Vector3 eulerRotation{0.f, Math::PI_2, 0.f};
  Quaternion rotation;
  Quaternion::RotationYawPitchRollToRef(eulerRotation.y, eulerRotation.x,
                                        eulerRotation.z, rotation);
  parent->rotationQuaternion()->multiplyInPlace(rotation);
  parent->position().rotateByQuaternionAroundPointToRef(
    rotation, child->absolutePosition(), parent->position());
  EXPECT_TRUE(parent->position().subtract(Vector3(1.f, 0.f, 0.f)).length()
              < 0.00001f);
  EXPECT_TRUE(parent->rotationQuaternion()
                ->toEulerAngles()
                .subtract(eulerRotation)
                .length()
              < 0.00001f);
  EXPECT_TRUE(
    child->absolutePosition().subtract(Vector3(0.f, 0.f, 0.f)).length()
    < 0.00001f);
}
