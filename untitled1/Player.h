#ifndef PLAYER_H
#define PLAYER_H

#include <QColor>
#include "Tank.h"

class Player {
private:
    QList<Tank*> tanks;
    bool isTurn;

public:
    Player() : isTurn(false) {}

    void addTank(Tank* tank) {
        if (tank != nullptr && !ownsTank(tank)) {
            tanks.append(tank);
        }
    }

    const QList<Tank*>& getTanks() const {
        return tanks;
    }

    bool ownsTank(Tank* tank) const {
        return tanks.contains(tank);
    }

    void setTurn(bool turn) {
        isTurn = turn;
    }

    bool getTurn() const {
        return isTurn;
    }
};


#endif // PLAYER_H
