#!/bin/ksh
set -a

. $HOME/bin/setenv
[ $? != 0 ] && exit 2

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

releaseBrake

if [ $1 = "cw" ];then
  moveCW
else
  moveCCW
fi
 
sleep $DURATION
engageBrake
