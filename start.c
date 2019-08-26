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
#define TIMEOUT 600 // 60 seconds to sign
#define FULLSCREEN TRUE  // start in full screen mode
int tmoutid;

char buf[200];
char name[100];
char imie[100];
char nazwisko[100];
char miasto[100];
char tresc[100];
int n,m,i,j,jest, firma, ret;
time_t czas;
char data[55];
struct tm *loctime;

double colr=0, colg=0, colb=1.0; // default line color, rgb
double scolr=1.0, scolg=1.0, scolb=1.0; // default statement text color, rgb
double line_width = 2;           // default pen width

double xx0;
double yy0;
double xx1;
double yy1;
double xx2;
double yy2;
double xx3;
double yy3;
int pointno;

int podpisane = 0;
struct tm *loctime;
time_t czasp, czasb;
struct timespec tim, tim1;
char tresc1[100];
char tresc2[100];
char tekst2[256];
char tekst3[256];
wchar_t wtekst2[100];
wchar_t wtekst3[100];
char country[15];
char country_mark_s[7];
char country_mark_e[7];
char saved_country[15];
static cairo_surface_t *surface = NULL;

int sockfd, newsockfd, portno, clilen;
int sign();
int sendpng();
int odbierz();
void catch_signal();
void interpolate();
void read_parm();

