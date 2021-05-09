#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <log4pi.h>

#include "motor.h"
#include "Options.h"

#define RELAY_DEACTIVATED 1
#define RELAY_ACTIVATED   0

using namespace std;
using namespace common::utility;
using namespace common::synchronized;

    
// enum RotorPin {
//     //                    WPi,    BCM,  HDR
//     options.ClockwisePin=          29, //  21,   40
//     options.BrakePin=              28, //  20,   38
//     options.CCWPin=                27, //  16,   36
//     options.OutputEnable=           4, //  23,   16
//     options.RotorPower=            26  //  12,   32
// };

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
    logger.debug("    OutputEnable:     %2d", options.OutputEnable);
    logger.debug("    RotorPower:       %2d", options.RotorPower);
    logger.debug("    CW:               %2d", options.ClockwisePin);
    logger.debug("    CCW:              %2d", options.CCWPin);
    logger.debug("    BrakePin:         %2d", options.BrakePin);

    initPins();

    pinMode(options.BrakePin,         OUTPUT);
    pinMode(options.ClockwisePin,     OUTPUT);
    pinMode(options.CCWPin,           OUTPUT);
    pinMode(options.OutputEnable,     OUTPUT);
    pinMode(options.RotorPower,       OUTPUT);

    initPins();
    externalPowerActivation(true);
    return 0;
}

void initPins() {
    digitalWrite(options.ClockwisePin,     RELAY_DEACTIVATED);
    digitalWrite(options.CCWPin,           RELAY_DEACTIVATED);
    digitalWrite(options.BrakePin,         RELAY_DEACTIVATED);
    digitalWrite(options.OutputEnable,     1);
    digitalWrite(options.RotorPower,       RELAY_DEACTIVATED);
}


void externalPowerActivation(bool enable) {
    digitalWrite(options.RotorPower, (enable)?RELAY_ACTIVATED:RELAY_DEACTIVATED);
}


bool isRotorMoving() {
    return _isRotorMoving.get();
}

bool isRotorReallyMoving() {
    return _isRotorReallyMoving;
}

void deactivateRotor() {
    digitalWrite(options.ClockwisePin,  RELAY_DEACTIVATED);
    digitalWrite(options.CCWPin,        RELAY_DEACTIVATED);
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
    digitalWrite(options.BrakePin,      RELAY_DEACTIVATED);
    digitalWrite(options.RotorPower,    RELAY_DEACTIVATED);

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
    int motorPin;

    digitalWrite(options.RotorPower, RELAY_ACTIVATED);
    delay(200);

    if (direction>0) {
        motorPin=options.ClockwisePin;
        vector="clockwise";
    } else {
        motorPin=options.CCWPin;
        vector="counter-clockwise";
    }

    logger.debug("actiating rotor motor %s; pin=%d", vector, motorPin);


    logger.debug("parking brake disengaged");
    digitalWrite(options.BrakePin, RELAY_ACTIVATED);
    brakeStatus=RELAY_ACTIVATED;
    _isRotorReallyMoving=true;

    delay(100);
    digitalWrite(motorPin, RELAY_ACTIVATED);
    return true;
}

int getBrakeStatus() {
    return brakeStatus;
}
