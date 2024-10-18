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
#include "Player.h"

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


    QPoint selectedTankPosition;
    bool tankSelected = false;
    QGraphicsEllipseItem *selectedTank = nullptr;

public:
    GameLaunch(QWidget *parent = nullptr)
        : QGraphicsView(parent), numRows(gameMap.getRows()), numCols(gameMap.getCols()), tileSize(50){
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
            player1.addTank(tank);
        } else {
            player2.addTank(tank);
        }
    }


    Tank* findTankAt(int row, int col) {
        foreach(Tank *tank, player1.getTanks()) {
            if (tank->getRow() == row && tank->getCol() == col) {
                return tank;
            }
        }
        foreach(Tank *tank, player2.getTanks()) {
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



void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::RightButton) {
        // Se obtiene el ítem que se está tocando
        QGraphicsItem *item = itemAt(event->pos());

        if (!item) {
            qDebug() << "No item found at position" << event->pos();
            return;
        }

        // Comprobar si el ítem es un QGraphicsEllipseItem (tanque)
        QGraphicsEllipseItem *tank = dynamic_cast<QGraphicsEllipseItem *>(item);
        if (tank) {
            // Se obtienen las coordenadas del tanque
            int row = tank->data(0).toInt();
            int col = tank->data(1).toInt();

            // Verificar a qué jugador pertenece el tanque
            bool isPlayer1Tank = false;
            for (Tank* t : player1.getTanks()) {
                if (t->getRow() == row && t->getCol() == col) {
                    isPlayer1Tank = true;
                    break;
                }
            }

            bool isPlayer2Tank = false;
            for (Tank* t : player2.getTanks()) {
                if (t->getRow() == row && t->getCol() == col) {
                    isPlayer2Tank = true;
                    break;
                }
            }

            // Comprobar si es el turno del jugador correspondiente
            if (player1.getTurn() && isPlayer1Tank) {
                // Acciones para el tanque del jugador 1
                QGraphicsRectItem *rect = dynamic_cast<QGraphicsRectItem *>(findCellAt(row, col));
                if (rect) {
                    rect->setBrush(Qt::magenta); // Color para el jugador 1
                    rect->update();  // Actualizar celda
                    scene.update();  // Forzar actualización de la escena
                }

                qDebug() << "Jugador 1 actuó sobre el tanque en posición (" << row << ", " << col << ")";
                switchTurn();  // Cambiar el turno después de la acción
            } else if (player2.getTurn() && isPlayer2Tank) {
                // Acciones para el tanque del jugador 2
                QGraphicsRectItem *rect = dynamic_cast<QGraphicsRectItem *>(findCellAt(row, col));
                if (rect) {
                    rect->setBrush(Qt::green); // Color para el jugador 2
                    rect->update();  // Actualizar celda
                    scene.update();  // Forzar actualización de la escena
                }

                qDebug() << "Jugador 2 actuó sobre el tanque en posición (" << row << ", " << col << ")";
                switchTurn();  // Cambiar el turno después de la acción
            } else {
                qDebug() << "No puedes interactuar con ese tanque.";
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
