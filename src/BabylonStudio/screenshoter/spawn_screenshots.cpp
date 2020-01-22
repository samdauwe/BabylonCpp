#include <babylon/samples/samples_index.h>
#include <babylon/core/logging.h>
#include <iostream>
#include <string>
#include <babylon/core/filesystem.h>
#include <babylon/babylon_imgui/babylon_studio.h>
#include <babylon/samples/sample_spawn.h>
#include <babylon/samples/samples_info.h>
#include <nlohmann/json.hpp>

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

namespace BABYLON {
namespace impl {

using SpawnScreenshotsStats = std::unordered_map<std::string, unsigned int>;

SpawnScreenshotsStats MakeEmptySpawnScreenshotsStats()
{
  SpawnScreenshotsStats r;
  r["nbSuccess"] = 0;
  r["nbProcessFailures"] = 0;
  r["nbProcessHung"] = 0;
  r["nbEmptyRendering"] = 0;
  return r;
}


void spawnScreenshots(const std::string & exeName, bool flagAsync)
{
  using namespace BABYLON::SamplesInfo;

#ifdef _WIN32
  BABYLON_LOG_ERROR("spawnScreenshots", " needs fix under windows...");
  return;
#endif

  auto spawnScreenshotsStats = MakeEmptySpawnScreenshotsStats();

  BABYLON::Samples::SamplesIndex& samplesIndex = Samples::SamplesIndex::Instance();
  auto samples = samplesIndex.getSampleNames();
  for (auto sampleName : samples)
  {
    BABYLON_LOG_INFO("ScreenshotAllSamples", sampleName);

    std::vector<std::string> command =  { exeName, "-s", sampleName, "-p"};
    if (flagAsync)
      command.push_back("-A");

      
    SpawnOptions spawnOptions;
    spawnOptions.MaxExecutionTimeSeconds = 15.;
    spawnOptions.CopyOutputToMainProgramOutput = false;
    auto spawnResult = SpawnWaitSubProcess(command, spawnOptions);

    if (spawnResult.ExitStatus != 0)
    {
      BABYLON_LOG_WARN("ScreenshotAllSamples", "Subprocess has failed for sample ", sampleName);
      std::string outputFilename = screenshotsDirectory() + "/" + sampleName + ".Fail.txt";
      FILE* f                    = fopen(outputFilename.c_str(), "w");
      fprintf(f, "%s", spawnResult.StdOutErr.c_str());
      fclose(f);
      ++spawnScreenshotsStats["nbProcessFailures"];
    }

    if (spawnResult.MaxExecutionTimePassed)
    {
      BABYLON_LOG_WARN("ScreenshotAllSamples", "Subprocess MaxExecutionTimePassed for sample ",
                       sampleName);
      std::string outputFilename = screenshotsDirectory() + "/" + sampleName + ".TooSlow.txt";
      FILE * f = fopen(outputFilename.c_str(), "w");
      fprintf(f, "Process was killed because it took too long");
      fclose(f);
      ++spawnScreenshotsStats["nbProcessHung"];
    }

    if ( (spawnResult.ExitStatus == 0) && (!spawnResult.MaxExecutionTimePassed))
    {
      bool isRenderingEmpty = BABYLON::Samples::ReadScreenshot_IsImageEmpty(sampleName);
      if (isRenderingEmpty) {
        BABYLON_LOG_WARN("ScreenshotAllSamples", "Empty 3D rendering for sample ",
                         sampleName);
        ++spawnScreenshotsStats["nbEmptyRendering"];

        std::string imageFilename = screenshotsDirectory() + "/" + sampleName + ".jpg";
        Filesystem::removeFile(imageFilename);

        std::string outputFilename = screenshotsDirectory() + "/" + sampleName + ".EmptyRendering.txt";
        FILE * f = fopen(outputFilename.c_str(), "w");
        fprintf(f, "3D Rendering is empty");
        fclose(f);
      }
      else
        ++spawnScreenshotsStats["nbSuccess"];
    }
  }

  // Write Stats file
  {
    nlohmann::json jsonData;
    jsonData = spawnScreenshotsStats;
    std::string outputFilename = screenshotsDirectory() + "/_stats.txt";
    FILE * f = fopen(outputFilename.c_str(), "w");
    std::string jsonString = jsonData.dump(4);
    fprintf(f, "%s", jsonString.c_str());
    fclose(f);
    BABYLON_LOG_INFO("spawnScreenshots", "stats: ", jsonData.dump());
  }
}


} // namespace impl
} // namespace BABYLON
