#ifndef GAMELAUNCH_H
#define GAMELAUNCH_H

#include <QGraphicsView>
#include <QGraphicsScene>
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
    QList<QGraphicsLineItem*> currentPathLines; // Lista para almacenar las líneas de la ruta actual

    QList<QGraphicsTextItem*> player1HealthTexts;
    QList<QGraphicsTextItem*> player2HealthTexts;

    public:
        GameLaunch(QWidget *parent = nullptr)
            : QGraphicsView(parent), numRows(gameMap.getNumRows()), numCols(gameMap.getNumCols()), tileSize(50) {
            // Configurar la escena
            scene.setSceneRect(0, 0, tileSize * numCols, tileSize * numRows);
            this->setScene(&scene);
            scene.setBackgroundBrush(QBrush(QColor(139, 115, 85))); // Example: Sky blue color

            gameMap.generateObstacles();
            drawGrid();
            placeInitialTanks();
            gameMap.printMatrix();
            placeTexts();



            setWindowTitle("Tank Attack!");
            setFixedSize(tileSize * numCols + 20, tileSize * numRows + 20);

            player1.setTurn(true);
            player2.setTurn(false);

        }

    protected:

        void drawGrid() {

            for (int row = 0; row < numRows; ++row) {
                for (int col = 0; col < numCols; ++col) {
                    QGraphicsRectItem *rect = scene.addRect(col * tileSize, row * tileSize, tileSize, tileSize);

                    //esto es la solución que se encontró para poder poner el texto y que no afectara al resto de cosas
                    if (row >= numRows - 2) {
                        rect->setBrush(Qt::black);
                        rect->setPen(Qt::NoPen);
                    } else if (gameMap.isObstacle(row, col)) {
                        rect->setBrush(QColor(139, 115, 85));
                    } else {
                        rect->setBrush(QColor(210, 180, 140));
                        rect->setPen(QPen(Qt::black));
                    }
                }
            }
        }


    //esto es para poner las cosillas del display de la vida del jugador
    void placeTexts() {
            QGraphicsTextItem *player1Label = new QGraphicsTextItem("Player 1:");
            player1Label->setPos(10, tileSize * (numRows - 2) + 10);
            player1Label->setDefaultTextColor(Qt::white);
            scene.addItem(player1Label);

            int startX1 = 100;
            int startY = tileSize * (numRows - 2) + 10;

            for (int i = 0; i < player1.getTanks().size(); ++i) {
                Tank* tank = player1.getTanks()[i];
                QGraphicsTextItem *healthText = new QGraphicsTextItem(QString("Health: %1").arg(tank->getHealth()));
                healthText->setPos(startX1 + i * 150, startY);
                healthText->setDefaultTextColor(Qt::white);
                scene.addItem(healthText);
                player1HealthTexts.append(healthText);
            }

            QGraphicsTextItem *player2Label = new QGraphicsTextItem("Player 2:");
            player2Label->setPos(10, tileSize * (numRows - 1) + 10);
            player2Label->setDefaultTextColor(Qt::white);
            scene.addItem(player2Label);

            int startX2 = 100;
            int startY2 = tileSize * (numRows - 1) + 10;

            for (int i = 0; i < player2.getTanks().size(); ++i) {
                Tank* tank = player2.getTanks()[i];
                QGraphicsTextItem *healthText = new QGraphicsTextItem(QString("Health: %1").arg(tank->getHealth()));
                healthText->setPos(startX2 + i * 150, startY2);
                healthText->setDefaultTextColor(Qt::white);
                scene.addItem(healthText);
                player2HealthTexts.append(healthText);
                scene.addItem(healthText);
            }
        }

        void updateHealthDisplay() {
            for (int i = 0; i < player1HealthTexts.size(); ++i) {
                if (i < player1.getTanks().size()) {
                    player1HealthTexts[i]->setPlainText(QString("Tank %1 Health: %2").arg(i + 1).arg(player1.getTanks()[i]->getHealth()));
                }
            }

            for (int i = 0; i < player2HealthTexts.size(); ++i) {
                if (i < player2.getTanks().size()) {
                    player2HealthTexts[i]->setPlainText(QString("Tank %1 Health: %2").arg(i + 1).arg(player2.getTanks()[i]->getHealth()));
                }
            }
        }


        void updateHealthTexts() {
            for (int i = 0; i < player1.getTanks().size(); ++i) {
                Tank* tank = player1.getTanks()[i];
                player1HealthTexts[i]->setPlainText(QString("Health: %1").arg(tank->getHealth()));
            }

            // Actualizar textos de vida del jugador 2
            for (int i = 0; i < player2.getTanks().size(); ++i) {
                Tank* tank = player2.getTanks()[i];
                player2HealthTexts[i]->setPlainText(QString("Health: %1").arg(tank->getHealth()));
            }
        }

        void switchTurn() {
            // Cambiar los turnos entre los jugadores
            player1.setTurn(!player1.getTurn());
            player2.setTurn(!player2.getTurn());
            clearCurrentPath(); // Borrar la ruta actual al cambiar de turno
        }

        void placeTank(int row, int col, const QColor &color) {
            int maxHealth = 100;
            Tank *tank = new Tank(row, col, color, &scene, maxHealth);

            if (color == Qt::red || color == Qt::blue) {
                player1.addTank(tank);
            } else {
                player2.addTank(tank);
            }
        }

        void placeInitialTanks() {
        QRandomGenerator *randGen = QRandomGenerator::global();

        // Colocar tanques rojos y azules en las primeras dos columnas de la izquierda
        for (int i = 0; i < 2; ++i) {
            int row, col;

            // Buscar posiciones aleatorias para los tanques rojos
            do {
                row = randGen->bounded(numRows);
                col = randGen->bounded(0, 2); // Se limitan las columnas
            } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));

            placeTank(row, col, Qt::red); // Se coloca el tanque y se agrega al jugador 1
            gameMap.addEdge(row, col); // Marca la celda como ocupada
        }

        for (int i = 0; i < 2; ++i) {
            int row, col;

            // Buscar posiciones aleatorias para los tanques azules
            do {
                row = randGen->bounded(numRows);
                col = randGen->bounded(0, 2);
            } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));

            placeTank(row, col, Qt::blue); // Se coloca el tanque y se agrega al jugador 2
            gameMap.addEdge(row, col);
        }

        // Colocar tanques amarillos y celestes en las primeras dos columnas de la derecha
        for (int i = 0; i < 2; ++i) {
            int row, col;

            // Buscar posiciones aleatorias para los tanques amarillos
            do {
                row = randGen->bounded(numRows);
                col = randGen->bounded(numCols - 2, numCols);
            } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));

            placeTank(row, col, Qt::yellow); // Se coloca el tanque y se agrega al jugador 1
            gameMap.addEdge(row, col);
        }

        for (int i = 0; i < 2; ++i) {
            int row, col;

            // Buscar posiciones aleatorias para los tanques celestes
            do {
                row = randGen->bounded(numRows);
                col = randGen->bounded(numCols - 2, numCols);
            } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));

            placeTank(row, col, Qt::cyan); // Se coloca el tanque y se agrega al jugador 2
            gameMap.addEdge(row, col);
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
                    drawPath(convertToQVector(path)); // Dibujar la ruta calculada
                    for (const auto& point : path) {
                        moveTank(tank, point.x(), point.y());
                    }
                } else {
                    qDebug() << "Usando movimiento aleatorio para el tanque.";
                    auto path = Pathfinding::randomMove(gameMap, tank->getRow(), tank->getCol());
                    drawPath(convertToQVector(path)); // Dibujar la ruta calculada
                    for (const auto& point : path) {
                        moveTank(tank, point.x(), point.y());
                    }
                }
            } else if (color == Qt::red || color == Qt::yellow) {
                // 80% Dijkstra y 20% movimiento aleatorio
                if (randomPercentage <= 80) {
                    qDebug() << "Usando Dijkstra para el tanque.";
                    auto path = Pathfinding::dijkstraPath(gameMap, tank->getRow(), tank->getCol(), targetRow, targetCol);
                    drawPath(convertToQVector(path)); // Dibujar la ruta calculada
                    for (const auto& point : path) {
                        moveTank(tank, point.x(), point.y());
                    }
                } else {
                    qDebug() << "Usando movimiento aleatorio para el tanque.";
                    auto path = Pathfinding::randomMove(gameMap, tank->getRow(), tank->getCol());
                    drawPath(convertToQVector(path)); // Dibujar la ruta calculada
                    for (const auto& point : path) {
                        moveTank(tank, point.x(), point.y());
                    }
                }
            }
        }

        QVector<QPoint> convertToQVector(const std::vector<QPoint> &vec) {
            QVector<QPoint> qVec;
            for (const auto &point : vec) {
                qVec.append(point);
            }
            return qVec;
        }

        void drawPath(const QVector<QPoint> &path) {
            clearCurrentPath(); // Borrar la ruta anterior
            QPen pen(Qt::green);
            pen.setWidth(2);
            for (int i = 0; i < path.size() - 1; ++i) {
                QGraphicsLineItem *line = scene.addLine(path[i].x() * tileSize + tileSize / 2, path[i].y() * tileSize + tileSize / 2,
                                                        path[i + 1].x() * tileSize + tileSize / 2, path[i + 1].y() * tileSize + tileSize / 2, pen);
                currentPathLines.append(line);
            }
        }

        void clearCurrentPath() {
            for (QGraphicsLineItem *line : currentPathLines) {
                scene.removeItem(line);
                delete line;
            }
            currentPathLines.clear();
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
