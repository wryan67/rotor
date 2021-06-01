#!/bin/ksh

if [ "$1" = "" ];then
  echo "usage: runtest.sh output_filename"
  exit 2
fi


move.sh cw 11 &
MOVEPID=$!

sleep 0.5

./vc -c 0 -g 1 -f 5 -s 10 > $1 

wait $MOVEPID
sleep 0.1

move.sh ccw 11

echo reccnt: `cat $1 | wc -l`
