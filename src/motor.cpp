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
};

static Logger logger{"RotorMotor"};

static int64_t          startTime;
static SynchronizedBool _isRotorMoving{false};

int initRotorMotor() {
    logger.info("initializing motor");
    logger.debug("GPIO WiringPi motor pins:");
    logger.debug("    isMotorReadyPin:  %2d", isMotorReadyPin);
    logger.debug("    CW:               %2d", ClockwisePin);
    logger.debug("    CCW:              %2d", CCWPin);
    logger.debug("    BrakePin:         %2d", BrakePin);

    pinMode(isMotorReadyPin,  INPUT);
    pinMode(BrakePin,         OUTPUT);
    pinMode(ClockwisePin,     OUTPUT);
    pinMode(CCWPin,           OUTPUT);

    deactivateRotor();
    return 0;
}

bool isRotorMotorReady() {
    return digitalRead(isMotorReadyPin);
}

bool isRotorMoving() {
    return _isRotorMoving.get();
}

void deactivateRotor() {
    auto now = currentTimeMillis();
    long travelTime = now - startTime;

    uint parkingDelay = 5000;

    if (travelTime<1000) {
        parkingDelay = 1000;
    }    

    digitalWrite(ClockwisePin,  RELAY_DEACTIVATED);
    digitalWrite(CCWPin,        RELAY_DEACTIVATED);

    delay(parkingDelay);
    digitalWrite(BrakePin,      RELAY_DEACTIVATED);

    _isRotorMoving.set(false);

    auto end = currentTimeMillis();
    auto parkingTime = end - now;
    logger.info("travel elapsed time: %ld; parking time: %ld", travelTime, parkingTime);
}

bool activateRotor(float direction) {

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

    logger.debug("moving %s; pin=%d", vector, motorPin);
    digitalWrite(BrakePin, RELAY_ACTIVATED);
    delay(10);
    digitalWrite(motorPin, RELAY_ACTIVATED);
    return true;
}
