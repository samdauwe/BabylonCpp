#ifndef BABYLON_EXTENSIONS_PATH_FINDING_RECTANGULAR_MAZE_H
#define BABYLON_EXTENSIONS_PATH_FINDING_RECTANGULAR_MAZE_H

#include <babylon/babylon_global.h>
#include <babylon/core/random.h>
#include <babylon/extensions/pathfinding/a_star_search.h>

namespace BABYLON {
namespace Extensions {

enum class WallType {
  Empty,
  UpperLeft,
  Vertical,
  Horizontal,
  UpperRight,
  LowerLeft,
  LowerRight,
  RightTee,
  LeftTee,
  UpTee,
  DownTee,
  FourWay,
  Up,
  Down,
  Left,
  Right,
  Visited,
  Used,
  GoLeft,
  GoRight,
  GoDown,
  GoUp
};

struct Cell {
  bool leftOpen        = false;
  bool rightOpen       = false;
  bool upOpen          = false;
  bool downOpen        = false;
  bool visited         = false;
  std::size_t id       = 0;
  std::size_t parentId = 0;
  double cost          = 0.0;
  double costSoFar     = 0.0;
}; // end of struct Cell

bool operator==(const Cell& lhs, const Cell& rhs)
{
  return lhs.id == rhs.id;
}

bool operator!=(const Cell& lhs, const Cell& rhs)
{
  return lhs.id != rhs.id;
}

struct RectangularMaze {
  using Node     = Cell;
  using NodeId   = std::size_t;
  using Location = std::tuple<std::size_t, std::size_t>;
  std::size_t _rows;
  std::size_t _columns;
  std::vector<Cell> _cells;
  std::vector<Cell> _path;

  RectangularMaze(size_t rows, size_t columns)
      : _rows{rows}
      , _columns{columns}
      , _cells{std::vector<Cell>(rows * columns, Cell())}
  {
  }

  ~RectangularMaze()
  {
  }

  Location location(const std::size_t cellId) const
  {
    const size_t row = cellId / _columns;
    const size_t col = cellId - (row * _columns);
    return Location{row, col};
  }

  Cell& cell(const std::size_t cellId)
  {
    return _cells[cellId];
  }

  Cell& operator()(const std::size_t cellId)
  {
    return _cells[cellId];
  }

  const Cell& operator()(const std::size_t cellId) const
  {
    return _cells[cellId];
  }

  const Cell& cell(const std::size_t cellId) const
  {
    return _cells[cellId];
  }

  Cell& cell(const std::size_t row, const std::size_t col)
  {
    return _cells[(row * _columns) + col];
  }

  const Cell& cell(const std::size_t row, const std::size_t col) const
  {
    return _cells[(row * _columns) + col];
  }

  std::size_t cellId(const Location& location) const
  {
    return (std::get<0>(location) * _columns) + std::get<1>(location);
  }

  bool isValid(const Location& location)
  {
    return cellId(location) < _cells.size();
  }

  std::vector<Cell> neighbors(const Cell& _cell) const
  {
    std::vector<Cell> neighborsNodes;
    std::size_t row, col;
    std::tie(row, col) = location(_cell.id);

    // Can go up
    if (row != 0 && cell(row - 1, col).downOpen) {
      neighborsNodes.emplace_back(cell(row - 1, col));
    }

    // Can go left
    if (col != 0 && cell(row, col - 1).rightOpen) {
      neighborsNodes.emplace_back(cell(row, col - 1));
    }

    // Can go right
    if (col < _columns - 1 && cell(row, col + 1).leftOpen) {
      neighborsNodes.emplace_back(cell(row, col + 1));
    }

    // Can go down
    if (row < _rows - 1 && cell(row + 1, col).upOpen) {
      neighborsNodes.emplace_back(cell(row + 1, col));
    }

    return neighborsNodes;
  }

  double cost(const Cell& /*cell1*/, const Cell& cell2) const
  {
    return cell2.cost;
  }

  void setCost(const Location& location, double cost)
  {
    if (!isValid(location)) {
      return;
    }
    cell(cellId(location)).cost = cost;
  }

  double heuristic(const Cell& cell1, const Cell& cell2) const
  {
    std::size_t x1, y1, x2, y2;
    std::tie(x1, y1) = location(cell1.id);
    std::tie(x2, y2) = location(cell2.id);
    const auto diffX = x1 > x2 ? x1 - x2 : x2 - x1;
    const auto diffy = y1 > y2 ? y1 - y2 : y2 - y1;
    return diffX + diffy;
  }

  void reset()
  {
    Cell cell{false, false, false, false, false, 0, 0, 1.0, 0.0};
    std::fill(_cells.begin(), _cells.end(), cell);
  }

  void generateEmptyGrid()
  {
    Cell cell{true, true, true, true, true, 0, 0, 1.0, 0.0};
    std::fill(_cells.begin(), _cells.end(), cell);
  }

