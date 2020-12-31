#!/bin/ksh

LIBS="-lwiringPi -lwiringPiDev -lwiringPiPca9685 -luuid -llog4pi"
g++ `pkg-config --cflags gtk+-3.0` -o rotor engine.cpp rotor.cpp `pkg-config --libs gtk+-3.0` $LIBS
RET=$?

exit $RET
