#include <gtest/gtest.h>

#include <babylon/core/promise.h>

namespace BABYLON {

TEST(TestPromise, Void_Int_PromiseInt)
{
  // State observation variables.
  bool chained   = false;
  bool resolved  = false;
  bool resolved2 = false;

  const int value  = 42;
  const int value2 = 47;

  // Create the promise and assign a resolve handler.
  Promise<int> prom;
  prom.future()
    .then<int>([&](int result, Promise<int>&& prom2) {
      EXPECT_TRUE(chained);
      EXPECT_FALSE(resolved);
      EXPECT_FALSE(resolved2);
      EXPECT_EQ(value, result);

      resolved = true;
      prom2.resolve(value2);
    })
    .then([&](int result) {
      EXPECT_TRUE(chained);
      EXPECT_TRUE(resolved);
      EXPECT_FALSE(resolved2);
      EXPECT_EQ(value2, result);

      resolved2 = true;
    });

  // The handler is chained, check that it hasn't been run.
  chained = true;
  EXPECT_FALSE(resolved);
  EXPECT_FALSE(resolved2);

  // Resolve the promise and check that the resolved handler is true
  prom.resolve(value);
  EXPECT_TRUE(resolved);
  EXPECT_TRUE(resolved2);
}

TEST(TestPromise, Void_Int_PromiseVoid)
{
  // State observation variables.
  bool chained   = false;
  bool resolved  = false;
  bool resolved2 = false;

  const int value = 42;

  // Create the promise and assign a resolve handler.
  Promise<int> prom;
  prom.future()
    .then([&](int result, Promise<>&& prom2) {
      EXPECT_TRUE(chained);
      EXPECT_FALSE(resolved);
      EXPECT_FALSE(resolved2);
      EXPECT_EQ(value, result);

      resolved = true;
      prom2.resolve();
    })
    .then([&]() {
      EXPECT_TRUE(chained);
      EXPECT_TRUE(resolved);
      EXPECT_FALSE(resolved2);

      resolved2 = true;
    });

  // The handler is chained, check that it hasn't been run.
  chained = true;
  EXPECT_FALSE(resolved);
  EXPECT_FALSE(resolved2);

  // Resolve the promise and check that the resolved handler is true
  prom.resolve(value);
  EXPECT_TRUE(resolved);
  EXPECT_TRUE(resolved2);
}

TEST(TestPromise, FutureInt_Int)
{
  // State observation variables.
  bool chained   = false;
  bool resolved  = false;
  bool resolved2 = false;

  const int value  = 42;
  const int value2 = 47;

  // Create the promise and assign a resolve handler.
  Promise<int> prom;
  Promise<int> prom2;
  prom.future()
    .then([&](int result) {
      EXPECT_TRUE(chained);
      EXPECT_FALSE(resolved);
      EXPECT_FALSE(resolved2);
      EXPECT_EQ(value, result);

      resolved = true;
      return prom2.future();
    })
    .then([&](int result) {
      EXPECT_TRUE(chained);
      EXPECT_TRUE(resolved);
      EXPECT_FALSE(resolved2);
      EXPECT_EQ(value2, result);

      resolved2 = true;
    });

  // The handler is chained, check that it hasn't been run.
  chained = true;
  EXPECT_FALSE(resolved);
  EXPECT_FALSE(resolved2);

  // Resolve the promise and check that the resolved handler is true
  prom.resolve(value);
  EXPECT_TRUE(resolved);
  EXPECT_FALSE(resolved2);

  // Now resolve the 2nd promise.
  prom2.resolve(value2);
  EXPECT_TRUE(resolved);
  EXPECT_TRUE(resolved2);
}

TEST(TestPromise, Int_Int)
{
  // State observation variables.
  bool chained   = false;
  bool resolved  = false;
  bool resolved2 = false;

  const int value  = 42;
  const int value2 = 47;

  // Create the promise and assign a resolve handler.
  Promise<int> prom;
  prom.future()
    .then([&](int result) {
      EXPECT_TRUE(chained);
      EXPECT_FALSE(resolved);
      EXPECT_FALSE(resolved2);
      EXPECT_EQ(value, result);

      resolved = true;
      return value2;
    })
    .then([&](int result) {
      EXPECT_TRUE(chained);
      EXPECT_TRUE(resolved);
      EXPECT_FALSE(resolved2);
      EXPECT_EQ(value2, result);

      resolved2 = true;
    });

  // The handler is chained, check that it hasn't been run.
  chained = true;
  EXPECT_FALSE(resolved);
  EXPECT_FALSE(resolved2);

  // Resolve the promise and check that the resolved handler is true
  prom.resolve(value);
  EXPECT_TRUE(resolved);
  EXPECT_TRUE(resolved2);
}

TEST(TestPromise, Void_Int)
{
  // State observation variables.
  bool chained   = false;
  bool resolved  = false;
  bool resolved2 = false;

  const int value = 42;

  // Create the promise and assign a resolve handler.
  Promise<int> prom;
  prom.future()
    .then([&](int result) {
      EXPECT_TRUE(chained);
      EXPECT_FALSE(resolved);
      EXPECT_FALSE(resolved2);
      EXPECT_EQ(value, result);

      resolved = true;
    })
    .then([&]() {
      EXPECT_TRUE(chained);
      EXPECT_TRUE(resolved);
      EXPECT_FALSE(resolved2);

      resolved2 = true;
    });

  // The handler is chained, check that it hasn't been run.
  chained = true;
  EXPECT_FALSE(resolved);
  EXPECT_FALSE(resolved2);

  // Resolve the promise and check that the resolved handler is true
  prom.resolve(value);
  EXPECT_TRUE(resolved);
  EXPECT_TRUE(resolved2);
}

TEST(TestPromise, ConnectPromiseInt)
{
  // State observation variables.
  bool chained  = false;
  bool resolved = false;

  const int value = 42;

  // Create the promise and assign a resolve handler.
  Promise<int> prom;
  Promise<int> prom2;
  prom2.future().then([&](int result) {
    EXPECT_TRUE(chained);
    EXPECT_FALSE(resolved);
    EXPECT_EQ(value, result);

    resolved = true;
  });
  prom.future().then(std::move(prom2));

  // The handler is chained, check that it hasn't been run.
  chained = true;
  EXPECT_FALSE(resolved);

  // Resolve the promise and check that the resolved handler is true
  prom.resolve(value);
  EXPECT_TRUE(resolved);
}

} // end of namespace BABYLON
