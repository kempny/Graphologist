/****************************************************
wyswietl IP port numer miasto imie nazwisko
Kody powrotu:
1 - podpisano
2 - anulowano podpis
3 - timeout podczas podpisywania
4 - timeout polaczenia z urzadzeniem do podo[isywania
5 - podpisano, ale blad transmisji pliku z podpisem
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
#include <netdb.h>
#include <arpa/inet.h>


/*  zmienne  */
time_t czas1;
time_t czas2;
struct tm *loctime;
fd_set set, set1;
#define TRUE 1
unsigned char crc;
char buf[200];
char buf1[50];
char numer[200];
char imie[200];
char nazwisko[200];
char miasto[200];
int n,m,i,j,rob1, sleep_time, jest;
unsigned char znak, znak1;
int fd;
time_t czas;
char data[55];
struct tm *loctime;
struct termios opcje;
unsigned char byte;





/* S T A R T */
void main (argc, argv)
     int argc;
     char *argv[];
{

int sockfd, newsockfd, portno, clilen;



    if (argc != 8)
     {
     printf( "./wyswietl IP PORT nazwa_zdjecia tresc miasto imie nazwisko\n");
     exit(1);
     }
  struct sockaddr_in serv_addr;
  struct hostent *server;

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
//        perror("ERROR opening socket");
    }
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_aton(argv[1], (struct in_addr *)&serv_addr.sin_addr.s_addr);

    serv_addr.sin_port = htons(atoi(argv[2]));
    /* polacz z serwerem */
    time (&czas1);   
    time (&czas2);   
    rob1=0;
    while (czas2 - czas1 < 5) // 5 sekund na polaczenie z wyswietlaczem
     {
       if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        {
//        perror("ERROR connecting");
          time (&czas2);
          sleep_time = 1;
          while (sleep_time > 0)
             sleep_time = sleep (sleep_time);
        }
       else
        {
         rob1=1;
         break;
        }
     }
    if(!rob1)
     {
      printf("Brak polaczenia z wyswietlaczem\n");
      exit(1);
     }
/*
    strcpy(buf, argv[3]);
    strcat(buf, "\n");
    write (sockfd, buf, strlen(buf));
*/
    strcpy(buf, argv[4]);
    strcat(buf, "\n");
    write (sockfd, buf, strlen(buf));
  
    strcpy(buf, argv[5]);
    strcat(buf, "\n");
    write (sockfd, buf, strlen(buf));

    strcpy(buf, argv[6]);
    strcat(buf, "\n");
    write (sockfd, buf, strlen(buf));

    strcpy(buf, argv[7]);
    strcat(buf, "\n");
    write (sockfd, buf, strlen(buf));
  
// odbierz odpowiedz
    m=0;
    jest=0;
    buf[m]='0';
    time (&czas1);
    time (&czas2);

    while (czas2 - czas1 < 70) // 70 sekund na odpowiedz, potem timeout
      {
         if ((read (sockfd, &buf[m], 1)) != -1)
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
      strcpy(buf, "04");  //local timeout, network connection error?
      printf("odpowiedz: Timeout %s\n", buf); fflush(0);
     }
    else
      printf("odpowiedz: %s\n", buf); fflush(0);

    if(atoi(buf)==1)  // zlozono podpis
     {
        time (&czas1);
        time (&czas2);
        sprintf(buf1, "%s.png", argv[3]);
        fd = open(buf1, O_RDWR | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE | S_IRGRP  | S_IWGRP| S_IROTH );

       for(;;)
        {
  
         while (czas2 - czas1 < 6) // 6 sekund na transmisje, potem timeout
          {
            if ((read (sockfd, &byte, 1)) == 1)
             {
               m=byte;   // liczba znakow w bloku 
                 break;
             }
            time (&czas2);
          } 
         if (m == 0)
           break;
         crc = 0;
         for (i=0;i<m;i++)
          {
           while (czas2 - czas1 < 6) // 6 sekund na transmisje, potem timeout
            {
             if ((read (sockfd, &byte, 1)) == 1)
               {
                crc= crc^byte;
                write(fd, &byte,1);
                break;
               } 
             time (&czas2);
            } 
          }
         while (czas2 - czas1 < 6) // 6 sekund na transmisje, potem timeout
          {
            if ((read (sockfd, &byte, 1)) == 1) //odczytaj crc
             {
               if( byte != crc)
                 {
                   printf("Blad transmisji");
                   strcpy(buf,"5");
                 }
               break;
             }
            time (&czas2);
          } 
       }
     }    
    close(sockfd);
    close(fd);
    exit(atoi(buf));

} //od main
