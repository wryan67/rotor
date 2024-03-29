
#include <gtk/gtk.h>
#include <math.h>
#include <thread>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <ads1115rpi.h>
#include <neopixel.h>  
#include <atomic>
#include <vector>
#include <mutex>
#include <algorithm>
#include <utility>

#include <time.h>
#include <sys/time.h>


#include "motor.h"
#include "Options.h"

using namespace std;
using namespace common::utility;
using namespace common::synchronized;

Options options = Options();
bool debug=false;

char configFolder[2048];

#define BASE 200
#define SPI_CHAN 0
#define MCP3008_SINGLE  8
#define MCP3008_DIFF    0

int screenBreakpoint=320;

bool cwLimitPoint=false;
int  ADS1115_ADDRESS=0x48;
int  a2dHandle=-1;

bool parked=true;
FILE *logFile=nullptr;

float calibrationA;
float calibrationB;
float calibrationC;
  int calibrationEast, calibrationWest;

vector<pair<uint64_t, float>*> points;
bool capturePoints=false;

int screenWidth;
int screenHeight;

bool skipWindow = false;
long long skipTime = 0;

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18   // BCM numbering system
#define DMA                     10   // DMA=Direct Memory Access
int  led_count =                 1;  // number of pixels in your led strip
int  operationIndicator = 0;
int  stopColor    = 0xff0000;
int  movingColor  = 0x00ff00;
int  brakingColor = 0xffff00;


// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

Logger     logger("main");

atomic<bool> stoppingRotor{false};
atomic<bool> isSettingsDialogueActive{false};
atomic<bool> calledHideSettings{false};
atomic<bool> showingRealizedIp{false};
atomic<bool> showingWifiUpdates{false};
atomic<bool> detectingIntenet{false};
bool hasInternet{false};

vector<string> realizedIPs;
vector<string> wifiNetworks;


GtkWidget *drawingArea=nullptr;
GtkWidget *timeWindow=nullptr;
GObject   *mainWindow=nullptr;

GtkLabel  *utcLabel=nullptr;
GtkLabel  *utcTime=nullptr;
GtkLabel  *utcDate=nullptr;

char       utcTimeBuffer[512];
char       utcDateBuffer[512];

GtkLabel  *timeSeparator=nullptr;
GtkLabel  *localLabel=nullptr;
GtkLabel  *localTime=nullptr;
GtkLabel  *localDate=nullptr;

char       localTimeBuffer[512];
char       localDateBuffer[512];

GtkBuilder *settingsBuilder;
GtkWindow  *settingsWindow;
GtkListBox *countryListBox;
GtkListBox *timezoneListBox;
GtkListBox *availableNetworksListBox;
gulong      availableNetworksListBoxSignal;
GtkEntry   *ssidEntry;
GtkEntry   *passwdEntry;
GtkCheckButton *showPasswd;

vector<string> systemCountries;
vector<string> systemTimezones;
vector<string> timezones;
vector<string> countries;
char currTimezone[4096];

float currentVolts=-1;
float rotorDegree=0;
bool  forceCompassRedraw=false;
bool  forceVoltageDebugDisplay=false;

mutex displayLock;
mutex pickSSID;
mutex conversionInProgress;

GtkWidget *degreeInputBox;

static cairo_surface_t *surface = NULL;
static void drawCompass(bool newSurface);
static void createDrawingSurface(GtkWidget *widget);

char degreeTextBox[32];
bool ignoreMargins=false;

int           sampleMode=-1;
unsigned int  windowSize=0;
float         totalSampleVolts=0;
long          totalSamples;
float         lastDisplayVolts=-9;
vector<float> samples;
float         cableDisconnectedVolts;

struct directionalType {
  int east,  se, south,   sw, west,  nw, north, ne;
} directional = {
        90, 135,   180,  225,  270, 315,     0, 45
};

float getDegree(float volts);

float translateRotor2Display(float degrees) {
    if (degrees<180) {
        return degrees+180;
    } else {
        return degrees-180;
    }
}

float translateDisplay2Rotor(float degrees) {
    float workingDegrees=degrees-180;
    if (workingDegrees<0) {
        workingDegrees+=360;
    }    
    return workingDegrees;
}

void getScreenResolution() {
  // old: 480x320
  // new: 800x480


  GdkRectangle workarea = {0};
  gdk_monitor_get_workarea(
  gdk_display_get_primary_monitor(gdk_display_get_default()),
    &workarea);

  screenWidth=workarea.width;
  screenHeight=workarea.height;

  logger.debug("screen width x height: %d x %d", screenWidth, screenHeight);

  if (screenWidth<480 || screenHeight<320) {
    logger.error("minimum screen resolution is 840x320");
    exit(7);
  }
}

void hideMouse() {
  char cmd[2048];
  delay(1000);
  sprintf(cmd,"xdotool mousemove %d %d", screenWidth, screenHeight);
  system(cmd);
}

// float lastVolts=999;
long long lastConversion=-1;

void voltageCatcher() {
  if (!conversionInProgress.try_lock()) {
    return;
  }

  float volts = readVoltage(a2dHandle);
  currentVolts=volts;
  

  switch (sampleMode) {
    case -1: {
      conversionInProgress.unlock();
      return;
    }
    case 0: {
      ++windowSize;
      conversionInProgress.unlock();
      return;
    }
    case 1: {
      totalSampleVolts+=volts;      
      samples.push_back(volts);

      if (samples.size()>=windowSize) {
        ++sampleMode;
      }      
    }
    case 2: {
      if (lastConversion<0) lastConversion=currentTimeMillis();

      if (skipWindow) {
        long long now = currentTimeMillis();
        if (now<skipTime) {
          conversionInProgress.unlock();
          return;
        }
        skipWindow=false;
      }
      
      totalSampleVolts+=volts;      
      samples.push_back(volts);

      auto oldestSample = samples.at(0);
      totalSampleVolts-=oldestSample;
      samples.erase(samples.begin());

      float volts = totalSampleVolts / (float)windowSize;

      float newDegree = getDegree(volts);

      if (abs(newDegree)>360) {
          newDegree=360;
      }
      if (newDegree<0) {
          newDegree=0;
      }
      int lastDegree=rotorDegree;
      rotorDegree=newDegree;

      if (abs(newDegree-lastDegree)>options.wobbleLimit) {
          forceVoltageDebugDisplay=true;
          lastDegree=newDegree;
      }

      if (abs(volts-lastDisplayVolts)>0.05 || forceVoltageDebugDisplay) {
          int limitSwitch = digitalRead(options.LimitSwitch);

          logger.debug("bs=%d ls=%d ch[0]=%.3f i-degree=%.1f d-degree=%.0f gain=%d r1=%d", 
                  getBrakeStatus(), limitSwitch,  volts, newDegree, translateRotor2Display(newDegree), 
                  options.gain, options.aspectFixedResistorOhms);
          lastDisplayVolts=volts;
          forceVoltageDebugDisplay=false;
      }
      conversionInProgress.unlock();
      return;
    }
  }
  conversionInProgress.unlock();

}


atomic<bool> cableDisconnectDialogActive{false};
bool cableMonitorMessageWritten=false;

void resetCableDisconnectWarning() {
  delay(2500);
  cableMonitorMessageWritten=false;
}

int showCableDisconnect(gpointer data) {
  bool expect=false;
  if (!cableDisconnectDialogActive.compare_exchange_strong(expect,true)) {
    return false;
  }

  char message[4096] = "\nThe cable is disconnected";

  if (currentVolts > 5.28) {
    sprintf(message,
              "The cable is disconnected\n" 
              "The ADS1115 chip cannot accept\n" 
              "voltage higher than 5.3 volts.\n"
              "current voltage is %.2g volts", currentVolts
    );
  }

  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget *dialog = gtk_message_dialog_new ((GtkWindow*)mainWindow,
                            flags,
                            GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_CLOSE,
                            message);

  gtk_window_set_title((GtkWindow*)dialog,"cable status");
  gtk_dialog_run((GtkDialog*)dialog);
  gtk_widget_destroy (dialog);
  cableDisconnectDialogActive=false;

  thread(hideMouse).detach();
  thread(resetCableDisconnectWarning).detach();


  return false;
}



