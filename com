#!/bin/ksh

LIBS="-lwiringPi -lwiringPiDev"
g++ `pkg-config --cflags gtk+-3.0` -o rotor rotor.cpp `pkg-config --libs gtk+-3.0` -luuid -llog4pi $LIBS
RET=$?

exit $RET
