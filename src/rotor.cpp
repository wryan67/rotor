
#include <gtk/gtk.h>
#include <math.h>
#include <thread>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <ads1115rpi.h>
#include <neopixel.h>  
#include <atomic>

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

int  ADS1115_ADDRESS=0x48;
int  a2dHandle=-1;

bool parked=true;
FILE *logFile=nullptr;

vector<pair<long long, float>*> points;
bool capturePoints=false;

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
GtkWidget *drawingArea=nullptr;

float rotorDegree=0;
float wobbleLimit=3;
bool  forceCompassRedraw=false;
bool  forceVoltageDisplay=false;

mutex displayLock;
mutex updateTextLock;

GtkWidget *degreeInputBox;

static cairo_surface_t *surface = NULL;
static void drawCompass(bool newSurface);
static void createDrawingSurface(GtkWidget *widget);

char degreeTextBox[32];
bool ignoreMargins=false;


struct directionalType {
  int east,  se, south,   sw, west,  nw, north, ne;
} directional = {
        90, 135,   180,  225,  270, 315,     0, 45
};

float translateRotor2Display(float degrees) {
    float workingDegrees=degrees+180;
    if (workingDegrees>360) {
        workingDegrees-=360;
    }    
    return workingDegrees;
}

float translateDisplay2Rotor(float degrees) {
    float workingDegrees=degrees-180;
    if (workingDegrees<0) {
        workingDegrees+=360;
    }    
    return workingDegrees;
}


void a2dSetup() {

    a2dHandle = wiringPiI2CSetup(ADS1115_ADDRESS);

	if (a2dHandle<0)
	{
		fprintf(stderr, "opening ads1115 failed: %s\n", strerror(errno));
		exit(3);
	}
}

int textBoxWidgetUpdate(gpointer data) {
    gtk_entry_set_text(GTK_ENTRY(degreeInputBox),degreeTextBox);
    updateTextLock.unlock();
    return 0;
}


void updateTextBox(float degree, bool forceRedraw) {
    updateTextLock.lock();


    sprintf(degreeTextBox,"%.1f", degree);

    g_idle_add(textBoxWidgetUpdate, nullptr);

}


bool hitLimitSwitch() {
    int limitSwitch = digitalRead(options.limitSwitchPin);
    if (limitSwitch==1) {
        logger.debug("Limit switch triggered");
        return true;
    } else {
        return false;
    }
}
static void moveRotorWorker(float degrees, float newDegree) {

    // logFile=fopen("/home/pi/travel.csv","w");
    
    parked=false;
    if (!activateRotor(degrees)) {
        logger.error("failed to start rotor motor");
        return;
    }
    logger.info("moving %.0f degrees to %.1f", degrees, newDegree);

    neopixel_setPixel(operationIndicator, movingColor);
    neopixel_render();

    usleep(50*1000); // debounce limit switch relay

    if (degrees>0) {
        while (rotorDegree<newDegree && !hitLimitSwitch()) {
            usleep(250);  // 0.25 ms
        }
    } else {
        while (rotorDegree>newDegree && !hitLimitSwitch()) {
            usleep(250);  // 0.25 ms
        }
    }
    logger.info("stopping rotor");
    neopixel_setPixel(operationIndicator, brakingColor);
    neopixel_render();

    deactivateRotor();
    if (logFile) fprintf(logFile,"parked\n");
    parked=true;
    float targetDeviation=rotorDegree-newDegree;
    if (debug) {
        forceVoltageDisplay=true;
        usleep(options.catcherDelay*1000);
    }
    logger.info("rotor parked; rotorDegree=%.1f; target deviation=%.1f", 
                    rotorDegree, abs(targetDeviation));
    neopixel_setPixel(operationIndicator, stopColor);
    neopixel_render();

    forceCompassRedraw=true;

    if (logFile) {
        fclose(logFile);
        delay(20);
        logFile=nullptr;
    } 
}

static void moveRotor(float degrees) {
    float currentDegree=rotorDegree;

    if (!isRotorMotorReady()) {
        logger.error("the rotor motor reports 'not ready'.  Is the external turned power on?");
        return;
    }

    if (isRotorMoving()) {
        logger.error("rotor is already moving...");
        return;
    }
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
      updateTextBox(d, false);
      return;
    }
    throw (runtime_error("unknown error"));
  } catch (runtime_error &e) {
    logger.error("invalid degree entered in text box %s; caused by %s", raw, e.what());
  } catch (...) {
    logger.error("invalid degree entered in text box %s", raw);
  }
  updateTextBox(translateRotor2Display(rotorDegree), false);

}




