#include <babylon/inspector/babylon_inspector_app.h>
#include "SamplesRunner/HelloScene.h"
#include "chdir_to_exe.h"

int main(int , char** argv)
{
  ChdirToExe(argv[0]);
  BABYLON::BabylonInspectorApp app;
  app.RunApp(SamplesRunner::MakeHelloScene());
  return 0;
}
