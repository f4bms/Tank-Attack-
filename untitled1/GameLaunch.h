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
#include "Graph.h"
#include "Tank.h"

class GameLaunch : public QGraphicsView {
    Q_OBJECT

private:
    Map gameMap;
    QGraphicsScene scene;
    int numRows;
    int numCols;
    int tileSize;
    bool player1Turn; // este cosillo es para ver quien va jugando
    QList<Tank*> player1Tanks; // Lista de tanques del jugador 1
    QList<Tank*> player2Tanks; // Lista de tanques del jugador 2


    QPoint selectedTankPosition;
    bool tankSelected = false;
    QGraphicsEllipseItem *selectedTank = nullptr;

public:
    GameLaunch(QWidget *parent = nullptr)
        : QGraphicsView(parent), numRows(gameMap.getRows()), numCols(gameMap.getCols()), tileSize(50), player1Turn(true) {
        // Configurar la escena
        scene.setSceneRect(0, 0, tileSize * numCols, tileSize * numRows);
        this->setScene(&scene);


        gameMap.generateObstacles();
        gameMap.printMatrix(); // Imprimir la matriz después de generar obstáculos
        drawGrid();
        placeInitialTanks();

        setWindowTitle("Game Map with Obstacles - 10x20");
        setFixedSize(tileSize * numCols + 20, tileSize * numRows + 20);
    }

protected:
    // Dibuja la cuadrícula y los obstáculos en la escena
    void drawGrid() {
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                QGraphicsRectItem *rect = scene.addRect(col * tileSize, row * tileSize, tileSize, tileSize);

                //coordenadas de la celda como datos del rectángulo
                rect->setData(0, row);
                rect->setData(1, col);

                // marcar el obstaculo
                if (gameMap.isObstacle(row, col)) {
                    rect->setBrush(Qt::red);
                } else {

                    /* en la docu aquí se puede explicar q hubo problemas con esto.
                     * puedo poner q cuando se estaba buscando para hacer el tracking no se estaba pintando pq estaba vacio no blanco
                     */
                    rect->setBrush(Qt::white);
                    rect->setPen(QPen(Qt::black));
                }
            }
        }
    }

    void placeTank(int row, int col, const QColor &color) {
        Tank *tank = new Tank(row, col, color, &scene);
        if (color == Qt::red || color == Qt::blue) {
            player1Tanks.append(tank);
        } else {
            player2Tanks.append(tank);
        }
    }


    Tank* findTankAt(int row, int col) {
        foreach(Tank *tank, player1Tanks) {
            if (tank->getRow() == row && tank->getCol() == col) {
                return tank;
            }
        }
        foreach(Tank *tank, player2Tanks) {
            if (tank->getRow() == row && tank->getCol() == col) {
                return tank;
            }
        }
        return nullptr;
    }

    //esto puede ser otra alternativa para no pintarlo
    void highlightTank(QGraphicsEllipseItem *tank) {
        tank->setPen(QPen(Qt::green, 3));
    }

    // Este y el de arriba son cosillas como para si no quiero hacerlo como abajo
    void resetTankAppearance(QGraphicsEllipseItem *tank) {
        tank->setPen(QPen(Qt::NoPen));  // Quitar el borde
    }

    void placeInitialTanks() {
    QRandomGenerator *randGen = QRandomGenerator::global();

        /* Se ponen los tanques azules y rojos solo en las primeras dos columnas de la izquierda
         * aquí primero se ponen los obstaculos y hasta despues de colocan los tanques para q no se bloqueen, etc
         */
    for (int i = 0; i < 2; ++i) {
        int row, col;

        // Buscar posiciones aleatorias para los tanques rojos
        do {
            row = randGen->bounded(numRows);
            col = randGen->bounded(0, 2); // Se limitan las columnas
        } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col)); // Se hace la verificación de q no sea un obstáculo o una celda ocupada

        placeTank(row, col, Qt::red); // Se coloca el tanque
        gameMap.addEdge(row, col); // se pone la celda como ocupadaocupado
    }

    for (int i = 0; i < 2; ++i) {
        int row, col;

        // Se busacn posiciones aleatorias para los tanques azules
        do {
            row = randGen->bounded(numRows);
            col = randGen->bounded(0, 2);
        } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));

        placeTank(row, col, Qt::blue);
        gameMap.addEdge(row, col);
    }

    /* Se ponen los tanques amarillos y celestes solo en las primeras dos columnas de la derecha
     * aquí primero se ponen los obstaculos y hasta despues de colocan los tanques para q no se bloqueen, etc
     */
    for (int i = 0; i < 2; ++i) {
        int row, col;

        // Se buscan posiciones aleatorias para los tanques amarillos
        do {
            row = randGen->bounded(numRows);
            col = randGen->bounded(numCols - 2, numCols);
        } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));

        placeTank(row, col, Qt::yellow);
        gameMap.addEdge(row, col);
    }

    for (int i = 0; i < 2; ++i) {
        int row, col;

        // Se buscanposiciones aleatorias para los celestes
        do {
            row = randGen->bounded(numRows);
            col = randGen->bounded(numCols - 2, numCols);
        } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));

        placeTank(row, col, Qt::cyan);
        gameMap.addEdge(row, col);
    }
}

 void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::RightButton) {
        // Se obtiene q es lo q se está tocando
        QGraphicsItem *item = itemAt(event->pos());

        // Por si no me sirven están estos mensajes
        if (!item) {
            qDebug() << "No item found at position" << event->pos();
            return;
        }

        // Comprobar si el ítem es un QGraphicsEllipseItem (tanque)-> esto es tricky pq de hecho solo si tocas el tanque de vdd(el circulito) te puedes mover
        QGraphicsEllipseItem *tank = dynamic_cast<QGraphicsEllipseItem *>(item);
        if (tank) {

            //Se obtienen las coordenadas de donde está el tanque
            int row = tank->data(0).toInt();
            int col = tank->data(1).toInt();

            QGraphicsItem *cellItem = findCellAt(row, col);
            QGraphicsRectItem *rect = dynamic_cast<QGraphicsRectItem *>(cellItem);

            //esto es para pintar sobre q jugador estoy
            if (rect) {
                if (player1Turn) {
                    rect->setBrush(Qt::magenta);
                } else {
                    rect->setBrush(Qt::green);
                }
                rect->update();  // Actualizar celda
                scene.update();  // Forzar actualización (por si no servía jsjs)
                player1Turn = !player1Turn;  // Cambiar de turno
            } else {
                qDebug() << "No rect found at cell position (" << row << ", " << col << ")";
            }
        } else {
            qDebug() << "Item at position is not a QGraphicsEllipseItem (tank)";
        }
    }
}

    //hay un find tank at pero el tanque es un circulo no es un rectangulo entonces buscarlo como rectangulo no me serviria de mucho
QGraphicsItem* findCellAt(int row, int col) {
    foreach(QGraphicsItem *item, scene.items()) {
        QGraphicsRectItem *rect = dynamic_cast<QGraphicsRectItem *>(item);
        if (rect) {
            int rectRow = rect->data(0).toInt();
            int rectCol = rect->data(1).toInt();
            if (rectRow == row && rectCol == col) {
                return rect;
            }
        }
    }
    return nullptr;
}


    void moveTank(QGraphicsEllipseItem* tank, int oldRow, int oldCol, int newRow, int newCol) {
        gameMap.removeEdge(oldRow, oldCol);

        gameMap.addEdge(newRow, newCol);

        updateTankGraphics(tank, oldRow, oldCol, newRow, newCol);
    }

    void updateTankGraphics(QGraphicsEllipseItem* tank, int oldRow, int oldCol, int newRow, int newCol) {
        if (tank) {
            tank->setRect(newCol * tileSize + 10, newRow * tileSize + 10, tileSize - 20, tileSize - 20);

            tank->setData(0, newRow);
            tank->setData(1, newCol);
        }
    }


};

#endif //GAMELAUNCH_H
