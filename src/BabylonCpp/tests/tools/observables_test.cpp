#include <gtest/gtest.h>

#include <babylon/math/vector2.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

struct Mouse {

  enum class ClickType : int { LEFT = 0x01, RIGHT = 0x02 };

  Mouse()
  {
  }

  ~Mouse()
  {
  }

  void raiseLeftClick()
  {
    if (onClickObservable.hasObservers()) {
      onClickObservable.notifyObservers(this, int(ClickType::LEFT));
    }
  }

  void raiseRightClick()
  {
    if (onClickObservable.hasObservers()) {
      onClickObservable.notifyObservers(this, int(ClickType::RIGHT));
    }
  }

  void raiseMove(Vector2&& position)
  {
    if (onMoveObservable.hasObservers()) {
      onMoveObservable.notifyObservers(&position);
    }
  }

  Observable<Mouse> onClickObservable;
  Observable<Vector2> onMoveObservable;

}; // end of struct button

TEST(TestObservables, MouseClickObserver)
{
  using namespace BABYLON;

  size_t leftClickCounter = 0, rightClickCounter = 0;
  Mouse mouse;
  auto& obervable = mouse.onClickObservable;
  // Add an Observer
  EXPECT_FALSE(obervable.hasObservers());
  auto leftClickObserver = obervable.add(
    [&leftClickCounter](Mouse*, EventState&) { ++leftClickCounter; },
    int(Mouse::ClickType::LEFT));
  EXPECT_TRUE(obervable.hasObservers());
  // Remove the previously registered Observer
  obervable.remove(leftClickObserver);
  EXPECT_FALSE(obervable.hasObservers());
  // Add the previously removed observer
  leftClickObserver = obervable.add(
    [&leftClickCounter](Mouse*, EventState&) { ++leftClickCounter; },
    int(Mouse::ClickType::LEFT));
  EXPECT_TRUE(obervable.hasObservers());
  // Add another observer
  obervable.add(
    [&rightClickCounter](Mouse*, EventState&) { ++rightClickCounter; },
    int(Mouse::ClickType::RIGHT));
  EXPECT_TRUE(obervable.hasObservers());
  // Raise events
  for (size_t i = 0; i != 5; ++i) {
    // Raise left click event
    EXPECT_EQ(leftClickCounter, i);
    EXPECT_EQ(rightClickCounter, 2 * i);
    mouse.raiseLeftClick();
    EXPECT_EQ(leftClickCounter, (i + 1));
    EXPECT_EQ(rightClickCounter, 2 * i);
    // Raise right click event
    mouse.raiseRightClick();
    mouse.raiseRightClick();
    EXPECT_EQ(leftClickCounter, (i + 1));
    EXPECT_EQ(rightClickCounter, 2 * (i + 1));
  }
  // Remove all Observers
  obervable.clear();
  EXPECT_FALSE(obervable.hasObservers());
}

TEST(TestObservables, MouseObserver)
{
  using namespace BABYLON;

  Vector2 mousePosition;
  Mouse mouse;
  auto& obervable = mouse.onMoveObservable;
  // Add an Observer
  EXPECT_FALSE(obervable.hasObservers());
  auto observer = obervable.add([&mousePosition](
    Vector2* position, EventState&) { mousePosition = *position; });
  EXPECT_TRUE(obervable.hasObservers());
  // Remove the previously registered Observer
  obervable.remove(observer);
  EXPECT_FALSE(obervable.hasObservers());
  // Add the previously removed observer
  observer = obervable.add([&mousePosition](Vector2* position, EventState&) {
    mousePosition = *position;
  });
  EXPECT_TRUE(obervable.hasObservers());
  // Raise an event
  float x = 0.f, y = 0.f;
  for (size_t i = 0; i != 5; ++i) {
    x = y = static_cast<float>(i);
    mouse.raiseMove(Vector2{x, y});
    EXPECT_EQ(mousePosition.x, x);
    EXPECT_EQ(mousePosition.y, y);
  }
  // Remove all Observers
  obervable.clear();
  EXPECT_FALSE(obervable.hasObservers());
}

} // end of namespace BABYLON
