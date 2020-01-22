#include <gtest/gtest.h>

#include <babylon/samples/samples_info.h>

using namespace BABYLON;

TEST(SamplesInfo, AllSamples)
{
  auto allSamples = SamplesInfo::AllSamples();
  EXPECT_GT(allSamples.size(), 0);
}