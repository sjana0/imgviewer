QT += core gui widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

HEADERS       = ./src/imgview.h
SOURCES       = ./src/imgview.cpp \
                ./src/main.cpp

TARGET = ./build/imgview
# install
target.path = /home/kchn/.local/bin
INSTALLS += target