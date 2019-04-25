QT += widgets \
    charts

LIBS += -lboost_system -lboost_filesystem
INCLUDEPATH += /usr/inlude/boost

CONFIG += console

SOURCES += \
    src/main.cpp \
    src/crde_main_window.cpp \
    src/crde_credits_view.cpp \
    src/crde_credits_timeline.cpp

HEADERS += \
    src/crde_main_window.h \
    src/crde_credits_view.h \
    src/crde_credits_timeline.h \

