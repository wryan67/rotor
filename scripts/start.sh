#!/bin/ksh
set -a

. $HOME/bin/setenv


if [ "$1" != "boot" ];then
  $HOME/bin/stop.sh
else
  FULLSCREEN="-f"
fi

mkdir -p $HOME/logs

if [ "$DISPLAY" = "" ];then
  DISPLAY=":0"
fi

#xdotool mousemove `fbset -s | sed -ne 's/^mode "\([0-9]*\)x\([0-9]*\)".*/\1 \2/p'`

cd $HOME/bin

# type "rotor -h" for help

echo CW=$CW

set -x
nohup $HOME/bin/rotor -d -x $RESISTOR $FULLSCREEN  $* > $HOME/logs/rotor.log 2>&1 &

chown `logname` $HOME/logs/rotor.log
