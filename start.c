/****************************************************
start numer_portu
*****************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <math.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>

#define _(str) gettext (str)

/*  zmienne  */
time_t czas1;
time_t czas2;
struct tm *loctime;
#define PRZERWA 100000 // 0,1 s
#define TIMEOUT 60 // 60 seconds to sign
int tmoutid;

char buf[200];
char numer[100];
char imie[100];
char nazwisko[100];
char miasto[100];
char tresc[100];
int n,m,i,j,jest, firma, ret;
time_t czas;
char data[55];
struct tm *loctime;

double xx;
double yy;
int podpisane = 0;
struct tm *loctime;
time_t czasp, czasb;
struct timespec tim, tim1;
char numer[100];
char tresc1[100];
char tresc2[100];
char tekst2[100];
char tekst3[100];
char country[15];
char country_mark_s[7];
char country_mark_e[7];
char saved_country[15];
static cairo_surface_t *surface = NULL;

int sockfd, newsockfd, portno, clilen;
int sign();
int sendpng();
int odbierz();


/* S T A R T */
void main (argc, argv)
     int argc;
     char *argv[];
{
// save default environment locale
    setlocale(LC_ALL,"");
    strcpy(saved_country, setlocale(LC_ALL,NULL));

    if (argc != 2)
     {
     printf( _("Usage: ./start port_number, e.g.\n ./start 5089\n"));
     exit(1);
     }
    
//  socket programming

    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
 
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }
listen:
      listen(sockfd,1);
      clilen = sizeof(cli_addr);
    /* Przyjmij polaczenie klienta */
   while (TRUE)
    {
po_interrupt:
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, 
                                &clilen);
      if (newsockfd < 0) 
      {
        goto po_interrupt;
      }
      int flags = fcntl(newsockfd, F_GETFL, 0);
      fcntl(newsockfd, F_SETFL, flags |O_RDWR | O_NONBLOCK  | O_NDELAY);

// locale
      jest = odbierz(country);
      if (jest == 0)
        {
         close(newsockfd);
         goto listen;
        }
      setlocale(LC_ALL,country);
      bindtextdomain ("start", getenv("PWD"));
      textdomain ("start");
      if (country[0] == 'C')
       {
        sprintf(country_mark_s, "<en>\n");
        sprintf(country_mark_e, "</en>\n");
       }
      else
       {
        sprintf(country_mark_s, "<%c%c>\n", country[0], country[1]);
        sprintf(country_mark_e, "</%c%c>\n", country[0], country[1]);
       }

// content
      jest = odbierz(tresc);
      if (jest == 0)
       {
        close(newsockfd);
        goto listen;
       }

// city
      jest = odbierz(miasto);
      if (jest == 0)
       {
        close(newsockfd);
        goto listen;
       }

// first name
      jest = odbierz(imie);
      if (jest == 0)
       {
        close(newsockfd);
        goto listen;
       }

// last name
      jest = odbierz(nazwisko);
      if (jest == 0)
       {
        close(newsockfd);
        goto listen;
       }
// display window to sign
      podpisane=0;
      ret=sign(tresc, miasto, imie, nazwisko);
  
// exit code = 1 - signed, 2 - canceled, 3 - timeout:
    
      sprintf(buf, "%02d\n", ret); 
      write(newsockfd, buf, strlen(buf));
// send to client program
      if(ret==1)
       sendpng();
      shutdown(newsockfd,2);
      close(newsockfd);
      setlocale(LC_ALL,saved_country);
    } 
} // main

int odbierz(char* tekst)
{
char buf[100];
     m=0;
     jest=0;
     buf[m]='0';
     time (&czas1);
     time (&czas2);

     while (czas2 - czas1 < 3) // 3 seconds,  timeout
      {
         if ((read (newsockfd, &buf[m], 1)) != -1)
           {
            if (buf[m] == 0x0a)
             {
               buf[m]=0x0;
               jest=1;
               break;
             }
            m++;
           }
         time (&czas2);
      }
    strcpy(tekst, buf);
    return(jest);
}

