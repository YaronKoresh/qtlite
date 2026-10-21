QT += testlib
CONFIG += qt console warn_on testcase
CONFIG -= app_bundle

TEMPLATE = app

# Include paths for source files
INCLUDEPATH += ../src

# Common headers
HEADERS += \
    ../src/core/projectstructs.h \
    ../src/core/pathvalidator.h

isEmpty(TEST) {
    TEST = test_pathvalidator
}

equals(TEST, test_pathvalidator) {
    TARGET = test_pathvalidator
    SOURCES += test_pathvalidator.cpp
}

equals(TEST, test_settingspersistence) {
    TARGET = test_settingspersistence
    SOURCES += test_settingspersistence.cpp
    HEADERS += ../src/core/settingspersistence.h
}

equals(TEST, test_snippetmanager) {
    TARGET = test_snippetmanager
    SOURCES += test_snippetmanager.cpp
    SOURCES += ../src/core/snippetmanager.cpp
    HEADERS += ../src/core/snippetmanager.h
}

equals(TEST, test_codebundler) {
    TARGET = test_codebundler
    SOURCES += test_codebundler.cpp
    SOURCES += ../src/core/codebundler.cpp
    HEADERS += ../src/core/codebundler.h
}

equals(TEST, test_editortabmanager) {
    TARGET = test_editortabmanager
    QT += widgets
    SOURCES += test_editortabmanager.cpp
    SOURCES += ../src/ui/editortabmanager.cpp
    SOURCES += ../src/editor/codeeditor.cpp
    SOURCES += ../src/editor/linenumberarea.cpp
    SOURCES += ../src/highlighting/cpphighlighter.cpp
    SOURCES += ../src/highlighting/pythonhighlighter.cpp
    HEADERS += ../src/ui/editortabmanager.h
    HEADERS += ../src/editor/codeeditor.h
    HEADERS += ../src/editor/linenumberarea.h
    HEADERS += ../src/highlighting/syntaxhighlighterbase.h
    HEADERS += ../src/highlighting/cpphighlighter.h
    HEADERS += ../src/highlighting/pythonhighlighter.h
}

equals(TEST, test_compilationengine) {
    TARGET = test_compilationengine
    QT += widgets
    SOURCES += test_compilationengine.cpp
    SOURCES += ../src/compilation/compilationengine.cpp
    HEADERS += ../src/compilation/compilationengine.h
}

equals(TEST, test_pythonhighlighter) {
    TARGET = test_pythonhighlighter
    QT += widgets
    SOURCES += test_pythonhighlighter.cpp
    SOURCES += ../src/highlighting/pythonhighlighter.cpp
    HEADERS += ../src/highlighting/pythonhighlighter.h
    HEADERS += ../src/highlighting/syntaxhighlighterbase.h
}
