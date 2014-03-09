#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "mainwindow.h"

#include <QMessageBox>

SettingsDialog::SettingsDialog(MainWindow *mainWin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    mainWin(mainWin)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    InitSettingsUI();
}

void SettingsDialog::InitSettingsUI()
{
    Settings s = mainWin->GetSettings();
    int q = s.jpeg_quality, m = s.bch_m, t = s.bch_t;
    size_t rn = s.row_num, rd = s.row_denom;
    size_t bn = s.block_num, bd = s.block_denom;
    ui->qualitySpinBox->setValue(q);
    ui->mSpinBox->setValue(m);
    ui->tSpinBox->setValue(t);
    ui->rowsNumSpinBox->setValue(rn);
    ui->rowsDenomSpinBox->setValue(rd);
    ui->blocksNumSpinBox->setValue(bn);
    ui->blocksDenomSpinBox->setValue(bd);
    if (m == 13 && t == 105) {       // ecc2data = 0.2
        ui->presetsComboBox->setCurrentIndex(0);
    } else if (m == 5 && t == 3) { // ecc2data = 1
        ui->presetsComboBox->setCurrentIndex(1);
    } else if (m == 6 && t == 6) { // ecc2data = 2
        ui->presetsComboBox->setCurrentIndex(2);
    } else if (m == 5 && t == 4) {   // ecc2data = 3
        ui->presetsComboBox->setCurrentIndex(3);
    } else {
        ui->presetsComboBox->setCurrentIndex(4);
    }
    ui->blockSizeSpinBox->setValue(s.rst_block_size);
}

void SettingsDialog::on_presetsComboBox_currentIndexChanged(int index)
{
    preset_changed[0] = true;
    preset_changed[1] = true;
    if (index == 0) { // ecc2data = 0.2
        ui->mSpinBox->setValue(13);
        ui->tSpinBox->setValue(105);
    } else if (index == 1) { // ecc2data = 1
        ui->mSpinBox->setValue(5);
        ui->tSpinBox->setValue(3);
    } else if (index == 2) { // ecc2data = 2
        ui->mSpinBox->setValue(6);
        ui->tSpinBox->setValue(6);
    } else if (index == 3) { // ecc2data = 3
        ui->mSpinBox->setValue(5);
        ui->tSpinBox->setValue(4);
    }
    QApplication::processEvents();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_mSpinBox_valueChanged(int arg1)
{
    if (!preset_changed[0]) {
        ui->presetsComboBox->setCurrentIndex(4); // Custom
    }
    preset_changed[0] = false;
}

void SettingsDialog::on_tSpinBox_valueChanged(int arg1)
{
    if (!preset_changed[1]) {
        ui->presetsComboBox->setCurrentIndex(4); // Custom
    }
    preset_changed[1] = false;
}

void SettingsDialog::on_buttonBox_accepted()
{
   if (!mainWin->SetJpegQuality(ui->qualitySpinBox->value())) {
       QMessageBox::warning(this, "Error", "Invalid JPEG quality");
   }
   if (!mainWin->SetBchParams(ui->mSpinBox->value(), ui->tSpinBox->value())) {
       QMessageBox::warning(this, "Error", "Invalid BCH parameters");
   }
   if (!mainWin->SetRowInterlace(ui->rowsNumSpinBox->value(),
                                 ui->rowsDenomSpinBox->value()) ||
       !mainWin->SetBlockInterlace(ui->blocksNumSpinBox->value(),
                                   ui->blocksDenomSpinBox->value())) {
       QMessageBox::warning(this, "Error", "Invalid interlace ratio");
       return;
   }
   if (!mainWin->SetBlockSize(ui->blockSizeSpinBox->value())) {
       QMessageBox::warning(this, "Error", "Invalid RST block size");
       return;
   }
}
