/*
  Copyright (C) 2017 by Sergey A Kryukov: derived work
  http://www.SAKryukov.org
  http://www.codeproject.com/Members/SAKryukov
  https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed

  Based on original work by Sergey Ryazanov:
  "The Impossibly Fast C++ Delegates", 18 Jul 2005
  https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates

  MIT license:
  http://en.wikipedia.org/wiki/MIT_License

  Original publication:
   https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed
*/

#pragma once

namespace SA {

template <typename T>
class delegate_base;

template <typename RET, typename... PARAMS>
class delegate_base<RET(PARAMS...)> {

protected:
  using stub_type = RET (*)(void* this_ptr, PARAMS...);

  struct InvocationElement {
    InvocationElement() = default;
    InvocationElement(void* this_ptr, stub_type aStub)
        : object(this_ptr), stub(aStub)
    {
    }
    void Clone(InvocationElement& target) const
    {
      target.stub   = stub;
      target.object = object;
    } // Clone
    bool operator==(const InvocationElement& another) const
    {
      return another.stub == stub && another.object == object;
    } //==
    bool operator!=(const InvocationElement& another) const
    {
      return another.stub != stub || another.object != object;
    } //!=
    void* object   = nullptr;
    stub_type stub = nullptr;
  }; // InvocationElement

}; // class delegate_base

} /* namespace SA */
