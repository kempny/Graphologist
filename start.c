/****************************************************
start numer_portu
*****************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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


/*  zmienne  */
time_t czas1;
time_t czas2;
struct tm *loctime;
#define PRZERWA 100000 // 0,1 sekundy
#define TIMEOUT 60 // 60 sekund na podpis
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
static cairo_surface_t *surface = NULL;

int sockfd, newsockfd, portno, clilen;
int sign();
int sendpng();


/* S T A R T */
void main (argc, argv)
     int argc;
     char *argv[];
{




    if (argc != 2)
     {
     printf( "Musisz podac numer portu, np.\n ./start 5977\n");
     exit(1);
     }

//  zaprogramuj socket

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
/*
// odbierz numer dokumentu
     m=0;
     jest=0;
     buf[m]='0';
     time (&czas1);
     time (&czas2);

     while (czas2 - czas1 < 3) // 3 sekundy na nazwe miasta, potem timeout
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
    strcpy(numer, buf);

    if (jest == 0)
     {
      close(newsockfd);
      goto listen;
     }
*/
// odbierz tresc
     m=0;
     jest=0;
     buf[m]='0';
     time (&czas1);
     time (&czas2);

     while (czas2 - czas1 < 3) // 3 sekundy na tresc, potem timeout
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
    if (jest == 0)
     {
      close(newsockfd);
      goto listen;
     }
strcpy(tresc, buf);
// odbierz miasto
     m=0;
     jest=0;
     buf[m]='0';
     time (&czas1);
     time (&czas2);

     while (czas2 - czas1 < 3) // 3 sekundy na tresc, potem timeout
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
    if (jest == 0)
     {
      close(newsockfd);
      goto listen;
     }
strcpy(miasto, buf);
// odbierz imie
     m=0;
     jest=0;
     buf[m]='0';
     time (&czas1);
     time (&czas2);

     while (czas2 - czas1 < 3) // 3 sekundy na tresc, potem timeout
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
    if (jest == 0)
     {
      close(newsockfd);
      goto listen;
     }
strcpy(imie, buf);
// odbierz nazwisko
     m=0;
     jest=0;
     buf[m]='0';
     time (&czas1);
     time (&czas2);

     while (czas2 - czas1 < 3) // 3 sekundy na tresc, potem timeout
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
    if (jest == 0)
     {
      close(newsockfd);
      goto listen;
     }
strcpy(nazwisko, buf);

// odpal program w okienku
     podpisane=0;
     ret=sign(tresc, miasto, imie, nazwisko);

// kod exit = 1 - podpisano, 2 - anulowan, 3 - timeout:
    
     sprintf(buf, "%02d\n", ret); 
     write(newsockfd, buf, strlen(buf));
// wyslij do programu glownego
     if(ret==1)
        sendpng();
     shutdown(newsockfd,2);
     close(newsockfd);
  } 
} //od main




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
char buffer[100];
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
sprintf(tekst, "%s, dnia %s", miasto, buffer);
cairo_show_text (cr, tekst);

cairo_move_to (cr, 1, 38);
fp = fopen (tresc2, "r"); 
fgets (buffer, 100, fp);
fclose(fp);


cairo_set_font_size (cr, 20.2);
buffer[strlen(buffer)-1] = '\x00'; // usun 0A
strncpy(tekst2, buffer, 60);  // do wyswietlania pozniej przy 'popraw'
strcpy(tekst3, &buffer[60]);  // do wyswietlania pozniej przy 'popraw'
cairo_show_text (cr, tekst2);
cairo_move_to (cr, 1, 58);
cairo_show_text (cr, tekst3);

cairo_move_to (cr, 1, 80);
cairo_set_font_size (cr, 18.2);
sprintf(tekst, "%s %s", imie, nazwisko);
cairo_show_text (cr, tekst);

cairo_destroy (cr);
      gtk_widget_queue_draw (widget);


  /* We've handled the configure event, no need for further processing. */
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

  /* Paint to the surface, where we store our state */
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 0, 0.0, 1.0);
  cairo_rectangle (cr, x - 1, y - 1, 2, 2);
  cairo_fill (cr);
  cairo_destroy (cr);
  gtk_widget_queue_draw_area (widget, x - 1, y - 1, 2, 2);

  clock_gettime(CLOCK_REALTIME, &tim1);
  if((tim1.tv_sec-tim.tv_sec)*1000000 + (tim1.tv_nsec - tim.tv_nsec)/1000 < PRZERWA) //microsec
   {
    /* wyswietl punkty pomiedzy */
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

  /* We've handled it, stop processing */
  return TRUE;
}