/* S T A R T */
void main (argc, argv)
     int argc;
     char *argv[];
{
    signal(SIGTERM, catch_signal);
    read_parm();

// save default environment locale
    setlocale(LC_ALL,"");
    strcpy(saved_country, setlocale(LC_ALL,NULL));
    bindtextdomain ("start", getenv("PWD"));
    textdomain ("start");

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
//png name 
      jest = odbierz(name);
      if (jest == 0)
       {
        close(newsockfd);
        goto listen;
       }

// locale
      jest = odbierz(country);
      if (jest == 0)
        {
         close(newsockfd);
         goto listen;
        }


      if (strncmp(country, "en_EN",5) == 0) 
        strcpy(country, "C");

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
      podpisane = 0;
      pointno = 0;
      xx0 = 0;
      xx1 = 0;
      xx2 = 0;
      xx3 = 0;
      yy0 = 0;
      yy1 = 0;
      yy2 = 0;
      yy3 = 0;

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

void catch_signal(int signal_num)
{
  shutdown (newsockfd, 2);
  shutdown (sockfd, 2);
  close(newsockfd);
  close(sockfd);
  exit(0);
  
}

void read_parm()
{
  FILE *fp;
  char buf[120];
  char col[10];
  char *ptr;
  int icolr;
  int icolg;
  int icolb;

  fp = fopen("param", "r");

  while (fgets(buf, 120, fp) != NULL)
  {
   if (buf[0] == '#' || strlen(buf) == 1) // comments and empty lines
     continue;

   if (strncmp(buf,"linecolor", strlen("linecolor")) == 0)
    {
     ptr = strchr(buf,'#');
     ptr++;
     strcpy(col, ptr);
     sscanf(col, "%2x%2x%2x", &icolr, &icolg, &icolb);
     colr = (double)icolr /255;
     colg = (double)icolg /255;
     colb = (double)icolb /255;
    }

   if (strncmp(buf,"statementcolor", strlen("statementcolor")) == 0)
    {
     ptr = strchr(buf,'#');
     ptr++;
     strcpy(col, ptr);
     sscanf(col, "%2x%2x%2x", &icolr, &icolg, &icolb);
     scolr = (double)icolr /255;
     scolg = (double)icolg /255;
     scolb = (double)icolb /255;
    }

   if (strncmp(buf,"penwidth", strlen("penwidth")) == 0)
    {
     ptr = strchr(buf,'#');
     ptr++;
     line_width = atof(ptr);
    }
  }
}

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
  cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
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
int width;
cairo_text_extents_t extents;
width = gtk_widget_get_allocated_width (widget);
int i, ii;
char buffer[256];
char buffer1[256];
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
 cairo_set_source_rgb(cr, scolr, scolg, scolb); 
 cairo_set_font_size (cr, 18.2);
 time(&czasb);
 loctime = localtime (&czasb);
 strftime (buffer, 50, "%d-%m-%Y  %H:%M", loctime);
 sprintf(tekst, "%s, %s", miasto, buffer); 
 cairo_show_text (cr, tekst);

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

// Split statement text into separate lines
 cairo_select_font_face (cr, "Liberation",
    CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
 cairo_set_font_size (cr, 20.2);
 buffer[strlen(buffer)-1] = '\x00'; // remove 0A
 strcat(buffer, " ");  
 for(i=0; i <strlen(buffer); i++)
  {
    if(buffer[i] == ' ')
     {
      strncpy(buffer1,  buffer, i);
      buffer1[i] = 0x00;
      cairo_text_extents(cr, buffer1, &extents);
      if (extents.width < width)
        {
          strcpy (tekst2, buffer1);
          i++;
          ii = i;
        }
      else
        break;
     }
  }

 if(strlen(buffer) > strlen(tekst2) +1)
   for(i=ii; i<strlen(buffer); i++)
    {
      if(buffer[i] == ' ')
       {
        strncpy(buffer1,  &buffer[ii], i-ii);
        buffer1[i-ii] = 0x00;
        cairo_text_extents(cr, buffer1, &extents);
        if (extents.width < width)
          {
            strcpy (tekst3, buffer1);
            i++;
          }
        else
          break;
       }
    }
 else
      strcpy(tekst3,""); //only 1 line of text
//KUKU
 cairo_move_to (cr, 1, 38);
 cairo_show_text (cr, tekst2);
 cairo_move_to (cr, 1, 58);
 cairo_show_text (cr, tekst3);

 cairo_move_to (cr, 1, 80);
 cairo_select_font_face (cr, "FreeSans",
     CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
 cairo_set_font_size (cr, 18.2);
 sprintf(tekst, "%s %s", imie, nazwisko);  // name surname 
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

  double ww, hh;
  hh = gtk_widget_get_allocated_height (widget);
  ww = gtk_widget_get_allocated_width (widget);

  int poczx, poczy;
  cairo_t *cr;
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, colr, colg, colb);
//  cairo_rectangle (cr, x - line_width/2*.7, y - line_width/2*.7, line_width*.7, line_width*.7);
//  cairo_fill (cr);
//  cairo_destroy (cr);
      gtk_widget_queue_draw_area (widget, 0, 0, ww, hh);

  if(x != xx2 || y != yy2)
   {
//  Continuation of line drawing

    clock_gettime(CLOCK_REALTIME, &tim1);
    if((tim1.tv_sec-tim.tv_sec)*1000000 + (tim1.tv_nsec - tim.tv_nsec)/1000 < PRZERWA )
     {
       xx3 = x;
       yy3 = y;

       if (pointno == 1)
        {    // draw straight line between first two points 
         cr = cairo_create (surface);
         cairo_set_source_rgba (cr, colr, colg, colb, 1.0);
         cairo_move_to (cr, xx2,  yy2);
         cairo_line_to (cr, xx3,  yy3);
         cairo_set_line_width (cr,line_width);
         cairo_stroke (cr);
         if (xx2 < xx3) poczx = xx2; else poczx = xx3;
         if (yy2 < yy3) poczy = yy2; else poczy = yy3;
      gtk_widget_queue_draw_area (widget, 0, 0, ww, hh);
              (widget, poczx - line_width -2, poczy - line_width - 2, fabs(xx3 - xx2) + line_width + 4, fabs(yy3 - yy2) + line_width + 4);
        }
       if (pointno > 2)
         interpolate(widget); 
     }
    else
     {
// Begining of drawing a new line
      if ((xx2 > 0 || yy2 > 0) && (pointno !=1))
        {    // draw straight line between last two points ,
             // cubic interpolation can not handle it
         cr = cairo_create (surface);
         cairo_set_source_rgba (cr, colr, colg, colb, 1.0);
         cairo_move_to (cr, xx1,  yy1);
         cairo_line_to (cr, xx2,  yy2);
         cairo_set_line_width (cr, line_width);
         cairo_stroke (cr);
         if (xx1 < xx2) poczx = xx1; else poczx = xx2;
         if (yy1 < yy2) poczy = yy1; else poczy = yy2;
      gtk_widget_queue_draw_area (widget, 0, 0, ww, hh);
              (widget, poczx - line_width - 2, poczy - line_width - 2, fabs(xx2 - xx1)+line_width + 4, fabs(yy2 - yy1)+line_width + 4);
        }
      pointno = 0;
      xx2 = 0;
      yy2 = 0;
      xx3 = x;
      yy3 = y;
     } 

      clock_gettime(CLOCK_REALTIME, &tim);
      pointno++;
      podpisane++;
      xx0 = xx1;
      yy0 = yy1;
      xx1 = xx2;
      yy1 = yy2;
      xx2 = xx3;
      yy2 = yy3;
   }
}


void interpolate(GtkWidget *widget)
{
  cairo_t *cr;
  double ww, hh;
  hh = gtk_widget_get_allocated_height (widget);
  ww = gtk_widget_get_allocated_width (widget);

    /* interpolate points between rectangles, cubic interpolation */
  double poczx, poczy;
  double delta;
  double a0,a1,a2,a3;
  double xp, xk;
  double yp, yk;

  cr = cairo_create (surface);
  cairo_set_source_rgba (cr, colr, colg, colb, 1.0);

  xp = xx1;
  yp = yy1;

  for (delta=0; delta < 1; delta = delta + 1/ww)
//  for (delta=.001; delta < .999; delta = delta + .001)
     {
      a0 = -0.5*yy0 + 1.5*yy1 - 1.5*yy2 + 0.5*yy3;
      a1 = yy0 - 2.5*yy1 + 2*yy2 - 0.5*yy3;
      a2 = -0.5*yy0 + 0.5*yy2;
      a3 = yy1;
      yk = a0*delta*delta*delta+a1*delta*delta+a2*delta+a3;
      xk = xx1 + delta * (xx2 - xx1);
      cairo_move_to (cr, xp,  yp);
      cairo_line_to (cr, xk,  yk);
      cairo_set_line_width (cr, line_width);
      cairo_stroke (cr);


      if (xp < xk) poczx = xp; else poczx = xk;
      if (yp < yk) poczy = yp; else poczy = yk;
//KUKU

      gtk_widget_queue_draw_area (widget, 0, 0, ww, hh);
      xp = xk;
      yp = yk;
     }
  cairo_destroy (cr);
}


/* Handle button press events by drawing a rectangle
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

int w, h;
h = gtk_widget_get_allocated_height (widget);
w = gtk_widget_get_allocated_width (widget);

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
      cairo_set_source_rgb(cr, scolr, scolg, scolb); 
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

      xx0 = 0;
      xx1 = 0;
      xx2 = 0;
      xx3 = 0;
      yy0 = 0;
      yy1 = 0;
      yy2 = 0;
      yy3 = 0;

      return TRUE;
    }

int sendpng()
{
/* send the png to client, in blocks:
   lenght, 100 bytes of data, XOR of data bytes
   end of transmission - lenght = 0 */

int fd;
unsigned char buf[102];
char pngname[50];
unsigned char crc=0;
ssize_t count=999;
unsigned char ccount;
int ii;

 sprintf(pngname, "%s.png", name);
 fd = open(pngname, O_RDONLY);
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
 close(fd);
 remove(pngname);
}

