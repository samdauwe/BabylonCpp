#include <oimo/collision/broadphase/pair.h>

#include <utility>

namespace OIMO {

Pair::Pair() : shape1{nullptr}, shape2{nullptr}
{
}
Pair::Pair(Shape* s1, Shape* s2) : shape1{s1}, shape2{s2}
{
}

Pair::Pair(const Pair& p) : shape1{p.shape1}, shape2{p.shape2}
{
}

Pair::Pair(Pair&& p)
{
  *this = std::move(p);
}

Pair::~Pair()
{
}

Pair& Pair::operator=(const Pair& p)
{
  if (&p != this) {
    shape1 = p.shape1;
    shape2 = p.shape2;
  }

  return *this;
}

Pair& Pair::operator=(Pair&& p)
{
  if (&p != this) {
    std::swap(shape1, p.shape1);
    std::swap(shape2, p.shape2);
  }

  return *this;
}

} // end of namespace OIMO
