#ifndef BULLET_H
#define BULLET_H

#include <QPainter>
#include <QLineF>
#include <QVector>
#include <QPointF>
#include "Graph.h"

class Bullet : public QGraphicsItem {
public:
    Bullet(Tank* shooter, QPointF target, qreal angle, QGraphicsScene* scene, Map* map);
    void calculatePath();
    void update();
    void applyDamage(Tank* tank);
    void bounce(qreal wallAngle);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    Tank* shooter;
    QPointF target;
    qreal angle;
    QVector<QPointF> path;
    QGraphicsScene* scene;
    Map* map;

    bool checkCollision(QPointF point);
    void handleCollision(QPointF point);
    bool collidesWithTank(QPointF point);
    bool collidesWithShooter(QPointF point);
    bool collidesWithWall(QPointF point);
};

Bullet::Bullet(Tank* shooter, QPointF target, qreal angle, QGraphicsScene* scene, Map* map)
    : shooter(shooter), target(target), angle(angle), scene(scene), map(map) {
    calculatePath();
}

void Bullet::calculatePath() {
    // Implementar el pathfinding de línea vista
    QLineF line(shooter->getGraphicsItem()->pos(), target);
    path.append(line.p1());
    path.append(line.p2());
}

void Bullet::update() {
    for (const QPointF& point : path) {
        if (checkCollision(point)) {
            handleCollision(point);
            break;
        }
    }
}

void Bullet::applyDamage(Tank* tank) {
    if (tank->getColor() == Qt::cyan || tank->getColor() == Qt::blue) {
        tank->takeDamage(25);
    } else if (tank->getColor() == Qt::yellow || tank->getColor() == Qt::red) {
        tank->takeDamage(50);
    }
}

void Bullet::bounce(qreal wallAngle) {
    angle = (2 * wallAngle - angle);
}

QRectF Bullet::boundingRect() const {
    return QRectF(-5, -5, 10, 10);
}

void Bullet::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->setBrush(Qt::black);
    painter->drawEllipse(boundingRect());
}

bool Bullet::checkCollision(QPointF point) {
    // Verificar colisión con tanque o pared
    return collidesWithTank(point) || collidesWithWall(point);
}

void Bullet::handleCollision(QPointF point) {
    if (collidesWithTank(point)) {
        applyDamage(shooter);
    } else if (collidesWithWall(point)) {
        bounce(angle);
    }
}

bool Bullet::collidesWithTank(QPointF point) {
    // Verificar colisión con un tanque
    QList<QGraphicsItem*> items = scene->items(point);
    for (QGraphicsItem* item : items) {
        if (dynamic_cast<Tank*>(item)) {
            return true;
        }
    }
    return false;
}

bool Bullet::collidesWithShooter(QPointF point) {
    // Verificar si colisiona con el tanque que disparó
    return shooter->getGraphicsItem()->contains(point);
}

bool Bullet::collidesWithWall(QPointF point) {
    // Verificar colisión con una pared
    int row = point.y() / 50;
    int col = point.x() / 50;
    return map->isObstacle(row, col);
}

#endif // BULLET_H