  void addRectagularWall(const Location& start, const Location& end)
  {
    std::size_t x1, y1, x2, y2;
    std::tie(x1, y1) = start;
    std::tie(x2, y2) = end;

    if (x1 > x2) {
      std::swap(x1, x2);
    }
    x2 = std::min(x2, _rows - 1);
    if (y1 > y2) {
      std::swap(y1, y2);
    }
    y2 = std::min(y2, _columns - 1);

    for (std::size_t row = x1; row <= x2; ++row) {
      for (std::size_t col = y1; col <= y2; ++col) {
        if ((row == x1) || (row == x2)) {
          if ((col == y1) || (col == y2)) {
            cell(row, col).leftOpen  = (col != y1);
            cell(row, col).rightOpen = (col != y2);
            cell(row, col).upOpen    = (row != x1);
            cell(row, col).downOpen  = (row != x2);
          }
          else {
            cell(row, col).upOpen   = (row != x1);
            cell(row, col).downOpen = (row != x2);
          }
        }
        else if ((col == y1) || (col == y2)) {
          cell(row, col).leftOpen  = (col != y1);
          cell(row, col).rightOpen = (col != y2);
        }
      }
    }
  }

  void generateMaze()
  {
    Math::PCG pcg;

    // Reset maze
    reset();

    // Set starting row and column
    std::size_t r = 0;
    std::size_t c = 0;

    // The history is the stack of visited locations
    std::stack<Location> history;
    history.emplace(Location{r, c});

    // Trace a path though the cells of the maze and open walls along the path.
    // We do this with a while loop, repeating the loop until there is no
    // history, which would mean we backtracked to the initial start.
    while (!history.empty()) {
      // Designate this location as visited
      cell(r, c).visited = true;
      // Check if the adjacent cells are valid for moving to
      std::vector<char> check{};
      if (c > 0 && cell(r, c - 1).visited == false) {
        check.emplace_back('L');
      }
      if (r > 0 && cell(r - 1, c).visited == false) {
        check.emplace_back('U');
      }
      if (c < _columns - 1 && cell(r, c + 1).visited == false) {
        check.emplace_back('R');
      }
      if (r < _rows - 1 && cell(r + 1, c).visited == false) {
        check.emplace_back('D');
      }

      if (!check.empty()) { // If there is a valid cell to move to.
        // Mark the walls between cells as open if we move
        history.emplace(Location{r, c});
        const auto moveDirection
          = check[Math::distribution(pcg, std::size_t(0), check.size())];
        switch (moveDirection) {
          case 'L':
            cell(r, c).leftOpen = true;
            --c;
            cell(r, c).rightOpen = true;
            break;
          case 'U':
            cell(r, c).upOpen = true;
            --r;
            cell(r, c).downOpen = true;
            break;
          case 'R':
            cell(r, c).rightOpen = true;
            ++c;
            cell(r, c).leftOpen = true;
            break;
          case 'D':
            cell(r, c).downOpen = true;
            ++r;
            cell(r, c).upOpen = true;
            break;
        }
      }
      else {
        // If there are no valid cells to move to.
        // retrace one step back in history if no move is possible
        std::tie(r, c) = history.top();
        history.pop();
      }
    }

    // Open the walls at the start and finish
    cell(0, 0).leftOpen                     = true;
    cell(_rows - 1, _columns - 1).rightOpen = true;
  }

  std::vector<Location> findPath(const Location& start, const Location& goal)
  {
    // Result
    std::vector<Location> result;
    // Init cells
    std::size_t cellIdCtr = 0;
    for (auto& cell : _cells) {
      cell.visited = false;
      cell.id      = cellIdCtr++;
    }
    // Map locations to cell ids
    const std::size_t startCellId = isValid(start) ? cellId(start) : 0;
    const std::size_t goalCellId
      = isValid(goal) ? cellId(goal) : _cells.size() - 1;
    // Find path in maze
    _path = AStarSearch(*this, cell(startCellId), cell(goalCellId));
    // Convert to path of locations
    result.reserve(_path.size());
    for (auto& cell : _path) {
      result.emplace_back(location(cell.id));
    }
    return result;
  }

