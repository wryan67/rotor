#!/bin/ksh

ROTOR_USER=pi
ROTOR_HOST=rotor3
ROTOR_PI=$ROTOR_USER@$ROTOR_HOST

if [ "$1" != "-c" ];then
  echo compile
  make
  [ $? != 0 ] && exit 2
fi

echo copy files
tar cf /tmp/rotor.tar fonts/*.ttf \
                      -C bin . \
                      -C ../gtk `cd gtk; ls *.ui *.css` \
                      -C ../scripts deploy.sh 
                      
if [ "$ROTOR_HOST" = "localhost" ];then
  chmod a+r /tmp/rotor.tar
else
  scp /tmp/rotor.tar $ROTOR_PI:/tmp/rotor.tar || exit $?
fi

echo extract
ssh $ROTOR_PI "mkdir -p /home/$ROTOR_USER/bin; cd /home/$ROTOR_USER/bin && tar xf /tmp/rotor.tar"
echo deploy
ssh $ROTOR_PI /home/$ROTOR_USER/bin/deploy.sh

