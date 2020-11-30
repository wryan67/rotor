#include <gtk/gtk.h>
#include <log4pi.h>
#include <math.h>
#include <thread>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


using namespace std;
using namespace common::utility;
using namespace common::synchronized;

Logger logger("main");
GtkWidget *drawingArea;

float rotorDegree=0;
mutex displayLock;

GtkWidget *degreeInputBox;

static cairo_surface_t *surface = NULL;
static void drawCompass(bool newSurface);


static void moveExact(GtkWidget *widget, gpointer data) {
  g_print("Move to\n");
}

static void moveCounterClockwise(GtkWidget *widget, gpointer data) {
  auto newDegree=rotorDegree-1;
  if (newDegree<0) {
    newDegree+=360;
  }
  rotorDegree=newDegree;
  g_print("Move counter clockwise\n");
}

static void moveClockwise(GtkWidget *widget, gpointer data) {
  auto newDegree=rotorDegree+1;
  if (newDegree>359) {
    newDegree-=360;
  }
  rotorDegree=newDegree;
  g_print("Move clockwise\n");
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

  width = gtk_widget_get_allocated_width(drawingArea) - gtk_widget_get_margin_left(drawingArea);
  height = gtk_widget_get_allocated_height (drawingArea) - gtk_widget_get_margin_top(drawingArea);

  gdouble x=width/2.0-1;
  gdouble y=height/2.0-1;

  int radius = x-2;
  auto degree=rotorDegree;


  /* Paint to the surface, where we store our state */
  cr = cairo_create(surface);

  cairo_set_source_rgb (cr, 246/255.0, 245/255.0, 244/255.0);
  cairo_paint(cr);

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
  

  
  /* Now invalidate the affected region of the drawing area. */
  cairo_destroy(cr);
  // gtk_widget_queue_draw_area(drawingArea, 0, 0, width-1, height-1);
  // gtk_widget_queue_draw(drawingArea);
  // gtk_widget_show(drawingArea);
  displayLock.unlock();

}

void createDrawingSurface(GtkWidget *widget) {
  displayLock.lock();

  if (surface) {
    cairo_surface_destroy (surface);
  }
  auto mTop  = gtk_widget_get_margin_top(widget);
  auto mLeft = gtk_widget_get_margin_left(widget);
  auto width  = gtk_widget_get_allocated_width(widget)+mLeft;
  auto height = gtk_widget_get_allocated_height(widget)+mTop;
  auto window = gtk_widget_get_window(widget);

  gtk_widget_set_margin_top(widget, 0);
  gtk_widget_set_margin_left(widget,  0);


  if (width>height) {
    int margin;
    margin=(width-height)/2.0+0.5;

    logger.info("width=%d(%d) height=%d(%d) margin=%d",width,mLeft,height,mTop,margin);
    width=height;
    gtk_widget_set_margin_left(widget, margin);
  } else if (height>width) {
    int margin = (height-width)/2.0+0.5;
    height=width;
    gtk_widget_set_margin_top(widget, margin);
  }

  surface = gdk_window_create_similar_surface(window, CAIRO_CONTENT_COLOR, width, height);

  drawCompass(true);
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
  auto lastDegree=-1;
  while (true) {
    if (lastDegree!=rotorDegree) {
      lastDegree=rotorDegree;
      try {
        gtk_widget_queue_draw(drawingArea);
        gtk_widget_show(drawingArea);
      } catch (std::exception &e) {
        logger.warn("%s",e.what());
      } catch (...) {
        logger.warn("unhandled exception in renderCompass");
      }
      usleep(60*1000);
      logger.info("degree=%d",lastDegree);
    }
  }
}

int main(int argc, char **argv) {
  GtkBuilder *builder;
  GObject *window;
  GObject *button;
  GError *error = NULL;

  gtk_init (&argc, &argv);

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
  g_signal_connect (button, "clicked", G_CALLBACK (moveCounterClockwise), NULL);

  button = gtk_builder_get_object (builder, "MoveRightButton");
  g_signal_connect (button, "clicked", G_CALLBACK (moveClockwise), NULL);

  button = gtk_builder_get_object (builder, "quit");
  g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);

  degreeInputBox = (GtkWidget *) gtk_builder_get_object (builder, "DegreeInputBox");

  drawingArea = (GtkWidget *) gtk_builder_get_object (builder, "CompassDrawingArea");
 
  createDrawingSurface(drawingArea);
  g_signal_connect (drawingArea,"configure-event", G_CALLBACK (configure_event_cb), NULL);
  g_signal_connect (drawingArea, "draw", G_CALLBACK (draw_cb), NULL);

  thread compass(renderCompass);


  gtk_main();

  return 0;
}
#pragma clang diagnostic pop
#pragma GCC diagnostic pop