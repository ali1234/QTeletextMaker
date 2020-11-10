QT += widgets
requires(qtConfig(filedialog))

HEADERS       = document.h \
                levelonecommands.h \
                levelonepage.h \
                mainwidget.h \
                mainwindow.h \
                pagebase.h \
                pageenhancementsdockwidget.h \
                pageoptionsdockwidget.h \
                palettedockwidget.h \
                render.h \
                x26dockwidget.h \
                x26model.h \
                x26triplets.h
SOURCES       = document.cpp \
                levelonecommands.cpp \
                levelonepage.cpp \
                main.cpp \
                mainwidget.cpp \
                mainwindow.cpp \
                pagebase.cpp \
                pageenhancementsdockwidget.cpp \
                pageoptionsdockwidget.cpp \
                palettedockwidget.cpp \
                render.cpp \
                x26dockwidget.cpp \
                x26model.cpp \
                x26triplets.cpp
RESOURCES     = qteletextmaker.qrc

# install
target.path = /usr/local/bin
INSTALLS += target
