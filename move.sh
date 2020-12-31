#!/bin/ksh

usage() {
  echo usage:  move.sh [cw,ccw] [seconds]
  exit 2
}

if [ "$1" = "" ];then
  usage
fi
# old ccw
# pca9685 -p${PIN} -c45 -f2048

PIN=8

typeset -l DIR=$1

if [ "$DIR" = "cw" ];then
  set -x
  pca9685 -p${PIN} -c1400 -f2500 -s $2 > /dev/null 2>&1
elif [ "$DIR" = "ccw" ];then
  set -x
  pca9685 -p${PIN} -c1400 -f3800 -s $2 > /dev/null 2>&1
else
  usage
fi
#set +x
#sleep $2
#pca9685 -p${PIN} -c1400 -f4000 > /dev/null 2>&1
