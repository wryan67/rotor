#!/bin/ksh
PATH=/home/pi/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/local/games:/usr/games

IPADDR=$(hostname -I)
HOSTNAME=$(hostname -s)

while [ "$IPADDR" = "" -o "$HOSTNAME" = "raspberrypi" ];do
  sleep 1
  IPADDR=$(hostname -I)
done

/home/pi/bin/ipAddr
