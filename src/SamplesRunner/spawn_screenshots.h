#include <string>

namespace BABYLON {
namespace impl {
// this implementation will spawn a new process for each sample
// (so that failing samples will not stop the screenshot generation)
void spawnScreenshots(const std::string & exeName, bool skipFailing = true);
}
}
