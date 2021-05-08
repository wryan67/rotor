#!/bin/ksh

ROTOR_PI=pi@rotor2

echo compile
make
[ $? != 0 ] && exit 2

echo copy binary
for FILE in bin/* gtk/*
do 
  scp $FILE $ROTOR_PI:~/bin || exit $?
done

echo set permissions
ssh $ROTOR_PI sudo chmod u+s ~/bin/rotor
ssh $ROTOR_PI sudo chmod g+w ~/bin/rotor 
ssh $ROTOR_PI sudo chown root ~/bin/rotor

echo copy fonts
for FILE in fonts/*
do 
  ssh $ROTOR_PI sudo mkdir -p /usr/share/fonts/truetype/user 
  ssh $ROTOR_PI sudo chmod 777 /usr/share/fonts/truetype/user 
  scp "$FILE" $ROTOR_PI:/usr/share/fonts/truetype/user
done
