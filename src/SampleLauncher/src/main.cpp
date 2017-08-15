#include <argtable3/argtable3.h>
#include <babylon/babylon_version.h>
#include <babylon/samples/sample_launcher.h>
#include <babylon/samples/samples_index.h>

/**
 * @brief The sample launcher.
 * @param l list samples if l > 0
 * @param v enable verbose mode if v > 0
 * @param sample name of the sample to run
 * @return exit code
 */
int sampleLauncherMain(int l, int v, const char* sample)
{
  using namespace BABYLON::Samples;
  int exitcode = 0;
  if (l > 0) {
    const auto sampleNames = SamplesIndex::getSampleNames();
    printf("Found %ld sample(s):\n", sampleNames.size());
    for (const auto& sampleName : sampleNames) {
      printf(" - %s\n", sampleName.c_str());
    }
  }
  else {
    if (v > 0) {
      printf("verbose is enabled (-v)\n");
    }
    // Check if sample exists and is enabled
    const std::string sampleName{sample};
    if (!SamplesIndex::sampleExists(sampleName)) {
      printf("Sample with name \"%s\" does not exists.\n", sample);
      return 1;
    }
    if (!SamplesIndex::isSampleEnabled(sampleName)) {
      printf("Sample with name \"%s\" is not enabled.\n", sample);
      return 1;
    }
    // Create the sample launcher
    SampleLauncher sampleLauncher;
    if (sampleLauncher.intialize()) {
      // Create the renderable scene
      auto canvas = sampleLauncher.getRenderCanvas();
      auto scene  = SamplesIndex::createRenderableScene(sampleName, canvas);
      sampleLauncher.setRenderableScene(scene);
      // Run the example
      exitcode = sampleLauncher.run();
    }
  }
  return exitcode;
}

int main(int argc, char** argv)
{
  /** Program arguments definition **/
  struct arg_lit* list   = arg_lit0("lL", NULL, "list samples");
  struct arg_str* sample = arg_str0(
    "S", "sample", "<SAMPE>", "sample to launch (default is \"BasicScene\")");
  struct arg_lit* verbose = arg_lit0("v", "verbose,debug", "verbose messages");
  struct arg_lit* help    = arg_lit0(NULL, "help", "print this help and exit");
  struct arg_lit* version
    = arg_lit0(NULL, "version", "print version information and exit");
  struct arg_end* end  = arg_end(20);
  void* argtable[]     = {list, sample, verbose, help, version, end};
  const char* progname = "SampleLauncher";
  int nerrors;
  int exitcode = 0;

  /** Verify the argtable[] entries were allocated sucessfully **/
  if (arg_nullcheck(argtable) != 0) {
    /** NULL entries were detected, some allocations must have failed **/
    printf("%s: insufficient memory\n", progname);
    exitcode = 1;
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
  }

  /** Set the default sample values prior to parsing **/
  sample->sval[0] = "BasicScene";

  /** Parse the command line as defined by argtable[] **/
  nerrors = arg_parse(argc, argv, argtable);

  /** Special case: '--help' takes precedence over error reporting **/
  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    printf(
      "This program acts as a sample launcher for demonstrating the usage of "
      "the BabylonCpp library\n");
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    exitcode = 0;
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
  }

  /** Special case: '--version' takes precedence error reporting **/
  if (version->count > 0) {
    printf("%s\n", BABYLONCPP_NAME_VERSION);
    exitcode = 0;
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
  }

  /** If the parser returned any errors then display them and exit **/
  if (nerrors > 0) {
    /** Display the error details contained in the arg_end struct. **/
    arg_print_errors(stdout, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    exitcode = 1;
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
  }

  /** Normal case: run sample **/
  exitcode = sampleLauncherMain(list->count, verbose->count, sample->sval[0]);

  /** Deallocate each non-null entry in argtable[] **/
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  return exitcode;
}
