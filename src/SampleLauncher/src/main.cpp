#include <argtable3/argtable3.h>

#include <sstream>
#include <utility>
#include <babylon/babylon_version.h>
#include <babylon/core/delegates/delegate.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/samples/sample_launcher.h>
#include <babylon/samples/samples_index.h>
#ifdef WITH_IMGUI_BABYLON
#include <babylon/inspector/inspector.h>
#endif

#if _MSC_VER
#include <windows.h>
#include <direct.h>
#include <filesystem>
#endif

struct ConsoleLogger {

  using log_message_t = SA::delegate<void(const BABYLON::LogMessage&)>;

  static void onLogMessage(const BABYLON::LogMessage& logMessage)
  {
    printf("%s\n", logMessage.toString().c_str());
#if _MSC_VER
    std::string msg_n = logMessage.toString() + "\n";
    OutputDebugString(msg_n.c_str());
#endif
  }

  static log_message_t logListenerDelegate;

}; // end of struct ConsoleLogger

ConsoleLogger::log_message_t ConsoleLogger::logListenerDelegate
  = SA::delegate<void(
    const BABYLON::LogMessage&)>::create<&ConsoleLogger::onLogMessage>();

/**
 * @brief Initializes the logging.
 */
void initializeLogging()
{
  static_assert(
    std::is_same<SA::delegate<void(const BABYLON::LogMessage&)>,
                 decltype(ConsoleLogger::logListenerDelegate)>::value,
    "!");
  // Intialize log levels
  std::vector<std::pair<unsigned int, std::string>> _logLevels;
  for (auto& logLevel : BABYLON::LogLevels::Levels) {
    unsigned int logType = logLevel.first;
    if ((logType != BABYLON::LogLevels::LEVEL_QUIET)
        && (logType != BABYLON::LogLevels::LEVEL_TRACE)) {
      _logLevels.emplace_back(logLevel);
    }
  }
  // Subscribe to channels
  for (auto& logLevel : _logLevels) {
    unsigned int logType = logLevel.first;
    if (logType != BABYLON::LogLevels::LEVEL_QUIET) {
      BABYLON::LoggerInstance().registerLogMessageListener(
        logType, ConsoleLogger::logListenerDelegate);
    }
  }
}

// a simple "Consumable value", i.e a value that is reset when it is consumed
template<typename T>
class ConsumableValue
{
public:
  ConsumableValue(std::optional<T> v = std::nullopt) : _value(v) {};
  bool hasValue() { return _value.has_value(); }
  void setValue(T value) { _value = value; }
  T consumeValue() { return std::exchange(_value, std::nullopt).value(); } // will throw if empty!
private:
   std::optional<T> _value;
};

/**
 * @brief Runs a sample with the specified name
 * @param samples list with available samples
 * @param sampleName name of the sample to run
 * @param showInspectorWindow whether or not to shoz the inspector window
 * @return exit code
 */
int runSample(const BABYLON::Samples::SamplesIndex& samples,
              const std::string& startSampleName, bool showInspectorWindow,
              long runTimeMillis = 0)
{
  // The inspector enables to switch to another sample via
  // the callback BABYLON::Inspector::OnSampleChanged
  // which will the content of "nextSample"
  ConsumableValue<std::string> nextSample(startSampleName);
  #ifdef WITH_IMGUI_BABYLON
  BABYLON::Inspector::OnSampleChanged = [&nextSample](const std::string & s) {
    nextSample.setValue(s);
  };
  #endif

  int exitcode = 0;
  // Create the sample launcher
  BABYLON::Samples::SampleLauncherOptions options;
  options.showInspectorWindow = showInspectorWindow;


  while (nextSample.hasValue())
  {
    std::string sampleName = nextSample.consumeValue();
    BABYLON::Samples::SampleLauncher sampleLauncher{options};
    if (sampleLauncher.intialize()) {
      // Create the renderable scene
      auto canvas = sampleLauncher.getRenderCanvas();
      auto scene  = samples.createRenderableScene(sampleName, canvas);
      sampleLauncher.setRenderableScene(scene);
      // Run the example: it will stop when a new sample was selected via the inspector
      exitcode = sampleLauncher.run([&nextSample]() { return nextSample.hasValue(); }, runTimeMillis);
    }
  }
  return exitcode;
}

/**
 * @brief The sample launcher.
 * @param l list samples if l > 0
 * @param v enable verbose mode if v > 0
 * @param sample name of the sample to run
 * @return exit code
 */
