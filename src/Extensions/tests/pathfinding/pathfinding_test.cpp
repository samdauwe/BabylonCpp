#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/extensions/pathfinding/rectangular_maze.h>

TEST(TestAny, PathFindingWithoutCosts)
{
  using namespace BABYLON::Extensions;
  using L = RectangularMaze::Location;

  // Calculate path in grid containing wall and using costs
  std::vector<L> path;
  {
    RectangularMaze maze(10, 10);
    maze.generateEmptyGrid();
    maze.addRectagularWall(L{7, 1}, L{8, 4});
    path = maze.findPath(L{8, 0}, L{5, 9});
  }
  // Compare with expected path
  const std::vector<L> expectedPath{L{8, 0}, L{7, 0}, L{6, 0}, L{5, 0}, L{5, 1},
                                    L{5, 2}, L{5, 3}, L{5, 4}, L{5, 5}, L{5, 6},
                                    L{5, 7}, L{5, 8}, L{5, 9}};
  EXPECT_EQ(path.size(), expectedPath.size());
  for (std::size_t i = 0; i < path.size(); ++i) {
    std::size_t x1, y1, x2, y2;
    std::tie(x1, y1) = path[i];
    std::tie(x2, y2) = expectedPath[i];
    EXPECT_EQ(x1, x2);
    EXPECT_EQ(y1, y2);
  }
}

TEST(TestAny, PathFindingWithCosts)
{
  using namespace BABYLON::Extensions;
  using L = RectangularMaze::Location;

  // Calculate path in grid containing wall and using costs
  std::vector<L> path;
  {
    RectangularMaze maze(10, 10);
    maze.generateEmptyGrid();
    maze.addRectagularWall(L{7, 1}, L{8, 4});
    for (auto& l : {L{7, 0}, L{7, 5}, L{6, 6}, L{5, 7}}) {
      maze.setCost(l, 5.0);
    }
    path = maze.findPath(L{8, 0}, L{5, 9});
  }
  // Compare with expected path
  const std::vector<L> expectedPath{
    L{8, 0}, L{9, 0}, L{9, 1}, L{9, 2}, L{9, 3}, L{9, 4}, L{9, 5}, L{8, 5},
    L{8, 6}, L{7, 6}, L{7, 7}, L{6, 7}, L{6, 8}, L{5, 8}, L{5, 9}};
  EXPECT_EQ(path.size(), expectedPath.size());
  for (std::size_t i = 0; i < path.size(); ++i) {
    std::size_t x1, y1, x2, y2;
    std::tie(x1, y1) = path[i];
    std::tie(x2, y2) = expectedPath[i];
    EXPECT_EQ(x1, x2);
    EXPECT_EQ(y1, y2);
  }
}
