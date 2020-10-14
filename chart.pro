QT += widgets
requires(qtConfig(filedialog))

HEADERS     = mainwindow.h \
              accessiblepieview.h \
              piemodel.h \
              pieview.h
RESOURCES   = chart.qrc
SOURCES     = main.cpp \
              accessiblepieview.cpp \
              mainwindow.cpp \
              piemodel.cpp \
              pieview.cpp
unix:!mac:!vxworks:!integrity:!haiku:LIBS += -lm

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/itemviews/chart
INSTALLS += target
