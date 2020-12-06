#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define DEFAULT "\033[30;1m"
#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[38;1m"

int counter =0;

int round_robin_mode (int N) {
 int next_number = counter % N;
 return next_number;
}
int random_mode (int N) {
 int next_number = rand();
 next_number = next_number % N;
 return next_number;
}


int main (int argc, char** argv) {
 if (argc < 2 || argc >3) {
  printf(RED"Usage: %s <nChildren> [--random][--round-robin]"WHITE"\n", argv[0]);
  exit(0);
 }
 int N = atoi(argv[1]);
 if (N < 1) {
  printf(RED"Please give positive numbers"WHITE"\n");
  exit(0);
 }

 bool mode = false;
 if (argc == 3) {
  if (strcmp(argv[2], "--round-robin") == 0) {
   mode = false;
  }
  else if (strcmp(argv[2], "--random") == 0) {
   mode = true;
  }
  else {
   printf(RED"Usage: %s <nChildren> [--random][--round-robin]"WHITE"\n", argv[0]);
   exit(0);
  }
 }

 int read_pipe[N][2];
 int write_pipe[N][2];
 for (int i=0; i<N; i++) {
  if (pipe(read_pipe[i]) != 0) {
   perror("pipe");
  }
  if (pipe(write_pipe[i]) != 0) {
   perror("pipe");
  }
 }

 int pidarray[N];
 pid_t pid;
 for (int i=0; i<N; i++) {
  pid = fork();
  if (pid == -1) {
   perror("fork");
  }
  else if (pid == 0) {
   for (int j=0; j<N; j++) {
    if (j != i) {
     close(read_pipe[j][0]);
     close(read_pipe[j][1]);
     close(write_pipe[j][0]);
     close(write_pipe[j][1]);
    }
   }
   close(read_pipe[i][1]);
   close(write_pipe[i][0]);

   int value = 2;
   while(1) {
    if (read(read_pipe[i][0], &value, sizeof(int)) == -1) {
     perror("read");
    }
    printf(CYAN"[Child %d] [%d] Child received %d!"WHITE"\n", i, getpid(), value);
    value++;
    sleep(5);
    if (write(write_pipe[i][1], &value, sizeof(int)) == -1) {
     perror("write");
     exit(-1);
    }
    printf(GREEN"[Child %d] [%d] Child Finished hard work, writing back %d"WHITE"\n", i, getpid(), value);
   }
   close(read_pipe[1][0]);
   close(write_pipe[i][1]);
  }
  else {
   pidarray[i] = pid;
  }
 }
 
 if (pid > 0) {
  for (int j=0; j<N; j++){
   close(read_pipe[j][0]);
   close(write_pipe[j][1]);
  }
  while(1) {
   fd_set inset;
   int maxfd, newmaxfd;
   int value;

   FD_ZERO(&inset);
   FD_SET(STDIN_FILENO, &inset);
   for (int i=0; i<N; i++) {
    FD_SET(write_pipe[i][0], &inset);
   }
   maxfd = MAX(STDIN_FILENO, write_pipe[0][0]);
   for (int i=1; i<N; i++) {
    newmaxfd = MAX(maxfd, write_pipe[1][0]);
   }
   newmaxfd++;

   int ready_fds = select(newmaxfd, &inset, NULL, NULL, NULL);
   if (ready_fds <= 0) {
    perror("select");
    continue;
   }
   
   if (FD_ISSET(STDIN_FILENO, &inset)) {
    char buffer[101];
    int n_read = read(STDIN_FILENO, buffer, 100);
    if (n_read == -1) {
     perror("read");
     exit(-1);
    }
    buffer[n_read] = '\0';
    if (n_read == 2 && strcmp(buffer, "0") == 0) {
     int val = 0;
     if (mode == true) {
      counter = random_mode(N);
      printf(MAGENTA"[Parent] Assigned %d to child %d "WHITE"\n", val, counter);
      write(read_pipe[counter][1], &val, sizeof(int));
     }
     else {
      counter = round_robin_mode(N);
      printf(MAGENTA"[Parent] Assigned %d to child %d "WHITE"\n", val, counter);
      write(read_pipe[counter][1], &val, sizeof(int));
      counter++;
     }
    }
    else if ((n_read == 5) && (strcmp(buffer, "exit\0")) == 0) {
     for (int i=0; i<N; i++) {
      kill(pidarray[i], SIGTERM);
      wait(NULL);
      printf(CYAN"Waiting for %d "WHITE"\n", (N-i));
     }
     printf(RED"All children terminated "WHITE"\n");
     exit(0);
    }
    else if ((value = atoi(buffer)) != 0) {
     if (mode == true) {
      counter = random_mode(N);
      printf(MAGENTA"[Parent] Assigned %d to child %d "WHITE"\n", value, counter);
      write(read_pipe[counter][1], &value, sizeof(int));
     }
     else {
      counter = round_robin_mode(N);
      printf(MAGENTA"[Parent] Assigned %d to child %d "WHITE"\n", value, counter);
      write(read_pipe[counter][1], &value, sizeof(int));
      counter++;
     }
    }
    else {
     printf(BLUE"Type a positive number to send job to a child!"WHITE"\n");
    }
   }
   for (int i=0; i<N; i++) {
    if (FD_ISSET(write_pipe[i][0], &inset)) {
     int myvalue;
     read (write_pipe[i][0], &myvalue, sizeof(int));
     printf(YELLOW"[Parent] Received result from child %d --> %d "WHITE"\n", i, myvalue);
    }
   }
  }
 }
return 0;
}








































































































































