#include <gtest/gtest.h>

#include <babylon/core/shared_queue.h>

TEST(TestSharedQueue, PushAndPop)
{
  using namespace BABYLON;
  SharedQueue<int> testQueue;
  EXPECT_TRUE(testQueue.empty());
  testQueue.push(2);
  EXPECT_FALSE(testQueue.empty());
  EXPECT_EQ(testQueue.size(), 1);
  int poppedItem;
  bool result = testQueue.tryAndPop(poppedItem);
  EXPECT_EQ(poppedItem, 2);
  EXPECT_TRUE(result);
  EXPECT_TRUE(testQueue.empty());
  result = testQueue.tryAndPop(poppedItem);
  EXPECT_FALSE(result);
}
