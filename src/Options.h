#pragma once

#include <log4pi.h>
#include <stdio.h>
#include <getopt.h>

using namespace std;
using namespace common::utility;
using namespace common::synchronized;

class Options {
private:
    Logger logger{"options"};

public:
    LogLevel logLevel   = INFO;
    bool     fullscreen = false;
    int      a2dDataReady                   = 2;
    int      gain                           = 0;
    int      v3channel                      = 3;
    int      sps                            = 5;
    int      aspectVoltageChannel           = 0;
    int      aspectVariableResistorOhms     = 500;
    int      aspectFixedResistorOhms        = 1100;
    float    aspectSourceVoltage            = 15.0;
    float    zenerDiode                     = 5.1;
    float    zenerDiodeTolerance            = 3;

    float    wobbleLimit                    = 2;      // degrees

    int      ClockwisePin;  
    int      BrakePin;  
    int      CCWPin;      
    int      OutputEnable;
    int      RotorPower;    
    int      LimitSwitch                    = 0;


    char *sampleFileName;
    FILE *sampleFile;

// methods
    void  usage();
    bool  commandLineOptions(int argc, char ** argv);
};

extern Options options;
