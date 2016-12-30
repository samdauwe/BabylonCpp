#include <babylon/math/isize.h>

namespace BABYLON {

ISize::ISize() : width{0}, height{0}
{
}

ISize::ISize(int iWidth, int iHeight) : width{iWidth}, height{iHeight}
{
}

ISize::~ISize()
{
}

} // end of namespace BABYLON
