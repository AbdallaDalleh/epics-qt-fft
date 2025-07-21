TEMPLATE  = lib
CONFIG   += plugin
QT       += designer widgets uiplugin charts
TARGET    = fftwidget

DESTDIR          = build-dir
OBJECTS_DIR      = $$DESTDIR
MOC_DIR          = $$DESTDIR
RCC_DIR          = $$DESTDIR
UI_DIR           = $$DESTDIR
QMAKE_DISTCLEAN += -r $$DESTDIR

FORMS += \
    fft_viewer.ui \

HEADERS += \
    fft_viewer_plugin.h \
    fft_viewer.h \
    widgets_collection_plugin.h

SOURCES += \
    fft_viewer_plugin.cpp \
    fft_viewer.cpp \
    widgets_collection_plugin.cpp

headers.path      = $$[QT_INSTALL_HEADERS]/
headers.files     = $$HEADERS
target.path       = $$[QT_INSTALL_PLUGINS]/designer
uninstall.target  = uninstall
distclean.depends = uninstall

INSTALLS            += headers target
QMAKE_EXTRA_TARGETS += uninstall distclean

unix:!macx: LIBS += -L$$(EPICS_BASE)/lib/linux-x86_64/ -lca \
                    -L$$(EPICS_BASE)/lib/linux-x86_64/ -lCom \
                    -lopencv_core

# -L$$(QE_TARGET_DIR)/lib/linux-x86_64/ -lQEFramework \
# -L$$(QE_TARGET_DIR)/lib/linux-x86_64/designer -lQEPlugin \
# -L$$(QWT_ROOT)/lib/ -lqwt \

DEPENDPATH += $$(EPICS_BASE)/include \
              $$(EPICS_BASE)/include/os/Linux

INCLUDEPATH += $$(EPICS_BASE)/include \
               $$(EPICS_BASE)/include/os/Linux \
               $$(EPICS_BASE)/include/compiler/gcc

