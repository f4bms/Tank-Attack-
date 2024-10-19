#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <vector>
#include <queue>
#include <QPoint>
#include <limits>
#include "Graph.h"

class Pathfinding {
public:
    struct DijkstraNode {
        int row, col;
        int distance;
        bool operator<(const DijkstraNode& other) const {
            return distance > other.distance; // Menor distancia tiene prioridad
        }
    };

    static std::vector<QPoint> bfsPath(const Map& gameMap, int startRow, int startCol, int targetRow, int targetCol) {
        if (!gameMap.isValidIndex(startRow, startCol) || !gameMap.isValidIndex(targetRow, targetCol)) {
            return {};
        }

        int numRows = gameMap.getNumRows(); // Obtener número de filas
        int numCols = gameMap.getNumCols(); // Obtener número de columnas

        std::vector<std::vector<bool>> visited(numRows, std::vector<bool>(numCols, false));
        std::vector<std::vector<QPoint>> previous(numRows, std::vector<QPoint>(numCols, {-1, -1}));
        std::queue<QPoint> queue;

        queue.push({startRow, startCol});
        visited[startRow][startCol] = true;

        std::vector<QPoint> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // Arriba, Derecha, Abajo, Izquierda

        while (!queue.empty()) {
            QPoint current = queue.front();
            queue.pop();

            if (current.x() == targetRow && current.y() == targetCol) {
                std::vector<QPoint> path;
                for (QPoint at = {targetRow, targetCol}; at.x() != -1; at = previous[at.x()][at.y()]) {
                    path.push_back(at);
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            for (const QPoint& dir : directions) {
                int newRow = current.x() + dir.x();
                int newCol = current.y() + dir.y();
                if (gameMap.isValidIndex(newRow, newCol) && !visited[newRow][newCol] && !gameMap.isObstacle(newRow, newCol)) {
                    queue.push({newRow, newCol});
                    visited[newRow][newCol] = true;
                    previous[newRow][newCol] = current;
                }
            }
        }

        return {};
    }

    static std::vector<QPoint> dijkstraPath(const Map& gameMap, int startRow, int startCol, int targetRow, int targetCol) {
        if (!gameMap.isValidIndex(startRow, startCol) || !gameMap.isValidIndex(targetRow, targetCol)) {
            return {};
        }

        int numRows = gameMap.getNumRows(); // Obtener número de filas
        int numCols = gameMap.getNumCols(); // Obtener número de columnas

        std::vector<std::vector<int>> distance(numRows, std::vector<int>(numCols, std::numeric_limits<int>::max()));
        std::vector<std::vector<QPoint>> previous(numRows, std::vector<QPoint>(numCols, {-1, -1}));
        std::priority_queue<DijkstraNode> queue;

        distance[startRow][startCol] = 0;
        queue.push({startRow, startCol, 0});

        std::vector<QPoint> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // Arriba, Derecha, Abajo, Izquierda

        while (!queue.empty()) {
            DijkstraNode current = queue.top();
            queue.pop();

            if (current.row == targetRow && current.col == targetCol) {
                std::vector<QPoint> path;
                for (QPoint at = {targetRow, targetCol}; at.x() != -1; at = previous[at.x()][at.y()]) {
                    path.push_back(at);
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            for (const QPoint& dir : directions) {
                int newRow = current.row + dir.x();
                int newCol = current.col + dir.y();
                if (gameMap.isValidIndex(newRow, newCol) && !gameMap.isObstacle(newRow, newCol)) {
                    int newDist = distance[current.row][current.col] + 1;
                    if (newDist < distance[newRow][newCol]) {
                        distance[newRow][newCol] = newDist;
                        previous[newRow][newCol] = {current.row, current.col};
                        queue.push({newRow, newCol, newDist});
                    }
                }
            }
        }

        return {};
    }

    static std::vector<QPoint> randomMove(const Map& gameMap, int startRow, int startCol) {
        if (!gameMap.isValidIndex(startRow, startCol)) {
            return {};
        }

        std::vector<QPoint> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // Arriba, Derecha, Abajo, Izquierda
        std::random_shuffle(directions.begin(), directions.end()); // Mezclar direcciones

        for (const QPoint& dir : directions) {
            int newRow = startRow + dir.x();
            int newCol = startCol + dir.y();
            if (gameMap.isValidIndex(newRow, newCol) && !gameMap.isObstacle(newRow, newCol)) {
                return {{startRow, startCol}, {newRow, newCol}};
            }
        }

        return {{startRow, startCol}};
    }
};

#endif // PATHFINDING_H
