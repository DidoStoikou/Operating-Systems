#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE "\033[37m"

#define MAX(a, b) ((a) > (b) ? (a) : (b))


int main(int argc, char** argv) {
  bool debug_check = false;    
  char *host = "tcp.akolaitis.os.grnetcloud.net";
  int input = 1; 
  if(argc > 4) {
    printf("Correct form :./tsanakas4 [--host_HOST] [--port_PORT] [--debug]\n");
    exit(0);
  }
if (argc == 4) {
  if((strncmp(argv[1],"--host-tcp.akolaitis.os.grnetcloud.net",38) == 0) && (strncmp(argv[2],"--port-8080",11) == 0) && (strncmp(argv[3],"--debug",7) == 0)) {
    debug_check = true;
  }
  else {
    printf("Can't connect to the given server or the format is wrong");
    exit(0);
  }
}
if(argc == 3) {
  if (((strncmp(argv[1],"--host-tcp.akolaitis.os.grnetcloud.net",38) == 0) || (strncmp(argv[1],"--port-8080",11) == 0)) && (strncmp(argv[2],"--debug",7) == 0)) {
    debug_check = true;
  }
  else if ((strncmp(argv[1],"--host-tcp.akolaitis.os.grnetcloud.net",38) == 0) && (strncmp(argv[2],"--port-8080",11) == 0)){
    debug_check = false;
  }
  else {
    printf("Can't connect to the given server or the format is wrong");
    exit(0);
  }
}
if (argc == 2) {
if ((strncmp(argv[1],"--host-tcp.akolaitis.os.grnetcloud.net",38) == 0) || (strncmp(argv[1],"--port-8080",11) == 0)) {
  debug_check  = false;
}
else if(strncmp(argv[1],"--debug",7) == 0) {
  debug_check = true;
}
else {
  printf("Can't connect to the given server or the format is wrong");
  exit(0);
}
}


int sd;
struct sockaddr_in server;
struct sockaddr_in client;

if((sd = socket(AF_INET,SOCK_STREAM,0)) < 0) {   
  perror("socket");
  exit(-1);
}
client.sin_family = AF_INET;   
client.sin_port  = htons(0);   
client.sin_addr.s_addr = htonl(INADDR_ANY); 

if (bind(sd,&client,sizeof(client)) < 0) { 
  perror("bind");
  return -1;
}

struct hostent *server_host;
server_host = gethostbyname(host);  

server.sin_family = AF_INET;
server.sin_port = htons(8080);
bcopy(server_host->h_addr, &server.sin_addr, server_host->h_length);  

printf(BLUE"Connecting!"WHITE"\n");

if((connect(sd,&server,sizeof(server))) < 0) {   
  perror("connect");
  exit(-1);
}
printf(GREEN"Connected to %s!"WHITE"\n",host);


while (1) {   
        fd_set inset;  
        int maxsd;

        FD_ZERO(&inset);      
        FD_SET(STDIN_FILENO, &inset);   
        FD_SET(sd, &inset);
        maxsd = MAX(STDIN_FILENO, sd) + 1;
        int ready_sds = select(maxsd, &inset, NULL, NULL, NULL);
        if (ready_sds <= 0) {
            perror("select");
            continue;
        }
        if (FD_ISSET(STDIN_FILENO, &inset)) {
          char buffer[101];
          int n_read = read(STDIN_FILENO, buffer, 100);
          buffer[n_read] = '\0';

          if (n_read > 0 && buffer[n_read-1] == '\n') {
            buffer[n_read-1] = '\0';
          }

          if (n_read==-1){
	    perror("read");
	    exit(-1);
          }


          if (strncmp(buffer,"help",4) == 0) {
            input = 1; 
            printf(YELLOW"Available commands:"WHITE"\n");
            printf(YELLOW"help                    :Print this help message"WHITE"\n");
            printf(YELLOW"exit                    :Exit"WHITE"\n");
            printf(YELLOW"get                     :Retrive sensor data"WHITE"\n");
            printf(YELLOW"N name surname reason   :Ask permission to go out"WHITE"\n");
          }

          else if (strncmp(buffer,"exit",4) == 0) {
            exit(0);
          }

          else if (strncmp(buffer,"get",3) == 0) {
            input = 4;  
            write(sd,buffer,n_read);  
            if (debug_check == true) {
              printf(YELLOW"[DEBUG] sent 'get'"WHITE"\n");
            }
          }

          else if (input == 2) { 
            input = 3;
            write(sd, buffer, n_read); 
            if (debug_check == true) {
              printf(YELLOW"[DEBUG] sent '%s'"WHITE"\n",buffer);
            }
          }
          else {  
            input = 2;  
            write(sd, buffer, n_read);
            if (debug_check == true) {
              printf(YELLOW"[DEBUG] sent '%s'"WHITE"\n",buffer);
            }
          }
        }
        if (FD_ISSET(sd, &inset)) {
          char buffer[101];
          int n_read = read(sd, buffer, 100);
          buffer[n_read] = '\0';

          if (n_read > 0 && buffer[n_read-1] == '\n') {
                  buffer[n_read-1] = '\0';
          }

          if(strncmp(buffer, "try again", 9) == 0) {
            input = 1;
            if (debug_check == true) {
              printf(YELLOW"[DEBUG] read '%s'"WHITE"\n",buffer);
            }
            printf(MAGENTA"try again"WHITE"\n");
          }
          if (input == 4) { 
            if (debug_check == true) {
              printf(YELLOW"[DEBUG] read '%s'"WHITE"\n",buffer); 
            }
            printf(BLUE"----------------------------\nLatest event:\n");
            int a = atoi(buffer);
            int b = atoi(buffer+1);
            int c = atoi(buffer+5);
            long int d = atoi(buffer+10);

           if (a==0){
              printf("boot(0)\n");
            }
           if (a==1) {
             printf("setup(1)\n");
            }
           if (a==2) {
             printf("interval(2)\n");
            }
           if (a==3) {
              printf("button(3)\n");
            }
           if (a==4) {
             printf("motion(4)\n");
           }
           printf("Temperature is: %d,%d\n",c/100,c%100);
           printf("Light level is: %d\n",b%100);
           int year = 1900+localtime(&d)->tm_year;	
   				 int month = localtime(&d)->tm_mon;
   			         int day = localtime(&d)->tm_mday;
   				 int hour = localtime(&d)->tm_hour;
   				 int minute = localtime(&d)->tm_min;
   				 int second = localtime(&d)->tm_sec;
           printf("Timestamp is: %d-%d-%d %d:%d:%d"WHITE"\n",year,month,day,hour,minute,second);
           input = 1;
          }
        if (input == 2) { 
          if (debug_check == true) {
            printf(YELLOW"[DEBUG] read '%s'"WHITE"\n",buffer);
          }
          printf(BLUE"Send verification code '%s'"WHITE"\n",buffer);
        }
        if (input == 3) { 
          if (debug_check == true) {
            printf(YELLOW"[DEBUG] read '%s'"WHITE"\n",buffer);
          }
          if (strncmp(buffer, "invalid code", 12) == 0) {
            printf(MAGENTA"try again"WHITE"\n");
          }
          else {
            printf(GREEN"Response:'%s'"WHITE"\n",buffer);
          }
          input = 1;
        }
      }
    }
}

