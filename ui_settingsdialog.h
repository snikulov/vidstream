/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QGroupBox *BCHGroupBox;
    QComboBox *presetsComboBox;
    QLabel *label_2;
    QSpinBox *mSpinBox;
    QSpinBox *tSpinBox;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *bchInfoLabel;
    QGroupBox *ResolutionGroupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_9;
    QSpinBox *widthSpinBox;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_10;
    QSpinBox *heightSpinBox;
    QGroupBox *JpegGroupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *lumQualitySpinBox;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_8;
    QSpinBox *chromQualitySpinBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_7;
    QSpinBox *blockSizeSpinBox;
    QLabel *label_5;
    QSpinBox *rowsNumSpinBox;
    QSpinBox *rowsDenomSpinBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QStringLiteral("SettingsDialog"));
        SettingsDialog->resize(490, 380);
        gridLayout = new QGridLayout(SettingsDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 2);

        BCHGroupBox = new QGroupBox(SettingsDialog);
        BCHGroupBox->setObjectName(QStringLiteral("BCHGroupBox"));
        presetsComboBox = new QComboBox(BCHGroupBox);
        presetsComboBox->setObjectName(QStringLiteral("presetsComboBox"));
        presetsComboBox->setGeometry(QRect(70, 20, 111, 23));
        presetsComboBox->setEditable(false);
        presetsComboBox->setFrame(true);
        label_2 = new QLabel(BCHGroupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 20, 57, 23));
        QFont font;
        font.setPointSize(10);
        label_2->setFont(font);
        mSpinBox = new QSpinBox(BCHGroupBox);
        mSpinBox->setObjectName(QStringLiteral("mSpinBox"));
        mSpinBox->setGeometry(QRect(30, 50, 47, 23));
        mSpinBox->setMinimum(0);
        mSpinBox->setMaximum(32768);
        tSpinBox = new QSpinBox(BCHGroupBox);
        tSpinBox->setObjectName(QStringLiteral("tSpinBox"));
        tSpinBox->setGeometry(QRect(106, 50, 71, 23));
        tSpinBox->setMaximum(32768);
        label_3 = new QLabel(BCHGroupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 50, 20, 23));
        label_3->setFont(font);
        label_4 = new QLabel(BCHGroupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(90, 50, 20, 23));
        label_4->setFont(font);
        bchInfoLabel = new QLabel(BCHGroupBox);
        bchInfoLabel->setObjectName(QStringLiteral("bchInfoLabel"));
        bchInfoLabel->setGeometry(QRect(10, 80, 221, 81));

        gridLayout->addWidget(BCHGroupBox, 0, 1, 1, 1);

        ResolutionGroupBox = new QGroupBox(SettingsDialog);
        ResolutionGroupBox->setObjectName(QStringLiteral("ResolutionGroupBox"));
        verticalLayout = new QVBoxLayout(ResolutionGroupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_9 = new QLabel(ResolutionGroupBox);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_4->addWidget(label_9);

        widthSpinBox = new QSpinBox(ResolutionGroupBox);
        widthSpinBox->setObjectName(QStringLiteral("widthSpinBox"));
        widthSpinBox->setMinimum(1);
        widthSpinBox->setMaximum(1280);

        horizontalLayout_4->addWidget(widthSpinBox);

        horizontalLayout_4->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_10 = new QLabel(ResolutionGroupBox);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_5->addWidget(label_10);

        heightSpinBox = new QSpinBox(ResolutionGroupBox);
        heightSpinBox->setObjectName(QStringLiteral("heightSpinBox"));
        heightSpinBox->setMinimum(1);
        heightSpinBox->setMaximum(720);

        horizontalLayout_5->addWidget(heightSpinBox);

        horizontalLayout_5->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_5);


        gridLayout->addWidget(ResolutionGroupBox, 1, 1, 1, 1);

        JpegGroupBox = new QGroupBox(SettingsDialog);
        JpegGroupBox->setObjectName(QStringLiteral("JpegGroupBox"));
        verticalLayout_2 = new QVBoxLayout(JpegGroupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(JpegGroupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);

        horizontalLayout->addWidget(label);

        lumQualitySpinBox = new QSpinBox(JpegGroupBox);
        lumQualitySpinBox->setObjectName(QStringLiteral("lumQualitySpinBox"));
        lumQualitySpinBox->setMinimum(1);
        lumQualitySpinBox->setMaximum(100);

        horizontalLayout->addWidget(lumQualitySpinBox);

        horizontalLayout->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_8 = new QLabel(JpegGroupBox);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_3->addWidget(label_8);

        chromQualitySpinBox = new QSpinBox(JpegGroupBox);
        chromQualitySpinBox->setObjectName(QStringLiteral("chromQualitySpinBox"));
        chromQualitySpinBox->setMinimum(1);
        chromQualitySpinBox->setMaximum(100);

        horizontalLayout_3->addWidget(chromQualitySpinBox);

        horizontalLayout_3->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_7 = new QLabel(JpegGroupBox);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_2->addWidget(label_7);

        blockSizeSpinBox = new QSpinBox(JpegGroupBox);
        blockSizeSpinBox->setObjectName(QStringLiteral("blockSizeSpinBox"));
        blockSizeSpinBox->setMinimum(1);
        blockSizeSpinBox->setMaximum(3600);

        horizontalLayout_2->addWidget(blockSizeSpinBox);

        horizontalLayout_2->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout_2);

        label_5 = new QLabel(JpegGroupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setFont(font);

        verticalLayout_2->addWidget(label_5);

        rowsNumSpinBox = new QSpinBox(JpegGroupBox);
        rowsNumSpinBox->setObjectName(QStringLiteral("rowsNumSpinBox"));
        rowsNumSpinBox->setMinimum(1);
        rowsNumSpinBox->setMaximum(20);

        verticalLayout_2->addWidget(rowsNumSpinBox);

        rowsDenomSpinBox = new QSpinBox(JpegGroupBox);
        rowsDenomSpinBox->setObjectName(QStringLiteral("rowsDenomSpinBox"));
        rowsDenomSpinBox->setMinimum(1);
        rowsDenomSpinBox->setMaximum(20);

        verticalLayout_2->addWidget(rowsDenomSpinBox);


        gridLayout->addWidget(JpegGroupBox, 0, 0, 2, 1);

        gridLayout->setRowStretch(0, 1);
        gridLayout->setRowStretch(1, 1);
        gridLayout->setColumnStretch(0, 1);
        gridLayout->setColumnStretch(1, 1);

        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsDialog, SLOT(reject()));

        presetsComboBox->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "Dialog", 0));
        BCHGroupBox->setTitle(QApplication::translate("SettingsDialog", "BCH", 0));
        presetsComboBox->clear();
        presetsComboBox->insertItems(0, QStringList()
         << QApplication::translate("SettingsDialog", "1:5", 0)
         << QApplication::translate("SettingsDialog", "1:1", 0)
         << QApplication::translate("SettingsDialog", "2:1", 0)
         << QApplication::translate("SettingsDialog", "3:1", 0)
         << QApplication::translate("SettingsDialog", "Custom", 0)
        );
        label_2->setText(QApplication::translate("SettingsDialog", "Presets:", 0));
        mSpinBox->setSpecialValueText(QString());
        mSpinBox->setSuffix(QString());
        mSpinBox->setPrefix(QString());
        label_3->setText(QApplication::translate("SettingsDialog", "m:", 0));
        label_4->setText(QApplication::translate("SettingsDialog", "t:", 0));
        bchInfoLabel->setText(QString());
        ResolutionGroupBox->setTitle(QApplication::translate("SettingsDialog", "Downscaling resolution", 0));
        label_9->setText(QApplication::translate("SettingsDialog", "Width:", 0));
        label_10->setText(QApplication::translate("SettingsDialog", "Height:", 0));
        JpegGroupBox->setTitle(QApplication::translate("SettingsDialog", "JPEG", 0));
        label->setText(QApplication::translate("SettingsDialog", "Lum quality:", 0));
        label_8->setText(QApplication::translate("SettingsDialog", "Chrom quality:", 0));
        label_7->setText(QApplication::translate("SettingsDialog", "RST block size:", 0));
        blockSizeSpinBox->setSuffix(QString());
        label_5->setText(QApplication::translate("SettingsDialog", "Row interlace ratio:", 0));
        rowsNumSpinBox->setSuffix(QApplication::translate("SettingsDialog", " rows transmitted", 0));
        rowsNumSpinBox->setPrefix(QString());
        rowsDenomSpinBox->setSuffix(QApplication::translate("SettingsDialog", " rows", 0));
        rowsDenomSpinBox->setPrefix(QApplication::translate("SettingsDialog", "out of every ", 0));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
