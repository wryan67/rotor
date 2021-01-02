#!/bin/ksh

ROTOR=192.168.33.166

for FILE in bin/* gtk/*
do 
  scp $FILE pi@$ROTOR:/home/pi/bin || exit $?
done
