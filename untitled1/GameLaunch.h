#ifndef GAMELAUNCH_H
#define GAMELAUNCH_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QPen>
#include <QDebug>
#include <QBrush>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QMessageBox>
#include "Graph.h"
#include "Tank.h"
#include "Player.h"
#include "Pathfinding.h" // Incluye los algoritmos de movimiento

// Clase Bullet
class Bullet {
public:
    QPointF position;
    QPointF direction;
    qreal speed;
    Tank* shooter;
    QGraphicsEllipseItem* graphicsItem;
    int bounceCount; // Contador de rebotes
    QGraphicsScene* scene; // Puntero a la escena
    int tileSize; // Tamaño de la celda
    const Map& gameMap; // Referencia al mapa

    Bullet(QPointF pos, QPointF dir, qreal spd, Tank* tank, QGraphicsScene* scene, int tileSize, const Map& gameMap)
        : position(pos), direction(dir), speed(spd), shooter(tank), bounceCount(0), scene(scene), tileSize(tileSize), gameMap(gameMap) {
        graphicsItem = scene->addEllipse(position.x(), position.y(), 5, 5, QPen(Qt::black), QBrush(Qt::black));
    }

    void move() {
        position += direction * speed;
        graphicsItem->setPos(position);
    }

    bool checkCollision(Tank* tank) {
        return (tank->getGraphicsItem()->contains(position));
    }

    void reflect() {
        if (position.x() <= 0 || position.x() >= scene->sceneRect().width()) {
            direction.setX(-direction.x());
            bounceCount++;
        }
        if (position.y() <= 0 || position.y() >= scene->sceneRect().height()) {
            direction.setY(-direction.y());
            bounceCount++;
        }
    }

    bool checkWallCollision() {
        // Verificar colisión con los bordes de la pantalla
        if (position.x() <= 0 || position.x() >= scene->sceneRect().width() ||
            position.y() <= 0 || position.y() >= scene->sceneRect().height()) {
            reflect();
            return true;
            }

        // Verificar colisión con los obstáculos del mapa
        int row = static_cast<int>(position.y() / tileSize);
        int col = static_cast<int>(position.x() / tileSize);
        if (gameMap.getValue(row, col) == Map::OBSTACLE) {
            reflect();
            return true;
        }

        // Verificar colisión con los bordes negros
        for (QGraphicsItem* item : scene->items()) {
            QGraphicsRectItem* wall = dynamic_cast<QGraphicsRectItem*>(item);
            if (wall && wall->pen().color() == Qt::black && wall->contains(position)) {
                reflect();
                return true;
            }
        }

        return false;
    }

    bool shouldDisappear() {
        return bounceCount >= 3;
    }
};

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
    Tank* selectedTank = nullptr;
    QList<QGraphicsLineItem*> currentPathLines;
    QList<QGraphicsTextItem*> player1HealthTexts;
    QList<QGraphicsTextItem*> player2HealthTexts;
    QList<Bullet*> bullets;

    QTimer gameTimer;
    QLabel* timerLabel;
    int remainingTime; // En segundos, para 4 minutos será 240 segundos
    bool isOccupied(int row, int col) {
        for (Tank* tank : player1.getTanks()) {
            if (tank->getRow() == row && tank->getCol() == col) {
                return true;
            }
        }
        for (Tank* tank : player2.getTanks()) {
            if (tank->getRow() == row && tank->getCol() == col) {
                return true;
            }
        }
        return false;
    }

public:
    GameLaunch(QWidget *parent = nullptr)
        : QGraphicsView(parent), numRows(gameMap.getNumRows()), numCols(gameMap.getNumCols()), tileSize(50), remainingTime(240) {
        // Configurar la escena
        scene.setSceneRect(0, 0, tileSize * numCols, tileSize * numRows);
        this->setScene(&scene);
        gameMap.generateObstacles();
        drawGrid();
        placeInitialTanks();
        gameMap.printMatrix();
        placeTexts();
        setWindowTitle("Tank Attack!");
        setFixedSize(tileSize * numCols + 20, tileSize * numRows + 20);
        player1.setTurn(true);
        player2.setTurn(false);
        updateHealthDisplay();

        // Configuración del temporizador de juego
        timerLabel = new QLabel(this);
        timerLabel->setStyleSheet("color: black; font: bold 16px;");
        timerLabel->setGeometry(10, 10, 100, 30);
        updateTimerLabel();

        connect(&gameTimer, &QTimer::timeout, this, &GameLaunch::updateGameTimer);
        gameTimer.start(1000); // Disminuir cada segundo

        // Temporizador para la actualización de balas
        startTimer(16); // Aproximadamente 60 FPS
    }

