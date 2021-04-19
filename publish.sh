#!/bin/ksh

ROTOR_PI=rotor

make
[ $? != 0 ] && exit 2

for FILE in bin/* gtk/*
do 
  scp $FILE pi@$ROTOR_PI:/home/pi/bin || exit $?
done

for FILE in fonts/*
do 
  scp "$FILE" $ROTOR_PI:/usr/share/fonts/truetype/user
done
