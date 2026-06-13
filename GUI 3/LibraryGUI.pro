QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = LibraryGUI
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/loginwindow.cpp \
    src/registerdialog.cpp \
    src/userdashboard.cpp \
    src/librariandashboard.cpp \
    src/bookmanagerdialog.cpp \
    src/tablemanagerdialog.cpp \
    src/usermanagerdialog.cpp \
    src/reservationdialog.cpp \
    src/library_core.cpp

HEADERS += \
    src/mainwindow.h \
    src/loginwindow.h \
    src/registerdialog.h \
    src/userdashboard.h \
    src/librariandashboard.h \
    src/bookmanagerdialog.h \
    src/tablemanagerdialog.h \
    src/usermanagerdialog.h \
    src/reservationdialog.h \
    src/library_core.h \
    src/styles.h

RESOURCES += resources.qrc
