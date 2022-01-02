QT += core gui widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

HEADERS       = ./headers/imgview.h
#				./headers/SubQDir.hpp
SOURCES       = ./src/imgview.cpp \
				./src/main.cpp
#				./src/SubQDir.cpp

TARGET = ./build/picv
# install
target.path = /home/kchn/.local/bin
INSTALLS += target