static void
clear_surface (void)
{
  cairo_t *cr;
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);
  cairo_destroy (cr);
}

/* Create a new surface of the appropriate size to store our scribbles */
static gboolean
configure_event_cb (GtkWidget         *widget,
                    GdkEventConfigure *event,
                    gpointer           data)
{
char tekst[100];

char buffer[256];
FILE *fp;

  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                        CAIRO_CONTENT_COLOR,
                                        gtk_widget_get_allocated_width (widget),
                                        gtk_widget_get_allocated_height (widget));
  /* Initialize the surface to white */
 cairo_t *cr;
    clear_surface ();
 cr = cairo_create (surface);
 cairo_select_font_face (cr, "FreeSans",
    CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
 cairo_move_to (cr, 1, 15);
 cairo_set_font_size (cr, 18.2);
 time(&czasb);
 loctime = localtime (&czasb);
 strftime (buffer, 50, "%d-%m-%Y  %H:%M", loctime);
 sprintf(tekst, "%s, %s", miasto, buffer);
 cairo_show_text (cr, tekst);
 cairo_move_to (cr, 1, 38);

 fp = fopen (tresc2, "r"); 
// find starting mark of language
 strcpy(buffer,"");
 jest=0;
  while (fgets (buffer, 100, fp) != NULL)
    if (strcmp (buffer, country_mark_s) == 0)
      {
       jest = 1;
       break;
      } 
// read the statement
 if (jest ==1)
   {
    fgets (buffer, 256, fp);
    fclose(fp);
   }
 else
   {   //language  mark not found
    fclose(fp);
    fp = fopen (tresc2, "r"); 
    while (fgets (buffer, 256, fp) != NULL)
      if (strncmp (buffer, "<",1 ) == 0)
      {
       jest = 1;
       break;
      } 
    if (jest ==1)
     {
      fgets (buffer, 256, fp);
      fclose(fp);
     }
    else 
     {
       strcpy(buffer,"");
       fclose(fp);
     }
   }

 cairo_select_font_face (cr, "Liberation",
    CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
 cairo_set_font_size (cr, 20.2);
 buffer[strlen(buffer)-1] = '\x00'; // remove 0A
 strncpy(tekst2, buffer, 59);  
 strcpy(tekst3, &buffer[59]);  
 cairo_show_text (cr, tekst2);
 cairo_move_to (cr, 1, 58);
 cairo_show_text (cr, tekst3);

 cairo_move_to (cr, 1, 80);
 cairo_select_font_face (cr, "FreeSans",
     CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
 cairo_set_font_size (cr, 18.2);
 sprintf(tekst, "%s %s", imie, nazwisko);
 cairo_show_text (cr, tekst);
 
 cairo_destroy (cr);
       gtk_widget_queue_draw (widget);
 
 return TRUE;
}
/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean
draw_cb (GtkWidget *widget,
         cairo_t   *cr,
         gpointer   data)
{
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

  return FALSE;
}
/* Draw a rectangle on the surface at the given position */
static void
draw_brush (GtkWidget *widget,
            gdouble    x,
            gdouble    y)
{
double a;
int poczx, poczy;
  cairo_t *cr;

  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 0, 0.0, 1.0);
  cairo_rectangle (cr, x - 1, y - 1, 2, 2);
  cairo_fill (cr);
  cairo_destroy (cr);
  gtk_widget_queue_draw_area (widget, x - 1, y - 1, 2, 2);

  clock_gettime(CLOCK_REALTIME, &tim1);
  if((tim1.tv_sec-tim.tv_sec)*1000000 + (tim1.tv_nsec - tim.tv_nsec)/1000 < PRZERWA) //microsec
   {
    /* draw the line between rectangles */
    cr = cairo_create (surface);
    cairo_set_source_rgb (cr, 0, 0, 1.0);
    cairo_move_to (cr, xx, yy);
    cairo_line_to (cr, x, y);
    cairo_set_line_width (cr, 2);
    cairo_stroke (cr);
    cairo_destroy (cr);
    
    if (x < xx) poczx = x; else poczx = xx;
    if (y < yy) poczy = y; else poczy = yy;
    gtk_widget_queue_draw_area (widget, poczx-2, poczy-2, fabs(x - xx)+4, fabs(y - yy)+4);

    podpisane++;
   }

    clock_gettime(CLOCK_REALTIME, &tim);
    xx = x;
    yy = y;

}

/* Handle button press events by either drawing a rectangle
 * or clearing the surface, depending on which button was pressed.
 * The ::button-press signal handler receives a GdkEventButton
 * struct which contains this information.
 */
static gboolean
button_press_event_cb (GtkWidget      *widget,
                       GdkEventButton *event,
                       gpointer        data)
{
  /* paranoia check, in case we haven't gotten a configure event */
  if (surface == NULL)
    return FALSE;

  if (event->button == GDK_BUTTON_PRIMARY)
    {
      draw_brush (widget, event->x, event->y);
    }
  else if (event->button == GDK_BUTTON_SECONDARY)
    {
    }

  /* We've handled the event, stop processing */
  return TRUE;
}
/* Handle motion events by continuing to draw if button 1 is
 * still held down. The ::motion-notify signal handler receives
 * a GdkEventMotion struct which contains this information.
 */
static gboolean
motion_notify_event_cb (GtkWidget      *widget,
                        GdkEventMotion *event,
                        gpointer        data)
{
  /* paranoia check, in case we haven't gotten a configure event */
  if (surface == NULL)
    return FALSE;

  if (event->state & GDK_BUTTON1_MASK)
    draw_brush (widget, event->x, event->y);

  return TRUE;
}


static gboolean
popraw (GtkWidget      *widget)
    {
char tekst[100];
char buffer[256];
FILE *fp;

  cairo_t *cr;
  podpisane=0;
  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                        CAIRO_CONTENT_COLOR,
                                        gtk_widget_get_allocated_width (widget),
                                        gtk_widget_get_allocated_height (widget));
      clear_surface ();
      cr = cairo_create (surface);
      cairo_select_font_face (cr, "FreeSans",
         CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_move_to (cr, 1, 15);
      cairo_set_font_size (cr, 18.2);
      time(&czasb);
      loctime = localtime (&czasb);
      strftime (buffer, 50, "%d-%m-%Y  %H:%M", loctime);
      sprintf(tekst, "%s, %s", miasto, buffer);
      cairo_show_text (cr, tekst);
      cairo_move_to (cr, 1, 38);
      cairo_select_font_face (cr, "Liberation",
         CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, 20.2);
      cairo_show_text (cr, tekst2);
      cairo_move_to (cr, 1, 58);
      cairo_show_text (cr, tekst3);

      cairo_move_to (cr, 1, 80);
      cairo_select_font_face (cr, "FreeSans",
         CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, 18.2);
      sprintf(tekst, "%s %s", imie, nazwisko);
      cairo_show_text (cr, tekst);

      cairo_destroy (cr);
      gtk_widget_queue_draw (widget);

      return TRUE;
    }

int sendpng()
{
/* send the png to client, in blocks:
   lenght, 100 bytes of data, XOR of data bytes
   end of transmission - lenght = 0 */

int fd;
unsigned char buf[102];
unsigned char crc=0;
ssize_t count=999;
unsigned char ccount;
int ii;

 fd = open("podpis.png", O_RDONLY);
 while (count>0)
  {
   count=read(fd,buf,100);
   ccount = count;
   crc = 0;
   write(newsockfd, &ccount,1);
   for(ii=0; ii<count; ii++)
    {
     crc=crc^buf[ii];
     write(newsockfd, &buf[ii],1);
    }
   write(newsockfd, &crc, 1);
  }
}

static gboolean
wyslij (GtkWidget      *widget, GtkWindow *window)
 {
  char tekst[100];
  char buffer[256];

  cairo_t *cr;
      cairo_surface_write_to_png (surface, "podpis.png");
  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                        CAIRO_CONTENT_COLOR,
                                        gtk_widget_get_allocated_width (widget),
                                        gtk_widget_get_allocated_height (widget));
  clear_surface ();
  cr = cairo_create (surface);
  cairo_select_font_face (cr, "FreeSans",
  CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_move_to (cr, 17, 75);
  cairo_set_font_size (cr, 28.2);
  if (podpisane>50)
    strcpy(tekst, _("Sending to server"));
  else
    strcpy(tekst, _("No signature, repeat"));
  cairo_show_text (cr, gettext(tekst));
  cairo_destroy (cr);

  gtk_widget_queue_draw (widget);
  while (gtk_events_pending ())
        gtk_main_iteration ();
  sleep(2);

  if (podpisane>50)
   {
      gtk_window_close(window);
      while (gtk_events_pending ())
        gtk_main_iteration ();
      gtk_main_quit();
      ret = 1;
      return(ret);
   }
  else
   {
    cairo_t *cr;
      podpisane=0;
    if (surface)
    cairo_surface_destroy (surface);

    surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                        CAIRO_CONTENT_COLOR,
                                        gtk_widget_get_allocated_width (widget),
                                        gtk_widget_get_allocated_height (widget));
    clear_surface ();
    cr = cairo_create (surface);
    cairo_select_font_face (cr, "FreeSans",
       CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to (cr, 1, 15);
    cairo_set_font_size (cr, 18.2);
    time(&czasb);
    loctime = localtime (&czasb);
    strftime (buffer, 50, "%d-%m-%Y  %H:%M", loctime);
    sprintf(tekst, "%s, %s", miasto, buffer);
    cairo_show_text (cr, tekst);

    cairo_select_font_face (cr, "Liberation",
       CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size (cr, 20.2);
    cairo_move_to (cr, 1, 38);
    cairo_show_text (cr, tekst2);
    cairo_move_to (cr, 1, 58);
    cairo_show_text (cr, tekst3);




    cairo_move_to (cr, 1, 80);
    cairo_select_font_face (cr, "FreeSans",
       CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, 18.2);
    sprintf(tekst, "%s %s", imie, nazwisko);
    cairo_show_text (cr, tekst);

    cairo_destroy (cr);
    gtk_widget_queue_draw (widget);
    return TRUE;
   }
 }

static gboolean
tmout (gpointer      window)
    {
      gtk_window_close(window);
      while (gtk_events_pending ())
        gtk_main_iteration ();
      gtk_main_quit();
      ret = 3;
      return FALSE;
    }

static gboolean
anuluj (GtkWindow      *window)
    {
      gtk_window_close(window);
      while (gtk_events_pending ())
        gtk_main_iteration ();
      gtk_main_quit();
      ret = 2;
      return ret;
    }

int
sign (char *tresc, char *miasto, char *imie, char *nazwisko)
{
  GtkApplication *app;
  GtkBuilder *builder;
  GError *error = NULL;
  GtkWidget *drawing_area;
  GtkWidget *frame;
  GObject *window;
  GObject *tekst;
  GObject *podpis;
  GtkWidget *view;
  GtkWidget *text;
  GtkTextBuffer *buffer;
  GObject *button;
  GFile   *file;
  gchar *contents;
  gsize length;
  GtkStyleContext *context;
  GtkStyleContext *context1;
  GtkTextTag *tag;
  GtkTextIter start, end;
  char buf[5000];
  char buf1[5000];

  cairo_t *cr;
  int status;

  strcpy(tresc1, tresc);
  strcpy(tresc2, tresc);
  strcat(tresc1, ".txt");
  strcat(tresc2, ".osw");

  clock_gettime(CLOCK_REALTIME, &tim);
  gtk_init (NULL, NULL);

  setlocale(LC_ALL,country);
//  bindtextdomain ("start", getenv("PWD"));
//  textdomain ("start");

  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "templ.glade", &error) == 0)
    {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      return 1;
    }

  window = gtk_builder_get_object (builder, "window");
  gtk_window_set_title (GTK_WINDOW (window), _("Graphologist 1.1"));
  gtk_window_maximize (GTK_WINDOW (window));

  tekst = gtk_builder_get_object (builder, "tresc");
  view = gtk_text_view_new ();
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  gtk_widget_show (view);
  gtk_container_add(GTK_CONTAINER(tekst) , view);
  file = g_file_new_for_path(tresc1);
  if (g_file_load_contents (file, NULL, &contents, &length, NULL, NULL))
   {
    gtk_text_buffer_set_text (buffer, contents, length);
   }

// cut the text in the right language
  if(strstr(contents, country_mark_s) != NULL &&
     strstr(contents, country_mark_e) != NULL)
   {
     strcpy(buf, strstr(contents, country_mark_s));
     strcpy(contents, strchr(buf, '\n'));
     memset(strstr(contents, country_mark_e), 0,1);
   }
  else
   { 
     strcpy(contents, _("No text in this language!"));
   }
  gtk_text_buffer_set_text (buffer, contents, -1);

  GtkCssProvider *provider = gtk_css_provider_new();
  GtkCssProvider *provider1 = gtk_css_provider_new();
  context = gtk_widget_get_style_context (view);
  context1 = gtk_widget_get_style_context (view);
  gtk_style_context_add_provider (context,
                                GTK_STYLE_PROVIDER (provider),
                                GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_style_context_add_provider (context1,
                                GTK_STYLE_PROVIDER (provider1),
                                GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_css_provider_load_from_data (provider,
                                 "textview {"
                                 " font: 20px serif;"
                                 "}",
                                 -1,
                                 NULL);
  gtk_css_provider_load_from_data (provider1,
                                 "text {"
                                 "  color: black;"
                                 " background-color: lightgrey;"
                                 "}",
                                 -1,
                                 NULL);

  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (view), 3);

  g_free (contents);

  podpis = gtk_builder_get_object (builder, "podpis");
  drawing_area = gtk_drawing_area_new ();

  gtk_widget_show (drawing_area);

  g_signal_connect (drawing_area, "draw",
                    G_CALLBACK (draw_cb), NULL);
  g_signal_connect (drawing_area,"configure-event",
                    G_CALLBACK (configure_event_cb), NULL);

  /* Event signals */
  g_signal_connect (drawing_area, "motion-notify-event",
                    G_CALLBACK (motion_notify_event_cb), NULL);
  g_signal_connect (drawing_area, "button-press-event",
                    G_CALLBACK (button_press_event_cb), NULL);

  gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                     | GDK_BUTTON_PRESS_MASK
                                     | GDK_POINTER_MOTION_MASK);

  gtk_container_add (GTK_CONTAINER (podpis), drawing_area);


  window = gtk_builder_get_object (builder, "window");

  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  button = gtk_builder_get_object (builder, "but1");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (popraw), drawing_area);
  button = gtk_builder_get_object (builder, "but2");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (anuluj), window);
  button = gtk_builder_get_object (builder, "but3");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (wyslij), drawing_area), window;

  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(cssProvider, "./button.css", NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                               GTK_STYLE_PROVIDER(cssProvider),
                               GTK_STYLE_PROVIDER_PRIORITY_USER);
  tmoutid=g_timeout_add_seconds (TIMEOUT, tmout, window ) ;
  gtk_main ();
  g_source_remove(tmoutid);
  return ret;
}

