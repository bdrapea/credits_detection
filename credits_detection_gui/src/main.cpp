#include "crde_main_window.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    crde::gui::main_window mainwin;

    mainwin.showMaximized();

    return app.exec();
}
