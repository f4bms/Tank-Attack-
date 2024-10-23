#include "GameLaunch.h"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GameLaunch gameWindow;
    gameWindow.show();

    return app.exec();
}

