#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <stat/stat_data.hpp>
#include <worker.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void update_stat(const std::string&);

    ~MainWindow();

private slots:

    void on_checkBox_is_gray_stateChanged(int arg1);

    void on_comboBox_camera_resolution_currentIndexChanged(int index);

    void on_pushButton_operate_clicked();

    void on_spinBox_rst_num_valueChanged(int arg1);

    void on_spinBox_port_cmd_valueChanged(int arg1);

    void on_spinBox_port_data_valueChanged(int arg1);

    void on_comboBox_bch_mode_currentIndexChanged(int index);

    void on_spinBox_bch_m_valueChanged(int arg1);

    void on_spinBox_bch_t_valueChanged(int arg1);

    void on_doubleSpinBox_error_persent_valueChanged(double arg1);

    void on_spinBox_bch_m_editingFinished();

    void on_lineEdit_sent_bytes_textChanged(const QString &arg1);

    void on_spinBox_bw_valueChanged(int arg1);

    void on_spinBox_jpeg_quality_valueChanged(int arg1);

    void on_spinBox_fps_limit_valueChanged(int arg1);

    void on_comboBox_ip_selector_currentIndexChanged(int index);

    void on_comboBox_config_preset_currentIndexChanged(int index);

    void on_timer_overflow();

private:
    typedef boost::shared_ptr<boost::property_tree::ptree> config_ptr_t;
    Ui::MainWindow *ui;
    bool is_srv_running_;
    config_ptr_t cfg_;
    boost::shared_ptr<worker> logic_;

    stat_data_t stat_;

    QTimer * refresh_timer_;
};

// free form functions
bool ui_update(Ui::MainWindow &
               , const boost::property_tree::ptree &);

void cfg_update(boost::property_tree::ptree &
                , const Ui::MainWindow &);

int ui_set_resolution_index(
    Ui::MainWindow&
    , const boost::property_tree::ptree &);

int cfg_set_resolution_by_list_index(boost::property_tree::ptree &, int );

int ui_set_bch_preset_list_index(
    Ui::MainWindow&
    , const boost::property_tree::ptree &);

int cfg_set_bch_values_by_list_index(
    boost::property_tree::ptree&
    , Ui::MainWindow&, int);

#endif // MAINWINDOW_H
