#!/bin/ksh

ROTOR_USER=`whoami`

sudo chown root /home/$ROTOR_USER/bin/rotor
sudo chmod g+w  /home/$ROTOR_USER/bin/rotor 
sudo chmod u+s  /home/$ROTOR_USER/bin/rotor
sudo chmod a+x  /home/$ROTOR_USER/bin/rotor

TTFOLDER=/usr/share/fonts/truetype/user 
sudo mkdir -p  $TTFOLDER
sudo chmod 777 $TTFOLDER
cp /home/`whoami`/bin/fonts/* $TTFOLDER


