#!/bin/ksh
PIN=8
pca9685 -p$PIN -c1400 -f0 > /dev/null 2>&1
