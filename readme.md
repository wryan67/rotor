Antenna Rotor Controller
------------------------

### Prerequisites:

System Libraries:

    $ sudo apt install libgtk-3-dev
    $ sudo apt install glade

### GIT Hub software:

* ADS1115 Library - https://github.com/wryan67/ads1115_rpi_lib    

      $ git clone https://github.com/wryan67/ads1115_rpi_lib.git

* NeoPixel Library - https://github.com/wryan67/neopixel_rpi_lib

       $ git clone https://github.com/wryan67/neopixel_rpi_lib.git       

* Log4Pi -https://github.com/wryan67/log4pi

       $ git clone https://github.com/wryan67/log4pi.git

### Download:

    git clone https://github.com/wryan67/rotor.git
    
### Compile:

    $ make
    
### Run:

Follow the instructions for autostart, then setup
    $ . scripts/set
    $ ./bin/rotor
    
### Auto Start on Boot

To start rotor on boot:

* Copy all files from the scripts folder (in this project) to $HOME/bin, then setup lxsession autostart using these commands:

      $ mkdir /home/pi/bin
      $ cd scripts
      $ cp * /home/pi/bin
      $ cd /home/pi/bin
      $ sudo chown root rotor
      $ sudo chmod g+w  rotor
      $ sudo chmod u+s  rotor
      $ mkdir -p ~/.config/lxsession/LXDE-pi/
      $ ln /home/pi/bin/autostart ~/.config/lxsession/LXDE-pi/autostart

### Screenshot:

![screenshot1](https://raw.githubusercontent.com/wryan67/rotor/main/readme/screenshot1.png)
