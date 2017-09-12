#include <gtest/gtest.h>

#include <babylon/babylon_stl.h>
#include <babylon/core/fast_func.h>

#include <iostream>
#include <memory>
#include <string>

using EventType = int;

class IEvent {
public:
  IEvent(void)                     = default;
  IEvent(const IEvent& p_rIEvent)  = delete;
  IEvent(const IEvent&& p_rIEvent) = delete;
  IEvent& operator=(const IEvent& p_rIEvent) = delete;
  IEvent& operator=(const IEvent&& p_rrIEvent) = delete;
  virtual ~IEvent(void)                        = default;

  virtual EventType getType(void) const   = 0;
  virtual std::string getName(void) const = 0;

}; // IEvent class

using EventSPtr = std::shared_ptr<IEvent>;

class TestEvent : public IEvent {
public:
  static const EventType Type = 0x4a6ee2cf;

private:
  int m_Data = 100;

public:
  TestEvent(void)          = default;
  virtual ~TestEvent(void) = default;

  virtual EventType getType(void) const final
  {
    return TestEvent::Type;
  }
  virtual std::string getName(void) const final
  {
    return "TestEvent";
  }

  int getData(void) const
  {
    return m_Data;
  }

}; // TestEvent class

extern void rawFreeFuncNoParamA(void)
{
  int i = 1 + 1;
  int a = i + 1;
  int b = a * i;
  b     = b * 2;
}
extern void rawFreeFuncNoParamB(void)
{
  return;
}
extern void rawFreeFuncWithParamA(EventSPtr);
extern void rawFreeFuncWithParamB(EventSPtr);
inline void rawInlineFreeFuncNoParamA(void)
{
  std::clog << "inline free function A\n";
}
inline void rawInlineFreeFuncNoParamB(void)
{
  std::clog << "inline free function B\n";
}
inline void rawInlineFreeFuncWithParamA(EventSPtr evt)
{
  const auto name = evt->getName();
}
inline void rawInlineFreeFuncWithParamB(EventSPtr evt)
{
  const auto name = evt->getName();
}

class SingleInheritBaseClass {
public:
  static void rawStaticMemFuncNoParamA(void)
  {
  }
  static void rawStaticMemFuncNoParamB(void)
  {
  }

  void rawMemFuncNoParamA(void)
  {
  }
  void rawMemFuncNoParamB(void)
  {
  }
  void rawMemFuncWithParamA(EventSPtr)
  {
  }
  void rawMemFuncWithParamB(EventSPtr)
  {
  }
};

class SingleInheritClass : public SingleInheritBaseClass {
public:
  SingleInheritClass()
  {
  }
  virtual ~SingleInheritClass()
  {
  }
  virtual void rawMemFuncNoParamA(void)
  {
  }
  virtual void rawMemFuncNoParamB(void)
  {
  }
  virtual void rawMemFuncWithParamA(EventSPtr)
  {
  }
  virtual void rawMemFuncWithParamB(EventSPtr)
  {
  }
};

TEST(TestFastFunc, EqualityFreeFuncNoParam)
{
  using namespace BABYLON;
  auto freeFuncA_1 = FastFunc<void(void)>(&rawFreeFuncNoParamA);
  auto freeFuncA_2 = FastFunc<void(void)>(&rawFreeFuncNoParamA);
  EXPECT_TRUE(freeFuncA_1 == freeFuncA_2);

  auto inlineFreeFuncA_1 = FastFunc<void(void)>(&rawInlineFreeFuncNoParamA);
  auto inlineFreeFuncA_2 = FastFunc<void(void)>(&rawInlineFreeFuncNoParamA);
  EXPECT_TRUE(inlineFreeFuncA_1 == inlineFreeFuncA_2);

  auto inlineFreeFuncParamA_1
    = FastFunc<void(EventSPtr)>(&rawInlineFreeFuncWithParamA);
  auto inlineFreeFuncParamA_2
    = FastFunc<void(EventSPtr)>(&rawInlineFreeFuncWithParamA);
  EXPECT_TRUE(inlineFreeFuncParamA_1 == inlineFreeFuncParamA_2);
}

TEST(TestFastFunc, InequalityFreeFuncNoParam)
{
  using namespace BABYLON;
  auto freeFuncA = FastFunc<void(void)>(&rawFreeFuncNoParamA);
  auto freeFuncB = FastFunc<void(void)>(&rawFreeFuncNoParamB);
  EXPECT_TRUE(freeFuncA != freeFuncB);

  auto inlineFreeFuncA = FastFunc<void(void)>(&rawInlineFreeFuncNoParamA);
  auto inlineFreeFuncB = FastFunc<void(void)>(&rawInlineFreeFuncNoParamB);
  EXPECT_TRUE(inlineFreeFuncA != inlineFreeFuncB);

  auto inlineFreeFuncParamA
    = FastFunc<void(EventSPtr)>(&rawInlineFreeFuncWithParamA);
  auto inlineFreeFuncParamB
    = FastFunc<void(EventSPtr)>(&rawInlineFreeFuncWithParamB);
  EXPECT_TRUE(inlineFreeFuncParamA != inlineFreeFuncParamB);
}

TEST(TestFastFunc, Lambda)
{
  using namespace BABYLON;
  FastFunc<void(void)> lambda_1([](void) { return; });
  FastFunc<void(void)> lambda_2([](void) { return; });
  auto lambda_3                 = FastFunc<void(void)>([](void) { return; });
  FastFunc<void(void)> lambda_4 = [](void) { return; };
  EXPECT_TRUE(lambda_1 != lambda_2);
  EXPECT_TRUE(lambda_1 != lambda_3);
  EXPECT_TRUE(lambda_1 != lambda_4);
}

