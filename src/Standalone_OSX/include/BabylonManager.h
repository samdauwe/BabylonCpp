#ifndef BABYLON_MANAGER_H
#define BABYLON_MANAGER_H

#include <memory>

namespace BABYLON {
class IRenderableScene;
}

class BasicCanvas;

class BabylonManager {
public:
  int _width = 0;
  int _height = 0;
  
  BabylonManager(int width, int height);
  ~BabylonManager();
  
  void render();
  void setSize(int width, int height);
  
private:
  // Babylon scene related variables
  std::unique_ptr<BasicCanvas> _renderCanvas;
  std::shared_ptr<BABYLON::IRenderableScene> _renderableScene;
}; // end of class BABYLON_MANAGER_H

#endif
