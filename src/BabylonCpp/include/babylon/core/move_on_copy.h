#ifndef BABYLON_CORE_MOVE_ON_COPY_H
#define BABYLON_CORE_MOVE_ON_COPY_H

#include <utility>

namespace BABYLON {

template <typename Moveable>
struct MoveOnCopy {
  mutable Moveable _moveOnly;

  explicit MoveOnCopy(Moveable&& m) : _moveOnly(std::move(m))
  {
  }
  MoveOnCopy(MoveOnCopy const& t) : _moveOnly(std::move(t._moveOnly))
  {
  }
  MoveOnCopy(MoveOnCopy&& t) : _moveOnly(std::move(t._moveOnly))
  {
  }

  MoveOnCopy& operator=(MoveOnCopy const& other)
  {
    _moveOnly = std::move(other._moveOnly);
    return *this;
  }

  MoveOnCopy& operator=(MoveOnCopy&& other)
  {
    _moveOnly = std::move(other._moveOnly);
    return *this;
  }

  void operator()()
  {
    _moveOnly();
  }

  Moveable& get()
  {
    return _moveOnly;
  }

  Moveable release()
  {
    return std::move(_moveOnly);
  }
};

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_MOVE_ON_COPY_H
