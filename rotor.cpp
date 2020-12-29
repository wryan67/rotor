
#include <gtk/gtk.h>
#include <log4pi.h>
#include <math.h>
#include <thread>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define BASE 200
#define SPI_CHAN 0
#define MCP3008_SINGLE  8
#define MCP3008_DIFF    0

int spiSpeed    = 1000000;

static volatile int  spiHandle = -1;
static volatile bool sampelingActive = false;
static volatile int  spiChannel = 0;
static volatile int  channelType= MCP3008_SINGLE;
static volatile double refVolts = 3.3;


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


using namespace std;
using namespace common::utility;
using namespace common::synchronized;

Logger logger("main");
GtkWidget *drawingArea;

float actualRotorDegree=999;
float rotorDegree=0;
mutex displayLock;
mutex updateTextLock;

GtkWidget *degreeInputBox;

static cairo_surface_t *surface = NULL;
static void drawCompass(bool newSurface);
static void createDrawingSurface(GtkWidget *widget);

char degreeTextBox[32];
bool ignoreMargins=false;


struct directionalType {
  int east, se, south, sw, west, nw, north, ne;
} directional = {
  0, 45, 90, 135, 180, 225, 270, 315
};

void loadSpiDriver()
{
	if (system("gpio load spi") == -1)
	{
		fprintf(stderr, "Can't load the SPI driver: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void spiSetup()
{
	if ((spiHandle = wiringPiSPISetup(spiChannel, spiSpeed)) < 0)
	{
		fprintf(stderr, "opening SPI bus failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}


void updateTextBox(float degree, bool forceRedraw) {
  updateTextLock.lock();
  
  sprintf(degreeTextBox,"%.1f", degree);
  gtk_entry_set_text(GTK_ENTRY(degreeInputBox),degreeTextBox);
  
  updateTextLock.unlock();
}

static void moveRotor(float degrees) {
  auto newDegree=rotorDegree+degrees;
  if (newDegree<0) {
    newDegree+=360;
  }
  if (newDegree>=360) {
    newDegree-=360;
  }

  if (newDegree==rotorDegree) {
    return;
  }
  rotorDegree=newDegree;
  if (degrees>0) {
    logger.info("Move to %.1f; <<moving counter-clockwise>>",newDegree);
  } else {
    logger.info("Move to %.1f; <<moving counter-clockwise>>",newDegree);
  }
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
      moveRotor(d-rotorDegree);
      logger.info("Move to %.1f", d);
      updateTextBox(d, false);
      return;
    }
    throw (runtime_error("unknown error"));
  } catch (runtime_error &e) {
    logger.error("invalid degree entered in text box %s; caused by %s", raw, e.what());
  } catch (...) {
    logger.error("invalid degree entered in text box %s", raw);
  }
  updateTextBox(rotorDegree, false);

}




static void moveOneCounterClockwise(GtkWidget *widget, gpointer data) {
  moveRotor(-1);
}
static void moveOneClockwise(GtkWidget *widget, gpointer data) {
  moveRotor(1);
}
static void moveTenCounterClockwise(GtkWidget *widget, gpointer data) {
  moveRotor(-10);
}
static void moveTenClockwise(GtkWidget *widget, gpointer data) {
  moveRotor(10);
}

static void redraw(GtkWidget *widget, gpointer data) {
      gtk_widget_set_margin_top(widget, 0);
  gtk_widget_set_margin_left(widget,  0);

    createDrawingSurface(drawingArea);

}

static void moveTo(GtkWidget *widget, gpointer data) {
  int *direction = (int*)data;
  logger.info("directon=%d", *direction);

  moveRotor(*direction-rotorDegree);

}

static void drawCompass(bool newSurface) {
  if (!newSurface) {
    displayLock.lock();
  }
    
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
  auto degree=rotorDegree;


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
  displayLock.unlock();

}

static void createDrawingSurface(GtkWidget *widget) {
  displayLock.lock();

  if (surface) {
    cairo_surface_destroy (surface);
  }
  auto windowWidth  = gtk_widget_get_allocated_width(widget);
  auto windowHeight = gtk_widget_get_allocated_height(widget);

  auto mTop  = (ignoreMargins)?0:gtk_widget_get_margin_top(widget);
  auto mLeft = (ignoreMargins)?0:gtk_widget_get_margin_left(widget);
  auto width  = gtk_widget_get_allocated_width(widget)+mLeft;
  auto height = gtk_widget_get_allocated_height(widget)+mTop;
  auto window = gtk_widget_get_window(widget);

  gtk_widget_set_margin_top(widget, 0);
  gtk_widget_set_margin_left(widget,  0);


  logger.info("cds::window <%d,%d>", windowWidth, windowHeight);
  logger.info("cds::margins<%d,%d>", mTop, mLeft);
  
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
  displayLock.lock();
  drawCompass(true);
  displayLock.unlock();

  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint_with_alpha (cr, 1);

  return FALSE;
}


void renderCompass() {
  while (drawingArea==nullptr) {
    usleep(50*1000);
  }
  float lastDegree=999;
  while (true) {
    auto currDegree=rotorDegree;
    auto min=currDegree-0.5;
    auto max=currDegree+0.5;
    if (lastDegree<min || lastDegree>max) {
      lastDegree=currDegree;
      try {
        gtk_widget_queue_draw(drawingArea);
        gtk_widget_show(drawingArea);
      } catch (std::exception &e) {
        logger.warn("%s",e.what());
      } catch (...) {
        logger.warn("unhandled exception in renderCompass");
      }
      usleep(60*1000);
      updateTextBox(currDegree, false);
    }
  }
}

gboolean manualScreenRedraw(GtkWidget *widget, GdkEventWindowState *event, gpointer user_data) {

    logger.info("manual screen redraw");


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

unsigned int readChannel(int channel)
{
	if (0 > channel || channel > 7) {
		return -1;
	}

	unsigned char buffer[3] = { 1 }; 
	buffer[1] = (channelType + channel) << 4;

	wiringPiSPIDataRW(spiChannel, buffer, 3);

	return ((buffer[1] & 3) << 8) + buffer[2]; 
}



void voltageCatcher(int channel) {
  logger.tag(100,"init voltage catcher");

  // mcp3004Setup (BASE, SPI_CHAN) ; // 3004 and 3008 are the same 4/8 channels

  logger.tag(110,"voltageCatcher::starting loop");

  int lastValue=-1;
  while (true) {

    int bits = readChannel(channel);
    if (lastValue != bits) {
      double volts = (bits*refVolts) / 1024.0;
      rotorDegree = 360 * (volts/(1.1));
      logger.info("ch[0]=%5.3f rotorDegree=%3.0f",volts, rotorDegree);
      lastValue = bits;
    }
    delay(1000);
  }

}

int main(int argc, char **argv) {
  GtkBuilder *builder;
  GObject *window;
  GObject *button;
  GError *error = NULL;

	int loadSpi = false;
	int selectedChannel = 1;

	if (loadSpi == true) {
		loadSpiDriver();
	}
	
	
	if (wiringPiSetup() != 0) {
		fprintf(stderr, "wiringPi setup failed\n");
		exit(1);
	}

	spiSetup();

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

  button = gtk_builder_get_object (builder, "MoveLeftButton");
  g_signal_connect (button, "clicked", G_CALLBACK (moveOneCounterClockwise), NULL);

  button = gtk_builder_get_object (builder, "MoveRightButton");
  g_signal_connect (button, "clicked", G_CALLBACK (moveOneClockwise), NULL);

  button = gtk_builder_get_object (builder, "FastReverse");
  g_signal_connect (button, "clicked", G_CALLBACK (moveTenCounterClockwise), NULL);

  button = gtk_builder_get_object (builder, "FastForward");
  g_signal_connect (button, "clicked", G_CALLBACK (moveTenClockwise), NULL);

  button = gtk_builder_get_object (builder, "Redraw");
  g_signal_connect (button, "clicked", G_CALLBACK (redraw), NULL);

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

  thread compass(renderCompass);

  g_signal_connect( G_OBJECT(window), "window-state-event", G_CALLBACK(manualScreenRedraw), NULL);

  logger.info("argc=%d", argc);

  if (argc>1 && !strcmp(argv[1],"-f")) {
    gtk_window_fullscreen(GTK_WINDOW(window));
  }

  thread(voltageCatcher,0).detach();

  gtk_main();

  return 0;
}
#pragma clang diagnostic pop
#pragma GCC diagnostic pop