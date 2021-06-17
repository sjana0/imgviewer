QT += core gui widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

HEADERS       = ./src/imageviewer.h
SOURCES       = ./src/imageviewer.cpp \
                ./src/main.cpp

TARGET = imageviewer
# install
target.path = $$[OUT_PWD]/build/
INSTALLS += target