static void
close_window (void)
{
  if (surface)
    cairo_surface_destroy (surface);
}

static gboolean
popraw (GtkWidget      *widget)
    {
char tekst[100];
char buffer[100];
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
sprintf(tekst, "%s, dnia %s", miasto, buffer);
cairo_show_text (cr, tekst);

cairo_move_to (cr, 1, 38);

cairo_set_font_size (cr, 20.2);
cairo_show_text (cr, tekst2);
cairo_move_to (cr, 1, 58);
cairo_show_text (cr, tekst3);

cairo_move_to (cr, 1, 80);
cairo_set_font_size (cr, 18.2);
sprintf(tekst, "%s %s", imie, nazwisko);
cairo_show_text (cr, tekst);

cairo_destroy (cr);
      gtk_widget_queue_draw (widget);

      return TRUE;
    }
int sendpng()
{
/* wysyla obraz w blokach po maksymalnie 100 bajtow, dodatkowo piewszy bajt 
   to liczba bajtow tresci i ostatni - xor bajtow tresci.
   Koniec transmisji - blok bez tresci, tylko pierwszy bajt = 0  */
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
//printf("count:%02x ", ccount);fflush(0);
   for(ii=0; ii<count; ii++)
    {
     crc=crc^buf[ii];
     write(newsockfd, &buf[ii],1);
//printf("znak:%02x ", buf[ii]);fflush(0);
    }
   write(newsockfd, &crc, 1);
//printf("crc_sent:%02x ", crc);fflush(0);
  }
}

static gboolean
wyslij (GtkWidget      *widget, GtkWindow *window)
    {
char tekst[100];
char buffer[100];

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
  strcpy(tekst, "Wysyłam do serwera");
else
  strcpy(tekst, "Brak podpisu, powtórz");
cairo_show_text (cr, tekst);
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
sprintf(tekst, "%s, dnia %s", miasto, buffer);
cairo_show_text (cr, tekst);

cairo_move_to (cr, 1, 35);

cairo_set_font_size (cr, 14.2);
cairo_show_text (cr, tekst2);

cairo_move_to (cr, 1, 55);
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
  GtkTextBuffer *buffer;
  GObject *button;
  GFile   *file;
  gchar *contents;
  gsize length;
  GtkStyleContext *context;
  GtkTextTag *tag;
  GtkTextIter start, end;

  cairo_t *cr;
  int status;

  strcpy(tresc1, tresc);
  strcpy(tresc2, tresc);
  strcat(tresc1, ".txt");
  strcat(tresc2, ".osw");

  clock_gettime(CLOCK_REALTIME, &tim);
  gtk_init (NULL, NULL);

  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "templ.glade", &error) == 0)
    {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      return 1;
    }

window = gtk_builder_get_object (builder, "window");
gtk_window_set_title (GTK_WINDOW (window), "Grafolog 1.0");
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

GtkCssProvider *provider = gtk_css_provider_new();
context = gtk_widget_get_style_context (view);
gtk_style_context_add_provider (context,
                                GTK_STYLE_PROVIDER (provider),
                                GTK_STYLE_PROVIDER_PRIORITY_USER);
gtk_css_provider_load_from_data (provider,
                                 "textview {"
                                 "  color: red;"
                                 " font: 20px serif;"
                                 " background: red;"
                                 "}",
                                 -1,
                                 NULL);

/* Change left margin throughout the widget */
gtk_text_view_set_left_margin (GTK_TEXT_VIEW (view), 3);

/* Use a tag to change the color for just one part of the widget */
//tag = gtk_text_buffer_create_tag (buffer, "blue_foreground",
//                        "foreground", "blue", NULL);  
//gtk_text_buffer_get_iter_at_offset (buffer, &start, 7);
//gtk_text_buffer_get_iter_at_offset (buffer, &end, 12);
//gtk_text_buffer_apply_tag (buffer, tag, &start, &end);
g_free (contents);

  podpis = gtk_builder_get_object (builder, "podpis");
  drawing_area = gtk_drawing_area_new ();

  gtk_widget_show (drawing_area);

  /* Signals used to handle the backing surface */
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
//  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);

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

