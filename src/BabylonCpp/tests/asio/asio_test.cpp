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
    asio::DataTypeOrErrorMessage<std::string> r = asio::LoadFileSync_Text(filename, nullptr);
    bool success = std::holds_alternative<std::string>(r);
    EXPECT_TRUE(success);
  }
  {
    std::string filename = assets_folder() + "/non_existing_file";
    asio::DataTypeOrErrorMessage<std::string> r = asio::LoadFileSync_Text(filename, nullptr);
    bool success = std::holds_alternative<std::string>(r);
    EXPECT_FALSE(success);
  }
}

TEST(async_requests, LoadTextFileAsync)
{

  using namespace BABYLON;
  // simple test with existing file
  {
    bool nb_success           = false;
    bool nb_error             = false;
    auto onSuccessDisplaySize = [&nb_success](const std::string&) { nb_success = true;
    };
    auto onError = [&nb_error](const std::string&) { nb_error = true;
    };

    std::string filename = assets_folder() + "/samples_info.json";
    asio::LoadFileAsync_Text(filename, onSuccessDisplaySize, onError);
    asio::Service_WaitAll();

    EXPECT_TRUE(nb_success);
    EXPECT_FALSE(nb_error);
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
}

TEST(async_requests, LoadBinaryFileAsync)
{
  using namespace BABYLON;

  // simple test with lots of async calls
  {
    int nb_success            = 0;
    int nb_error              = 0;
    int nb_progress           = 0;
    auto onSuccessDisplaySize = [&nb_success](const ArrayBuffer&) {
      ++nb_success;
    };
    auto onError = [&nb_error](const std::string& message) {
      (void)message;
      //std::cerr << message << std::endl;
      ++nb_error;
    };
    auto onProgress = [&nb_progress](bool lengthComputable, size_t loaded, size_t total) {
      (void)lengthComputable; (void)loaded; (void) total;
      ++nb_progress;

      //char msg[200];
      //double MB = 1024. * 1024.;
      //if (lengthComputable)
      //{
      //  snprintf(msg, 200, "%.1lf/%.1lf MB read", (double)(loaded) / MB, (double)total / MB );
      //  std::cerr << msg << std::endl;
      //}
    };

    std::string filename1 = assets_folder() + "//glTF-Sample-Models/2.0/AntiqueCamera/glTF/camera_camera_Normal.png";
    std::string filename2 = assets_folder() + "/non_existing_file";
    int nbTests = 4;
    for (int i = 0; i < nbTests; i++) {
      std::string filename = (i % 2 == 0) ? filename1 : filename2;
      asio::LoadFileAsync_Binary(filename, onSuccessDisplaySize, onError, onProgress);
    }
    asio::Service_WaitAll();
    EXPECT_EQ(nb_success + nb_error, nbTests);
    EXPECT_EQ(nb_error, nbTests / 2);
    EXPECT_EQ(nb_success, nbTests / 2);
    EXPECT_GT(nb_progress, 0);
  }
}