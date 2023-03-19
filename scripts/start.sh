#!/bin/ksh
set -a

. $HOME/bin/setenv

getIdle() {
  IDLE=$(top -bn1 | sed -ne 's/^%cpu.*ni, *\(.*\) *id,.*/\1/pi')
  echo "$(date '+%Y%m%d %H:%M:%S') $IDLE" >> $HOME/logs/idletime.log
}

idleCheck() {
  elapsed=0
  getIdle
  while [ $IDLE -lt 90 ];do
    sleep 1
    getIdle
    ((elapsed=elapsed+1))
    if [ $elapsed -gt 90 ];then
      MSG="system too busy during boot" 
      echo "$MSG;elapsed=$elapsed seconds" > $HOME/logs/rotor.log 
      zenity --error --text="$MSG"
      exit 9
    fi
  done
  sleep 5
}


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

idleCheck
set -x
nohup $HOME/bin/rotor -d -x $RESISTOR $FULLSCREEN  $* > $HOME/logs/rotor.log 2>&1 &

chown `whoami` $HOME/logs/rotor.log
