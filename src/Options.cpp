#include "Options.h"

void Options::usage() {
	fprintf(stderr, "usage: rotor [options]\n");
	fprintf(stderr, "  Options:\n");
	fprintf(stderr, "  -h = help\n");
	fprintf(stderr, "  -d = debug\n");
	fprintf(stderr, "  -f = full screen\n");
	fprintf(stderr, "  -c = aspect voltage channel (default=0)\n");
	fprintf(stderr, "  -r = aspect reference voltage channel (default=1)\n");
	fprintf(stderr, "  -v = aspect variable resistor ohms (default=500)\n");
	fprintf(stderr, "  -x = aspect fixed resistor ohms (default=100)\n");

	exit(1);
}

bool Options::commandLineOptions(int argc, char **argv) {
	int c, index;

	const char* shortOptions = "c:dfhr:v:x:";

	static struct option longOptions[] = {
		{"aspectVoltageChannel",            required_argument, NULL, 'c'},
		{"aspectReferenceVoltageChannel",   required_argument, NULL, 'r'},
		{"aspectVariableResistorOhms",      required_argument, NULL, 'v'},
		{"aspectFixedResistorOhms",         required_argument, NULL, 'x'},

		{"debug",       optional_argument, NULL, 'd'},
		{"full screen", optional_argument, NULL, 'f'},
		{"help",        optional_argument, NULL, 'h'},
        {0, 0, 0, 0}
	};

	while ((c = getopt_long(argc, argv, shortOptions, longOptions, &index)) != -1) {
		switch (c) {
        case 'c':
            sscanf(optarg, "%d", &aspectVoltageChannel);
            break;
            
        case 'r':
            sscanf(optarg, "%d", &aspectReferenceVoltageChannel);
            break;

        case 'v':
            sscanf(optarg, "%d", &aspectVariableResistorOhms);
            break;

        case 'x':
            sscanf(optarg, "%d", &aspectFixedResistorOhms);
            break;

		case 'd':
            logLevel = ALL;
			break;

		case 'f':
			fullscreen = true;
			break;

		case 'h':
			usage();
			break;

		case '?':
			if (optopt == 'm' || optopt == 't')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character \\x%x.\n", optopt);

			usage();

		default:
			usage();
		}
	}

	//	for (int index = optind; index < argc; index++)
	//		printf("Non-option argument %s\n", argv[index]);

    logger.setGlobalLevel(options.logLevel);
    if (options.logLevel==ALL) {
        logger.info("debuggin turned on");
    }
	return true;
}



