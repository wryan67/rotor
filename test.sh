#!/bin/ksh

F=100
while [ $F -lt 4000 ];do
 echo $F
 pca9685 -p8 -c1400 -f $F
 sleep .5
 ((F=F+50))
done