int sampleLauncherMain(int l, int v, int i, const char* sampleGroup,
                       const char* sample)
{
  using namespace BABYLON::Samples;
  SamplesIndex samples;
  int exitcode = 0;
  if (l > 0) {
    std::ostringstream oss;
    // Get sample names and category names
    auto sampleNames   = samples.getSampleNames();
    auto categoryNames = samples.getCategoryNames();
    oss << "Found " << sampleNames.size() << " in " << categoryNames.size()
        << " categories:\n";
    // Print categorized samples
    for (const auto& categoryName : categoryNames) {
      sampleNames = samples.getSampleNamesInCategory(categoryName);
      oss << "       |- ";
      if (sampleNames.size() < 10) {
        oss << " " << sampleNames.size();
      }
      else if (sampleNames.size() < 100) {
        oss << sampleNames.size();
      }
      oss << " sample(s) in category \"" << categoryName.c_str() << "\"\n";
      for (const auto& sampleName : sampleNames) {
        oss << "           |- " << sampleName.c_str() << "\n";
      }
    }
    printf("%s", oss.str().c_str());
  }
  else {
    if (v > 0) {
      // Enable console logging
      initializeLogging();
    }
    // Check for rendering of sample group
    const std::string categoryName{sampleGroup};
    if (!categoryName.empty()
        && (samples.categoryExists(categoryName) || categoryName == "All")) {
      std::vector<std::string> categoryNames;
      if (categoryName == "All") {
        categoryNames = samples.getCategoryNames();
      }
      else {
        categoryNames = {categoryName};
      }
      for (const auto& _categoryName : categoryNames) {
        const auto sampleNames
          = samples.getSampleNamesInCategory(_categoryName);
        for (const auto& sampleName : sampleNames) {
          exitcode = runSample(samples, sampleName, i > 0, 10000);
          if (exitcode == 0) {
            break;
          }
        }
      }
    }
    else {
      // Check for rendering of sample
      const std::string sampleName{sample};
      if (!samples.sampleExists(sampleName)) {
        printf("Sample with name \"%s\" does not exists.\n", sample);
        return 1;
      }
      if (!samples.isSampleEnabled(sampleName)) {
        printf("Sample with name \"%s\" is not enabled.\n", sample);
        return 1;
      }
      // Run the sample
      exitcode = runSample(samples, sampleName, i > 0);
    }
  }
  return exitcode;
}

#ifdef _MSC_VER
void ChdirToExePath(char *argv0)
{
  std::filesystem::path exeFolder = std::filesystem::weakly_canonical(std::filesystem::path(argv0)).parent_path();
#ifdef _WIN32
  _chdir(exeFolder.string().c_str());
#else
  chdir(exeFolder.string().c_str());
#endif
}
#endif // _MSC_VER

int main(int argc, char** argv)
{
#ifdef _MSC_VER
  ChdirToExePath(argv[0]);
#endif
  /** Program arguments definition **/
  struct arg_lit* list   = arg_lit0("lL", NULL, "list samples");
  struct arg_str* sample = arg_str0(
    "S", "sample", "<SAMPE>", "sample to launch (default is \"BasicScene\")");
  struct arg_str* sampleGroup = arg_str0(
    "G", "sample-group", "<SAMPE-GROUP>",
    "sample group to launch (sample-group \"All\" contains all samples)");
  struct arg_lit* verbose = arg_lit0("v", "verbose,debug", "verbose messages");
  struct arg_lit* inspector
    = arg_lit0("i", "inspector", "show inspector window");
  struct arg_lit* help = arg_lit0(NULL, "help", "print this help and exit");
  struct arg_lit* version
    = arg_lit0(NULL, "version", "print version information and exit");
  struct arg_end* end = arg_end(20);
  void* argtable[]
    = {list, sample, sampleGroup, verbose, inspector, help, version, end};
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

  /**  Set the default sample group prior to parsing */
  sampleGroup->sval[0] = "";

  /** Parse the command line as defined by argtable[] **/
  nerrors = arg_parse(argc, argv, argtable);

  /** Special case: '--help' takes precedence over error reporting **/
  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    printf(
      "This program acts as a sample launcher for demonstrating the usage of "
      "the BabylonCpp library\n");
    arg_print_glossary(stdout, argtable, "  %-35s %s\n");
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
  exitcode = sampleLauncherMain(list->count, verbose->count, inspector->count,
                                sampleGroup->sval[0], sample->sval[0]);

  /** Deallocate each non-null entry in argtable[] **/
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  return exitcode;
}
