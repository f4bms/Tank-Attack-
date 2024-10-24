#ifndef TANK_H
#define TANK_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QColor>

class Tank {
private:
    int row;
    int col;
    QColor color;
    QGraphicsEllipseItem *tankItem;
    QGraphicsScene *scene;
    static const int tileSize = 50;
    int currentHealth;
    int maxHealth;

public:
    Tank(int row, int col, const QColor& color, QGraphicsScene *scene, int maxHealth)
        : row(row), col(col), color(color), scene(scene), maxHealth(maxHealth), currentHealth(maxHealth) {
        tankItem = scene->addEllipse(col * tileSize + 10, row * tileSize + 10, tileSize - 20, tileSize - 20,
                                      QPen(Qt::NoPen), QBrush(color));
        tankItem->setData(0, row); // fila
        tankItem->setData(1, col); // columna
    }

    QGraphicsEllipseItem* getGraphicsItem() const {
        return tankItem;
    }

    int getRow() const {
        return row;
    }

    int getCol() const {
        return col;
    }

    void setColor(const QColor& newColor) {
        color = newColor;
        tankItem->setBrush(color);
        tankItem->update();
    }

    QColor getColor() const {
        return color;
    }

    void updatePosition(int newRow, int newCol) {
        row = newRow;
        col = newCol;
        tankItem->setRect(col * tileSize + 10, row * tileSize + 10, tileSize - 20, tileSize - 20);
        tankItem->setData(0, row);
        tankItem->setData(1, col);
    }

    int getHealth() const {
        return currentHealth;
    }

    void takeDamage(int damage) {
        if (currentHealth > 0) {
            currentHealth -= damage;
            if (currentHealth < 0) {
                currentHealth = 0;
            }
        }
    }

    bool isDestroyed() const {
        return currentHealth <= 0;
    }

    int getMaxHealth() const {
        return maxHealth;
    }

    void resetHealth() {
        currentHealth = maxHealth;
    }

    qreal getAngle() const {
        return 90; // El ángulo de disparo es de 90 grados
    }
};

#endif // TANK_H