protected:
    void drawGrid() {
        // Dibujar la cuadrícula
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                QGraphicsRectItem *rect = scene.addRect(col * tileSize, row * tileSize, tileSize, tileSize);
                if (row >= numRows - 2) { // Verificar si es una de las últimas dos filas
                    rect->setBrush(Qt::black); // Pintar de negro
                    rect->setPen(Qt::NoPen); // Sin borde
                } else if (gameMap.isObstacle(row, col)) {
                    rect->setBrush(Qt::red);
                    rect->setPen(QPen(Qt::black)); // Borde negro
                } else {
                    rect->setBrush(Qt::white);
                    rect->setPen(QPen(Qt::lightGray));
                }
            }
        }
    }

    void placeTexts() {
        // Texto para el Jugador 1
        QGraphicsTextItem *player1Label = new QGraphicsTextItem("Player 1:");
        player1Label->setPos(10, tileSize * (numRows - 2) + 10);
        player1Label->setDefaultTextColor(Qt::white);
        scene.addItem(player1Label);

        int startX1 = 100;
        int startY = tileSize * (numRows - 2) + 10;
        for (int i = 0; i < 4; ++i) {
            Tank* tank = player1.getTanks()[i];
            QGraphicsTextItem *healthText = new QGraphicsTextItem(QString("Health: %1").arg(tank->getHealth()));
            healthText->setPos(startX1 + i * 150, startY);
            healthText->setDefaultTextColor(Qt::white);
            scene.addItem(healthText);
            player1HealthTexts.append(healthText);
        }

        // Texto para el Jugador 2
        QGraphicsTextItem *player2Label = new QGraphicsTextItem("Player 2:");
        player2Label->setPos(10, tileSize * (numRows - 1) + 10);
        player2Label->setDefaultTextColor(Qt::white);
        scene.addItem(player2Label);

        int startX2 = 100;
        int startY2 = tileSize * (numRows - 1) + 10;
        for (int i = 0; i < 4; ++i) {
            Tank* tank = player2.getTanks()[i];
            QGraphicsTextItem *healthText = new QGraphicsTextItem(QString("Health: %1").arg(tank->getHealth()));
            healthText->setPos(startX2 + i * 150, startY2);
            healthText->setDefaultTextColor(Qt::white);
            scene.addItem(healthText);
            player2HealthTexts.append(healthText);
        }
    }

    void updateHealthDisplay() {
        // Actualizar la visualización de la salud para los 4 tanques de cada jugador
        for (int i = 0; i < 4; ++i) {
            player1HealthTexts[i]->setPlainText(QString("Tank %1 Health: %2")
                .arg(i + 1).arg(player1.getTanks()[i]->getHealth()));
            player2HealthTexts[i]->setPlainText(QString("Tank %1 Health: %2")
                .arg(i + 1).arg(player2.getTanks()[i]->getHealth()));
        }
    }

    void updateHealthTexts() {
        // Actualizar textos de vida del jugador 1
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

        // Colocar 2 tanques rojos y 2 azules para el jugador 1
        for (int i = 0; i < 2; ++i) {
            int row, col;
            // Buscar posiciones aleatorias para los tanques rojos
            do {
                row = randGen->bounded(numRows);
                col = randGen->bounded(0, 2); // Limitamos las columnas a las dos primeras
            } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));
            placeTank(row, col, Qt::red); // Colocar tanque rojo
            gameMap.addEdge(row, col); // Marcar celda como ocupada
        }

        for (int i = 0; i < 2; ++i) {
            int row, col;
            // Buscar posiciones aleatorias para los tanques azules
            do {
                row = randGen->bounded(numRows);
                col = randGen->bounded(0, 2);
            } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));
            placeTank(row, col, Qt::blue); // Colocar tanque azul
            gameMap.addEdge(row, col);
        }

        // Colocar 2 tanques amarillos y 2 celestes para el jugador 2
        for (int i = 0; i < 2; ++i) {
            int row, col;
            // Buscar posiciones aleatorias para los tanques amarillos
            do {
                row = randGen->bounded(numRows);
                col = randGen->bounded(numCols - 2, numCols); // Limitamos las columnas a las dos últimas
            } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));
            placeTank(row, col, Qt::yellow); // Colocar tanque amarillo
            gameMap.addEdge(row, col);
        }

        for (int i = 0; i < 2; ++i) {
            int row, col;
            // Buscar posiciones aleatorias para los tanques celestes
            do {
                row = randGen->bounded(numRows);
                col = randGen->bounded(numCols - 2, numCols);
            } while (gameMap.isObstacle(row, col) || gameMap.isOccupied(row, col));
            placeTank(row, col, Qt::cyan); // Colocar tanque celeste
            gameMap.addEdge(row, col);
        }
    }

    void moveTank(Tank *tank, int newRow, int newCol) {
        if (gameMap.isObstacle(newRow, newCol) || isOccupied(newRow, newCol)) {
            qDebug() << "Movimiento inválido: la casilla está ocupada o es un obstáculo.";
            return;
        }
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
                int targetRow = event->position().y() / tileSize;
                int targetCol = event->position().x() / tileSize;
                executeMovementAlgorithm(selectedTank, targetRow, targetCol);
                switchTurn();
                selectedTank = nullptr;
            }
            }else if (event->button() == Qt::RightButton) {
                if (selectedTank) {
                    // Obtener la posición del tanque seleccionado y ajustar la posición de la bala
                    QPointF tankPosition = selectedTank->getGraphicsItem()->scenePos(); // Obtener la posición del tanque
                    QPointF bulletPosition = tankPosition + QPointF(20, 20); // Ajustar al centro del tanque (40x40 pixels)
                    QPointF targetPoint = event->position();
                    // Calcular la dirección desde el tanque hacia el punto de destino
                    QPointF direction = targetPoint - bulletPosition;
                    // Normalizar la dirección
                    qreal length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
                    if (length != 0) {
                        direction /= length; // Convertir a vector unitario
                    }
                    bullets.append(new Bullet(bulletPosition, direction, 5.0, selectedTank, &scene, tileSize, gameMap));
                    selectedTank = nullptr; // Deseleccionar el tanque
                } else {
                    QGraphicsItem *item = itemAt(event->position().toPoint());
                    if (!item) return;
                    Tank *tank = findTankAt(item->data(0).toInt(), item->data(1).toInt());
                    if (tank) {
                        selectedTank = tank;
                        qDebug() << "Tanque seleccionado en (" << tank->getRow() << ", " << tank->getCol() << ")";
                    }
                }
            }

    }


    void updateBullets() {
        for (int i = bullets.size() - 1; i >= 0; --i) { // Iterar de atrás hacia adelante
            Bullet* bullet = bullets[i];
            bullet->move();
            // Verificar colisión con tanques
            bool collided = false;
            for (Tank* tank : player1.getTanks()) {
                if (bullet->checkCollision(tank)) {
                    applyDamage(tank, bullet->shooter);
                    collided = true;
                    break;
                }
            }
            for (Tank* tank : player2.getTanks()) {
                if (bullet->checkCollision(tank)) {
                    applyDamage(tank, bullet->shooter);
                    collided = true;
                    break;
                }
            }
            // Verificar colisión con paredes y obstáculos
            if (bullet->checkWallCollision()) {
                collided = true;
            }
            // Eliminar la bala si ha colisionado o si debe desaparecer
            if (collided || bullet->shouldDisappear()) {
                scene.removeItem(bullet->graphicsItem);
                delete bullet;
                bullets.removeAt(i);
            }
        }
    }

    void applyDamage(Tank* target, Tank* shooter) {
        int damage = (target->getColor() == Qt::cyan || target->getColor() == Qt::blue) ? 25 : 50;
        target->takeDamage(damage);
        updateHealthDisplay();
    }

    void timerEvent(QTimerEvent *event) override {
        updateBullets();
    }
    void updateTimerLabel() {
        int minutes = remainingTime / 60;
        int seconds = remainingTime % 60;
        timerLabel->setText(QString("Time: %1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    }

    void updateGameTimer() {
        remainingTime--;
        updateTimerLabel();

        if (remainingTime <= 0) {
            endGame();
        }
    }

    void endGame() {
        gameTimer.stop(); // Detener el temporizador

        int player1TotalHealth = 0;
        int player2TotalHealth = 0;

        for (Tank* tank : player1.getTanks()) {
            player1TotalHealth += tank->getHealth();
        }
        for (Tank* tank : player2.getTanks()) {
            player2TotalHealth += tank->getHealth();
        }

        QString winnerText;
        if (player1TotalHealth > player2TotalHealth) {
            winnerText = "Player 1 Wins!";
        } else if (player2TotalHealth > player1TotalHealth) {
            winnerText = "Player 2 Wins!";
        } else {
            winnerText = "It's a Tie!";
        }

        QMessageBox::information(this, "Game Over", winnerText);
    }
};

#endif // GAMELAUNCH_H