TEST(TestFastFunc, EqualityStaticMemFuncNoParam)
{
  using namespace BABYLON;
  auto singleInheritStaticmemFuncNoParamA_1
    = FastFunc<void(void)>(&SingleInheritBaseClass::rawStaticMemFuncNoParamA);
  auto singleInheritStaticmemFuncNoParamA_2
    = FastFunc<void(void)>(&SingleInheritBaseClass::rawStaticMemFuncNoParamA);
  EXPECT_TRUE(singleInheritStaticmemFuncNoParamA_1
              == singleInheritStaticmemFuncNoParamA_2);
}

TEST(TestFastFunc, InequalityStaticMemFuncNoParam)
{
  using namespace BABYLON;
  auto singleInheritStaticmemFuncNoParamA
    = FastFunc<void(void)>(&SingleInheritBaseClass::rawStaticMemFuncNoParamA);
  auto singleInheritStaticmemFuncNoParamB
    = FastFunc<void(void)>(&SingleInheritBaseClass::rawStaticMemFuncNoParamB);
  EXPECT_TRUE(singleInheritStaticmemFuncNoParamA
              != singleInheritStaticmemFuncNoParamB);
}

TEST(TestFastFunc, EqualityMemFunc)
{
  using namespace BABYLON;

  SingleInheritBaseClass single_inhert_base_obj;
  auto singleInheritBMemFuncA_1 = FastFunc<void(void)>(
    &single_inhert_base_obj, &SingleInheritBaseClass::rawMemFuncNoParamA);
  auto singleInheritBMemFuncA_2 = FastFunc<void(void)>(
    &single_inhert_base_obj, &SingleInheritBaseClass::rawMemFuncNoParamA);
  EXPECT_TRUE(singleInheritBMemFuncA_1 == singleInheritBMemFuncA_2);
  auto singleInheritBMemFuncParamA_1 = FastFunc<void(EventSPtr)>(
    &single_inhert_base_obj, &SingleInheritBaseClass::rawMemFuncWithParamA);
  auto singleInheritBMemFuncParamA_2 = FastFunc<void(EventSPtr)>(
    &single_inhert_base_obj, &SingleInheritBaseClass::rawMemFuncWithParamA);
  EXPECT_TRUE(singleInheritBMemFuncParamA_1 == singleInheritBMemFuncParamA_2);

  SingleInheritBaseClass single_inhert_obj;
  auto singleInheritMemFuncA_1 = FastFunc<void(void)>(
    &single_inhert_obj, &SingleInheritBaseClass::rawMemFuncNoParamA);
  auto singleInheritMemFuncA_2 = FastFunc<void(void)>(
    &single_inhert_obj, &SingleInheritBaseClass::rawMemFuncNoParamA);
  EXPECT_TRUE(singleInheritMemFuncA_1 == singleInheritMemFuncA_2);
  auto singleInheritMemFuncParamA_1 = FastFunc<void(EventSPtr)>(
    &single_inhert_obj, &SingleInheritBaseClass::rawMemFuncWithParamA);
  auto singleInheritMemFuncParamA_2 = FastFunc<void(EventSPtr)>(
    &single_inhert_obj, &SingleInheritBaseClass::rawMemFuncWithParamA);
  EXPECT_TRUE(singleInheritMemFuncParamA_1 == singleInheritMemFuncParamA_2);
}

TEST(TestFastFunc, InequalityMemFunc)
{
  using namespace BABYLON;

  SingleInheritBaseClass single_inhert_base_obj;
  auto singleInheritBMemFuncA = FastFunc<void(void)>(
    &single_inhert_base_obj, &SingleInheritBaseClass::rawMemFuncNoParamA);
  auto singleInheritBMemFuncB = FastFunc<void(void)>(
    &single_inhert_base_obj, &SingleInheritBaseClass::rawMemFuncNoParamB);
  EXPECT_TRUE(singleInheritBMemFuncA != singleInheritBMemFuncB);
  auto singleInheritBMemFuncParamA = FastFunc<void(EventSPtr)>(
    &single_inhert_base_obj, &SingleInheritBaseClass::rawMemFuncWithParamA);
  auto singleInheritBMemFuncParamB = FastFunc<void(EventSPtr)>(
    &single_inhert_base_obj, &SingleInheritBaseClass::rawMemFuncWithParamB);
  EXPECT_TRUE(singleInheritBMemFuncParamA != singleInheritBMemFuncParamB);

  SingleInheritClass single_inhert_obj;
  auto singleInheritMemFuncA = FastFunc<void(void)>(
    &single_inhert_obj, &SingleInheritClass::rawMemFuncNoParamA);
  auto singleInheritMemFuncB = FastFunc<void(void)>(
    &single_inhert_obj, &SingleInheritClass::rawMemFuncNoParamB);
  EXPECT_TRUE(singleInheritMemFuncA != singleInheritMemFuncB);
  auto singleInheritMemFuncParamA = FastFunc<void(EventSPtr)>(
    &single_inhert_obj, &SingleInheritClass::rawMemFuncWithParamA);
  auto singleInheritMemFuncParamB = FastFunc<void(EventSPtr)>(
    &single_inhert_obj, &SingleInheritClass::rawMemFuncWithParamB);
  EXPECT_TRUE(singleInheritMemFuncParamA != singleInheritMemFuncParamB);
}
