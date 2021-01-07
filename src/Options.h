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
    int      aspectVoltageChannel           = 0;
    int      aspectReferenceVoltageChannel  = 1;
    int      aspectVariableResistorOhms     = 500;
    int      aspectFixedResistorOhms        = 100;

    float    rotorVcc                       = 5.0;
    bool     useAspectReferenceVoltageChannel=false;

    char *sampleFileName;
    FILE *sampleFile;

// methods
    void  usage();
    bool  commandLineOptions(int argc, char ** argv);
};

extern Options options;