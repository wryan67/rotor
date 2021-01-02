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

    char *sampleFileName;
    FILE *sampleFile;


// methods
    void  usage();
    bool  commandLineOptions(int argc, char ** argv);
};

extern Options options;