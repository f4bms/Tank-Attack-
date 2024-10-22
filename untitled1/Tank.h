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

public:
    Tank(int row, int col, const QColor& color, QGraphicsScene *scene)
        : row(row), col(col), color(color), scene(scene) {
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
};

#endif // TANK_H
