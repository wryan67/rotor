#!/bin/ksh

ROTOR_USER=pi
ROTOR_PI=$ROTOR_USER@rotor2

echo compile
if [ "$1" != "-c" ];then
  make
  [ $? != 0 ] && exit 2
fi

echo copy files
tar cf /tmp/rotor.tar fonts/*.ttf \
                      -C bin . \
                      -C ../gtk `cd gtk; ls *.ui *.css` \
                      -C ../scripts deploy.sh 
                      
scp /tmp/rotor.tar $ROTOR_PI:/tmp/rotor.tar || exit $?

echo set permissions/deploy
ssh $ROTOR_PI "mkdir -p /home/$ROTOR_USER/bin; cd /home/$ROTOR_USER/bin && tar xf /tmp/rotor.tar"
ssh $ROTOR_PI /home/$ROTOR_USER/bin/deploy.sh

