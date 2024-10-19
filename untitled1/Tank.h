#ifndef TANK_H
#define TANK_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QColor>
#include <queue>
#include <vector>
#include <limits>
#include <tuple>
#include <QRandomGenerator>
#include "Graph.h"

class Tank {
private:
    int row;
    int col;
    QColor color;
    QGraphicsEllipseItem *tankItem;
    QGraphicsScene *scene;
    Map *map;
    static const int tileSize = 50;
    static const int radius = 2;

public:
    Tank(int row, int col, const QColor& color, QGraphicsScene *scene, Map *map)
        : row(row), col(col), color(color), scene(scene), map(map) {
        tankItem = scene->addEllipse(col * tileSize + 10, row * tileSize + 10, tileSize - 20, tileSize - 20,
                                      QPen(Qt::NoPen), QBrush(color));
    }

    std::pair<int, int> getPosition() const {
        return {row, col};
    }

    void setColor(const QColor& newColor) {
        color = newColor;
        tankItem->setBrush(QBrush(newColor));
    }

    QColor getColor() const {
        return color;
    }

    void updateGraphics() {
        tankItem->setPos(col * tileSize + 10, row * tileSize + 10);
    }

    void moveToPosition(int newRow, int newCol) {
        if (map->isValidIndex(newRow, newCol) && !map->isObstacle(newRow, newCol)) {
            row = newRow;
            col = newCol;
            updateGraphics();
        }
    }

    void shoot() {
        // Aqui lógica para disparar pero estamos fase 1 prueba
    }

    void applyPowerUp(const std::string& powerUpType) {
        // Aqui lógica para aplicar un power-up pero esta en fase 2 prueba
    }

    void moveTo(int targetRow, int targetCol) {
        if (!map->isValidIndex(targetRow, targetCol) || map->isObstacle(targetRow, targetCol)) {
            return;
        }

        // Aqui se determina el algoritmo a usar según el color del tanque
        if (color == Qt::cyan || color == Qt::blue) {
            // 50% de probabilidad para BFS o movimiento aleatorio
            if (QRandomGenerator::global()->bounded(100) < 50) {
                bfs(targetRow, targetCol);
            } else {
                randomMove();
            }
        } else if (color == Qt::yellow || color == Qt::red) {
            // 80% de probabilidad para Dijkstra o movimiento aleatorio
            if (QRandomGenerator::global()->bounded(100) < 80) {
                dijkstra(targetRow, targetCol);
            } else {
                randomMove();
            }
        }
    }

private:
    void bfs(int targetRow, int targetCol) {
        // Implementación de BFS para encontrar la ruta
        std::queue<std::pair<int, int>> q;
        std::vector<std::vector<bool>> visited(map->getRows(), std::vector<bool>(map->getCols(), false));
        std::vector<std::vector<std::pair<int, int>>> parent(map->getRows(), std::vector<std::pair<int, int>>(map->getCols(), {-1, -1}));

        q.push({row, col});
        visited[row][col] = true;

        while (!q.empty()) {
            auto [currentRow, currentCol] = q.front();
            q.pop();

            if (currentRow == targetRow && currentCol == targetCol) {

                while (parent[currentRow][currentCol] != std::make_pair(-1, -1)) {
                    auto [prevRow, prevCol] = parent[currentRow][currentCol];
                    currentRow = prevRow;
                    currentCol = prevCol;
                    moveTo(currentRow, currentCol);
                }
                return;
            }

            // Comprobación de las posiciones adyacentes
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (abs(dr) != abs(dc) && map->isValidIndex(currentRow + dr, currentCol + dc) && !map->isObstacle(currentRow + dr, currentCol + dc) && !visited[currentRow + dr][currentCol + dc]) {
                        visited[currentRow + dr][currentCol + dc] = true;
                        parent[currentRow + dr][currentCol + dc] = {currentRow, currentCol};
                        q.push({currentRow + dr, currentCol + dc});
                    }
                }
            }
        }
    }

    void dijkstra(int targetRow, int targetCol) {
        // Implementación de Dijkstra para encontrar la ruta
        const int INF = std::numeric_limits<int>::max();
        std::vector<std::vector<int>> dist(map->getRows(), std::vector<int>(map->getCols(), INF));
        std::vector<std::vector<std::pair<int, int>>> parent(map->getRows(), std::vector<std::pair<int, int>>(map->getCols(), {-1, -1}));
        auto compare = [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
            return std::get<0>(a) > std::get<0>(b);
        };
        std::priority_queue<std::tuple<int, int, int>, std::vector<std::tuple<int, int, int>>, decltype(compare)> pq(compare);

        dist[row][col] = 0;
        pq.push({0, row, col});

        while (!pq.empty()) {
            auto [currentDist, currentRow, currentCol] = pq.top();
            pq.pop();

            if (currentRow == targetRow && currentCol == targetCol) {

                while (parent[currentRow][currentCol] != std::make_pair(-1, -1)) {
                    auto [prevRow, prevCol] = parent[currentRow][currentCol];
                    currentRow = prevRow;
                    currentCol = prevCol;
                    moveTo(currentRow, currentCol);
                }
                return;
            }

            // Comprobación de las posiciones adyacentes
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (abs(dr) != abs(dc) && map->isValidIndex(currentRow + dr, currentCol + dc) && !map->isObstacle(currentRow + dr, currentCol + dc)) {
                        int newDist = currentDist + 1;
                        if (newDist < dist[currentRow + dr][currentCol + dc]) {
                            dist[currentRow + dr][currentCol + dc] = newDist;
                            parent[currentRow + dr][currentCol + dc] = {currentRow, currentCol};
                            pq.push({newDist, currentRow + dr, currentCol + dc});
                        }
                    }
                }
            }
        }
    }

    void randomMove() {
        int attempts = 0;
        while (attempts < 5) {
            int randomRow = QRandomGenerator::global()->bounded(-radius, radius + 1);
            int randomCol = QRandomGenerator::global()->bounded(-radius, radius + 1);

            int newRow = row + randomRow;
            int newCol = col + randomCol;

            if (map->isValidIndex(newRow, newCol) && !map->isObstacle(newRow, newCol)) {
                moveTo(newRow, newCol);
                return;
            }

            attempts++;
        }
    }
};

#endif // TANK_H
