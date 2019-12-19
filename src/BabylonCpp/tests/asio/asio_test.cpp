#include <gtest/gtest.h>
#include <babylon/asio/asio.h>
#include <babylon/core/string.h>
#include <babylon/core/filesystem.h>
#include <babylon/babylon_common.h>

#include <thread>
#include <iostream>

#ifndef BABYLON_REPO_DIR
#error "undef"
#endif
//#error(BABYLON_REPO_DIR)
std::string assets_folder =
  std::string(BABYLON_REPO_DIR) +
  std::string("/assets/");

std::string textUrl =
  std::string("file:/") + assets_folder
  + "/samples_info.json";
std::string binaryUrl =
  std::string("file:/") + assets_folder
  + "/glTF-Sample-Models/2.0/AntiqueCamera/glTF/camera_camera_Normal.png";
std::string nonExistingFileUrl = "file:/non_existing_file";

std::vector<std::string> textFilesUrls = {
  textUrl, nonExistingFileUrl, textUrl, textUrl, nonExistingFileUrl, textUrl, nonExistingFileUrl
};

std::vector<std::string> binaryFilesUrls  = {
  binaryUrl, nonExistingFileUrl, binaryUrl, binaryUrl, nonExistingFileUrl, binaryUrl, nonExistingFileUrl
};

void SampleApplicationLoop()
{
  auto mainThreadId = std::this_thread::get_id();

  // Sample callbacks that will be called *synchronously*
  int nb_success_text = 0;
  int nb_success_binary = 0;
  int nb_error = 0;
  int nb_calls_to_progress = 0;

  auto onSuccessText = [&nb_success_text, mainThreadId](const std::string& /*textContent*/) {
    auto thisThreadId = std::this_thread::get_id();
    assert(thisThreadId == mainThreadId);
    ++nb_success_text;
  };
  auto onSuccessBinary = [&nb_success_binary, mainThreadId](const BABYLON::ArrayBuffer& /*data*/) {
    auto thisThreadId = std::this_thread::get_id();
    assert(thisThreadId == mainThreadId);
    ++nb_success_binary;
  };
  auto onError = [&nb_error, mainThreadId](const std::string& /*message*/) {
    auto thisThreadId = std::this_thread::get_id();
    assert(thisThreadId == mainThreadId);
    ++nb_error;
  };
  auto onProgress = [&nb_calls_to_progress, mainThreadId](bool /*lengthComputable*/, size_t /*loaded*/, size_t /*total*/) {
    auto thisThreadId = std::this_thread::get_id();
    assert(thisThreadId == mainThreadId);
    ++nb_calls_to_progress;
  };


  // Sample application main loop
  bool applicationShallExit = false;
  while(!applicationShallExit)
  {
    if (!textFilesUrls.empty())
    {
      std::string url = textFilesUrls.back();
      textFilesUrls.pop_back();
      BABYLON::asio::LoadUrlAsync_Text(url, onSuccessText, onError, onProgress);
    }
    if (!binaryFilesUrls.empty())
    {
      std::string url = binaryFilesUrls.back();
      binaryFilesUrls.pop_back();
      BABYLON::asio::LoadUrlAsync_Binary(url, onSuccessBinary, onError, onProgress);
    }

    applicationShallExit =
      textFilesUrls.empty() && binaryFilesUrls.empty() && ! BABYLON::asio::HasRemainingTasks();

    // Remember to call asio::HeartBeat_Sync() in your application main loop:
    // if there are pending callbacks, this will call the first one *synchronously*
    BABYLON::asio::HeartBeat_Sync();
  };

  printf("nb_success_text=%i, nb_success_binary=%i, nb_error=%i nb_calls_to_progress=%i\n",
    nb_success_text, nb_success_binary, nb_error, nb_calls_to_progress);
}

TEST(async_requests, LoadTextFileAsync)
{

  SampleApplicationLoop();

  using namespace BABYLON;
  // simple test with existing file
  /*
  {
    bool nb_success           = false;
    bool nb_error             = false;
    auto onSuccessDisplaySize = [&nb_success](const std::string&)
      { nb_success = true; };
    auto onError = [&nb_error](const std::string&)
      { nb_error = true; };

    std::string filename = assets_folder() + "/samples_info.json";
    asio::LoadFileAsync_Text(filename, onSuccessDisplaySize, onError);
    asio::Service_WaitAll_Sync();

    EXPECT_TRUE(nb_success);
    EXPECT_FALSE(nb_error);
  }
   */
/*
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
    asio::Service_WaitAll_Sync();

    EXPECT_FALSE(success);
    EXPECT_TRUE(error);
  }
  */

  // simple test with lots of async calls
  /*
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
    asio::Service_WaitAll_Sync();
    EXPECT_EQ(success + error, nbTests);
    EXPECT_EQ(error, nbTests / 2);
    EXPECT_EQ(success, nbTests / 2);
  }
*/
}

/*
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
    asio::Service_WaitAll_Sync();
    EXPECT_EQ(nb_success + nb_error, nbTests);
    EXPECT_EQ(nb_error, nbTests / 2);
    EXPECT_EQ(nb_success, nbTests / 2);
    EXPECT_GT(nb_progress, 0);
  }

}
*/