#!/bin/ksh

ROTOR_USER=pi
ROTOR_PI=$ROTOR_USER@rotor2

echo compile
make
[ $? != 0 ] && exit 2

echo copy binary
for FILE in bin/* gtk/*
do 
  ONE=`echo $FILE | cut -c1`
  [ "$ONE" = "#" ] && continue
  scp $FILE $ROTOR_PI:/home/$ROTOR_USER/bin || exit $?
done

echo set permissions
ssh $ROTOR_PI sudo chmod g+w /home/$ROTOR_USER/bin/rotor 
ssh $ROTOR_PI sudo chown root /home/$ROTOR_USER/bin/rotor
ssh $ROTOR_PI sudo chmod u+s /home/$ROTOR_USER/bin/rotor

echo copy fonts
for FILE in fonts/*
do 
  ssh $ROTOR_PI sudo mkdir -p /usr/share/fonts/truetype/user 
  ssh $ROTOR_PI sudo chmod 777 /usr/share/fonts/truetype/user 
  scp "$FILE" $ROTOR_PI:/usr/share/fonts/truetype/user
done