void cableMonitor() {

  logger.debug("cable disconnect volts = %f", cableDisconnectedVolts);
  
  while (true) {
    if (currentVolts > cableDisconnectedVolts) {
      if (!cableMonitorMessageWritten && mainWindow!=nullptr) {
        logger.info("cable disconnnected");
        cableMonitorMessageWritten=true;


        g_idle_add(showCableDisconnect, nullptr);

      }
    } else {
      cableMonitorMessageWritten=false;
    }
    delay(100);
  }
}

struct MessageWindowData {
  const char     *title;
  const char     *message;
  bool            terminate;
  GtkApplication *app;
  GtkWindow      *parent;
};



void gtkWindow(gpointer data) {
  MessageWindowData *msg = (MessageWindowData*) data;
 
  GtkWidget *dialog = gtk_message_dialog_new ((GtkWindow*)mainWindow,
      GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_ERROR,
      GTK_BUTTONS_CLOSE,
      msg->message
  );
  if (msg->parent!=nullptr) {
    gtk_window_set_transient_for((GtkWindow*)dialog,msg->parent);
  }
  getScreenResolution();
  thread(hideMouse).detach();

  gtk_window_set_title((GtkWindow*)dialog, msg->title);
  gtk_dialog_run((GtkDialog*)dialog);
  gtk_widget_destroy (dialog);

}

void softError(const char* title, const char *message, GtkWindow* parent) {
  MessageWindowData *data = new MessageWindowData;

  data->message=(char*)message;
  data->title=(char*)title;
  data->terminate=false;
  data->parent=parent;

  gtkWindow(data);
  delete data;
}

void gtkAppWindow(GtkApplication *app, gpointer data) {
  gtk_application_window_new(app);

  gtkWindow(data);
  g_application_quit(G_APPLICATION(app));
}


void applicaitonMessageWindow(MessageWindowData *msg) {

  GtkApplication *app = gtk_application_new ("org.rotor", G_APPLICATION_FLAGS_NONE);
  msg->app=app;

  g_signal_connect    (app, "activate", G_CALLBACK (gtkAppWindow), msg);
  g_application_run   (G_APPLICATION (app), 0, nullptr);
  g_object_unref      (app);

}

void bootError(const char *message) {
  MessageWindowData *error = new MessageWindowData;

  logger.error(message);
  error->message=message;
  error->title="boot failed";
  error->terminate=true;
  error->parent=nullptr;

  applicaitonMessageWindow(error);
  delete error;

  exit(4);
}


void a2dSetup() {
    cableDisconnectedVolts = (options.zenerDiode * (1-(options.zenerDiodeTolerance/100.0)))-0.01;

    a2dHandle = getADS1115Handle(ADS1115_ADDRESS);

    if (a2dHandle<0) {
        fprintf(stderr, "opening ads1115 failed: %s\n", strerror(errno));
        exit(3);
    }
    char *bootOverride = getenv("BOOTOVERRIDE");
    if (bootOverride) {
      return;
    }



    float v3 = readVoltageSingleShot(a2dHandle, options.v3channel, 0);


    if (abs((3.3-v3)/((3.3+v3)/2))>0.10) {
        for (int c=0;c<4;++c) {
            float volts = readVoltageSingleShot(a2dHandle, c, 0);
            logger.info("channel<%d>=%f", c, volts);
        }
        logger.info("a2dHandle=%d; a2d_address=%02x", a2dHandle, ADS1115_ADDRESS);
        logger.info("voltage on channel a%d=%f", options.v3channel, v3);
        logger.error("ads1115 chip is not working");

        char message[2048];
        sprintf(message, "The ADS1115 a2d chip does not appear to be functional on address 0x%02x", 
                      ADS1115_ADDRESS);
     
        bootError(message);
    }

    // options.sps=5;

// 0 : 000 : 8 SPS
// 1 : 001 : 16 SPS
// 2 : 010 : 32 SPS
// 3 : 011 : 64 SPS
// 4 : 100 : 128 SPS (default)
// 5 : 101 : 250 SPS
// 6 : 110 : 475 SPS
// 7 : 111 : 860 SPS

    unsigned int ads1115SPS[8] = { 8, 16, 32, 64, 128, 250, 475, 860 };

    unsigned int expectedSPS = 0.95*ads1115SPS[options.sps];
    logger.info("channel=%d; gain=%d; sps=%d", options.aspectVoltageChannel, options.gain, ads1115SPS[options.sps]);

    int   tries=3;

    wiringPiISR(options.a2dDataReady,INT_EDGE_FALLING, voltageCatcher);
    setADS1115ContinuousMode(a2dHandle, options.aspectVoltageChannel, options.gain, options.sps);

    // float targetPeriods = 3;
    // float sixtyHzPeriod = 1000.0 * ( 1.0 / 60.0); // 1000 ms * 1/60;
    // float targetWindow  = sixtyHzPeriod / 4;  // 60Hz period / 4;  //ms   4.2 ms

    // logger.info("60Hz period = %.1fms; targetPeriods=%.0f; targetWindow=%.1fms", sixtyHzPeriod, targetPeriods, targetWindow);

 
    long long elapsed;
    unsigned int savedSize;

    while (tries-->0) {
      delay(100);

      windowSize=0;
      totalSampleVolts=0;
      totalSamples=0;
      lastDisplayVolts=-9;
      samples.clear();
      forceVoltageDebugDisplay=false;
    
      skipWindow=false;
      ++sampleMode;
      auto startTime = currentTimeMillis();

      usleep(1000*1000);
      savedSize = windowSize;
      elapsed  = currentTimeMillis()-startTime;
      windowSize=20*windowSize/1000;
      ++sampleMode;

      logger.info("samples captured= %d; elapsed=%lld", savedSize, elapsed);
      logger.info("window size = %d", windowSize);


      if (savedSize > expectedSPS) {
        thread(cableMonitor).detach();
        return;
      }
      logger.info("retying...");
      usleep(1000*1000);
    }
    logger.error("Is your i2c bus overclocked?");
    char msg[2048];
    sprintf(msg,"SPS=%d, which is smaller than expected (%d).", savedSize, expectedSPS);
    bootError(msg);
}

int textBoxWidgetUpdate(gpointer data) {
    gtk_entry_set_text(GTK_ENTRY(degreeInputBox), degreeTextBox);
    return 0;
}

bool updateTimeFontSize=false;
PangoFontDescription *timeFont;
PangoFontDescription *dateFont;

int timeWidgetUpdate(gpointer data) {

    if (!updateTimeFontSize && screenHeight>screenBreakpoint) {
      updateTimeFontSize=true;
      timeFont = pango_font_description_new ();
      pango_font_description_set_family(timeFont,"DejaVu Sans Mono");
      pango_font_description_set_weight(timeFont, PANGO_WEIGHT_BOLD);
      pango_font_description_set_size(timeFont,22*PANGO_SCALE);

      dateFont = pango_font_description_new ();
      pango_font_description_set_family(dateFont,"Quicksand Light");
      pango_font_description_set_weight(dateFont, PANGO_WEIGHT_BOLD);
      pango_font_description_set_size(dateFont,18*PANGO_SCALE);


      auto attributes = gtk_label_get_attributes(utcTime);
      auto attr = pango_attr_font_desc_new(timeFont);
      pango_attr_list_change(attributes, attr);

      attributes = gtk_label_get_attributes(localTime);
      attr = pango_attr_font_desc_new(timeFont);
      pango_attr_list_change(attributes, attr);

      attributes = gtk_label_get_attributes(localDate);
      attr = pango_attr_font_desc_new(dateFont);
      pango_attr_list_change(attributes, attr);

      attributes = gtk_label_get_attributes(utcDate);
      attr = pango_attr_font_desc_new(dateFont);
      pango_attr_list_change(attributes, attr);
    }

    gtk_label_set_text(utcTime, utcTimeBuffer);
    gtk_label_set_text(utcDate, utcDateBuffer);
    
    gtk_label_set_text(localTime, localTimeBuffer);
    gtk_label_set_text(localDate, localDateBuffer);

    return 0;
}


