QT       += core gui widgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    logindialog.cpp \
    registerdialog.cpp \
    userdashboard.cpp \
    librariandashboard.cpp \
    bookmanagementdialog.cpp \
    tablemanagementdialog.cpp \
    usermanagementdialog.cpp \
    borrowdialog.cpp \
    returndialog.cpp \
    reservationdialog.cpp \
    membershipdialog.cpp \
    searchdialog.cpp \
    librarycore.cpp

HEADERS += \
    mainwindow.h \
    logindialog.h \
    registerdialog.h \
    userdashboard.h \
    librariandashboard.h \
    bookmanagementdialog.h \
    tablemanagementdialog.h \
    usermanagementdialog.h \
    borrowdialog.h \
    returndialog.h \
    reservationdialog.h \
    membershipdialog.h \
    searchdialog.h \
    librarycore.h

FORMS += \
    mainwindow.ui \
    logindialog.ui \
    registerdialog.ui \
    userdashboard.ui \
    librariandashboard.ui \
    bookmanagementdialog.ui \
    tablemanagementdialog.ui \
    usermanagementdialog.ui \
    borrowdialog.ui \
    returndialog.ui \
    reservationdialog.ui \
    membershipdialog.ui \
    searchdialog.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
