/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../VT/branches/separate/separate_n/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[14];
    char stringdata[288];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 9),
QT_MOC_LITERAL(2, 21, 0),
QT_MOC_LITERAL(3, 22, 22),
QT_MOC_LITERAL(4, 45, 21),
QT_MOC_LITERAL(5, 67, 32),
QT_MOC_LITERAL(6, 100, 4),
QT_MOC_LITERAL(7, 105, 28),
QT_MOC_LITERAL(8, 134, 23),
QT_MOC_LITERAL(9, 158, 28),
QT_MOC_LITERAL(10, 187, 7),
QT_MOC_LITERAL(11, 195, 31),
QT_MOC_LITERAL(12, 227, 25),
QT_MOC_LITERAL(13, 253, 33)
    },
    "MainWindow\0drawImage\0\0on_startButton_clicked\0"
    "on_openButton_clicked\0"
    "on_bandwidthSpinBox_valueChanged\0arg1\0"
    "on_errorSpinBox_valueChanged\0"
    "on_recordButton_clicked\0"
    "on_grayscaleCheckBox_clicked\0checked\0"
    "on_breakChannelCheckBox_clicked\0"
    "on_settingsButton_clicked\0"
    "on_grayscaleCheckBox_stateChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08,
       3,    0,   75,    2, 0x08,
       4,    0,   76,    2, 0x08,
       5,    1,   77,    2, 0x08,
       7,    1,   80,    2, 0x08,
       8,    0,   83,    2, 0x08,
       9,    1,   84,    2, 0x08,
      11,    1,   87,    2, 0x08,
      12,    0,   90,    2, 0x08,
       9,    0,   91,    2, 0x08,
      13,    1,   92,    2, 0x08,
       5,    1,   95,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::QString,    6,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->drawImage(); break;
        case 1: _t->on_startButton_clicked(); break;
        case 2: _t->on_openButton_clicked(); break;
        case 3: _t->on_bandwidthSpinBox_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_errorSpinBox_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_recordButton_clicked(); break;
        case 6: _t->on_grayscaleCheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_breakChannelCheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->on_settingsButton_clicked(); break;
        case 9: _t->on_grayscaleCheckBox_clicked(); break;
        case 10: _t->on_grayscaleCheckBox_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->on_bandwidthSpinBox_valueChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
