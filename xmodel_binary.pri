INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xmodel_fparts.h \
    $$PWD/xmodel_hex.h \
    $$PWD/xmodel_ximport.h \
    $$PWD/xmodel_xexport.h \
    $$PWD/xmodel_xsymbol.h \
    $$PWD/xmodel_xresource.h

SOURCES += \
    $$PWD/xmodel_fparts.cpp \
    $$PWD/xmodel_hex.cpp \
    $$PWD/xmodel_ximport.cpp \
    $$PWD/xmodel_xexport.cpp \
    $$PWD/xmodel_xsymbol.cpp \
    $$PWD/xmodel_xresource.cpp

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xmodel_binary.cmake
