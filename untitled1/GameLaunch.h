#ifndef GAMELAUNCH_H
#define GAMELAUNCH_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QPen>
#include <QDebug>
#include <QBrush>
#include <QWidget>
#include <iostream>
#include <random>
#include "Graph.h"
#include "Tank.h"
#include "Player.h"
#include "Pathfinding.h" // Incluye los algoritmos de movimiento

class GameLaunch : public QGraphicsView {
    Q_OBJECT

private:
    Map gameMap;
    QGraphicsScene scene;
    int numRows;
    int numCols;
    int tileSize;

    Player player1;
    Player player2;
    Tank* selectedTank = nullptr; // Tanque seleccionado

public:
    GameLaunch(QWidget *parent = nullptr)
        : QGraphicsView(parent), numRows(gameMap.getNumRows()), numCols(gameMap.getNumCols()), tileSize(50) {
        // Configurar la escena
        scene.setSceneRect(0, 0, tileSize * numCols, tileSize * numRows);
        this->setScene(&scene);

        gameMap.generateObstacles();
        gameMap.printMatrix(); // Imprimir la matriz después de generar obstáculos
        drawGrid();
        placeInitialTanks();

        setWindowTitle("Tank Attack!");
        setFixedSize(tileSize * numCols + 20, tileSize * numRows + 20);

        player1.setTurn(true);
        player2.setTurn(false);
    }

protected:
    void switchTurn() {
        // Cambiar los turnos entre los jugadores
        player1.setTurn(!player1.getTurn());
        player2.setTurn(!player2.getTurn());
    }

    void drawGrid() {
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                QGraphicsRectItem *rect = scene.addRect(col * tileSize, row * tileSize, tileSize, tileSize);
                if (gameMap.isObstacle(row, col)) {
                    rect->setBrush(Qt::red);
                } else {
                    rect->setBrush(Qt::white);
                    rect->setPen(QPen(Qt::black));
                }
            }
        }
    }

    void placeTank(int row, int col, const QColor &color) {
        Tank *tank = new Tank(row, col, color, &scene);
        if (color == Qt::red || color == Qt::blue) {
            player1.addTank(tank);
        } else {
            player2.addTank(tank);
        }
    }

    void placeInitialTanks() {
        QRandomGenerator *randGen = QRandomGenerator::global();
        // Colocar tanques en posiciones iniciales para los dos jugadores
        for (int i = 0; i < 4; ++i) {
            int row = randGen->bounded(numRows);
            int col = randGen->bounded(numCols);
            while (gameMap.isObstacle(row, col)) {
                row = randGen->bounded(numRows);
                col = randGen->bounded(numCols);
            }
            placeTank(row, col, (i % 2 == 0) ? Qt::red : Qt::blue);
        }
        for (int i = 0; i < 4; ++i) {
            int row = randGen->bounded(numRows);
            int col = randGen->bounded(numCols);
            while (gameMap.isObstacle(row, col)) {
                row = randGen->bounded(numRows);
                col = randGen->bounded(numCols);
            }
            placeTank(row, col, (i % 2 == 0) ? Qt::yellow : Qt::cyan);
        }
    }

    void moveTank(Tank *tank, int newRow, int newCol) {
        int oldRow = tank->getRow();
        int oldCol = tank->getCol();
        gameMap.removeEdge(oldRow, oldCol);
        gameMap.addEdge(newRow, newCol);
        tank->updatePosition(newRow, newCol);
        updateTankGraphics(tank);
    }

    void updateTankGraphics(Tank *tank) {
        tank->getGraphicsItem()->setRect(tank->getCol() * tileSize + 10, tank->getRow() * tileSize + 10, tileSize - 20, tileSize - 20);
    }

    Tank* findTankAt(int row, int col) {
        for (Tank* tank : player1.getTanks()) {
            if (tank->getRow() == row && tank->getCol() == col) {
                return tank;
            }
        }
        for (Tank* tank : player2.getTanks()) {
            if (tank->getRow() == row && tank->getCol() == col) {
                return tank;
            }
        }
        return nullptr;
    }

    void executeMovementAlgorithm(Tank *tank, int targetRow, int targetCol) {
        if (!tank) return;

        QColor color = tank->getColor();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 100);

        int randomPercentage = dist(gen);

        if (color == Qt::cyan || color == Qt::blue) {
            // 50% BFS y 50% movimiento aleatorio
            if (randomPercentage <= 50) {
                qDebug() << "Usando BFS para el tanque.";
                auto path = Pathfinding::bfsPath(gameMap, tank->getRow(), tank->getCol(), targetRow, targetCol);
                for (const auto& point : path) {
                    moveTank(tank, point.x(), point.y());
                }
            } else {
                qDebug() << "Usando movimiento aleatorio para el tanque.";
                auto path = Pathfinding::randomMove(gameMap, tank->getRow(), tank->getCol());
                for (const auto& point : path) {
                    moveTank(tank, point.x(), point.y());
                }
            }
        } else if (color == Qt::red || color == Qt::yellow) {
            // 80% Dijkstra y 20% movimiento aleatorio
            if (randomPercentage <= 80) {
                qDebug() << "Usando Dijkstra para el tanque.";
                auto path = Pathfinding::dijkstraPath(gameMap, tank->getRow(), tank->getCol(), targetRow, targetCol);
                for (const auto& point : path) {
                    moveTank(tank, point.x(), point.y());
                }
            } else {
                qDebug() << "Usando movimiento aleatorio para el tanque.";
                auto path = Pathfinding::randomMove(gameMap, tank->getRow(), tank->getCol());
                for (const auto& point : path) {
                    moveTank(tank, point.x(), point.y());
                }
            }
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            if (selectedTank) {
                int targetRow = event->position().y() / tileSize; // Asignar la fila de destino
                int targetCol = event->position().x() / tileSize; // Asignar la columna de destino
                executeMovementAlgorithm(selectedTank, targetRow, targetCol);
                switchTurn();
                selectedTank = nullptr; // Deseleccionar el tanque después de moverlo
            }
        } else if (event->button() == Qt::RightButton) {
            QGraphicsItem *item = itemAt(event->position().toPoint());
            if (!item) return;

            Tank *tank = findTankAt(item->data(0).toInt(), item->data(1).toInt());
            if (tank) {
                selectedTank = tank;
                qDebug() << "Tanque seleccionado en (" << tank->getRow() << ", " << tank->getCol() << ")";
            }
        }
    }
};

#endif // GAMELAUNCH_H
