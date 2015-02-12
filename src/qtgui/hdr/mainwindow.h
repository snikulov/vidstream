#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "config_iface.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionStart_triggered();

    void on_actionConfigure_triggered();

//    void update_video_ui(QImage img);

private:
    Ui::MainWindow *ui;
    cfg_ptr_t cfg_;
};

#endif // MAINWINDOW_H
