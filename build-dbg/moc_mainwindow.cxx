/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Tue Nov 12 11:31:21 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      23,   11,   11,   11, 0x08,
      34,   11,   11,   11, 0x08,
      56,   11,   11,   11, 0x08,
      78,   11,   11,   11, 0x08,
      87,   11,   11,   11, 0x08,
     111,   11,   11,   11, 0x08,
     137,   11,   11,   11, 0x08,
     163,   11,   11,   11, 0x08,
     197,   11,   11,   11, 0x08,
     233,   11,   11,   11, 0x08,
     270,   11,   11,   11, 0x08,
     305,   11,   11,   11, 0x08,
     343,   11,   11,   11, 0x08,
     379,   11,   11,   11, 0x08,
     412,   11,   11,   11, 0x08,
     438,   11,   11,   11, 0x08,
     465,   11,   11,   11, 0x08,
     481,   11,   11,   11, 0x08,
     502,   11,   11,   11, 0x08,
     535,   11,   11,   11, 0x08,
     569,   11,   11,   11, 0x08,
     598,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0slotExit()\0updateme()\0"
    "button_Exit_clicked()\0button_Open_clicked()\0"
    "mySlot()\0Buttom_SubVol_clicked()\0"
    "on_actionOpen_triggered()\0"
    "on_actionExit_triggered()\0"
    "on_actionReset_Camera_triggered()\0"
    "on_actionLeft_Side_View_triggered()\0"
    "on_actionRight_Side_View_triggered()\0"
    "on_actionTop_Side_View_triggered()\0"
    "on_actionBottom_Side_View_triggered()\0"
    "on_actionSliceSelection_triggered()\0"
    "on_actionBlack_White_triggered()\0"
    "actionDefault_triggered()\0"
    "actionBlue_Red_triggered()\0slot_LoadFile()\0"
    "slot_Load_Finished()\0"
    "on_actionSubVolumeXY_triggered()\0"
    "on_action_SubVolume_Z_triggered()\0"
    "on_actionPreview_triggered()\0"
    "on_actionSubVol_Export_triggered()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->slotExit(); break;
        case 1: _t->updateme(); break;
        case 2: _t->button_Exit_clicked(); break;
        case 3: _t->button_Open_clicked(); break;
        case 4: _t->mySlot(); break;
        case 5: _t->Buttom_SubVol_clicked(); break;
        case 6: _t->on_actionOpen_triggered(); break;
        case 7: _t->on_actionExit_triggered(); break;
        case 8: _t->on_actionReset_Camera_triggered(); break;
        case 9: _t->on_actionLeft_Side_View_triggered(); break;
        case 10: _t->on_actionRight_Side_View_triggered(); break;
        case 11: _t->on_actionTop_Side_View_triggered(); break;
        case 12: _t->on_actionBottom_Side_View_triggered(); break;
        case 13: _t->on_actionSliceSelection_triggered(); break;
        case 14: _t->on_actionBlack_White_triggered(); break;
        case 15: _t->actionDefault_triggered(); break;
        case 16: _t->actionBlue_Red_triggered(); break;
        case 17: _t->slot_LoadFile(); break;
        case 18: _t->slot_Load_Finished(); break;
        case 19: _t->on_actionSubVolumeXY_triggered(); break;
        case 20: _t->on_action_SubVolume_Z_triggered(); break;
        case 21: _t->on_actionPreview_triggered(); break;
        case 22: _t->on_actionSubVol_Export_triggered(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE