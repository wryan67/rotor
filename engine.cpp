#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <log4pi.h>

#include "engine.h"

#define RELAY_DEACTIVATED 1
#define RELAY_ACTIVATED   0

using namespace std;
using namespace common::utility;
using namespace common::synchronized;

enum RotorPin {
    ClockwisePin=27,
    BrakePin=28,
    CCWPin=29
};

static bool debug = false;

static Logger logger{"RotorEngine"};

int initRotorEngine() {
    pinMode(BrakePin,     OUTPUT);
    pinMode(ClockwisePin, OUTPUT);
    pinMode(CCWPin,       OUTPUT);

    deactivateRotor();
    return 0;
}


void deactivateRotor() {
    digitalWrite(ClockwisePin,  RELAY_DEACTIVATED);
    digitalWrite(CCWPin,        RELAY_DEACTIVATED);
    delay(5000);
    digitalWrite(BrakePin,      RELAY_DEACTIVATED);
}

void activateRotor(float direction) {
    
    if (direction=0) {
        return;
    }
    RotorPin motorPin=(direction>0)?ClockwisePin:CCWPin;

    digitalWrite(BrakePin, RELAY_ACTIVATED);
    delay(10);
    digitalWrite(motorPin, RELAY_ACTIVATED);
}
