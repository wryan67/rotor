#!/bin/ksh

sudo iwlist wlan0 scan 2>/dev/null | \
gawk '{
    if ($1=="Cell" && ssid!="") {
       print str "þ" ssid "þ" freq, hz
       ssid=""
       freq=""
       hz=""
    } 
    
    if (match($0, /ESSID:"(.*)"/, a)>0) {
      if (!/x00.x00/) { 
        ssid=a[1]
      }
    }
    if (match($0, /Frequency: *(.*) ([a-zA-Z]Hz)/, a)>0) {
      freq=sprintf("%.1f", a[1]);
      hz=a[2]
    }
    if (match($0, /Signal level=(.*) dBm/, a)>0) {
      str=a[1]
    }
    }END{
       if (ssid!="") {
         print str "þ" ssid "þ" freq, hz
       }
    }' | sort -t, -rn 2> /dev/null

