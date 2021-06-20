QT += core gui widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

HEADERS       = ./src/imageviewer.h
SOURCES       = ./src/imageviewer.cpp \
                ./src/main.cpp

TARGET = ./build/imageviewer
# install
target.path = /usr/local/bin
INSTALLS += target