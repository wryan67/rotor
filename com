#!/bin/ksh

# -lwiringPiPca9685 -lwiringPiDev 

BINARY=rotor
C_FLAGS="-O2 -Wall"
PKG_LIBS=`pkg-config --libs gtk+-3.0`
LIBS="-lwiringPi -lwiringPiADS1115rpi -luuid -llog4pi"
SOURCE=`find src -type f -name "*.cpp"`
PKG_FLAGS=`pkg-config --cflags gtk+-3.0`

rm -rf bin
mkdir bin

g++ $PKG_FLAGS $C_FLAGS $SOURCE $PKG_LIBS $LIBS -o bin/$BINARY
RET=$?

exit $RET
