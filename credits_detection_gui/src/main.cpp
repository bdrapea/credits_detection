#include "crde_main_window.h"

int main(int argc, char** argv)
{
    if(argc == 1)
    {
        std::cerr << "Please provide the executable path" << std::endl;
        return -1;
    }

    //Deleting files in graphs
    system("rm -r ../graphs/*");

    QApplication app(argc, argv);
    crde::gui::main_window mainwin(argv[1]);
    mainwin.showMaximized();

    return app.exec();
}