static void calibrate(GtkWidget *widget, gpointer data) {
    moveRotor(-rotorDegree);
    delay(1000);
    capturePoints=true;    
    moveRotor(180);
    capturePoints=false;
  
    if (points.size()<100) {
        logger.error("not enough data collected, calibration aborted");
        return;
    }
    auto start=points[0]->first;
    auto end=points[points.size()-1]->first;

    auto elapsed = end - start;

    if (elapsed<20000) {
        logger.error("elapsed time is less than 20 seconds, calibration aborted");
    }

    long count=0;
    double sumX=0;
    double sumY=0;
    for (auto point: points) {
        if (point->first<(start+1000)) continue;
        if (point->first>(end-5000)) continue;
        ++count;
        sumX+=point->first;
        sumY+=point->second;
    }

    double avgX=sumX/count;
    double avgY=sumY/count;
    

    double s1=0;
    double s2=0;
    for (auto point: points) {
        if (point->first<(start+1000)) continue;
        if (point->first>(end-5000)) continue;
        ++count;
        
        double c1=point->first - avgX;
        double c2=point->second -avgY;
        s1+=c1*c2;
        s2+=c1*c1;
    }
    double slope = s1/s2;
    
    FILE *slopeFile=fopen("~/slope.dat","w");
    fprintf(slopeFile,"%lf", slope);
    fclose(slopeFile);

    for (auto point:points) {
        delete point;
    }
    points.clear();
}


// static void moveOneClockwise(GtkWidget *widget, gpointer data) {
//   moveRotor(wobbleLimit);
// }
static void moveTenCounterClockwise(GtkWidget *widget, gpointer data) {
  moveRotor(-10);
}
static void moveTenClockwise(GtkWidget *widget, gpointer data) {
  moveRotor(10);
}

// static void redraw(GtkWidget *widget, gpointer data) {
//       gtk_widget_set_margin_top(widget, 0);
//   gtk_widget_set_margin_left(widget,  0);
//     createDrawingSurface(drawingArea);
// }

static void moveTo(GtkWidget *widget, gpointer data) {
  int *direction = (int*)data;
  float newDirection=translateDisplay2Rotor(*direction);
  logger.debug("directon=%d", newDirection);

  moveRotor(newDirection-rotorDegree);

}

static void drawCompass(bool newSurface) {
//   if (!newSurface) {
//     displayLock.lock();
//   }
    
  cairo_t *cr;
  guint width, height;
  // GdkRGBA color;
  // GtkStyleContext *context;
  // context = gtk_widget_get_style_context (drawingArea);

  int windowWidth  = gtk_widget_get_allocated_width(drawingArea);
  int windowHeight = gtk_widget_get_allocated_height (drawingArea);

  int marginLeft = gtk_widget_get_margin_left(drawingArea);
  int marginTop  = gtk_widget_get_margin_top(drawingArea);

  width = windowWidth - marginLeft;
  height = windowHeight - marginTop;

  // logger.info("drawCompas::window <%d,%d>", windowWidth, windowHeight);
  // logger.info("drawCompas::margin <%d,%d>", marginLeft, marginTop);


  gdouble x=width/2.0-1;
  gdouble y=height/2.0-1;

  int radius = x-2;
  auto degree=rotorDegree+90;
  if (degree<0) {
      degree=360-degree;
  }

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

  cairo_set_source_rgb (cr,  1,1,1);
  cairo_arc(cr, x, y, radius, 0,2*M_PI);
  cairo_fill(cr);

  cairo_set_source_rgb (cr, 0,0,0);

  cairo_arc(cr, x, y, 3, 0,2*M_PI);
  cairo_fill(cr);

  cairo_arc(cr, x, y, x-2, 0,2*M_PI);
  cairo_stroke(cr);

  radius-=5;
  double xPoint = radius * cos(degree * M_PI / 180.0);
  double yPoint = radius * sin(degree * M_PI / 180.0);

  double eXPoint = xPoint;
  double eYPoint = yPoint;

  cairo_move_to(cr,x,y);
  cairo_line_to(cr, x+eXPoint, y+eYPoint);
  cairo_close_path (cr);
  cairo_stroke(cr);

  radius-=(8*(x/150));
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
  
  cairo_destroy(cr);
//   displayLock.unlock();

}

