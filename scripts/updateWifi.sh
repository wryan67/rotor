#!/bin/ksh

test -t 0
TERM=$?

set `cat $HOME/.config/rotor/.ssid`

sed -e "s/@SSID@/$1/" -e "s/@PASSWD@/$2/" $HOME/bin/wpa_supplicant.template > /tmp/wpa_supplicant.conf

if [ ! -s /tmp/wpa_supplicant.conf ];then
  [ $TERM = 0 ] && echo failed 
  exit 2
fi

sudo cmp /tmp/wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf 
RET=$?

if [ $RET != 0 ];then
   if [ $TERM = 0 ];then
     echo reconfiguring wifi
     cat /tmp/wpa_supplicant.conf
   fi
   sudo cp /tmp/wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf 
   sudo wpa_cli -i wlan0 reconfigure 
fi

echo --------------------------------------
cat /etc/wpa_supplicant/wpa_supplicant.conf 
