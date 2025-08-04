QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#    ../widget/fft_viewer.cpp \
#    ../widget/fft_viewer_plugin.cpp \
#    ../widget/widgets_collection_plugin.cpp \
    main.cpp \
    main_window.cpp

HEADERS += \
#    ../widget/fft_viewer.h \
#    ../widget/fft_viewer_plugin.h \
#    ../widget/widgets_collection_plugin.h \
    main_window.h

FORMS += \
#    ../widget/fft_viewer.ui \
    main_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#SUBDIRS += \
#    ../widget/fft-widget.pro

unix:!macx: LIBS += -L$$(EPICS_BASE)/lib/linux-x86_64/ -lca \
                    -L$$(EPICS_BASE)/lib/linux-x86_64/ -lCom \
                    -lopencv_core

DEPENDPATH += $$(EPICS_BASE)/include \
              $$(EPICS_BASE)/include/os/Linux

INCLUDEPATH += $$(EPICS_BASE)/include \
               $$(QWT_ROOT)/include \
               $$(EPICS_BASE)/include/os/Linux \
               $$(EPICS_BASE)/include/compiler/gcc \
