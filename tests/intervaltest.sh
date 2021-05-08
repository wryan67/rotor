#!/bin/ksh

if [ "$1" = "" ];then
  echo "usage: runtest.sh output_filename"
  exit 2
fi


./vc -g 1 -f 20 > $1 &
VCPID=$!

for i in 0 1 2 3 4 5  
do
  sleep 5
  move.sh cw 5
done
sleep 5

kill -2 $VCPID