void updateTextBox(float degree) {

    sprintf(degreeTextBox,"%.0f", degree);

    g_idle_add(textBoxWidgetUpdate, nullptr);
}

void clearTextBox() {

    degreeTextBox[0]=0;

    g_idle_add(textBoxWidgetUpdate, nullptr);
}


bool hitLimitSwitch() {
    int limitSwitch = digitalRead(options.LimitSwitch);
    if (limitSwitch==1) {
        logger.info("Limit switch triggered");
        forceVoltageDebugDisplay=true;
        return true;
    } else {
        return false;
    }
}


static void stopRotor(float newDegree) {   
    bool expected=false;
    if (!stoppingRotor.compare_exchange_weak(expected, true)) {
      return;
    }

    logger.info("stopping rotor indicator");
    neopixel_setPixel(operationIndicator, brakingColor);
    neopixel_render();
    delay(1);

    deactivateRotor();
    neopixel_setPixel(operationIndicator, stopColor);
    neopixel_render();
    
    if (logFile) fprintf(logFile,"parked\n");

    delay(1000);
    float targetDeviation=rotorDegree-newDegree;

    logger.info("rotor parked; rotorDegree=%3.0f; target deviation=%.1f", 
                    rotorDegree, targetDeviation);
    forceVoltageDebugDisplay=true;
    neopixel_setPixel(operationIndicator, stopColor);
    neopixel_render();

    forceCompassRedraw=true;

    if (logFile) {
        fclose(logFile);
        delay(20);
        logFile=nullptr;
    } 
    parked=true;
    stoppingRotor=false;
    clearTextBox();
}

static void moveRotorWorker(float degrees, float newDegree) {

    parked=false;
    skipWindow=true;
    skipTime=currentTimeMillis()+1000;
    if (!activateRotor(degrees)) {
        skipWindow=false;
        logger.error("failed to start rotor motor");
            return;
    }
    skipTime=currentTimeMillis()+300;
    logger.info("moving %.0f degrees to %.1f", degrees, newDegree);

    neopixel_setPixel(operationIndicator, movingColor);
    neopixel_render();

    usleep(50*1000); // debounce limit switch relay

    if (cwLimitPoint && degrees<0) {
        logger.info("leaving twilight zone");
        delay(3000);
        cwLimitPoint=false;
    }

    if (degrees>0) { // clockwise
        while (rotorDegree<(newDegree-2) && !hitLimitSwitch()) {
            usleep(250);  // 0.25 ms
        }
    } else {    // counter-clockwise
        while (rotorDegree>(newDegree+2) && !hitLimitSwitch()) {
            usleep(250);  // 0.25 ms
        }
    }
    stopRotor(newDegree);

}

static void moveRotor(float degrees) {
    float currentDegree=rotorDegree;
    if (isRotorMoving()) {
        logger.error("rotor is already moving...");
        return;
    }

    // if (!isRotorMotorReady()) {
    //     logger.error("the rotor motor reports 'not ready'.  Is the external turned power on?");
    //     return;
    // }

    auto newDegree=currentDegree+degrees;
    if (newDegree<0) {
        newDegree+=360;
    }
    if (newDegree>=360) {
        newDegree-=360;
    }

    if (abs(newDegree-currentDegree)<1.5) {
        logger.info("current degree=%.1f; requested degree=%.1f", currentDegree, newDegree);
        logger.info("movement degree is too small.  doing nothing");
        return;
    }
    
    thread(moveRotorWorker,degrees,newDegree).detach();
}


static void moveExact(GtkWidget *widget, gpointer data) {
  auto raw = gtk_entry_get_text(GTK_ENTRY(degreeInputBox));

  thread(hideMouse).detach();

  int  len=strlen(raw);
  char s[len+1];
  memset(s,0,len+1);

  bool isAlpha=isalpha(raw[0]);
  char *c=&s[0];

  for (int i=0; i<len; ++i) {
    bool cDigit = isdigit(raw[i]);

    if (isAlpha && isalpha(raw[i])) {
      *c++=(char)tolower(raw[i]);    
      // ++p;
    } else if (!isAlpha && (cDigit || raw[i]=='.')) {
        *c++=(char)raw[i];    
        // ++p;
    }
  }

  float d=999;
  try {

    while (true) {

      if (!strcmp(s,"nw")) { d=directional.nw; break; }
      if (!strcmp(s,"ne")) { d=directional.ne; break; }
      if (!strcmp(s,"se")) { d=directional.se; break; }
      if (!strcmp(s,"sw")) { d=directional.sw; break; }

      if (isAlpha) s[1]=0;

      if (!strcmp(s,"n"))  { d=directional.north; break; }
      if (!strcmp(s,"e"))  { d=directional.east;  break; }
      if (!strcmp(s,"s"))  { d=directional.south; break; }
      if (!strcmp(s,"w"))  { d=directional.west;  break; }


      d=atof(s);
      break;
    }

    if (d==0) {
      while (true) {
        if (!strcmp(s,"0"))    break;
        if (!strcmp(s,"0.0"))  break;
        if (!strcmp(s,"e"))    break;
        if (!strcmp(s,"east")) break;
        throw (runtime_error("bad data"));    
      }
    }

    if (d>=-360 && d<0) {
      d+=360;
    }
    if (d>=0 && d<360) {
      moveRotor(translateDisplay2Rotor(d)-rotorDegree);
      logger.debug("Move to %.1f", d);
      updateTextBox(d);
      return;
    }
    throw (runtime_error("unknown error"));
  } catch (runtime_error &e) {
    logger.error("invalid degree entered in text box %s; caused by %s", raw, e.what());
  } catch (...) {
    logger.error("invalid degree entered in text box %s", raw);
  }
  updateTextBox(translateRotor2Display(rotorDegree));
}


static void moveTenCounterClockwise(GtkWidget *widget, gpointer data) {
  moveRotor(-10);
  thread(hideMouse).detach();
}
static void moveTenClockwise(GtkWidget *widget, gpointer data) {
  moveRotor(10);
  thread(hideMouse).detach();
}



static void moveTo(GtkWidget *widget, gpointer data) {
  int *direction = (int*)data;
  float newDirection=translateDisplay2Rotor(*direction);
  logger.debug("directon=%d", newDirection);

  moveRotor(newDirection-rotorDegree);

  thread(hideMouse).detach();

}


