#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <log4pi.h>

#include "engine.h"

using namespace std;
using namespace common::utility;
using namespace common::synchronized;

enum RotorPin {
    BrakePin=28,
    ClockwisePin=27,
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
    digitalWrite(ClockwisePin, LOW);
    digitalWrite(CCWPin, LOW);
    delay(5000);
    digitalWrite(BrakePin, LOW);
}

void activateRotor(float direction) {
    
    if (direction=0) {
        return;
    }
    RotorPin motorPin=(direction>0)?ClockwisePin:CCWPin;

    digitalWrite(BrakePin, HIGH);
    delay(10);
    digitalWrite(motorPin, HIGH);
}
