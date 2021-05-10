Antenna Rotor Controller
------------------------

### Prerequisites:

System Libraries:

    $ sudo apt install libgtk-3-dev
    $ sudo apt install glade
    $ sudo apt install xdotool

### GIT Hub software:

Pleaase read the installation instructions carefully on each github project.  They're all similar, but non necessarially the same. 

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

      $ mkdir -p /home/pi/bin
      $ cd scripts
      $ cp -p * /home/pi/bin
      $ cd /home/pi/bin
      $ sudo chown root rotor
      $ sudo chmod g+w  rotor
      $ sudo chmod u+s  rotor
      $ mkdir -p ~/.config/lxsession/LXDE-pi/
      $ ln /home/pi/bin/autostart ~/.config/lxsession/LXDE-pi/autostart

### Screenshot:

![screenshot3](https://raw.githubusercontent.com/wryan67/rotor/main/readme/screenshot3.png)