static void drawCompass(bool newSurface) {
   
  cairo_t *cr;
  guint width, height;

  int windowWidth  = gtk_widget_get_allocated_width(drawingArea);
  int windowHeight = gtk_widget_get_allocated_height (drawingArea);

  int marginLeft = gtk_widget_get_margin_left(drawingArea);
  int marginTop  = gtk_widget_get_margin_top(drawingArea);

  width = windowWidth - marginLeft;
  height = windowHeight - marginTop;



  /* Paint to the surface, where we store our state */
  cr = cairo_create(surface);

//  CAIRO_CONTENT_COLOR;
  // GdkRGBA bgColor {
  //   246,245,244,1
  // };
  // auto bgColor = CAIRO_CONTENT_COLOR;

  // gdk_cairo_set_source_rgba(cr, &bgColor);

//  cairo_set_source_rgb (cr, 246/255.0, 245/255.0, 244/255.0);
//   cairo_paint(cr);


// center
  gdouble x=width/2.0-1;
  gdouble y=height/2.0-1;

  int outerRadius = (x<y)?x-3:y-3;
  int radius = outerRadius;
  auto currDegree = rotorDegree;
  auto degree=currDegree+90;
  if (degree<0) {
      degree=360-degree;
  }

//   logger.info("drawCompas::window<%3d,%3d> margin<%3d,%3d> surface<%3d,%3d> r<%d>", 
//       windowWidth, windowHeight, marginLeft, marginTop, width, height, radius);

  cairo_set_source_rgb (cr, 1,1,1); // white
  cairo_arc(cr, x, y, radius, 0,2*M_PI);  
  cairo_fill(cr);

  cairo_set_source_rgb (cr, 0,0,0);  // black
  cairo_arc(cr, x, y, 3, 0,2*M_PI);  // center dot
  cairo_fill(cr);

  int ridge = (x<y)?x:y;

  cairo_arc(cr, x, y, ridge-3, 0,2*M_PI); // outer ridge circle
  cairo_stroke(cr);

  radius-=(ridge*0.05);  // 5%
  double xPoint = radius * cos(degree * M_PI / 180.0);
  double yPoint = radius * sin(degree * M_PI / 180.0);

  double eXPoint = xPoint;
  double eYPoint = yPoint;

  cairo_move_to(cr,x,y);  // center
  cairo_line_to(cr, x+eXPoint, y+eYPoint);
  cairo_close_path (cr);
  cairo_stroke(cr);

  radius-=ridge*0.1;  // - 1%
  degree-=3;
  xPoint = radius * cos(degree * M_PI / 180.0);
  yPoint = radius * sin(degree * M_PI / 180.0);
  cairo_move_to(cr, x+eXPoint, y+eYPoint);
  cairo_line_to(cr, x+xPoint, y+yPoint);
  cairo_close_path (cr);
  cairo_stroke(cr);


  degree+=6;
  xPoint = radius * cos(degree * M_PI / 180.0);
  yPoint = radius * sin(degree * M_PI / 180.0);
  cairo_move_to(cr, x+eXPoint, y+eYPoint);
  cairo_line_to(cr, x+xPoint,  y+yPoint);
  cairo_close_path (cr);
  cairo_stroke(cr);

// degree
  char deg[32];
  auto displayDegree = translateDisplay2Rotor(currDegree);
  sprintf(deg,"%.0f", displayDegree);

  const char *fontFace = "Courier New";
  cairo_text_extents_t extents;

  cairo_select_font_face(cr, fontFace, 
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
  
  
  cairo_set_font_size(cr, outerRadius / 2.5);
  cairo_text_extents(cr, deg, &extents);

  float halfText = extents.width/2;

  cairo_set_source_rgb(cr, 0,0,0);  

  float textY;
  if (currDegree<90 || currDegree>270) {
    textY=y-(outerRadius/3)+(extents.height/2);
  } else {
    textY=y+(outerRadius/3)+(extents.height/2);
  }
  cairo_move_to(cr, x-halfText, textY);
  cairo_show_text(cr, deg);

  cairo_text_extents(cr, "XXX", &extents);
  halfText = extents.width/2;

  int padTop=4;
  int padBottom=padTop;
  int padLeft=6;
  int padRight=padLeft+1;

  cairo_move_to(cr, x-halfText-padLeft,  textY+padBottom);
  cairo_line_to(cr, x+halfText+padRight, textY+padBottom);
  cairo_line_to(cr, x+halfText+padRight, textY-extents.height-padTop);
  cairo_line_to(cr, x-halfText-padLeft,  textY-extents.height-padTop);
  cairo_close_path (cr);
  cairo_stroke(cr);

  cairo_destroy(cr);
//   displayLock.unlock();

}

static void createDrawingSurface(GtkWidget *widget) {
//   displayLock.lock();

  if (surface) {
    cairo_surface_destroy (surface);
  }

  auto width  = gtk_widget_get_allocated_width(widget);
  auto height = gtk_widget_get_allocated_height(widget);
  
  auto window = gtk_widget_get_window(widget);

  gtk_widget_set_margin_top(widget, 0);
  gtk_widget_set_margin_left(widget,  0);



//  logger.info("createDrawingSurface <%3d, %3d>", width, height);
//  CAIRO_CONTENT_COLOR
  surface = gdk_window_create_similar_surface(window, CAIRO_CONTENT_COLOR_ALPHA, width, height);


  drawCompass(true);

  ignoreMargins=false;

}



/* Create a new surface of the appropriate size to store our scribbles */
static gboolean
configure_event_cb (GtkWidget         *widget,
                    GdkEventConfigure *event,
                    gpointer           data)
{

  createDrawingSurface(widget);
  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}

static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
  drawCompass(true);

  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint_with_alpha (cr, 1);

  return FALSE;
}

int compassWidgetUpdate(gpointer data) {
    gtk_widget_queue_draw(drawingArea);
    gtk_widget_show(drawingArea);
    displayLock.unlock();
    return 0;
}

void renderCompass() {
  usleep(3*1000);

  float lastDegree=999;
  while (true) {
    usleep(50*1000);
    auto currDegree=rotorDegree;
    auto delta=abs(currDegree - lastDegree);
    if (delta>options.wobbleLimit || forceCompassRedraw) {
      forceCompassRedraw=false;  
      lastDegree=currDegree;
      try {
        displayLock.lock();
        g_idle_add(compassWidgetUpdate, nullptr);
      } catch (std::exception &e) {
        logger.warn("%s",e.what());
      } catch (...) {
        logger.warn("unhandled exception in renderCompass");
      }
    }
  }
}

gboolean manualScreenRedraw(GtkWidget *widget, GdkEventWindowState *event, gpointer user_data) {

    logger.debug("manual screen redraw");


    if(event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED) {
        logger.info("window maximized");
        ignoreMargins=false;
    }

    return TRUE;
}

void setButton(GtkBuilder *builder, const char*buttonId, const char *action, GCallback callBack, void *dataPointer)
{
  GObject *button = gtk_builder_get_object (builder, buttonId);
  g_signal_connect (button, action, callBack, dataPointer);
}

void setButton(GtkBuilder *builder, const char*buttonId, char *action, GCallback callBack)
{
  GObject *button = gtk_builder_get_object (builder, buttonId);
  g_signal_connect (button, action, callBack, NULL);
}

float getDegree(float volts) {
    float vS = options.aspectSourceVoltage;
    float r1 = options.aspectFixedResistorOhms;
    float r2 = (volts * r1 ) / (vS - volts);
      
    float iDegree =  (r2/options.aspectVariableResistorOhms)*360.0;

    float calibration = 0;

    return calibration + iDegree;
  }


void timeUpdate() { 
  // old: 480x320
  // new: 800x480

  if (screenHeight>screenBreakpoint) {
    gtk_widget_set_visible((GtkWidget*)utcLabel,       true);
    gtk_widget_set_visible((GtkWidget*)timeSeparator,  true);
    gtk_widget_set_visible((GtkWidget*)localLabel,     true);
    gtk_widget_set_visible((GtkWidget*)localTime,      true);
    gtk_widget_set_visible((GtkWidget*)localDate,      true);

  }

  while(true) {    
    // logger.debug("screen dimensiopns: <%d,%d>", screenWidth, screenHeight);

    struct timeval currentTime;

    gettimeofday(&currentTime, nullptr);
    strftime(utcTimeBuffer, sizeof(utcTimeBuffer), "%H:%M:%S", gmtime(&currentTime.tv_sec));
    strftime(utcDateBuffer, sizeof(utcDateBuffer), "%Y/%m/%d", gmtime(&currentTime.tv_sec));

    if (screenHeight>screenBreakpoint) {
      strftime(localTimeBuffer, sizeof(localTimeBuffer), "%H:%M:%S", localtime(&currentTime.tv_sec));
      strftime(localDateBuffer, sizeof(localDateBuffer), "%Y/%m/%d", localtime(&currentTime.tv_sec));
    }

    g_idle_add(timeWidgetUpdate, nullptr);
    
    sleep(1);
  }
}


void initRotorDegrees() {
    usleep(1500*1000);
    forceCompassRedraw=true;
}

