#include <gtest/gtest.h>
#include <babylon/asio/asio.h>
#include <babylon/core/string.h>
#include <babylon/core/filesystem.h>
#include <babylon/babylon_common.h>

#include <thread>
#include <iostream>

std::string assets_folder = std::string(BABYLON_REPO_DIR) + std::string("/assets/");

std::string textUrl = std::string("file:/") + assets_folder + "/samples_info.json";
std::string binaryUrl = std::string("file:/") + assets_folder
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

  int nb_success_text = 0;
  int nb_success_binary = 0;
  int nb_error = 0;
  int nb_calls_to_progress = 0;

  // Sample callbacks that will be called *synchronously*
  auto onSuccessText = [&nb_success_text, mainThreadId](const std::string& /*textContent*/) {
    auto thisThreadId = std::this_thread::get_id();
    EXPECT_EQ(thisThreadId, mainThreadId);
    ++nb_success_text;
  };
  auto onSuccessBinary = [&nb_success_binary, mainThreadId](const BABYLON::ArrayBuffer& /*data*/) {
    auto thisThreadId = std::this_thread::get_id();
    EXPECT_EQ(thisThreadId, mainThreadId);
    ++nb_success_binary;
  };
  auto onError = [&nb_error, mainThreadId](const std::string& /*message*/) {
    auto thisThreadId = std::this_thread::get_id();
    EXPECT_EQ(thisThreadId, mainThreadId);
    ++nb_error;
  };
  auto onProgress = [&nb_calls_to_progress, mainThreadId](bool /*lengthComputable*/, size_t /*loaded*/, size_t /*total*/) {
    auto thisThreadId = std::this_thread::get_id();
    EXPECT_EQ(thisThreadId, mainThreadId);
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

  EXPECT_EQ(nb_success_text, 4);
  EXPECT_EQ(nb_success_binary, 4);
  EXPECT_EQ(nb_error, 6);
  EXPECT_GT(nb_calls_to_progress, 10);
//  printf("nb_success_text=%i, nb_success_binary=%i, nb_error=%i nb_calls_to_progress=%i\n",
//    nb_success_text, nb_success_binary, nb_error, nb_calls_to_progress);
}

TEST(async_requests, SampleApplicationLoop)
{
  SampleApplicationLoop();
}

TEST(async_requests, LoadText)
{
#if 0 // This code works, but features no real google test call
  std::cout << "\n";
  auto onSuccessText = [](const std::string& textContent) {
    std::cout << "\n==>\n" << textContent << "\n<==\n";
  };
  auto onError = [](const std::string& message) {
    std::cout << "Error:" << message << "!!!\n";
  };
  auto onProgress = [](bool lengthComputable, size_t loaded, size_t total) {
    std::cout << "onProgress " << loaded << " / " << total << " lengthComputable: " << lengthComputable << "\n";
  };

  BABYLON::asio::LoadUrlAsync_Text(textUrl, onSuccessText, onError, onProgress);
  BABYLON::asio::Service_WaitAll_Sync();
#endif
}
