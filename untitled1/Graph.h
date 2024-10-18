#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <QRandomGenerator>

class Map {

private:
    static const int rows = 12;  // Número de filas
    static const int cols = 18;  // Número de columnas
    int adjMatrix[rows][cols];  // Matriz de adyacencia 10x20

public:
    static const int OBSTACLE = -1;
    //constructor para inciar la matriz con ceros para luego ir agregando los obstaculos(por lo mismo el -1)
    Map() {
        resetMatrix();
        printMatrix();
    }


    void resetMatrix() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                adjMatrix[i][j] = 0;
            }
        }
    }

    bool isValidIndex(int i, int j) const {
        return (i >= 0 && i < rows && j >= 0 && j < cols);
    }

    bool isOccupied(int i, int j) const {
        return isValidIndex(i, j) && (adjMatrix[i][j] == 1 || adjMatrix[i][j] == OBSTACLE);
    }

    void addEdge(int i, int j) {
        if (isValidIndex(i, j)) {
            adjMatrix[i][j] = 1;
            // Eliminar la simetría ya que no es una matriz cuadrada
        }
    }

    // eliminar conexión entre dos nodos
    void removeEdge(int i, int j) {
        if (isValidIndex(i, j)) {
            adjMatrix[i][j] = 0;
        }
    }

    void printMatrix() const {
        if (rows == 0 || cols == 0) {
            std::cout << "Matrix is empty." << std::endl;
            return;
        }

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


    //verificar si dos nodos están conectados
    bool isConnected(int i, int j) const {
        return isValidIndex(i, j) && adjMatrix[i][j] == 1;
    }

    void generateObstacles() {
        int obstaclesAdded = 0;
        while (obstaclesAdded < 5) {
            // Generar posición inicial aleatoria
            int row = QRandomGenerator::global()->bounded(0, rows);
            int col = QRandomGenerator::global()->bounded(0, cols);

            // Decidir si el obstáculo será horizontal (0) o vertical (1)
            bool horizontal = QRandomGenerator::global()->bounded(0, 2) == 0;

            // Comprobar si se puede colocar el obstáculo de dos celdas
            if (horizontal && col < cols - 1) {  // Obstáculo horizontal
                if (adjMatrix[row][col] == 0 && adjMatrix[row][col + 1] == 0) { // Comprobar que ambas celdas están libres
                    adjMatrix[row][col] = -1;       // Primera celda
                    adjMatrix[row][col + 1] = -1;   // Segunda celda
                    obstaclesAdded++;
                }
            } else if (!horizontal && row < rows - 1) {  // Obstáculo vertical
                if (adjMatrix[row][col] == 0 && adjMatrix[row + 1][col] == 0) { // Comprobar que ambas celdas están libres
                    adjMatrix[row][col] = -1;       // Primera celda
                    adjMatrix[row + 1][col] = -1;   // Segunda celda
                    obstaclesAdded++;
                }
            }
        }
    }

    bool isObstacle(int i, int j) const {
        return isValidIndex(i, j) && adjMatrix[i][j] == OBSTACLE;
    }

    int getRows() const { return rows; }
    int getCols() const { return cols; }
};

#endif // MAP_H
