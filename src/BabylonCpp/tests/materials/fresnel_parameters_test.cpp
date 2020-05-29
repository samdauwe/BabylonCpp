#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../test_utils.h"
#include <babylon/engines/scene.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/ifresnel_parameters_serialized.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>

/**
 * @brief Test Suite for FresnelParameters.
 */

/**
 * @brief empty constructor has default values
 */
TEST(TestFresnelParameters, EmptyConstructorHasDefaultValues)
{
  using namespace BABYLON;

  FresnelParameters subject;

  // bias default is 0
  EXPECT_EQ(subject.bias, 0.f);
  // power default is 1
  EXPECT_EQ(subject.power, 1.f);

  EXPECT_TRUE(subject.isEnabled());
  EXPECT_TRUE(subject.leftColor.equals(Color3::White()));
  EXPECT_TRUE(subject.rightColor.equals(Color3::Black()));
}

/**
 * @brief new FresnelParameters({...}) with options specified
 */
TEST(TestFresnelParameters, ConstructorWithOptionsSpecified)
{
  using namespace BABYLON;

  IFresnelParametersCreationOptions options;
  {
    options.bias       = 1.f;
    options.power      = 0.f;
    options.isEnabled  = false;
    options.leftColor  = Color3::Black();
    options.rightColor = Color3::White();
  }
  FresnelParameters subject(options);

  // created with 1
  EXPECT_EQ(subject.bias, 1.f);
  // created with 0
  EXPECT_EQ(subject.power, 0.f);

  EXPECT_FALSE(subject.isEnabled());
  EXPECT_TRUE(subject.leftColor.equals(Color3::Black()));
  EXPECT_TRUE(subject.rightColor.equals(Color3::White()));
}

/**
 * @brief FresnelParameters.Parse({...}) with equality check
 */
TEST(TestFresnelParameters, ParseWithEqualityCheck)
{
  using namespace BABYLON;

  IFresnelParametersSerialized options;
  {
    options.isEnabled  = true;
    options.leftColor  = {1.f, 1.f, 1.f};
    options.rightColor = {0.f, 0.f, 0.f};
    options.bias       = 0.f;
    options.power      = 1.f;
  }
  const auto subject = FresnelParameters::Parse(options);
  EXPECT_TRUE(FresnelParameters().equals(*subject));
}

/**
 * @brief disabling FresnelParameters should mark materials as dirty (not ready)
 */
TEST(TestFresnelParameters, DisablingFresnelParameters)
{
  using namespace BABYLON;

  auto engine    = createSubject();
  auto scene     = Scene::New(engine.get());
  auto mesh      = Mesh::CreateBox("mesh", 1, scene.get());
  auto material  = StandardMaterial::New("material", scene.get());
  mesh->material = material;

  const auto subject                    = std::make_shared<FresnelParameters>();
  material->refractionFresnelParameters = subject;

  // EXPECT_TRUE(scene->_cachedMaterial != nullptr);

  // should mark materials as dirty and clear scene cache
  subject->isEnabled = false;
  EXPECT_TRUE(scene->_cachedMaterial == nullptr);
}
