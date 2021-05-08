#!/bin/ksh

DURATION=55

if [ "$1" = "" ];then
  echo "usage: runtest.sh output_filename"
  exit 2
fi


./vc -g 1 -f 20 > $1 &
VCPID=$!

sleep 2

move.sh cw $DURATION
sleep 5

move.sh ccw $DURATION
sleep 5

kill -2 $VCPID