static gboolean
wyslij (GtkWidget      *widget, GtkWindow *window)
 {
  char tekst[100];
  char buffer[256];
  char pngname[50];

  cairo_t *cr;
      sprintf(pngname, "%s.png", name);
      cairo_surface_write_to_png (surface, pngname);
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

  cairo_t *cr;
  int status;
  char tittle[20];

  strcpy(tresc1, tresc);
  strcpy(tresc2, tresc);
  strcat(tresc1, ".txt");
  strcat(tresc2, ".osw");

  if (access(tresc1, R_OK))
    return 6;
  if (access(tresc2, R_OK))
    return 6;

  clock_gettime(CLOCK_REALTIME, &tim);
  gtk_init (NULL, NULL);
  setlocale(LC_ALL,country);


  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "templ.glade", &error) == 0)
    {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      return 1;
    }

  window = gtk_builder_get_object (builder, "window");
  sprintf(tittle, "%s 2.1", _("Graphologist"));
  gtk_window_set_title (GTK_WINDOW (window), tittle);

  if (FULLSCREEN)
   {
    gtk_window_maximize (GTK_WINDOW (window));
//    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
   }
  else
   { 
    gtk_window_set_default_size(GTK_WINDOW (window), 600, 900);
    gtk_window_resize (GTK_WINDOW (window), 600, 900);
    gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
   }

  tekst = gtk_builder_get_object (builder, "tresc");
  view = gtk_text_view_new ();

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  gtk_widget_show (view);
  gtk_widget_set_hexpand (view, TRUE);
  gtk_container_add(GTK_CONTAINER(tekst) , view);

  file = g_file_new_for_path(tresc1);
  g_file_load_contents (file, NULL, &contents, &length, NULL, NULL);

// find the text in the right language
  if(strstr(contents, country_mark_s) != NULL &&
     strstr(contents, country_mark_e) != NULL)
   {
     memmove(contents, strstr(contents, country_mark_s), length);

     char cc1[strlen(contents)];
     strcpy(cc1, strchr(contents, '\n'));
     strcpy(contents, cc1);
     memset(strstr(contents, country_mark_e), 0,1);
   }
  else
   { 
     strcpy(contents, _("No text in this language!"));
   }
  gtk_text_buffer_set_text (buffer, contents, -1);

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
  gtk_css_provider_load_from_path(cssProvider, "./start.css", NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                               GTK_STYLE_PROVIDER(cssProvider),
                               GTK_STYLE_PROVIDER_PRIORITY_USER);
  tmoutid=g_timeout_add_seconds (TIMEOUT, tmout, window ) ;
  gtk_main ();
  g_source_remove(tmoutid);
  return ret;
}

