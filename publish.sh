#!/bin/ksh

ROTOR_PI=wryan@rotor2

make
[ $? != 0 ] && exit 2

for FILE in bin/* gtk/*
do 
  scp $FILE $ROTOR_PI:~/bin || exit $?
done

for FILE in fonts/*
do 
  scp "$FILE" $ROTOR_PI:/usr/share/fonts/truetype/user
done
