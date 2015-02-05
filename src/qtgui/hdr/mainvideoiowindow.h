#ifndef MAINVIDEOIOWINDOW_H
#define MAINVIDEOIOWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainVideoIoWindow;
}

class MainVideoIoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainVideoIoWindow(QWidget *parent = 0);
    ~MainVideoIoWindow();

private:
    Ui::MainVideoIoWindow *ui;
};

#endif // MAINVIDEOIOWINDOW_H
