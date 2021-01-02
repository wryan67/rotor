#include <wiringPi.h>
#include <pca9685.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <log4pi.h>

using namespace std;
using namespace common::utility;
using namespace common::synchronized;


static int pca9685fd = -1;

static int PCA9865_CAP      = 1400;
static int PCA9865_RES      = 4046;
static int PCA9865_ADDRESS  = 0x40;
static int pwmPin           = 8;
static int pwmFreq          = -1;

#define PCA9685_PIN_BASE 200

static bool debug = false;

static Logger logger{"RotorEngine"};

int  initRotorMotor() {
	if (int ret = wiringPiSetup()) {
		logger.error("Wiring Pi setup failed, ret=%d", ret);
		return 9;
	}

    if ((pca9685fd=pca9685Setup(PCA9685_PIN_BASE, PCA9865_ADDRESS, PCA9865_CAP)) <= 0) {
		logger.error("open pca9685 handle failed!");
		return 9;
	}

    return 0;
}

void setFrequency(int pin, int speed) {
	if (speed < 1) {
		speed = 0;
	} else if (speed >= 4096) {
		speed = 4096;
	}
	pwmWrite(PCA9685_PIN_BASE + pin, speed);
	if (debug) {
		printf("finished\n", pin, speed); fflush(stdout);
	}
}

void deactivateRotor() {
    setFrequency(pwmPin, 0);
}

void activateRotor(float direction) {
    setFrequency(pwmPin, 0);
    // if (direction==0) {
    //     return;
    // }

    // int f;
    // if (direction>0) {
    //     f=2500;
    // } else {
    //     f=3800;
    // }

    // setFrequency(pwmPin, f);
    // logger.info("freq=%d",f);
}
