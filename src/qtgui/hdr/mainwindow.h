#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "config_iface.hpp"

class service_worker;
namespace cv {
class Mat;
}

namespace Ui {
class MainWindow;
}

typedef boost::shared_ptr<cv::Mat> mat_ptr_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void post_image(mat_ptr_t img);

    void update_stat(const std::string& stat);

private slots:
    void on_actionStart_triggered();

    void on_actionConfigure_triggered();

    void slot_show_image(QImage * img);

signals:
    void signal_show_image(QImage * img);

private:

    void adjust_size();

    Ui::MainWindow * ui;
    QGraphicsScene * scene_;
    QGraphicsPixmapItem * item_;
    cfg_ptr_t cfg_;
    bool do_capture_;
    boost::shared_ptr<service_worker> worker_;
};


//void update_stat(Ui::MainWindow & u, const std::string& data);

#endif // MAINWINDOW_H
