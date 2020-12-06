#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

typedef enum {
    ENCRYPT,
    DECRYPT
} encrypt_mode;
char caesar(unsigned char ch, encrypt_mode mode, int key)
{
    if (ch >= 'a' && ch <= 'z') {
        if (mode == ENCRYPT) {
            ch += key;
            if (ch > 'z') ch -= 26;
        } else {
            ch -= key;
            if (ch < 'a') ch += 26;
        }
        return ch;
    }

    if (ch >= 'A' && ch <= 'Z') {
        if (mode == ENCRYPT) {
            ch += key;
            if (ch > 'Z') ch -= 26;
        } else {
            ch -= key;
            if (ch < 'A') ch += 26;
        }
        return ch;
    }

    return ch;	
}


int main(int argc,char **argv) {
	if ( argc==5 && ((strcmp(argv[1],"--input") == 0 && strcmp(argv[3],"--key") == 0 && 0<= atoi(argv[4]) && atoi(argv[4])<= 25)|| (strcmp(argv[3],"--input") == 0 && strcmp(argv[1],"--key") == 0 && 0<= atoi(argv[2]) && atoi(argv[2])<= 25))){
	remove("out.txt");
	int status;
	int key;
		if(strcmp(argv[1],"--input") == 0 && strcmp(argv[3],"--key") == 0 && 0<= atoi(argv[4]) && atoi(argv[4])<= 25) {
		key=atoi(argv[4]);
		}
		if(strcmp(argv[3],"--input") == 0 && strcmp(argv[1],"--key") == 0 && 0<= atoi(argv[2]) && atoi(argv[2])<= 25) {
		key=atoi(argv[2]);
		}
	int n_read, n_write;
	pid_t C1;
	pid_t C2;
	char buffer[1];
	C1 = fork();
	if (C1<0) printf ("Error \n");
		else if (C1 ==0) {
		int fd_in;
		if(strcmp(argv[1],"--input") == 0 && strcmp(argv[3],"--key") == 0 && 0<= atoi(argv[4]) && atoi(argv[4])<= 25) {
		fd_in = open(argv[2], O_RDONLY);
		}
		else {
		fd_in = open(argv[4], O_RDONLY);
		}
   			if (fd_in == -1) {
                	perror("open");
                	exit(-1);
                	}
		int fd_out = creat("out.txt", 0644);	
			if (fd_out == -1) {
			perror ("creat");
			exit(-1);
			}
			do {
			//Read at most BUFFER_SIZE bytes, returns number of bytes read
			n_read = read(fd_in, buffer, 1);
				if (n_read == -1) {
				perror("read");
            			exit(-1);
        			}
			buffer[0]=caesar(buffer[0], ENCRYPT, key);

			// Write at most n_read bytes, returns number of bytes written
        		n_write = write(fd_out, buffer, n_read);
        			if (n_write < n_read) {
            			perror("write");
            			exit(-1);
        			}
    			} while (n_read > 0); 
		close(fd_in);
		close(fd_out);	
		}
		else {
		wait(&status);
		C2 = fork();
			if (C2<0) printf ("Error \n");
			else if (C2 == 0) {
			int fd_in = open("out.txt", O_RDONLY);
   				if (fd_in == -1) {
                		perror("open");
                		exit(-1);
                		}
				do {
        			// Read at most BUFFER_SIZE bytes, returns number of bytes read
        			n_read = read(fd_in, buffer, 1);
        				if (n_read == -1) {
            				perror("read");
            				exit(-1);
        				}
				buffer[0]=caesar(buffer[0], DECRYPT, key);
				printf("%c",buffer[0]);
				} while (n_read>0);
			close(fd_in);
			}
			else {
			wait(&status);
			}
		}
	}
	else { 
		printf("wrong arguments \n");
	}
	return 0;
}



