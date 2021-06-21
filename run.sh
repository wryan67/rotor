#!/bin/ksh

sudo chown root bin/rotor
[ $? != 0 ] && exit 2

sudo chmod g+w  bin/rotor 
[ $? != 0 ] && exit 2

sudo chmod u+s  bin/rotor
[ $? != 0 ] && exit 2

cd gtk
../bin/rotor
