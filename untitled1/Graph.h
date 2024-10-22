#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <QRandomGenerator>

class Map {
private:
    static const int rows = 12;
    static const int cols = 18;
    int adjMatrix[rows][cols];  // Matriz de adyacencia

public:
    static const int OBSTACLE = -1;
    static const int FREE_SPACE = 0;
    static const int PATH = 1;

    // Constructor para inicializar la matriz con espacios libres
    Map() {
        resetMatrix();
        generateObstacles();
        printMatrix();
    }

    // Reiniciar la matriz
    void resetMatrix() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                adjMatrix[i][j] = FREE_SPACE;
            }
        }
    }

    // Comprobar si el índice es válido
    bool isValidIndex(int i, int j) const {
        return (i >= 0 && i < rows && j >= 0 && j < cols);
    }

    // Comprobar si una celda está ocupada
    bool isOccupied(int i, int j) const {
        return isValidIndex(i, j) && (adjMatrix[i][j] != FREE_SPACE);
    }

    // Añadir una arista o conexión
    void addEdge(int i, int j) {
        if (isValidIndex(i, j) && adjMatrix[i][j] == FREE_SPACE) {
            adjMatrix[i][j] = PATH;
        }
    }

    // Eliminar una arista o conexión
    void removeEdge(int i, int j) {
        if (isValidIndex(i, j) && adjMatrix[i][j] != OBSTACLE) {
            adjMatrix[i][j] = FREE_SPACE;
        }
    }

    // Imprimir la matriz de adyacencia
    void printMatrix() const {
        std::cout << "  ";
        for (int j = 0; j < cols; ++j) {
            std::cout << j << " ";
        }
        std::cout << std::endl;

        for (int i = 0; i < rows; ++i) {
            std::cout << i << " ";
            for (int j = 0; j < cols; ++j) {
                std::cout << adjMatrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    // Comprobar si una celda está conectada
    bool isConnected(int i, int j) const {
        return isValidIndex(i, j) && adjMatrix[i][j] == PATH;
    }

    // Generar obstáculos de manera aleatoria en el mapa
    void generateObstacles() {
        int obstaclesAdded = 0;
        const int maxObstacles = 5; // Número máximo de obstáculos

        while (obstaclesAdded < maxObstacles) {
            // Generar posición inicial aleatoria
            int row = QRandomGenerator::global()->bounded(0, rows);
            int col = QRandomGenerator::global()->bounded(0, cols);

            // Decidir orientación y tamaño del obstáculo
            bool horizontal = QRandomGenerator::global()->bounded(0, 2) == 0;
            int obstacleSize = QRandomGenerator::global()->bounded(2, 4); // Tamaño del obstáculo entre 2 y 3 celdas

            if (horizontal && col + obstacleSize <= cols) {  // Obstáculo horizontal
                bool canPlace = true;
                for (int k = 0; k < obstacleSize; ++k) {
                    if (isOccupied(row, col + k)) {
                        canPlace = false;
                        break;
                    }
                }
                if (canPlace) {
                    for (int k = 0; k < obstacleSize; ++k) {
                        adjMatrix[row][col + k] = OBSTACLE;
                    }
                    obstaclesAdded++;
                }
            } else if (!horizontal && row + obstacleSize <= rows) {  // Obstáculo vertical
                bool canPlace = true;
                for (int k = 0; k < obstacleSize; ++k) {
                    if (isOccupied(row + k, col)) {
                        canPlace = false;
                        break;
                    }
                }
                if (canPlace) {
                    for (int k = 0; k < obstacleSize; ++k) {
                        adjMatrix[row + k][col] = OBSTACLE;
                    }
                    obstaclesAdded++;
                }
            }
        }
    }

    // Comprobar si una celda es un obstáculo
    bool isObstacle(int i, int j) const {
        return isValidIndex(i, j) && adjMatrix[i][j] == OBSTACLE;
    }

    // Obtener el número de filas y columnas
    int getNumRows() const { return rows; }
    int getNumCols() const { return cols; }
};

#endif // MAP_H
