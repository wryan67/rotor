
#include <gtk/gtk.h>
#include <math.h>
#include <thread>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <ads1115rpi.h>
#include <neopixel.h>  
#include <atomic>
#include <vector>

#include <time.h>
#include <sys/time.h>


#include "motor.h"
#include "Options.h"

using namespace std;
using namespace common::utility;
using namespace common::synchronized;

Options options = Options();
bool debug=false;

#define BASE 200
#define SPI_CHAN 0
#define MCP3008_SINGLE  8
#define MCP3008_DIFF    0

bool cwLimitPoint=false;
int  ADS1115_ADDRESS=0x48;
int  a2dHandle=-1;

bool parked=true;
FILE *logFile=nullptr;

vector<pair<uint64_t, float>*> points;
bool capturePoints=false;

int screenWidth;
int screenHeight;

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18   // BCM numbering system
#define DMA                     10   // DMA=Direct Memory Access
int  led_count =                 1;  // number of pixels in your led strip
int  operationIndicator = 0;
int  stopColor    = 0xff0000;
int  movingColor  = 0x00ff00;
int  brakingColor = 0xffff00;

atomic<bool> stoppingRotor{false};

// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

Logger     logger("main");
GtkWidget *drawingArea=nullptr;

GtkWidget *timeWindow=nullptr;

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



float rotorDegree=0;
bool  forceCompassRedraw=false;
bool  forceVoltageDebugDisplay=false;

mutex displayLock;

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

struct directionalType {
  int east,  se, south,   sw, west,  nw, north, ne;
} directional = {
        90, 135,   180,  225,  270, 315,     0, 45
};

float getDegree(float volts);

float translateRotor2Display(float degrees) {
    if (degrees>180) {
        return degrees-180;
    } else {
        return degrees+180;
    }

    // float workingDegrees=degrees+180;
    // if (workingDegrees>360) {
    //     workingDegrees-=360;
    // }    
    // return workingDegrees;
}

float translateDisplay2Rotor(float degrees) {
    float workingDegrees=degrees-180;
    if (workingDegrees<0) {
        workingDegrees+=360;
    }    
    return workingDegrees;
}

void hideMouse() {
  char cmd[2048];

  delay(1000);
  sprintf(cmd,"xdotool mousemove %d %d", screenWidth, screenHeight);
  system(cmd);
}


void voltageCatcher() {

  float currentVolts   = readVoltage(a2dHandle);

  switch (sampleMode) {
    case 0: {

      totalSampleVolts+=currentVolts;      
      samples.push_back(currentVolts);

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
      return;
    }
    case 1: {
      ++windowSize;
      return;
    }
    case 2: {
      totalSampleVolts+=currentVolts;      
      samples.push_back(currentVolts);

      if (samples.size()>=windowSize) {
        sampleMode=0;
      }      
    }
  }
}

