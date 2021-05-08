#!/bin/ksh
set -a

if [ "$1" != "boot" ];then
  $HOME/bin/stop
fi

mkdir -p $HOME/logs

if [ "$DISPLAY" = "" ];then
  DISPLAY=":0"
fi

cd $HOME/bin

# type "rotor -h" for help

nohup sudo $HOME/bin/rotor -f -x 1000 -w 20 -s 100 $* > $HOME/logs/rotor.log 2>&1 &

chown pi $HOME/logs/rotor.log
