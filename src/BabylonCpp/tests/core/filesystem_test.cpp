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

TEST(TestFilesystem, joinPath)
{
  using namespace BABYLON;

  ::std::array<::std::string, 4> path{{"/", "usr", "local", "bin"}};
  EXPECT_EQ(
    Filesystem::joinPath<::std::string>(path[0], path[1], path[2], path[3]),
    "/usr/local/bin");
}

TEST(TestFilesystem, removeExtension)
{
  using namespace BABYLON;

  EXPECT_EQ(Filesystem::removeExtension("man"), "man");
  EXPECT_EQ(Filesystem::removeExtension("job.sh"), "job");
  EXPECT_EQ(Filesystem::removeExtension("winhelp.exe"), "winhelp");
}