void displayParameters() {
    if (!debug) {
        return;
    }
    logger.info("fullscreen:                        %s",(options.fullscreen)?"true":"false");
    logger.info("aspectVoltageChannel:              %d", options.aspectVoltageChannel);
    logger.info("aspectVariableResistorOhms:        %d", options.aspectVariableResistorOhms);
    logger.info("aspectFixedResistorOhms:           %d", options.aspectFixedResistorOhms);
    logger.info("limitSwitchPin:                    %d", options.LimitSwitch);
    logger.info("sps:                               %d", options.sps);

}

void neopixel_colortest() {
    logger.info("moving color:  %6x", movingColor);
    neopixel_setPixel(operationIndicator, movingColor);
    neopixel_render();
    delay(500);

    logger.info("braking color: %6x", brakingColor);
    neopixel_setPixel(operationIndicator, brakingColor);
    neopixel_render();
    delay(500);

    logger.info("stopped color: %6x", stopColor);
    neopixel_setPixel(operationIndicator, stopColor);
    neopixel_render();
    delay(500);
}

void neopixel_setup() {
    int ledType = WS2811_STRIP_RGB;
    int ret=neopixel_init(ledType, WS2811_TARGET_FREQ, DMA, GPIO_PIN, led_count+10);

    if (ret!=0) {
        fprintf(stderr, "neopixel initialization failed: %s\n", neopixel_error(ret));
        exit(5);
    }

    neopixel_setBrightness(32);

    neopixel_setPixel(operationIndicator, stopColor);
    neopixel_setPixel(operationIndicator+1, 0);
    neopixel_render();

    thread(neopixel_colortest).detach();
}

void initTime(GtkBuilder *builder) {
    timeWindow = (GtkWidget*) gtk_builder_get_object (builder, "TimeWindow");

    utcLabel  = (GtkLabel*) gtk_builder_get_object (builder, "utc-label");
    utcTime   = (GtkLabel*) gtk_builder_get_object (builder, "utc-time");
    utcDate   = (GtkLabel*) gtk_builder_get_object (builder, "utc-date");

    timeSeparator = (GtkLabel*) gtk_builder_get_object (builder, "time-separator");

    localLabel  = (GtkLabel*) gtk_builder_get_object (builder, "local-label");
    localTime   = (GtkLabel*) gtk_builder_get_object (builder, "local-time");
    localDate   = (GtkLabel*) gtk_builder_get_object (builder, "local-date");

}


void abortMovement() {
  thread(deactivateRotor).detach();
  thread(hideMouse).detach();
} 

void programStop() {
  gtk_main_quit();

  deactivateRotor();

  while(!parked) {
    delay(10);
  }

  exit(0);
}




int hideSettings(gpointer data) {
  gtk_window_close(settingsWindow);
  isSettingsDialogueActive=false;
  calledHideSettings=false;
  availableNetworksListBoxSignal=0;
  settingsBuilder=0;
  realizedIPs.clear();

  thread(hideMouse).detach();
  return FALSE;
}

void cancelSettings() {
  hideSettings(nullptr);
}

void gtk_widget_destroy_noarg(GtkWidget *widget, void*noarg) {
    gtk_widget_destroy(widget);
}

struct scollStuffStruct {
  int selectedRow;
};

typedef scollStuffStruct scrollStuff;

int timezoneScroller(gpointer data) {
  scrollStuff *stuff = (scrollStuff*)data;

  auto adjuster = gtk_list_box_get_adjustment(timezoneListBox);
  gdouble pageSize = gtk_adjustment_get_page_size(adjuster);

  if (stuff->selectedRow<0) {
    stuff->selectedRow=0;
  }

  gint wx, wy=0;
  gint mh, nh;

  auto row = gtk_list_box_get_row_at_index(timezoneListBox, stuff->selectedRow);

  gtk_widget_get_preferred_height((GtkWidget*)row, &mh, &nh);
  gtk_widget_translate_coordinates((GtkWidget*)row, gtk_widget_get_toplevel((GtkWidget*)timezoneListBox), 0, 0, &wx, &wy);

  double adjustment = wy - (pageSize - nh)/2.0;
  gtk_adjustment_set_value(adjuster, adjustment);

  // logger.info("pageSize=%.0f; wy=%ld; mh=%d nh=%d; adj=%.0f", pageSize, (long)wy, mh, nh, adjustment);

  return FALSE;
}

int removeSSIDSelection(gpointer data) {
//  gtk_list_box_unselect_all(availableNetworksListBox);
  return false;
}


int updateSSIDSetFocus(gpointer data) {
  removeSSIDSelection(availableNetworksListBox);
  gtk_widget_grab_focus((GtkWidget*)passwdEntry);
  return false;
}

bool ssidEvent=false;

int ssidAction(gpointer data) {
  char *action=(char*)data;
  logger.debug("ssid-action: %s", action);
  return false;
}

const char *ssidSignal="row-activated";

int ssidRowSelected(gpointer data) {
  GtkListBoxRow *sel = gtk_list_box_get_selected_row(availableNetworksListBox);

  if (sel==nullptr) {
    return false;
  }

  auto row = gtk_list_box_row_get_index(sel);

  auto network = wifiNetworks[row].c_str();

  char *freq=strstr((char*)network,"  ");

  if (freq) freq[0]=0;

  auto currText = gtk_entry_get_text(ssidEntry);

  if (strcmp(currText,network)==0) {
    g_idle_add(removeSSIDSelection,nullptr);
    return false;
  }

  gtk_entry_set_text(ssidEntry,network);

  g_idle_add(updateSSIDSetFocus,nullptr);

  return false;
}


int setWifiTabFocus(gpointer data) {
  logger.info("wifi tab clicked");
  gtk_entry_grab_focus_without_selecting(ssidEntry);
  return false;
}

int updateTimezones(gpointer data) {
    int options;
    const char *currCountry=nullptr;


    auto sel = gtk_list_box_get_selected_row(countryListBox);
    if (sel!=nullptr) {
      auto row = gtk_list_box_row_get_index(sel);

      currCountry = countries[row].c_str();
    } else {
      return FALSE;
    }
    gtk_container_foreach((GtkContainer *)timezoneListBox, gtk_widget_destroy_noarg, nullptr);

    timezones.clear();
    int selectedRow=-1;
    options=-1;

    for (string sysZone: systemTimezones) {
      char buf[4096];
      strcpy(buf,sysZone.c_str());

      if (strncmp(buf, currCountry, strlen(currCountry))!=0) {
        continue;
      }
      ++options;
      char *slash=strstr(buf,"/");
      int offset=(slash==0)?0:slash-(&buf[0])+1;
      auto label = gtk_label_new(&buf[offset]);
      gtk_label_set_xalign ((GtkLabel*)label, 0);
      gtk_list_box_insert(timezoneListBox, label, options);
      timezones.push_back(buf);
      if (strcmp(buf, currTimezone)==0) {
        selectedRow = options;
      }
    }
    if (selectedRow>=0) {
        auto row = gtk_list_box_get_row_at_index(timezoneListBox, selectedRow);
        gtk_list_box_select_row(timezoneListBox, row);
    }

    gtk_widget_show_all((GtkWidget*)timezoneListBox);

    scrollStuff *stuff = new scrollStuff;
    stuff->selectedRow = selectedRow;

    g_idle_add(timezoneScroller,stuff);

  return FALSE;
}

atomic<bool> updatingWifi{false};
void updateWifi() {
  bool expect = false;

  if (!updatingWifi.compare_exchange_strong(expect,true)) {
    return;
  }

  system("updateWifi.sh");

  updatingWifi=false;

}



