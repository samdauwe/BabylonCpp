#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/core/tree.h>

/**
 * @brief Creates a basic integer tree, with the following layout:
 *  Root
 *   └─-(1)
 *       └─-(4)
 *   └─-(2)
 *       └─-(5)
 *           └─-(7)
 *   └─-(3)
 *       └─-(6)
 */
void initializeChildNodes(BABYLON::Tree<int>& tree)
{
  using namespace BABYLON;

  tree.root().addChild(1).addChild(4);
  tree.root().addChild(2).addChild(5).addChild(7);
  tree.root().addChild(3).addChild(6);
}

/**
 * @brief Creates a basic integer tree using the sorting node creator, with the
 * following layout:
 *  Root
 *   └─-(1)
 *       └─-(4)
 *   └─-(2)
 *       └─-(5)
 *           └─-(7)
 *   └─-(3)
 *       └─-(6)
 */
void initializeChildNodesSorted(BABYLON::Tree<int>& tree)
{
  using namespace BABYLON;

  const auto comparator
    = [](const int& a, const int& b) -> bool { return a < b; };

  tree.root()
    .addChildSorted(2, comparator)
    .addChildSorted(5, comparator)
    .addChildSorted(7, comparator);
  tree.root().addChildSorted(1, comparator).addChildSorted(4, comparator);
  tree.root().addChildSorted(3, comparator).addChildSorted(6, comparator);
}

TEST(TestTree, BasicTree)
{
  using namespace BABYLON;

  Tree<int> tree(0);
  initializeChildNodes(tree);

  EXPECT_EQ(tree.size(), 8ull);
  EXPECT_EQ(tree.root().arity(), 3ull);
  EXPECT_EQ(tree.root().depth(), 0u);
  EXPECT_TRUE(tree.root().isRoot());
  EXPECT_FALSE(tree.root().isLeaf());
}

TEST(TestTree, BasicSortedTree)
{
  using namespace BABYLON;

  Tree<int> tree(0);
  initializeChildNodesSorted(tree);

  EXPECT_EQ(tree.size(), 8ull);
  EXPECT_EQ(tree.root().arity(), 3ull);
  EXPECT_EQ(tree.root().depth(), 0u);
  EXPECT_TRUE(tree.root().isRoot());
  EXPECT_FALSE(tree.root().isLeaf());

  // Iterate with root node included
  {
    const std::vector<int> e{0, 1, 4, 2, 5, 7, 3, 6};
    std::vector<int> v;
    for (auto& item : tree) {
      v.emplace_back(item.data());
    }
    EXPECT_THAT(v, ::testing::ContainerEq(e));
  }
}

TEST(TestTree, Iterate)
{
  using namespace BABYLON;

  Tree<int> tree(0);
  initializeChildNodes(tree);

  // Iterate with root node included
  {
    const std::vector<int> e{0, 1, 4, 2, 5, 7, 3, 6};
    std::vector<int> v;
    for (auto& item : tree) {
      v.emplace_back(item.data());
    }
    EXPECT_THAT(v, ::testing::ContainerEq(e));
  }

  // Iterate forward without root node included
  {
    const std::vector<int> e{1, 4, 2, 5, 7, 3, 6};
    std::vector<int> v;
    for (auto it = tree.begin(); ++it != tree.end();) {
      v.emplace_back((*it).data());
    }
    EXPECT_THAT(v, ::testing::ContainerEq(e));
  }

  // Iterate backward without root node included
  {
    const std::vector<int> e{6, 3, 7, 5, 2, 4, 1};
    std::vector<int> v;
    for (auto it = tree.end(); --it != tree.begin();) {
      v.emplace_back((*it).data());
    }
    EXPECT_THAT(v, ::testing::ContainerEq(e));
  }
}
