#!/bin/ksh
. setenv

usage() {
  echo "usage: move [cw|ccw] [seconds]"
  echo "note: seconds can be fractional; example: 0.5"
  exit 2
}


typeset -l INPUT=$1
DURATION=$2


[ "$INPUT" = "cw" ]  && DIR=$CW
[ "$INPUT" = "ccw" ] && DIR=$CCW

if [ "$DIR" = "" ];then
  echo "invalid direction"
  usage
fi

if [ "$DURATION" = "" ];then
  echo "seconds must be specified"
  usage
fi


OE=4
#BRAKE=28
#POWER=26
#CCW=27
#CW=29



gpio mode $OE output
gpio write $OE 1

for i in 26 27 28 29 ; do gpio write $i 1 ; done
for i in 26 27 28 29 ; do gpio mode $i output ; done

if [ $1 = "cw" ];then
  gpio write $POWER 0
  sleep 0.050
  gpio write $BRAKE 0
  sleep 0.200
  gpio write $CW 0
  sleep $DURATION
  gpio write $CW 1
else
  gpio write $POWER 0
  sleep 0.050
  gpio write $BRAKE 0
  sleep 0.200
  gpio write $CCW 0
  sleep $DURATION
  gpio write $CCW 1
fi
 

for i in 26 27 28 29 ; do gpio write $i 1 ; done
