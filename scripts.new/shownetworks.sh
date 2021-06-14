#!/bin/ksh

sudo iwlist wlan0 scan 2>/dev/null | \
gawk '{
    if ($1=="Cell" && ssid!="") {
       print str "," ssid
       ssid=""
    } 
    
    if (match($0, /ESSID:"(.*)"/, a)>0) {
      if (!/x00.x00/) { 
        ssid=a[1]
      }
    }
    if (match($0, /Signal level=(.*) dBm/, a)>0) {
      str=a[1]
    }
    }END{
       if (ssid!="") {
         print str "," ssid
       }
    }' | sort -t, -rn 2> /dev/null

