#include "babylon_run_standalone.h"
#include "HelloScene.h"

int main()
{
  auto scene = MakeHelloScene();
  bool fullScreen = false;
  runStandalone(scene, fullScreen);
  return 0;
}
