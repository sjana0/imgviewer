QT += core gui widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

HEADERS       = ./headers/imgview.h
SOURCES       = ./src/imgview.cpp \
				./src/main.cpp

TARGET = ./build/picv
# install
target.path = /home/kchn/.local/bin
INSTALLS += target