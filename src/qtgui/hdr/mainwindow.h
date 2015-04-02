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
    explicit MainWindow(bool cfg, QWidget *parent = 0);
    ~MainWindow();

    void post_image(mat_ptr_t img);

    void update_stat(const std::string& stat);

private slots:
    void on_actionStart_triggered();

    void on_actionConfigure_triggered();

    void slot_show_image(QImage * img);

    void on_comboBox_profiles_currentIndexChanged(int index);

signals:
    void signal_show_image(QImage * img);

private:

    void adjust_size();

    void adjust_preset();

    bool is_cfg_enabled_;

    Ui::MainWindow * ui;
    QGraphicsScene * scene_;
    QGraphicsPixmapItem * item_;
    cfg_ptr_t cfg_;
    bool do_capture_;
    boost::shared_ptr<service_worker> worker_;

    // profiles with presets for transmitter
    boost::shared_ptr<profile_list_t> presets_;
    boost::shared_ptr<int_pair_t> resolutions_;
    boost::shared_ptr<int_pair_t> bchmodes_;
};


//void update_stat(Ui::MainWindow & u, const std::string& data);

#endif // MAINWINDOW_H
