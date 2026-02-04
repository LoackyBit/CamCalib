/****************************************************************************
** Meta object code from reading C++ file 'camcalib.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../camcalib.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camcalib.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASScamcalibENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASScamcalibENDCLASS = QtMocHelpers::stringData(
    "camcalib",
    "selectImg",
    "",
    "viewImg",
    "fileImgCam1",
    "imgund",
    "calibrate"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASScamcalibENDCLASS_t {
    uint offsetsAndSizes[14];
    char stringdata0[9];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[12];
    char stringdata5[7];
    char stringdata6[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASScamcalibENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASScamcalibENDCLASS_t qt_meta_stringdata_CLASScamcalibENDCLASS = {
    {
        QT_MOC_LITERAL(0, 8),  // "camcalib"
        QT_MOC_LITERAL(9, 9),  // "selectImg"
        QT_MOC_LITERAL(19, 0),  // ""
        QT_MOC_LITERAL(20, 7),  // "viewImg"
        QT_MOC_LITERAL(28, 11),  // "fileImgCam1"
        QT_MOC_LITERAL(40, 6),  // "imgund"
        QT_MOC_LITERAL(47, 9)   // "calibrate"
    },
    "camcalib",
    "selectImg",
    "",
    "viewImg",
    "fileImgCam1",
    "imgund",
    "calibrate"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASScamcalibENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x0a,    1 /* Public */,
       3,    1,   39,    2, 0x0a,    2 /* Public */,
       5,    0,   42,    2, 0x0a,    4 /* Public */,
       6,    0,   43,    2, 0x0a,    5 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject camcalib::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASScamcalibENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASScamcalibENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASScamcalibENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<camcalib, std::true_type>,
        // method 'selectImg'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'viewImg'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'imgund'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'calibrate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void camcalib::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<camcalib *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->selectImg(); break;
        case 1: _t->viewImg((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->imgund(); break;
        case 3: _t->calibrate(); break;
        default: ;
        }
    }
}

const QMetaObject *camcalib::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *camcalib::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASScamcalibENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int camcalib::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
