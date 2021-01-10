#!/bin/ksh

. $HOME/bin/setenv


stopMotors

ps -fu root | awk '{if (/rotor/ && !/awk/ && !/tail/ && !/sudo/) system(sprintf("sudo kill %d", $2))}'

stopMotors
sleep 5
engageBrake