void setCalibrationVariables(int east, int west) {
  pair<int, int> p1 = { 90, west-180};
  pair<int, int> p2 = {180,      180};
  pair<int, int> p3 = {270, east+180};

  double x1 = p1.first;
  double x2 = p2.first;
  double x3 = p3.first;

  double y1 = p1.second;
  double y2 = p2.second;
  double y3 = p3.second;
  
  calibrationA = ( x1*(y3-y2) + x2*(y1-y3)+ x3*(y2-y1) ) /
                      ( (x1-x2)*(x1-x3)*(x2-x3) );

  calibrationB = (y2 - y1) /
                 (x2 - x1);

  calibrationB -= calibrationA * (x1 + x2);

  calibrationC = y1 - (calibrationA * x1 * x1) - (calibrationB * x1);

  logger.info("(x1,y1)= %3.0f, %3.0f", x1, y1);
  logger.info("(x2,y2)= %3.0f, %3.0f", x2, y2);
  logger.info("(x3,y3)= %3.0f, %3.0f", x3, y3);

  logger.info("a=%11.2e", calibrationA);
  logger.info("b=%8.3f",  calibrationB);
  logger.info("c=%8.3f",  calibrationC);
}


void saveSettings() {
//@@
    
    auto eastEntry   = (GtkEntry*) gtk_builder_get_object(settingsBuilder, "CalibrationEast");
    auto westEntry   = (GtkEntry*) gtk_builder_get_object(settingsBuilder, "CalibrationWest");

    auto eastText   = gtk_entry_get_text(eastEntry);
    auto westText   = gtk_entry_get_text(westEntry);

    int east=0;
    int west=0;

    sscanf(eastText,"%d", &east);
    sscanf(westText,"%d", &west);

    if (strlen(eastText)==0) {
      east=90;
    }
    if (strlen(westText)==0) {
      west=270;
    }

    if (east>45 && east<135 && west>225 && west<315) {
      char calibrationFilename[4096];
      sprintf(calibrationFilename,"%s/calibration", configFolder);
      FILE* calibration = fopen(calibrationFilename,"w");
      fprintf(calibration,"%d,%d\n", east, west);
      fclose(calibration);

      calibrationEast=east;
      calibrationWest=west;
      setCalibrationVariables(east,west);

    } else {
      logger.error("eastText=[%s]",eastText);
      logger.error("westText=[%s]",westText);     
      logger.error("calibration settings out of range: e=%d w=%d", east, west);
      softError("calibration","Calibration out of range error.\nEast must be between 46-134\nWest must be between 226-314", settingsWindow);      
      return;
    }

    auto sel = gtk_list_box_get_selected_row(timezoneListBox);
   
    if (sel!=nullptr) {
      auto row = gtk_list_box_row_get_index(sel);
      char cmd[4096];
      sprintf(cmd,"sudo timedatectl set-timezone '%s'", timezones[row].c_str());
      system(cmd);
      logger.info("new timezone: %s",timezones[row].c_str());
    }

    auto ssid   = gtk_entry_get_text(ssidEntry);
    auto passwd = gtk_entry_get_text(passwdEntry);

    if (strlen(ssid)>0) {
      char tmpstr[4096];
      sprintf(tmpstr,"%s/.ssid",configFolder);
      FILE *ssidFile = fopen(tmpstr,"w");

      if (ssidFile) {
        fprintf(ssidFile,"%s %s\n",ssid, passwd);
        fclose(ssidFile);

        thread(updateWifi).detach();
      } else {
        logger.warn("unable to open ssid file: %s", tmpstr);
      }
    }

    g_idle_add(hideSettings, nullptr);
}

void showWifiUpdatesDelay() {
  usleep(5000*1000);
  showingWifiUpdates=false;
}


int updateAvailableNetworks(GtkListBox *availableNetworksListBox) {

  gtk_list_box_unselect_all(availableNetworksListBox);
  gtk_container_foreach((GtkContainer *)availableNetworksListBox, gtk_widget_destroy_noarg, nullptr);


  const char *userText = gtk_entry_get_text(ssidEntry);

  GtkListBoxRow *selectedRow=nullptr;
  int options=-1;
  for (auto s: wifiNetworks) {
    ++options;
    auto label = gtk_label_new(s.c_str());

    gtk_label_set_xalign ((GtkLabel*)label, 0);
    gtk_list_box_insert(availableNetworksListBox, label, options);

    if (strcmp(s.c_str(), userText)==0) {
      selectedRow=gtk_list_box_get_row_at_index(availableNetworksListBox, options);
    }
  }

  if (selectedRow!=nullptr) {
    gtk_list_box_select_row(availableNetworksListBox, selectedRow);
  }

  gtk_widget_show_all((GtkWidget*)availableNetworksListBox);

  availableNetworksListBoxSignal = g_signal_connect (availableNetworksListBox, ssidSignal,     G_CALLBACK (ssidRowSelected),     (char*) ssidSignal);

  return false;
}

int showWifiUpdates(gpointer data) {
  if (!isSettingsDialogueActive || !data) {
    return false;
  }

  GtkBuilder *settingsBuilder=(GtkBuilder*)data;
  GtkListBox *availableNetworksListBox = (GtkListBox*) gtk_builder_get_object (settingsBuilder, "AvailableNetworks");

  if (availableNetworksListBox==nullptr) {
    showingWifiUpdates=false;
    return false;
  }

  updateAvailableNetworks(availableNetworksListBox);

  thread(showWifiUpdatesDelay).detach();

  return false;
}


void showWifiUpdatesController(GtkBuilder *settingsBuilder) {

  bool expect=false;
  if (!showingWifiUpdates.compare_exchange_strong(expect,true)) {
    return;
  }
  if (!isSettingsDialogueActive) {
    return;
  }

  char tmpstr[8192];

  FILE *inputFile=popen("shownetworks.sh", "r");
  if (inputFile==nullptr) {
    logger.error("unable to open shownetworks.h");
    showingWifiUpdates=false;
    return;
  }


  if (availableNetworksListBoxSignal) {
    if (g_signal_handler_is_connected(availableNetworksListBox, availableNetworksListBoxSignal)) {
      g_signal_handler_disconnect(availableNetworksListBox, availableNetworksListBoxSignal);
    }
  }

  vector<string> currentNetworks;

  while (fgets(tmpstr, sizeof(tmpstr), inputFile)) {
    int len=strlen(tmpstr);
    if (tmpstr[len-1]==10) tmpstr[len-1]=0;
    int strength;
    char *comma=strstr(tmpstr,"þ");
    char *network=comma+2;
    if (!comma) {
      continue;
    }

    sscanf(tmpstr,"%dþ", &strength);

    char *freq=strstr(network,"þ");
    if (freq) {
        freq[0]=' ';
        freq[1]=' ';
    }
    currentNetworks.push_back(network);
  }
  fclose(inputFile);

   bool changed = (currentNetworks.size() != wifiNetworks.size());

  if (!changed && currentNetworks.size()>0) {
    for (uint i=0; i<currentNetworks.size();++i) {
      if (currentNetworks[i].compare(wifiNetworks[i])) {
        changed=true;
        break;
      }
    }       
  }

  if (!changed || currentNetworks.size()<1) {
    showingWifiUpdates=false;
    return;
  }

  wifiNetworks.clear();
  for (auto s: currentNetworks) {
    wifiNetworks.push_back(s);
  }
  g_idle_add(showWifiUpdates,settingsBuilder);

  return;
}

PangoFontDescription *realizedIPFont=nullptr;

void showRealizedIPDelay() {
  usleep(1000*1000);
  showingRealizedIp=false;
}

int showRealizedIp(gpointer data) {
  if (!settingsBuilder) {
    showingRealizedIp=false;
    return false;
  }
    GtkListBox* realizedIp = (GtkListBox*) gtk_builder_get_object(settingsBuilder, "RealizedIp");

  if (realizedIp==nullptr) {
    showingRealizedIp=false;
    return false;
  }

  gtk_container_foreach((GtkContainer *)realizedIp, gtk_widget_destroy_noarg, nullptr);

  for (auto s: realizedIPs) {
    auto label = gtk_label_new(s.c_str());

    if (realizedIPFont==nullptr) {
      realizedIPFont = pango_font_description_new ();
      pango_font_description_set_family(realizedIPFont,"Courier");
      pango_font_description_set_size(realizedIPFont,10*PANGO_SCALE);
    }    
    
    // gtk_label_set_markup((GtkLabel*)label, "<span face='Courier'>text</span>");

    if (!strstr(s.c_str(),"internet")) {
      gtk_widget_modify_font(label, realizedIPFont);    
    }

    gtk_label_set_xalign ((GtkLabel*)label, 0);
    gtk_widget_set_margin_left(label, 10);
    gtk_list_box_insert(realizedIp, label, -1);
  }

  gtk_widget_show_all((GtkWidget*)realizedIp);

  thread(showRealizedIPDelay).detach();
  return false;
}

