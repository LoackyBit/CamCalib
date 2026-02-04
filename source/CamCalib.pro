QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp camcalib.cpp
HEADERS += camcalib.h
FORMS += camcalib.ui

win32 {
    QT += printsupport
    QT += core gui
    INCLUDEPATH += .
    CONFIG+= c++17 gui console
    DEFINES += EXV_SHARED
    LIBS += -L"C:/opencv-build/install/x64/mingw/bin" -lopencv_core490 -lopencv_highgui490 -lopencv_imgcodecs490 -lopencv_imgproc490 -lopencv_calib3d490
#    LIBS += -L"C:/exiv2-0.28.7/bin" -lexiv2
    LIBS += -L"C:/exiv2-0.28.7/build/lib" -lexiv2
    INCLUDEPATH += C:\opencv-build\install\include\opencv2
    INCLUDEPATH += C:\opencv-build\install\include
    INCLUDEPATH += C:\exiv2-0.28.7\include\exiv2
    INCLUDEPATH += C:\exiv2-0.28.7\build
    INCLUDEPATH += C:\exiv2-0.28.7\include
    TARGET = camcalib
    target.path=.\
    INSTALLS += target
}

unix {
    CONFIG += c++17
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4 exiv2

    # Default rules for deployment.
    qnx: target.path = /tmp/$${TARGET}/bin
    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}
