#!/bin/ksh

usage() {
  echo usage:  move.sh [cw,ccw] [seconds]
  exit 2
}

if [ "$1" = "" ];then
  usage
fi

PIN=8

typeset -l DIR=$1

if [ "$DIR" = "cw" ];then
  pca9685 -p${PIN} -c1400 -f2500
elif [ "$DIR" = "ccw" ];then
  pca9685 -p${PIN} -c45 -f2048
else
  usage
fi

sleep $2
pca9685 -p${PIN} -c0 -f0