void detectInternet() {
    bool expect=false;

  vector<string> currRealizedIPs;

  if (!detectingIntenet.compare_exchange_strong(expect,true)) {
    return;
  }
  int rs=system("ping -c1 8.8.8.8 > /dev/null 2>&1");

  hasInternet=(rs==0);

  usleep(2000*1000);
  detectingIntenet=false;
}

void showRealizedIpController() {  
  char net[128];
  char ip[128];
  char tmpstr[1024];
  char tmpstr2[2048];
  bool expect=false;

  vector<string> currRealizedIPs;

  if (!showingRealizedIp.compare_exchange_strong(expect,true)) {
    return;
  }

  FILE *inputFile=popen("showip.sh", "r");
  if (inputFile==nullptr) {
    showingRealizedIp=false;
    return;
  }
  while (fscanf(inputFile,"%s %s\n", net, ip)>0) {
    sprintf(tmpstr,"%s:",net);
    sprintf(tmpstr2,"%-8s%s",tmpstr,ip);
    currRealizedIPs.push_back(tmpstr2);
  }
  fclose(inputFile);

  const char *internetMessage;
  if (hasInternet) {
    internetMessage="internet detected";
  } else {
    internetMessage="no internet";
  }
  currRealizedIPs.push_back(internetMessage);

  bool changed = (realizedIPs.size() != currRealizedIPs.size());

  if (!changed && currRealizedIPs.size()>0) {
    for (uint i=0;i<currRealizedIPs.size();++i) {
      if (currRealizedIPs[i].compare(realizedIPs[i])) {
        changed=true;
        break;
      }
    }
  }

  realizedIPs.clear();
  for (auto s: currRealizedIPs) {
    realizedIPs.push_back(s.c_str());
  }

  g_idle_add(showRealizedIp,nullptr);
}

void wifiUpdate() {
  while (true) {
    usleep(1000*1000);

    if (isSettingsDialogueActive) {
      thread(showRealizedIpController).detach();
      thread(showWifiUpdatesController,settingsBuilder).detach();      
      thread(detectInternet).detach();
    }
  }
}

void showPassword() {

  bool show = gtk_toggle_button_get_active((GtkToggleButton*)showPasswd);

  gtk_entry_set_visibility(passwdEntry, show);

  gtk_entry_grab_focus_without_selecting(passwdEntry);

}


void readSSID() {
  char ssid[512];
  char passwd[512];

  ssidEntry   = (GtkEntry*) gtk_builder_get_object (settingsBuilder, "SSID");
  passwdEntry = (GtkEntry*) gtk_builder_get_object (settingsBuilder, "WiFiPassword");
  showPasswd  = (GtkCheckButton*) gtk_builder_get_object (settingsBuilder, "ShowPassword");

  g_signal_connect (showPasswd, "toggled", G_CALLBACK (showPassword), NULL);


  char tmpstr[4096];
  sprintf(tmpstr,"%s/.ssid",configFolder);
  FILE *ssidFile = fopen(tmpstr,"r");

  if (!ssidFile) {
    return;
  }
  fscanf(ssidFile, "%s %s", ssid, passwd);

  gtk_entry_set_text(ssidEntry,   ssid);
  gtk_entry_set_text(passwdEntry, passwd);
  
  fclose(ssidFile);
}
int resetCalibration(gpointer data) {
  logger.info("reset calibration");
  auto eastEntry = (GtkEntry*) gtk_builder_get_object (settingsBuilder, "CalibrationEast");
  auto westEntry = (GtkEntry*) gtk_builder_get_object (settingsBuilder, "CalibrationWest");

  gtk_entry_set_text(eastEntry, "90");
  gtk_entry_set_text(westEntry, "270");

  return false;
}

void readCalibration() {

  char calibrationFilename[4096];
  sprintf(calibrationFilename,"%s/calibration", configFolder);
  FILE* calibration = fopen(calibrationFilename,"r");
  if (!calibration) {
    return;
  }

  fscanf(calibration,"%d,%d\n", &calibrationEast, &calibrationWest);
  fclose(calibration);
 
  setCalibrationVariables(calibrationEast, calibrationWest);

  
}

void initCalibrationSettings() {
  char eastText[512];
  char westText[512];

  auto eastEntry = (GtkEntry*) gtk_builder_get_object (settingsBuilder, "CalibrationEast");
  auto westEntry = (GtkEntry*) gtk_builder_get_object (settingsBuilder, "CalibrationWest");

  readCalibration();

  sprintf(eastText,"%d",calibrationEast);
  sprintf(westText,"%d",calibrationWest);

  gtk_entry_set_text(eastEntry, eastText);
  gtk_entry_set_text(westEntry, westText);

  
}


bool timezonesInitialized=false;

#define vectorContainsString(v,e) (std::find((v).begin(), (v).end(), e)!=(v).end())

void initializeTimezones() {
  char buf[4096];

  systemTimezones.clear();
  systemCountries.clear();

  FILE *timezoneInput = popen("timedatectl list-timezones", "r");

  while (fgets(buf, sizeof(buf), timezoneInput) != nullptr) {
    buf[strlen(buf)-1]=0;
    systemTimezones.push_back(buf);

    char *slash=strstr(buf,"/");
    if (slash) {
      *slash=0;
    }

    if (!vectorContainsString(systemCountries,buf)) {
      systemCountries.push_back(buf);
    }
  }

  timezonesInitialized=true;
}


