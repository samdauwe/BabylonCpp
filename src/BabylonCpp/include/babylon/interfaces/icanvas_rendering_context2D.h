#ifndef BABYLON_INTERFACES_ICANVAS_RENDERING_CONTEXT2D_H
#define BABYLON_INTERFACES_ICANVAS_RENDERING_CONTEXT2D_H

#include <iostream>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/maths/isize.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ICanvasRenderingContext2D {

public:
  std::string fillStyle;
  std::string font;
  void fillRect(int x, int y, int width, int height)
  {
    std::cout << x << ", " << y << ", ";
    std::cout << width << ", " << height << ", " << '\n';
  }
  void fillText(const std::string& text, int x, int y)
  {
    std::cout << x << ", " << y << ", ";
    std::cout << text << ", " << '\n';
  }
  ISize measureText(const std::string& text)
  {
    std::cout << text << ", " << '\n';
    return ISize(0, 0);
  }
  void drawImage(const Image& /*img*/, int /*sx*/, int /*sy*/, int /*sWidth*/,
                 int /*sHeight*/, int /*dx*/, int /*dy*/, int /*dWidth*/,
                 int /*dHeight*/)
  {
  }

}; // end of class ICanvasRenderingContext2D

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_ICANVAS_RENDERING_CONTEXT2D_H
