#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "config_iface.hpp"

class service_worker;
namespace cv {
class Mat;
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void post_image(cv::Mat * img);

private slots:
    void on_actionStart_triggered();

    void on_actionConfigure_triggered();

    void slot_show_image(QImage img);

signals:
    void signal_show_image(QImage img);

//    void update_video_ui(QImage img);

private:
    Ui::MainWindow *ui;
    cfg_ptr_t cfg_;
    bool do_capture_;
    boost::shared_ptr<service_worker> worker_;
};

#endif // MAINWINDOW_H
