QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

INCLUDEPATH += C:/SFML-2.6.1/include


LIBS += -LC:/SFML-2.6.1/lib \
    -lsfml-audio \
    -lsfml-system-s \
    -lsfml-window-s \
    -lsfml-graphics-s \
    -lsfml-network-s

LIBS += -lopengl32 \
        -lgdi32 \
        -lwinmm \
        -lws2_32

CONFIG += c++17
CONFIG += static

DEFINES += SFML_STATIC
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controller.cpp \
    main.cpp \
    mainwindow.cpp \
    pathdata.cpp \
    qcustomplot.cpp \
    selectedsoundkeyfilter.cpp \
    soundfile.cpp \
    soundutils.cpp \
    waveform.cpp

HEADERS += \
    controller.h \
    mainwindow.h \
    pathData.h \
    qcustomplot.h \
    selectedsoundkeyfilter.h \
    soundfile.h \
    soundutils.h \
    wavHeader.h \
    waveform.h

FORMS += \
    mainwindow.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
