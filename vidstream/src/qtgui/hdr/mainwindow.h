#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <worker.hpp>

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

    void on_checkBox_is_gray_stateChanged(int arg1);

    void on_comboBox_camera_resolution_currentIndexChanged(int index);

    void on_pushButton_operate_clicked();

    void on_spinBox_rst_num_valueChanged(int arg1);

    void on_spinBox_port_cmd_valueChanged(int arg1);

    void on_spinBox_port_data_valueChanged(int arg1);

    void on_lineEdit_channel_speed_textChanged(const QString &arg1);

    void on_comboBox_bch_mode_currentIndexChanged(int index);

    void on_spinBox_bch_m_valueChanged(int arg1);

    void on_spinBox_bch_t_valueChanged(int arg1);

    void on_spinBox_error_persent_valueChanged(int arg1);

    void on_spinBox_lum_quality_valueChanged(int arg1);

    void on_spinBox_chrome_quality_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    bool is_srv_running_;
    boost::shared_ptr<boost::property_tree::ptree> cfg_;
    boost::shared_ptr<worker> logic_;
};

#endif // MAINWINDOW_H