void a2dSetup() {

    a2dHandle = getADS1115Handle(ADS1115_ADDRESS);

    if (a2dHandle<0) {
        fprintf(stderr, "opening ads1115 failed: %s\n", strerror(errno));
        exit(3);
    }

    float v3 = readVoltageSingleShot(a2dHandle, options.v3channel, 0);

    if (abs((3.3-v3)/((3.3+v3)/2))>0.10) {
        for (int c=0;c<4;++c) {
            float volts = readVoltageSingleShot(a2dHandle, c, 0);
            logger.info("channel<%d>=%f", c, volts);
        }
        logger.info("a2dHandle=%d; a2d_address=%02x", a2dHandle, ADS1115_ADDRESS);
        logger.info("voltage on channel a%d=%f", options.v3channel, v3);
        logger.error("ads1115 chip is not working; check external power?");
		    exit(4);
    }

    options.sps=5;
    logger.info("channel=%d; gain=%d; sps=%d", options.aspectVoltageChannel, options.gain, options.sps);

    wiringPiISR(2,INT_EDGE_FALLING, voltageCatcher);
    setADS1115ContinuousMode(a2dHandle, options.aspectVoltageChannel, options.gain, options.sps);

    float targetPeriods = 3;
    float sixtyHzPeriod = 1000.0 * ( 1.0 / 60.0); // 1000 ms * 1/60;
    float targetWindow= round(sixtyHzPeriod * targetPeriods);  //ms

    logger.info("60Hz period = %.1f; targetPeriods=%.0f; targetWindow=%.1fms", sixtyHzPeriod, targetPeriods, targetWindow);

    int   tries=3;
    float pct;
    int   expectedSampleWindow = targetWindow * 2500 / 1000;

    while (tries-->0) {
      delay(500);

      sampleMode=-1;
      delay(5);
      windowSize=0;
      totalSampleVolts=0;
      totalSamples=0;
      lastDisplayVolts=-9;
      samples.clear();
      forceVoltageDebugDisplay=false;

      sampleMode = 1;
      usleep(targetWindow*1000);
      ++sampleMode;

      logger.info("window size = %d", windowSize);

      pct = 100 * ((long)windowSize-expectedSampleWindow) / ((expectedSampleWindow+windowSize)/2.0);
      if (abs(pct)<=11) {
        return;
      }
      logger.info("retying...");
    }
    logger.info("pct=%.0f", pct);
    logger.info("calculated window size = %d ", windowSize);
    logger.error("unable to reach target window size of %d.", expectedSampleWindow);
    if (pct<0) {
      logger.error("Is your i2c bus overclocked?", expectedSampleWindow);
    } else {
      logger.error("Target windows is larger than expected.");
      logger.error("Unknown error");
    }
    exit(19);
}

int textBoxWidgetUpdate(gpointer data) {
    gtk_entry_set_text(GTK_ENTRY(degreeInputBox), degreeTextBox);
    return 0;
}

int timeWidgetUpdate(gpointer data) {

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
    
    if (logFile) fprintf(logFile,"parked\n");

    delay(100);
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
    if (!activateRotor(degrees)) {
        logger.error("failed to start rotor motor");
            return;
    }
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

  int radius = (x<y)?x-3:y-3;
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
  
  cairo_set_font_size(cr, 20);
  cairo_text_extents(cr, deg, &extents);

  float halfText = extents.width/2;

  cairo_set_source_rgb(cr, 0,0,0);  

  float textY;
  if (currDegree<90 || currDegree>270) {
    textY=y-(extents.height*1.5);
  } else {
    textY=y+(extents.height*2.5);
  }
  cairo_move_to(cr, x-halfText, textY);
  cairo_show_text(cr, deg);

  int padTop=3;
  int padBottom=padTop+1;
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
    switch (options.aspectFixedResistorOhms) {
        case 1000:    return  5.7849*volts*volts + 42.320*volts + 1.6645;
        case 1375:    return  6.9425*volts*volts + 61.667*volts + 1.0252;
        default:
            fprintf(stderr,"voltage to degree equation not defined for r1=%d'n", 
                            options.aspectFixedResistorOhms );
        return 0;
    }
}

int screenBreakpoint=320;

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

