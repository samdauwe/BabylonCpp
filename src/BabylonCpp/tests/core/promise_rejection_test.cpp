#include <gtest/gtest.h>

#include <babylon/core/promise.h>

namespace BABYLON {

class CustomException : public std::exception {
public:
  CustomException(const std::string& message) : msg_(message)
  {
  }
  virtual char const* what() const noexcept
  {
    return msg_.c_str();
  }

private:
  std::string msg_;
}; // end of class CustomException

struct TestPromiseRejection : public testing::Test {
  const std::string message = "Test error";
  const CustomException test_error;

  TestPromiseRejection() : test_error(message)
  {
  }
}; // end of struct TestPromiseRejection

TEST_F(TestPromiseRejection, Reject)
{
  // State observation variables.
  bool chained  = false;
  bool rejected = false;

  // Create the promise and assign a resolve handler.
  Promise<> prom;
  prom.future().then(
    [&]() { FAIL() << "Entered resolve handler for rejected promise."; },
    [&](const std::exception& err) {
      EXPECT_TRUE(chained);
      EXPECT_FALSE(rejected);

      EXPECT_EQ(message, err.what());

      rejected = true;
    });

  // The handler is chained, check that it hasn't been run.
  chained = true;
  EXPECT_FALSE(rejected);

  // Resolve the promise and check that the rejected handler is true
  prom.reject(test_error);
  EXPECT_TRUE(rejected);
}

TEST_F(TestPromiseRejection, RejectBubbling)
{
  // State observation variables.
  bool chained  = false;
  bool rejected = false;

  // Create the promise and assign a resolve handler.
  Promise<> prom;
  prom.future()
    .then([&]() { FAIL() << "Entered resolve handler for rejected promise."; })
    .then(
      [&]() { FAIL() << "Entered resolve handler after rejected promise."; },
      [&](const std::exception& err) {
        EXPECT_TRUE(chained);
        EXPECT_FALSE(rejected);

        EXPECT_EQ(message, err.what());

        rejected = true;
      });

  // The handler is chained, check that it hasn't been run.
  chained = true;
  EXPECT_FALSE(rejected);

  // Resolve the promise and check that the rejected handler is true
  prom.reject(test_error);
  EXPECT_TRUE(rejected);
}

TEST_F(TestPromiseRejection, UnhandledRejection)
{
  // Create the promise and assign a resolve handler.
  Promise<> prom;
  prom.future()
    .then([&]() { FAIL() << "Entered resolve handler for rejected promise."; })
    .then(
      [&]() { FAIL() << "Entered resolve handler after rejected promise."; });

  // Resolve the promise and check that the rejected handler is true
  EXPECT_THROW(prom.reject(test_error), std::exception);
}

} // end of namespace BABYLON
