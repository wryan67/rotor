Antenna Rotor Controller
------------------------
This program is designed to work with any rotor that uses a variable resistor for it's aspect indicator and limit swithes to prevent turning past the starting/stopping point.  Checkout the readme folder for display pinouts and a [[circuit diagram]](https://raw.githubusercontent.com/wryan67/rotor/main/readme/rotor.png) for the system.

The rotor tested with this software utilizes a 500Ω variable resistor which looks like this:  [[variable resistor]](https://raw.githubusercontent.com/wryan67/rotor/main/readme/aspect%20indicator.jpg)


### Prerequisites:

System Libraries:

    $ sudo apt install libgtk-3-dev
    $ sudo apt install glade
    $ sudo apt install xdotool

## Requirements

Overclocking the I2C bus is required to achive the desired 2500 samples per second.  Edit the /boot/config.txt file using:

    $ sudo vi /boot/config.txt 

Next, look for the i2c_arm parameter.  Note that if you've already enable the i2c bus via raspi-config, then you may have multiple lines in the config.txt file that have i2c_arm.  Look for the one that is uncommented. 

    #dtparam=i2c_arm=on
    - or -
    dtparam=i2c_arm=on

Change it to:

    dtparam=i2c_arm=on,i2c_arm_baudrate=400000

## Disable warning lights
While you're in the boot config, you may want to disable the power indicator and lightning bolt indicator.  These indicators do not work properly due to an under sized capacitor on the RPi.  Newer models have larger capacitors, so if you have a brand new RPi 4, this step may not be necessary, but for the older RPi 4's, this indicator is extremely annotying. 

    # only affects power led operation
    dtparam=pwr_led_trigger=none
    dtparam=pwr_led_activelow=off
    # affects lightning bolt indicator
    avoid_warnings=1




### Required GIT Hub software:

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

![screenshot3](https://raw.githubusercontent.com/wryan67/rotor/main/readme/screenshot4.png)
