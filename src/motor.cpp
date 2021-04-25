#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <log4pi.h>

#include "motor.h"

#define RELAY_DEACTIVATED 1
#define RELAY_ACTIVATED   0

using namespace std;
using namespace common::utility;
using namespace common::synchronized;

    
enum RotorPin {
    isMotorReadyPin=26,
    ClockwisePin=27,
    BrakePin=28,
    CCWPin=29,
    ExternalPower=25
};

powerType power = {
    -1, 1, 0
};

int powerSetting = power.automatic;


static Logger logger{"RotorMotor"};

static int   brakeStatus=-1;
static int64_t          startTime=-1;
static SynchronizedBool _isRotorMoving{false};
static bool _isRotorReallyMoving=false;

static float motorDirection=0;

int initRotorMotor() {
    logger.info("initializing motor");
    logger.debug("GPIO WiringPi motor pins:");
    logger.debug("    isMotorReadyPin:  %2d", isMotorReadyPin);
    logger.debug("    ExternalPower:    %2d", ExternalPower);
    logger.debug("    CW:               %2d", ClockwisePin);
    logger.debug("    CCW:              %2d", CCWPin);
    logger.debug("    BrakePin:         %2d", BrakePin);

    initPins();

    pinMode(isMotorReadyPin,  INPUT);
    pinMode(BrakePin,         OUTPUT);
    pinMode(ClockwisePin,     OUTPUT);
    pinMode(CCWPin,           OUTPUT);
    pinMode(ExternalPower,    OUTPUT);

    initPins();
    externalPowerActivation(true);
    return 0;
}

void initPins() {
    digitalWrite(ClockwisePin,  RELAY_DEACTIVATED);
    digitalWrite(CCWPin,        RELAY_DEACTIVATED);
    digitalWrite(BrakePin,      RELAY_DEACTIVATED);
    digitalWrite(ExternalPower, RELAY_DEACTIVATED);
}


void externalPowerActivation(bool enable) {
    digitalWrite(ExternalPower, (enable)?RELAY_ACTIVATED:RELAY_DEACTIVATED);
}

 bool isRotorMotorReady() {
     return digitalRead(isMotorReadyPin);
 }

bool isRotorMoving() {
    return _isRotorMoving.get();
}

bool isRotorReallyMoving() {
    return _isRotorReallyMoving;
}

void deactivateRotor() {
    digitalWrite(ClockwisePin,  RELAY_DEACTIVATED);
    digitalWrite(CCWPin,        RELAY_DEACTIVATED);
    logger.debug("deactivating rotor motor");

    auto now = currentTimeMillis();
    long travelTime = now - startTime;
    if (startTime<0) {
        travelTime=0;
    }

    uint parkingDelay = 5000;

    if (travelTime<1000) {
        parkingDelay = 1000;
    }    

    _isRotorReallyMoving=false;

    logger.debug("parking delay=%u", parkingDelay);
    delay(parkingDelay);

    logger.debug("parking brake engaged");
    digitalWrite(BrakePin,      RELAY_DEACTIVATED);
    brakeStatus=RELAY_DEACTIVATED;

    _isRotorMoving.set(false);

    auto end = currentTimeMillis();
    long parkingTime = end - now;
    logger.info("travel elapsed time: %ld; parking time: %ld", travelTime, parkingTime);
}

bool isRotorMovingClockwise() {
    if (_isRotorMoving.get() && motorDirection>0) {
        return true;
    } else {
        return false;
    }
}

bool activateRotor(float direction) {
    motorDirection=direction;
    if (direction==0) {
        logger.error("requested motor move with direction of zero");
        return false;
    }

    if (!_isRotorMoving.commit(false,true)) {
        logger.error("rotor is already moving...");
        return false;
    }


    startTime = currentTimeMillis();

    const char *vector;
    RotorPin motorPin;

    if (direction>0) {
        motorPin=ClockwisePin;
        vector="clockwise";
    } else {
        motorPin=CCWPin;
        vector="counter-clockwise";
    }

    logger.debug("actiating rotor motor %s; pin=%d", vector, motorPin);


    logger.debug("parking brake disengaged");
    digitalWrite(BrakePin, RELAY_ACTIVATED);
    brakeStatus=RELAY_ACTIVATED;
    _isRotorReallyMoving=true;

    delay(100);
    digitalWrite(motorPin, RELAY_ACTIVATED);
    return true;
}

int getBrakeStatus() {
    return brakeStatus;
}