int showSettings(gpointer data) {
    GObject    *button;
    GError     *error=nullptr;

    /* Construct a GtkBuilder instance and load our UI description */
    settingsBuilder = gtk_builder_new ();
    if (gtk_builder_add_from_file (settingsBuilder, "settings.ui", &error) == 0) {
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }

    auto wifiTab    = (GtkWidget*)  gtk_builder_get_object (settingsBuilder, "WifiTab");
    settingsWindow  = (GtkWindow*)  gtk_builder_get_object (settingsBuilder, "SettingsWindow");
    countryListBox  = (GtkListBox*) gtk_builder_get_object (settingsBuilder, "CountryListBox");
    timezoneListBox = (GtkListBox*) gtk_builder_get_object (settingsBuilder, "TimezoneListBox");
    availableNetworksListBox = (GtkListBox*) gtk_builder_get_object (settingsBuilder, "AvailableNetworks");

    readSSID();
    initCalibrationSettings();

    if (screenHeight<321) {
      GtkScrolledWindow *sw = (GtkScrolledWindow*) gtk_builder_get_object(settingsBuilder, "RegionScrolledWindow");
      gtk_scrolled_window_set_min_content_height(sw, -1);

      sw = (GtkScrolledWindow*) gtk_builder_get_object(settingsBuilder, "TimezoneScrolledWindow");
      gtk_scrolled_window_set_min_content_height(sw, -1);

      sw = (GtkScrolledWindow*) gtk_builder_get_object(settingsBuilder, "AvailableNetworksScrolledWindow");
      gtk_scrolled_window_set_min_content_height(sw, -1);
    }
    FILE* timezoneInputFile = popen("timedatectl | sed -ne 's/.*Time zone: *\\([^ ]*\\) (.*)$/\\1/p'", "r");

    fgets(currTimezone,sizeof(currTimezone),timezoneInputFile);
    currTimezone[strlen(currTimezone)-1]=0;
    fclose(timezoneInputFile);

    char currCountry[4096];

    memset(currCountry,0,sizeof(currCountry));
    for (int i=0; currTimezone[i]!=0 && currTimezone[i]!='/';++i) {
      currCountry[i]=currTimezone[i];
    }

    logger.info("current timezone: %s %s",currTimezone, currCountry);

    while (!timezonesInitialized) {
      usleep(10000);
    }

    GtkListBoxRow *row=nullptr;
    
    int options=-1;
    for (string buf: systemCountries) {
      ++options;
      countries.push_back(buf);
      auto label = gtk_label_new(buf.c_str());
      gtk_label_set_xalign ((GtkLabel*)label, 0);
      gtk_list_box_insert(countryListBox, label, options);


      if (strcmp(currCountry,buf.c_str())==0) {
        row = gtk_list_box_get_row_at_index(countryListBox,options);
        gtk_list_box_select_row(countryListBox,row);
      }
    }


    updateTimezones(nullptr);
    updateAvailableNetworks(availableNetworksListBox);
    gtk_widget_show_all((GtkWidget*)settingsWindow);


    button = gtk_builder_get_object (settingsBuilder, "SaveSettingsButton");
    g_signal_connect (button, "clicked", G_CALLBACK (saveSettings), NULL);

    button = gtk_builder_get_object (settingsBuilder, "CancelSettingsButton");
    g_signal_connect (button,            "clicked",       G_CALLBACK (cancelSettings),  NULL);
    g_signal_connect (settingsWindow,    "destroy",       G_CALLBACK (cancelSettings),  NULL);
    g_signal_connect (countryListBox,    "row-selected",  G_CALLBACK (updateTimezones), NULL);
    g_signal_connect (wifiTab,           "focus", G_CALLBACK (setWifiTabFocus), NULL);
    
    button = gtk_builder_get_object (settingsBuilder, "CalibrationReset");
    g_signal_connect (button,            "clicked",       G_CALLBACK (resetCalibration),  NULL);


    availableNetworksListBoxSignal = g_signal_connect (availableNetworksListBox, ssidSignal,     G_CALLBACK (ssidRowSelected),     (char*) ssidSignal);

    return FALSE;
}


void settingsDialogue() {
    bool expected1=false;
    bool expected2=false;

    if (!isSettingsDialogueActive.compare_exchange_weak(expected1, true)) {
      if (!calledHideSettings.compare_exchange_weak(expected2, true)) {
        hideSettings(nullptr);
      }
      return;
    }

    g_idle_add(showSettings, nullptr);
}

static gboolean compassClick(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {

  gint width;
  gint height;

  gtk_window_get_size((GtkWindow *)mainWindow, &width, &height);

// double click = GDK_2BUTTON_PRESS
    double breakpoint = width/3.0;
    if (event->type == GDK_BUTTON_PRESS && event->x < breakpoint) {
      // logger.info("mouse click button <%d> @ (%.0f,%.0f); breakpoint=%f; screenWidth=%d", event->button, event->x, event->y, breakpoint, screenWidth);

      settingsDialogue();
    }

    return TRUE;
}


void makeConfigFolder() {

  sprintf(configFolder,"%s/.config/rotor", getenv("HOME"));

  DIR* dir = opendir(configFolder);
  if (dir) {
    closedir(dir);
  } else {
    char cmd[4096];
    sprintf(cmd,"mkdir -p %s",configFolder);
    system(cmd);
  }
  FILE* theme = fopen ("theme.css", "r");
  if (theme==nullptr) {
    char bin[2048];
    sprintf(bin,"%s/bin",getenv("HOME"));
    chdir(bin);
  } else {
    fclose(theme);
  }
}

int main(int argc, char **argv) {
  GtkBuilder *uiBuilder;
  GObject    *button;
  GError     *error=nullptr;


  int rs=setuid(0);
  if (rs<0) {
      bootError("sorry, this app must run as root; check setuid bit");
  }

  makeConfigFolder();


  if (!options.commandLineOptions(argc, argv)) {
    exit(1);
  }
  debug=(logger.getGlobalLevel()<=DEBUG);

  displayParameters();

  if (wiringPiSetup() != 0) {
    bootError("wiringPi setup failed");
  }
  
  pinMode(options.LimitSwitch, INPUT);
  // doesn't work on RPi 4
  //pullUpDnControl(options.LimitSwitch, PUD_UP);

  a2dSetup();

  if (initRotorMotor()!=0) {
      bootError("rotor motor initializaion failed");
  }

  gtk_init(&argc, &argv);

  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(cssProvider, "theme.css", NULL);

  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                              GTK_STYLE_PROVIDER(cssProvider),
                              GTK_STYLE_PROVIDER_PRIORITY_USER);

  /* Construct a GtkBuilder instance and load our UI description */
  uiBuilder = gtk_builder_new ();
  if (gtk_builder_add_from_file (uiBuilder, "layout.ui", &error) == 0) {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      return 1;
  }

  /* Connect signal handlers to the constructed widgets. */
  mainWindow = gtk_builder_get_object (uiBuilder, "window");
  g_signal_connect (mainWindow, "destroy", G_CALLBACK (programStop), NULL);

  button = gtk_builder_get_object (uiBuilder, "MoveExactButton");
  g_signal_connect (button, "clicked", G_CALLBACK (moveExact), NULL);

  button = gtk_builder_get_object (uiBuilder, "FastReverse");
  g_signal_connect (button, "clicked", G_CALLBACK (moveTenCounterClockwise), NULL);

  button = gtk_builder_get_object (uiBuilder, "FastForward");
  g_signal_connect (button, "clicked", G_CALLBACK (moveTenClockwise), NULL);


  button = gtk_builder_get_object (uiBuilder, "abort");
  g_signal_connect (button, "clicked", G_CALLBACK (abortMovement), NULL);

  drawingArea = (GtkWidget *) gtk_builder_get_object (uiBuilder, "CompassDrawingArea");
  g_signal_connect (mainWindow, "button-press-event", G_CALLBACK (compassClick), NULL);



  setButton(uiBuilder, "northButton", "clicked", G_CALLBACK(moveTo), &directional.north);
  setButton(uiBuilder, "southButton", "clicked", G_CALLBACK(moveTo), &directional.south);
  setButton(uiBuilder, "eastButton", "clicked", G_CALLBACK(moveTo),  &directional.east);
  setButton(uiBuilder, "westButton", "clicked", G_CALLBACK(moveTo),  &directional.west);

  setButton(uiBuilder, "seButton", "clicked", G_CALLBACK(moveTo), &directional.se);
  setButton(uiBuilder, "swButton", "clicked", G_CALLBACK(moveTo), &directional.sw);
  setButton(uiBuilder, "nwButton", "clicked", G_CALLBACK(moveTo), &directional.nw);
  setButton(uiBuilder, "neButton", "clicked", G_CALLBACK(moveTo), &directional.ne);




  degreeInputBox = (GtkWidget *) gtk_builder_get_object (uiBuilder, "DegreeInputBox");

  initTime(uiBuilder);

  createDrawingSurface(drawingArea);
  g_signal_connect (drawingArea,"configure-event", G_CALLBACK (configure_event_cb), NULL);
  g_signal_connect (drawingArea, "draw", G_CALLBACK (draw_cb), NULL);
  g_signal_connect (G_OBJECT(mainWindow), "window-state-event", G_CALLBACK(manualScreenRedraw), NULL);


  if (options.fullscreen) {
      logger.info("entering full screen mode");
      gtk_window_fullscreen(GTK_WINDOW(mainWindow));
  }

  getScreenResolution();
  readCalibration();
  
  thread(initializeTimezones).detach();
  thread(hideMouse).detach();
  thread(timeUpdate).detach();
  thread(renderCompass).detach();
  thread(initRotorDegrees).detach();
  thread(wifiUpdate).detach();
  neopixel_setup();

  gtk_main();

  return 0;
}
// #pragma clang diagnostic pop
#pragma GCC diagnostic pop
void stopMotor();
