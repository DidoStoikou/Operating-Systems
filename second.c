#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

int cnt = 0; 
int sec = 100;
int pidarray[50];
int children;

void handleruser1child(int sig) {
 int y= getpid();
 int x = 1;
 while (y != pidarray[x]){
  x++;
 }
 printf ("[Child Process %d:%d] Value: (%d)\n", x, y, cnt);
 return;
}

void handleruser1father(int sig) {
 int y = getpid();
 printf ("[Father Process: %d] will ask current values (SIGUSR1) from all active children processes\n", y);
 for(int i=1; i<= children; i++) {
  kill(pidarray[i], SIGUSR1);
 }
 return;
}

void handleruser2(int sig) {
 int y = getpid();
 printf("[Process: %d] Echo!\n", y);
 return;
}

void handlertermfather(int sig) {
 for(int i=1; i<=children; i++) {
  printf("[Father Process: %d] Will terminate (SIGTERM) child process %d: %d\n", pidarray[0], i, pidarray[i]);
  kill(pidarray[i], SIGTERM);
 }
}

void handleralarm(int sig) {
 int y = getpid();
 int x=1;
 while (y != pidarray[x]){
  x++;
 }
 printf("[Child Process %d: %d] Time Expired! Final Value: %d\n", x, y, cnt);
 exit(0);
}


int main (int argc, char **argv) {
children = argc - 1;
pidarray[0] = getpid();
int status;
void handleruser1child(int sig);
void handleruser1father(int sig);
void handleruser2(int sig);
void handlertermfather(int sig);
void handleralarm(int sig);

if (children < 1) {
 printf("Please give delays\n");
 exit(-1);
}
for (int j=1; j<argc; j++) {
 if (atoi(argv[j]) <= 0 ) {
  printf("Please give positive delays\n");
  exit(-1);
 }
}
printf("[Maximum execution time of children is set to %d secs\n\n", sec);

pid_t pid;

printf("[Father Process: %d] Was created and will have %d children!\n", pidarray[0], children);

for (int i=1; i<argc; i++) {
 pid = fork();
 if (pid < 0) {
  perror("fork");
 }
 else if (pid == 0) {
  alarm(sec);
  int pidchild = getpid();
  pidarray[i] = pidchild;
  printf("[Child Process %d: %d] Was created and will pause!\n", i, pidchild);
  raise(SIGSTOP);
  printf("[Child Process %d: %d] Is starting!\n", i, pidchild);

  struct sigaction actionuser1child;
  actionuser1child.sa_handler = handleruser1child;
  actionuser1child.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &actionuser1child, NULL);

  struct sigaction actionuser2;
  actionuser2.sa_handler = handleruser2;
  actionuser2.sa_flags = SA_RESTART;
  sigaction(SIGUSR2, &actionuser2, NULL);
 
  struct sigaction actionalarm;
  actionalarm.sa_handler = handleralarm;
  actionalarm.sa_flags = SA_RESTART;
  sigaction(SIGALRM, &actionalarm, NULL);

  while (1) {
   cnt++ ;
   sleep(atoi(argv[i]));
  }
  break;
 }
 else {
  pidarray[i] = pid;
 }
}

if (pid > 0) {
 struct sigaction actionuser2;
 actionuser2.sa_handler = handleruser2;
 actionuser2.sa_flags = SA_RESTART;
 sigaction(SIGUSR2, &actionuser2, NULL);

 struct sigaction actionuser1father;
 actionuser1father.sa_handler = handleruser1father;
 actionuser1father.sa_flags = SA_RESTART;
 sigaction(SIGUSR1, &actionuser1father, NULL);

 struct sigaction actionterm;
 actionterm.sa_handler = handlertermfather;
 actionterm.sa_flags = SA_RESTART;
 sigaction(SIGTERM, &actionterm, NULL);
 
 for (int j=1; j<argc; j++) {
  waitpid(-1, &status, WUNTRACED);
 }
 for (int h=1; h<argc; h++) {
  kill(pidarray[h], SIGCONT);
 }
 
 printf("[Father Process: %d] Waiting for %d children that are still working!!\n", pidarray[0], children);
 for (int k=1; k<argc; k++) {
  wait(&status);
  children--;
  if (children == 1) {
   printf("[Father Process: %d] Waiting for %d child that is still working!!\n", pidarray[0], children);
  }
  else if (children > 1) {
   printf("[Father Process: %d] Waiting for %d children that are still working!!\n", pidarray[0], children);
  }
  else if (children == 0 ) {
   printf("[Father Process: %d] Will exit: all children are terminated.\n", pidarray[0]);
  }
 }
} 
return 0;
}




















































  
