static void createDrawingSurface(GtkWidget *widget) {
//   displayLock.lock();

  if (surface) {
    cairo_surface_destroy (surface);
  }
//   auto windowWidth  = gtk_widget_get_allocated_width(widget);
//   auto windowHeight = gtk_widget_get_allocated_height(widget);

  auto mTop  = (ignoreMargins)?0:gtk_widget_get_margin_top(widget);
  auto mLeft = (ignoreMargins)?0:gtk_widget_get_margin_left(widget);
  auto width  = gtk_widget_get_allocated_width(widget)+mLeft;
  auto height = gtk_widget_get_allocated_height(widget)+mTop;
  auto window = gtk_widget_get_window(widget);

  gtk_widget_set_margin_top(widget, 0);
  gtk_widget_set_margin_left(widget,  0);


  
  if (width>height) {
    int margin = (width-height)/2.0+0.5;
    width=height;
    if (!ignoreMargins) gtk_widget_set_margin_left(widget, margin);
  } else if (height>width) {
    int margin = (height-width)/2.0+0.5;
    height=width;
    if (!ignoreMargins) gtk_widget_set_margin_top(widget, margin);
  }


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
//   displayLock.lock();
  drawCompass(true);
//   displayLock.unlock();

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
    if (delta>wobbleLimit || forceCompassRedraw) {
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
      updateTextBox(translateRotor2Display(currDegree), false);
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

void voltageProcess(float volts) {
    
}


void voltageCatcher() {
    logger.debug("init voltage catcher; channel=%d", options.aspectVoltageChannel);
  
    float lastDegree=999;
    float lastWindowValue=-1;
    float lastValue = -1;

    bool  limitReached=false;
    vector<float> slidingVolts;

    auto start = currentTimeMillis();

    bool first=true;

    long long lastStatic=0;

    while (true) {
        
        float volts = readVoltage(a2dHandle, options.aspectVoltageChannel, 0);

        if (volts>options.rotorVcc) {
            lastStatic=currentTimeMillis();
        }


        if (volts<0) {
            logger.error("volts read on channel %d is less than zero: %f", options.aspectVoltageChannel, volts);
        }

        if (lastStatic) {
            auto now=currentTimeMillis();
            auto elapsed=now - lastStatic;
            if (elapsed<250) {
                continue;
            } else {
                lastStatic=0;
            }
        } 

        if (capturePoints) {
            auto point = new pair<long long,float>;
            point->first=currentTimeMillis();
            point->second=volts;
            points.push_back(point);
        } 

        if (lastValue<0) {
            lastValue=volts;
            continue;
        } else {
            float pDiff = abs((volts-lastValue)/((volts+lastValue)/2));

            if (!parked && logFile) {
                fprintf(logFile, "%lld,curr=%f,last=%f,pDiff=%f\n", 
                    currentTimeMillis(), volts, lastValue, pDiff);
            }

            if (pDiff>0.25) {
                lastValue=volts;
                continue;
            }
        }


        slidingVolts.push_back(volts);

        if (!limitReached) {
            if (slidingVolts.size()<options.windowSize) {
                usleep(options.catcherDelay*1000);
                continue;
            }
            limitReached=true;
        }

        if (first) {
            auto end = currentTimeMillis();
            first=false;
            long delta = end - start;
            logger.debug("time to fill window<%d>: %ld", options.windowSize, delta);
        } 
        float totalVolts=0;

        slidingVolts.erase(slidingVolts.begin());

        for (auto v:slidingVolts) totalVolts+=v;

        volts=totalVolts/options.windowSize;
        lastValue=volts;


        if (lastWindowValue == volts && !forceVoltageDisplay) {
            usleep(options.catcherDelay*1000);
            continue;
        }
        lastWindowValue=volts;

        float rotorVcc;
        if (options.useAspectReferenceVoltageChannel) {
            rotorVcc = readVoltage(a2dHandle, options.aspectReferenceVoltageChannel, 0);
        } else {
            rotorVcc = options.rotorVcc;
        }
        if (rotorVcc<3) {
            logger.error("voltage reference is less then 3 volts, should be ~5 volts");
            continue;
        }

        float  rotorVout = (rotorVcc*options.aspectVariableResistorOhms) / 
                    (options.aspectFixedResistorOhms+options.aspectVariableResistorOhms);

        float newDegree = 360.0 * (volts/rotorVout);

        if (abs(newDegree)>360) {
            newDegree=360;
        }
        if (newDegree<0) {
            newDegree=0;
        }
        rotorDegree=newDegree;

        if (abs(newDegree-lastDegree)>wobbleLimit || forceVoltageDisplay) {
            forceVoltageDisplay=false;
            logger.debug("ch[0]=%.3f newDegree=%.1f displayDegree=%.1f", 
                            volts, newDegree, translateRotor2Display(newDegree));
            lastDegree=newDegree;
        }

        usleep(options.catcherDelay*1000);
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
    logger.info("useAspectReferenceVoltageChannel:  %s",(options.useAspectReferenceVoltageChannel)?"true":"false");
    logger.info("aspectVoltageChannel:              %d", options.fullscreen);
    logger.info("aspectReferenceVoltageChannel:     %d", options.aspectReferenceVoltageChannel);
    logger.info("aspectVariableResistorOhms:        %d", options.aspectVariableResistorOhms);
    logger.info("aspectFixedResistorOhms:           %d", options.aspectFixedResistorOhms);
    logger.info("limitSwitchPin:                    %d", options.limitSwitchPin);
    logger.info("catchderDelay:                     %d ms", options.catcherDelay);

    if (!options.useAspectReferenceVoltageChannel) {
        logger.info("rotorVcc:                          %f", options.rotorVcc);

        float  rotorVout = (options.rotorVcc*options.aspectVariableResistorOhms) / 
                (options.aspectFixedResistorOhms+options.aspectVariableResistorOhms);
        logger.info("max allowed voltage:           %.2f", rotorVout);
    }
}

int main(int argc, char **argv) {
    GtkBuilder *builder;
    GObject    *window;
    GObject    *button;
    GError     *error=nullptr;

	if (!options.commandLineOptions(argc, argv)) {
		exit(1);
	}
    debug=(logger.getGlobalLevel()<=DEBUG);

    displayParameters();

	if (wiringPiSetup() != 0) {
		logger.error("wiringPi setup failed");
		exit(2);
	}
    
    pinMode(options.limitSwitchPin, INPUT);

    if (initRotorMotor()!=0) {
        logger.error("rotor motor initializaion failed");
		exit(4);
    }

    int ledType = WS2811_STRIP_RGB;

    int ret=neopixel_init(ledType, WS2811_TARGET_FREQ, DMA, GPIO_PIN, led_count*2);

    if (ret!=0) {
        fprintf(stderr, "neopixel initialization failed: %s\n", neopixel_error(ret));
        exit(5);
    }

    neopixel_setPixel(operationIndicator, stopColor);
    neopixel_render();

	a2dSetup();

    gtk_init (&argc, &argv);

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
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    button = gtk_builder_get_object (builder, "MoveExactButton");
    g_signal_connect (button, "clicked", G_CALLBACK (moveExact), NULL);

    button = gtk_builder_get_object (builder, "Calibrate");
    g_signal_connect (button, "clicked", G_CALLBACK (calibrate), NULL);

    button = gtk_builder_get_object (builder, "FastReverse");
    g_signal_connect (button, "clicked", G_CALLBACK (moveTenCounterClockwise), NULL);

    button = gtk_builder_get_object (builder, "FastForward");
    g_signal_connect (button, "clicked", G_CALLBACK (moveTenClockwise), NULL);


    button = gtk_builder_get_object (builder, "quit");
    g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);

    setButton(builder, "northButton", "clicked", G_CALLBACK(moveTo), &directional.north);
    setButton(builder, "southButton", "clicked", G_CALLBACK(moveTo), &directional.south);
    setButton(builder, "eastButton", "clicked", G_CALLBACK(moveTo),  &directional.east);
    setButton(builder, "westButton", "clicked", G_CALLBACK(moveTo),  &directional.west);

    setButton(builder, "seButton", "clicked", G_CALLBACK(moveTo), &directional.se);
    setButton(builder, "swButton", "clicked", G_CALLBACK(moveTo), &directional.sw);
    setButton(builder, "nwButton", "clicked", G_CALLBACK(moveTo),  &directional.nw);
    setButton(builder, "neButton", "clicked", G_CALLBACK(moveTo),  &directional.ne);

    degreeInputBox = (GtkWidget *) gtk_builder_get_object (builder, "DegreeInputBox");
    drawingArea = (GtkWidget *) gtk_builder_get_object (builder, "CompassDrawingArea");
    
    createDrawingSurface(drawingArea);
    g_signal_connect (drawingArea,"configure-event", G_CALLBACK (configure_event_cb), NULL);
    g_signal_connect (drawingArea, "draw", G_CALLBACK (draw_cb), NULL);
    g_signal_connect (G_OBJECT(window), "window-state-event", G_CALLBACK(manualScreenRedraw), NULL);


    if (options.fullscreen) {
        logger.info("entering full screen mode");
        gtk_window_fullscreen(GTK_WINDOW(window));
    }
    
    
    thread(voltageCatcher).detach();
    thread(renderCompass).detach();
    thread(initRotorDegrees).detach();

    gtk_main();

    return 0;
}
// #pragma clang diagnostic pop
#pragma GCC diagnostic pop
