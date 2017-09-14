#ifndef BABYLON_INTERFACES_ICANVAS_RENDERING_CONTEXT2D_H
#define BABYLON_INTERFACES_ICANVAS_RENDERING_CONTEXT2D_H

#include <iostream>

#include <babylon/babylon_global.h>
#include <babylon/math/isize.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ICanvasRenderingContext2D {

public:
  string_t fillStyle;
  string_t font;
  void fillRect(int x, int y, int width, int height)
  {
    ::std::cout << x << ", " << y << ", ";
    ::std::cout << width << ", " << height << ", " << '\n';
  }
  void fillText(const string_t& text, int x, int y)
  {
    ::std::cout << x << ", " << y << ", ";
    ::std::cout << text << ", " << '\n';
  }
  ISize measureText(const string_t& text)
  {
    ::std::cout << text << ", " << '\n';
    return ISize(0, 0);
  }

}; // end of class ICanvasRenderingContext2D

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_ICANVAS_RENDERING_CONTEXT2D_H
