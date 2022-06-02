INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xabstracttableview.h \
    $$PWD/xdevicetableeditview.h \
    $$PWD/xdevicetableview.h

SOURCES += \
    $$PWD/xabstracttableview.cpp \
    $$PWD/xdevicetableeditview.cpp \
    $$PWD/xdevicetableview.cpp

!contains(XCONFIG, dialogtextinfo) {
    XCONFIG += dialogtextinfo
    include($$PWD/../FormatDialogs/dialogtextinfo.pri)
}

!contains(XCONFIG, xshortcuts) {
    XCONFIG += xshortcuts
    include($$PWD/../XShortcuts/xshortcuts.pri)
}

!contains(XCONFIG, xinfodb) {
    XCONFIG += xinfodb
    include($$PWD/../XInfoDB/xinfodb.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xabstracttableview.cmake
