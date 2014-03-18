/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *image_corrupt;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *openButton;
    QPushButton *settingsButton;
    QPushButton *startButton;
    QSpinBox *errorSpinBox;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *mode1_radioButton;
    QRadioButton *mode2_radioButton;
    QCheckBox *breakChannelCheckBox;
    QCheckBox *grayscaleCheckBox;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *infoLabel1;
    QLabel *infoLabel2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(893, 523);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
        image_corrupt = new QLabel(centralWidget);
        image_corrupt->setObjectName(QStringLiteral("image_corrupt"));
        image_corrupt->setMaximumSize(QSize(873, 453));
        image_corrupt->setScaledContents(true);

        horizontalLayout->addWidget(image_corrupt);

        horizontalLayout->setStretch(0, 1);

        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 9, -1, -1);
        openButton = new QPushButton(centralWidget);
        openButton->setObjectName(QStringLiteral("openButton"));

        horizontalLayout_2->addWidget(openButton);

        settingsButton = new QPushButton(centralWidget);
        settingsButton->setObjectName(QStringLiteral("settingsButton"));

        horizontalLayout_2->addWidget(settingsButton);

        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QStringLiteral("startButton"));

        horizontalLayout_2->addWidget(startButton);

        errorSpinBox = new QSpinBox(centralWidget);
        errorSpinBox->setObjectName(QStringLiteral("errorSpinBox"));
        errorSpinBox->setMaximum(15);

        horizontalLayout_2->addWidget(errorSpinBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        mode1_radioButton = new QRadioButton(centralWidget);
        mode1_radioButton->setObjectName(QStringLiteral("mode1_radioButton"));
        mode1_radioButton->setChecked(true);

        horizontalLayout_3->addWidget(mode1_radioButton);

        mode2_radioButton = new QRadioButton(centralWidget);
        mode2_radioButton->setObjectName(QStringLiteral("mode2_radioButton"));

        horizontalLayout_3->addWidget(mode2_radioButton);

        breakChannelCheckBox = new QCheckBox(centralWidget);
        breakChannelCheckBox->setObjectName(QStringLiteral("breakChannelCheckBox"));

        horizontalLayout_3->addWidget(breakChannelCheckBox);

        grayscaleCheckBox = new QCheckBox(centralWidget);
        grayscaleCheckBox->setObjectName(QStringLiteral("grayscaleCheckBox"));

        horizontalLayout_3->addWidget(grayscaleCheckBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        infoLabel1 = new QLabel(centralWidget);
        infoLabel1->setObjectName(QStringLiteral("infoLabel1"));
        infoLabel1->setMinimumSize(QSize(0, 0));

        horizontalLayout_4->addWidget(infoLabel1);

        infoLabel2 = new QLabel(centralWidget);
        infoLabel2->setObjectName(QStringLiteral("infoLabel2"));

        horizontalLayout_4->addWidget(infoLabel2);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalLayout->setStretch(0, 1);
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        image_corrupt->setText(QString());
        openButton->setText(QApplication::translate("MainWindow", "Open Video...", 0));
        settingsButton->setText(QApplication::translate("MainWindow", "Settings", 0));
        startButton->setText(QApplication::translate("MainWindow", "Start", 0));
        errorSpinBox->setSuffix(QApplication::translate("MainWindow", "% errors", 0));
        mode1_radioButton->setText(QApplication::translate("MainWindow", "Mode 1", 0));
        mode2_radioButton->setText(QApplication::translate("MainWindow", "Mode 2", 0));
        breakChannelCheckBox->setText(QApplication::translate("MainWindow", "Break channel", 0));
        grayscaleCheckBox->setText(QApplication::translate("MainWindow", "Grayscale", 0));
        infoLabel1->setText(QString());
        infoLabel2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
