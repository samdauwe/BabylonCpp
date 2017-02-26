#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/core/filesystem.h>

TEST(TestFilesystem, extension)
{
  using namespace BABYLON;

  EXPECT_EQ(Filesystem::extension("/usr/bin/man"), "");
  EXPECT_EQ(Filesystem::extension("c:\\windows\\winhelp.exe"), "exe");
}

TEST(TestFilesystem, baseDir)
{
  using namespace BABYLON;

  EXPECT_EQ(Filesystem::baseDir("/usr/bin/man"), "/usr/bin");
  EXPECT_EQ(Filesystem::baseDir("c:\\windows\\winhelp.exe"), "c:\\windows");
}

TEST(TestFilesystem, baseName)
{
  using namespace BABYLON;

  EXPECT_EQ(Filesystem::baseName("/usr/bin/man"), "man");
  EXPECT_EQ(Filesystem::baseName("c:\\windows\\winhelp.exe"), "winhelp.exe");
}

TEST(TestFilesystem, removeExtension)
{
  using namespace BABYLON;

  EXPECT_EQ(Filesystem::removeExtension("man"), "man");
  EXPECT_EQ(Filesystem::removeExtension("job.sh"), "job");
  EXPECT_EQ(Filesystem::removeExtension("winhelp.exe"), "winhelp");
}
