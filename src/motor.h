
int  initRotorMotor();
bool isRotorMoving();
bool isRotorReallyMoving();
bool activateRotor(float direction);
void deactivateRotor();

bool isRotorMovingClockwise();
int  getBrakeStatus();
void initPins();

struct powerType {
  int automatic,  on, off;
};

extern int powerSetting;
extern powerType power;


 
