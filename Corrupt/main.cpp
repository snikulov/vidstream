#include "mainwindow.h"
#include "params.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile f(TRANSMITTER_EXECUTABLE);
    if (!f.exists()) {
        QMessageBox::warning(NULL, "Error", "Transmitter executable not present in current directory");
        return 0;
    }
    MainWindow w;
    w.show();

    return a.exec();
}
