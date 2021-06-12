#!/bin/ksh

for net in `ifconfig | sed -ne 's/^\([^ ]*\):.*/\1/p' | grep -v lo`
do
  ifconfig $net | awk '{if ($1=="inet") print "'$net'", $2}'
done
