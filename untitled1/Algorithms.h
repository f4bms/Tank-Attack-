#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <queue>
#include <QPoint>
#include <limits>
#include "Graph.h"

// Estructura para almacenar la información del nodo en Dijkstra
struct DijkstraNode {
    int row, col;
    int distance;
    bool operator<(const DijkstraNode& other) const {
        return distance > other.distance; // Menor distancia tiene prioridad
    }
};

// BFS para encontrar la ruta más corta entre dos puntos
std::vector<QPoint> bfsPath(const Map& gameMap, int startRow, int startCol, int targetRow, int targetCol) {
    if (!gameMap.isValidIndex(startRow, startCol) || !gameMap.isValidIndex(targetRow, targetCol)) {
        return {};
    }

    std::vector<std::vector<bool>> visited(gameMap.numRows, std::vector<bool>(gameMap.numCols, false));
    std::vector<std::vector<QPoint>> previous(gameMap.numRows, std::vector<QPoint>(gameMap.numCols, {-1, -1}));
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

        // Explorar vecinos
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

// Dijkstra para encontrar la ruta más corta basada en la distancia
std::vector<QPoint> dijkstraPath(const Map& gameMap, int startRow, int startCol, int targetRow, int targetCol) {
    if (!gameMap.isValidIndex(startRow, startCol) || !gameMap.isValidIndex(targetRow, targetCol)) {
        return {};
    }

    std::vector<std::vector<int>> distance(gameMap.numRows, std::vector<int>(gameMap.numCols, std::numeric_limits<int>::max()));
    std::vector<std::vector<QPoint>> previous(gameMap.numRows, std::vector<QPoint>(gameMap.numCols, {-1, -1}));
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

        // Explorar vecinos
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

// Movimiento aleatorio hacia una dirección no bloqueada
std::vector<QPoint> randomMove(const Map& gameMap, int startRow, int startCol) {
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

#endif // ALGORITHMS_H

