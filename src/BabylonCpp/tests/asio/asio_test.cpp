#include <gtest/gtest.h>
#include <babylon/asio/asio.h>
#include <babylon/core/string.h>
#include <babylon/core/filesystem.h>
#include <babylon/babylon_common.h>

#include <iostream>

TEST(async_requests, LoadTextFileSync)
{
  return;
  using namespace BABYLON;
  {
    std::string filename = assets_folder() + "/samples_info.json";
    asio::DataTypeOrErrorMessage<std::string> r = asio::LoadTextFileSync(filename);
    bool success = std::holds_alternative<std::string>(r);
    EXPECT_TRUE(success);
  }
  {
    std::string filename = assets_folder() + "/non_existing_file";
    asio::DataTypeOrErrorMessage<std::string> r = asio::LoadTextFileSync(filename);
    bool success = std::holds_alternative<std::string>(r);
    EXPECT_FALSE(success);
  }
}

TEST(async_requests, LoadTextFileAsync)
{

  using namespace BABYLON;
  // simple test with existing file
  {
    bool success = false;
    bool error = false;
    auto onSuccessDisplaySize = [&success](const std::string&) {
      success = true;
    };
    auto onError = [&error](const std::string&) {
      error = true;
    };

    std::string filename = assets_folder() + "/samples_info.json";
    asio::LoadFileAsync_Text(filename, onSuccessDisplaySize, onError);
    asio::Service_WaitAll();

    EXPECT_TRUE(success);
    EXPECT_FALSE(error);
  }
  // simple test with non existing file
  {
    bool success = false;
    bool error = false;
    auto onSuccessDisplaySize = [&success](const std::string&) {
      success = true;
    };
    auto onError = [&error](const std::string&) {
      error = true;
    };
    std::string filename = assets_folder() + "/non_existing_file";
    asio::LoadFileAsync_Text(filename, onSuccessDisplaySize, onError);
    asio::Service_WaitAll();

    EXPECT_FALSE(success);
    EXPECT_TRUE(error);
  }
  // simple test with lots of async calls
  {
    int success = 0;
    int error = 0;
    auto onSuccessDisplaySize = [&success](const std::string&) {
      ++success;
    };
    auto onError = [&error](const std::string& message) {
      (void)message;
      //std::cerr << message << std::endl;
      ++error;
    };

    std::string filename1 = assets_folder() + "/samples_info.json";
    std::string filename2 = assets_folder() + "/non_existing_file";
    int nbTests = 100;
    for (int i = 0; i < nbTests; i++) {
      std::string filename = (i % 2 == 0) ? filename1 : filename2;
      asio::LoadFileAsync_Text(filename, onSuccessDisplaySize, onError);
    }
    asio::Service_WaitAll();
    EXPECT_EQ(success + error, nbTests);
    EXPECT_EQ(error, nbTests / 2);
    EXPECT_EQ(success, nbTests / 2);
  }
//
}

