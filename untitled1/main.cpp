#include "GameLaunch.h"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Crear la ventana del juego
    GameLaunch gameWindow;
    gameWindow.show();

    return app.exec();
}