void voltageCatcherOld() {
    logger.debug("init voltage catcher; channel=%d", options.aspectVoltageChannel);
  
    float lastDegree=999;

    // vector<pair<uint64_t,float>*> slidingVolts;

    float voltsMax = (options.rotorVcc * options.aspectVariableResistorOhms) /
                     (options.aspectFixedResistorOhms + options.aspectVariableResistorOhms);

    // float twilightZone = voltsMax * 0.95;
    // float vccFudge = voltsMax*0.99;
    float lastVolts=-1;
    float lastDisplayVolts=-1;
    long long lastDisplayTime=0;
    // uint32_t count=0;

    float maxGain = getADS1115MaxGain(options.gain);

    while (true) {
        auto now = currentTimeMillis();
        float volts = readVoltageSingleShot(a2dHandle, options.aspectVoltageChannel, options.gain);

        if (volts>=maxGain && lastVolts<(maxGain/2)) {
            logger.warn("volts >= maxGain<%f>; setting to zero", maxGain);
            volts=0;
        }

        if (volts<0) {
            logger.error("volts read on channel %d is less than zero: %f", options.aspectVoltageChannel, volts);
            volts=0;
        }

        if (volts>voltsMax) {
            logger.error("volts read on channel %d is more than max allowed: actual=%f, allowed=%f; maxGain=%f", options.aspectVoltageChannel, volts, voltsMax, maxGain);
            volts=voltsMax;
        }

        if (lastVolts<0) {
            lastVolts=volts;
            continue;
        } else {
            float pDiff = abs((volts-lastVolts)/((volts+lastVolts)/2));

            if (!parked && logFile) {
                fprintf(logFile, "%lld,curr=%f,last=%f,pDiff=%f\n", 
                    now, volts, lastVolts, pDiff);
            }

            if (pDiff>0.25) {
                lastVolts=volts;
                continue;
            }
        }
        
        float newDegree = getDegree(volts);

        if (abs(newDegree)>360) {
            newDegree=360;
        }
        if (newDegree<0) {
            newDegree=0;
        }
        rotorDegree=newDegree;

        if (abs(newDegree-lastDegree)>options.wobbleLimit) {
            forceVoltageDebugDisplay=true;
            lastDegree=newDegree;
        }


        if (abs(volts-lastDisplayVolts)>0.010 || forceVoltageDebugDisplay) {
            int limitSwitch = digitalRead(options.LimitSwitch);

            long long now = currentTimeMillis();
            long long elapsed = now - lastDisplayTime;

            if (elapsed<500) {
                //forceVoltageDebugDisplay=true;
            } else {
                logger.debug("bs=%d ls=%d ch[0]=%.3f i-degree=%.1f d-degree=%.0f gain=%d r1=%d", 
                        getBrakeStatus(), limitSwitch,  volts, newDegree, translateRotor2Display(newDegree), 
                        options.gain, options.aspectFixedResistorOhms);
                lastDisplayVolts=volts;
                lastDisplayTime=now;
                forceVoltageDebugDisplay=false;
            }
        }

        lastVolts=volts;
        usleep(100);
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

void initTime(GtkBuilder *builder) {
    timeWindow = (GtkWidget*) gtk_builder_get_object (builder, "TimeWindow");

    utcLabel  = (GtkLabel*) gtk_builder_get_object (builder, "utc-label");
    utcTime   = (GtkLabel*) gtk_builder_get_object (builder, "utc-time");
    utcDate   = (GtkLabel*) gtk_builder_get_object (builder, "utc-date");

    timeSeparator = (GtkLabel*) gtk_builder_get_object (builder, "time-separator");

    localLabel  = (GtkLabel*) gtk_builder_get_object (builder, "local-label");
    localTime   = (GtkLabel*) gtk_builder_get_object (builder, "local-time");
    localDate   = (GtkLabel*) gtk_builder_get_object (builder, "local-date");

/*
    gtk_label_set_text(utcLabel,      nullptr);
    gtk_label_set_text(utcTime,       nullptr);
    gtk_label_set_text(utcDate,       nullptr);
    gtk_label_set_text(timeSeparator, nullptr);
    gtk_label_set_text(localLabel,    nullptr);
    gtk_label_set_text(localTime,     nullptr);
    gtk_label_set_text(localDate,     nullptr);
*/


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

  if (screenWidth<480 || screenHeight<320) {
    logger.error("minimum screen resolution is 840x320");
    exit(7);
  }
}

// moveTenClockwise(GtkWidget *widget, gpointer data)

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


int main(int argc, char **argv) {
    GtkBuilder *builder;
    GObject    *window;
    GObject    *button;
    GError     *error=nullptr;

    int rs=setuid(0);
    if (rs<0) {
        fprintf(stderr,"sorry, this app must run as root; check setuid bit\n");
    }

    FILE* theme = fopen ("theme.css", "r");
    if (theme==nullptr) {
        chdir("/home/pi/bin");
    }

	if (!options.commandLineOptions(argc, argv)) {
		exit(1);
	}
    debug=(logger.getGlobalLevel()<=DEBUG);

    displayParameters();

	if (wiringPiSetup() != 0) {
		logger.error("wiringPi setup failed");
		exit(2);
	}
    
    pinMode(options.LimitSwitch, INPUT);
    // doesn't work on RPi 4
    //pullUpDnControl(options.LimitSwitch, PUD_UP);
  	a2dSetup();

    if (initRotorMotor()!=0) {
        logger.error("rotor motor initializaion failed");
    		exit(4);
    }



    gtk_init(&argc, &argv);

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "theme.css", NULL);

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                GTK_STYLE_PROVIDER(cssProvider),
                                GTK_STYLE_PROVIDER_PRIORITY_USER);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "layout.ui", &error) == 0) {
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }

    /* Connect signal handlers to the constructed widgets. */
    window = gtk_builder_get_object (builder, "window");
    g_signal_connect (window, "destroy", G_CALLBACK (programStop), NULL);

    button = gtk_builder_get_object (builder, "MoveExactButton");
    g_signal_connect (button, "clicked", G_CALLBACK (moveExact), NULL);

    button = gtk_builder_get_object (builder, "FastReverse");
    g_signal_connect (button, "clicked", G_CALLBACK (moveTenCounterClockwise), NULL);

    button = gtk_builder_get_object (builder, "FastForward");
    g_signal_connect (button, "clicked", G_CALLBACK (moveTenClockwise), NULL);


    button = gtk_builder_get_object (builder, "abort");
    g_signal_connect (button, "clicked", G_CALLBACK (abortMovement), NULL);

    setButton(builder, "northButton", "clicked", G_CALLBACK(moveTo), &directional.north);
    setButton(builder, "southButton", "clicked", G_CALLBACK(moveTo), &directional.south);
    setButton(builder, "eastButton", "clicked", G_CALLBACK(moveTo),  &directional.east);
    setButton(builder, "westButton", "clicked", G_CALLBACK(moveTo),  &directional.west);

    setButton(builder, "seButton", "clicked", G_CALLBACK(moveTo), &directional.se);
    setButton(builder, "swButton", "clicked", G_CALLBACK(moveTo), &directional.sw);
    setButton(builder, "nwButton", "clicked", G_CALLBACK(moveTo), &directional.nw);
    setButton(builder, "neButton", "clicked", G_CALLBACK(moveTo), &directional.ne);



    degreeInputBox = (GtkWidget *) gtk_builder_get_object (builder, "DegreeInputBox");
    drawingArea = (GtkWidget *) gtk_builder_get_object (builder, "CompassDrawingArea");

    initTime(builder);



    createDrawingSurface(drawingArea);
    g_signal_connect (drawingArea,"configure-event", G_CALLBACK (configure_event_cb), NULL);
    g_signal_connect (drawingArea, "draw", G_CALLBACK (draw_cb), NULL);
    g_signal_connect (G_OBJECT(window), "window-state-event", G_CALLBACK(manualScreenRedraw), NULL);


    if (options.fullscreen) {
        logger.info("entering full screen mode");
        gtk_window_fullscreen(GTK_WINDOW(window));
    }

    getScreenResolution();
    
    thread(hideMouse).detach();
    thread(timeUpdate).detach();
    // thread(voltageCatcher).detach();
    thread(renderCompass).detach();
    thread(initRotorDegrees).detach();
    neopixel_setup();

    gtk_main();

    return 0;
}
// #pragma clang diagnostic pop
#pragma GCC diagnostic pop
void stopMotor();
