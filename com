#!/bin/ksh

g++ `pkg-config --cflags gtk+-3.0` -o rotor rotor.cpp `pkg-config --libs gtk+-3.0` -luuid -llog4pi 
RET=$?

exit $RET
