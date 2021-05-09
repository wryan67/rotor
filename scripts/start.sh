#!/bin/ksh
set -a

. $HOME/bin/setenv

RESISTOR=1375

if [ "$1" != "boot" ];then
  $HOME/bin/stop.sh
fi

mkdir -p $HOME/logs

if [ "$DISPLAY" = "" ];then
  DISPLAY=":0"
fi

cd $HOME/bin

# type "rotor -h" for help

echo CW=$CW

set -x
nohup $HOME/bin/rotor -d -f -x $RESISTOR  $* > $HOME/logs/rotor.log 2>&1 &

chown `logname` $HOME/logs/rotor.log
