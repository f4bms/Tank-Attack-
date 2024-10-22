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


    /*depende de como trabajemos la vida podemos tratar lo del health distinto
     */
    int getHealth() const {
        return currentHealth;
    }

    /*or ejemplo, aquí se le debe de pasar una cantidad de daño y el mae atualiza lo que le pase
     *Tmb si la vida se reduce a menos de cero, se ajusta a cero, esto tmb, depende del enfoque podriamos cambiarlo a q si llega a 0 o lleg a menos de 0 tenga un booleano como "alive" o "dead"
     *creo que tmb voy a hacer eso de una vez
     */
    void takeDamage(int damage) {
        if (currentHealth > 0) {
            currentHealth -= damage;
            if ( currentHealth< 0) {
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

    // Method to reset health (optional)
    void resetHealth() {
        currentHealth = maxHealth;
    }
};

#endif // TANK_H