  std::vector<Location> findPath()
  {
    return findPath(location(0), location(_cells.size() - 1));
  }

}; // end of struct RectangularMaze

// For debugging
std::basic_iostream<char>::basic_ostream&
operator<<(std::basic_iostream<char>::basic_ostream& out,
           const RectangularMaze& maze)
{
  const auto numRows = maze._rows;
  const auto numCols = maze._columns;

  const auto renderRows = numRows * 2 + 1;
  const auto renderCols = numCols * 2 + 1;

  std::vector<std::vector<WallType>> cells(
    renderRows, std::vector<WallType>(renderCols, WallType::Empty));

  // Walls
  for (std::size_t row = 0; row < numRows; ++row) {
    for (std::size_t col = 0; col < numCols; ++col) {
      const auto renderRow = row * 2 + 1;
      const auto renderCol = col * 2 + 1;
      const auto& cell     = maze.cell(row, col);
      // upper
      if (!cell.upOpen) {
        cells[renderRow - 1][renderCol] = WallType::Horizontal;
      }
      // left
      if (!cell.leftOpen) {
        cells[renderRow][renderCol - 1] = WallType::Vertical;
      }
      // right
      if (!cell.rightOpen) {
        cells[renderRow][renderCol + 1] = WallType::Vertical;
      }
      // lower
      if (!cell.downOpen) {
        cells[renderRow + 1][renderCol] = WallType::Horizontal;
      }
    }
  }

  // Print path
  for (std::size_t i = 0; i < maze._path.size() - 1; ++i) {
    std::size_t r1, c1, r2, c2;
    std::tie(r1, c1) = maze.location(maze._path[i].id);
    std::tie(r2, c2) = maze.location(maze._path[i + 1].id);
    const auto renderRow        = r1 * 2 + 1;
    const auto renderCol        = c1 * 2 + 1;
    cells[renderRow][renderCol] = WallType::Used;
    // Directions
    if (r1 == r2) {
      // Go Left
      if (c2 < c1) {
        cells[renderRow][renderCol] = WallType::GoLeft;
      }
      // Go Right
      else if (c2 > c1) {
        cells[renderRow][renderCol] = WallType::GoRight;
      }
    }
    else if (c1 == c2) {
      // Go Up
      if (r2 < r1) {
        cells[renderRow][renderCol] = WallType::GoUp;
      }
      // Go Down
      else if (r2 > r1) {
        cells[renderRow][renderCol] = WallType::GoDown;
      }
    }
  }
  // Destination reached
  {
    auto& loc = maze._path.back();
    std::size_t r, c;
    std::tie(r, c) = maze.location(loc.id);
    cells[r * 2 + 1][c * 2 + 1] = WallType::Used;
  }

  // Walls
  for (std::size_t row = 0; row < renderRows; row += 2) {
    for (std::size_t col = 0; col < renderCols; col += 2) {
      const auto up
        = (row == 0) ? false : cells[row - 1][col] != WallType::Empty;
      const auto left
        = (col == 0) ? false : cells[row][col - 1] != WallType::Empty;
      const auto right
        = (col == numCols * 2) ? false : cells[row][col + 1] != WallType::Empty;
      const auto down
        = (row == numRows * 2) ? false : cells[row + 1][col] != WallType::Empty;

      if (up && right && down && left) {
        cells[row][col] = WallType::FourWay;
      }
      if (up && right && down && !left) {
        cells[row][col] = WallType::RightTee;
      }
      if (up && right && !down && left) {
        cells[row][col] = WallType::UpTee;
      }
      if (up && !right && down && left) {
        cells[row][col] = WallType::LeftTee;
      }
      if (!up && right && down && left) {
        cells[row][col] = WallType::DownTee;
      }

      if (up && right && !down && !left) {
        cells[row][col] = WallType::LowerLeft;
      }
      if (up && !right && !down && left) {
        cells[row][col] = WallType::LowerRight;
      }
      if (!up && !right && down && left) {
        cells[row][col] = WallType::UpperRight;
      }
      if (!up && right && down && !left) {
        cells[row][col] = WallType::UpperLeft;
      }
      if (!up && right && !down && left) {
        cells[row][col] = WallType::Horizontal;
      }
      if (up && !right && down && !left) {
        cells[row][col] = WallType::Vertical;
      }

      if (up && !right && !down && !left) {
        cells[row][col] = WallType::Up;
      }
      if (!up && right && !down && !left) {
        cells[row][col] = WallType::Right;
      }
      if (!up && !right && down && !left) {
        cells[row][col] = WallType::Down;
      }
      if (!up && !right && !down && left) {
        cells[row][col] = WallType::Left;
      }
    }
  }

  for (std::size_t row = 0; row < renderRows; ++row) {
    for (std::size_t col = 0; col < renderCols; ++col) {
      const auto square = [&]() {
        switch (cells[row][col]) {
          case WallType::Empty:
            return " ";
          case WallType::UpperLeft:
            return "┌";
          case WallType::Vertical:
            return "│";
          case WallType::Horizontal:
            return "─";
          case WallType::UpperRight:
            return "┐";
          case WallType::LowerLeft:
            return "└";
          case WallType::LowerRight:
            return "┘";
          case WallType::RightTee:
            return "├";
          case WallType::LeftTee:
            return "┤";
          case WallType::UpTee:
            return "┴";
          case WallType::DownTee:
            return "┬";
          case WallType::FourWay:
            return "┼";
          case WallType::Up:
            return "╵";
          case WallType::Down:
            return "╷";
          case WallType::Left:
            return "╴";
          case WallType::Right:
            return "╶";
          case WallType::Visited:
            return "·";
          case WallType::Used:
            return "*";
          case WallType::GoLeft:
            return "\u2190";
          case WallType::GoRight:
            return "\u2192";
          case WallType::GoDown:
            return "\u2193";
          case WallType::GoUp:
            return "\u2191";
          default:
            return " ";
        }
      }();

      out << square;
    }
    out << '\n';
  }

  return out;
}

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_PATH_FINDING_RECTANGULAR_MAZE_H
