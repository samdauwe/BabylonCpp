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
