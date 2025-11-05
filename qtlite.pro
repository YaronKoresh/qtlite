QT += widgets
TARGET = QtLite
TEMPLATE = app

# Source files
SOURCES += \
    src/main.cpp \
    src/core/snippetmanager.cpp \
    src/core/codebundler.cpp \
    src/editor/codeeditor.cpp \
    src/editor/linenumberarea.cpp \
    src/highlighting/cpphighlighter.cpp \
    src/highlighting/pythonhighlighter.cpp \
    src/compilation/compilationengine.cpp \
    src/ui/uicontroller.cpp \
    src/ui/editortabmanager.cpp \
    src/ui/qtliteapp.cpp

# Header files
HEADERS += \
    src/core/projectstructs.h \
    src/core/pathvalidator.h \
    src/core/settingspersistence.h \
    src/core/snippetmanager.h \
    src/core/codebundler.h \
    src/editor/codeeditor.h \
    src/editor/linenumberarea.h \
    src/highlighting/syntaxhighlighterbase.h \
    src/highlighting/cpphighlighter.h \
    src/highlighting/pythonhighlighter.h \
    src/compilation/compilationengine.h \
    src/ui/uicontroller.h \
    src/ui/editortabmanager.h \
    src/ui/qtliteapp.h
