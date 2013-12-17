/*
 * server.c
 * Multi-threading to handle multiple connection
 * October 12, 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#define SIZE 1024
extern int h_errno;

int server_sockfd;


void sig_handler(int signal)
{
  printf("intrupt Generate Closing Connection\n");
  close(server_sockfd);
  exit(0);
}

void *sock_operation(void *);


int main(int argc, int *argv[])
{    
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = &sig_handler;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGKILL, &act, NULL);

    int client_sockfd;
    struct sockaddr_in server_address, client_address;
    int server_len, client_len;
    int pid, status;
    pthread_t thread_id;
    int thread_ret;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    

    /* Creating a Socket */
    unlink ("Server_socket");
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_len = sizeof(server_address);
    
    memset(&server_address, 0, server_len);
    
    /*	Name the socket. man 7 ip, man 7 tcp */	
    server_address.sin_family = AF_INET;    //address family
    server_address.sin_port = htons(9374);  //port in n/w byte order
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("192.168.1.3");   //IP host address
    
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    
    getsockname (server_sockfd, (struct sockaddr *)&server_address, &server_len);
    printf("server IP = %s \n", inet_ntoa(server_address.sin_addr));
    printf("server port = %d\n", ntohs(server_address.sin_port));
    
    printf("Server Waiting......\n\n");
    listen(server_sockfd, 5);
    
    client_len = sizeof(client_address);

    while(1)
    {
	client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
	printf("client_sockfd ==== %d\n",client_sockfd);
	if (client_sockfd == 0)
        {
	    printf("Socket not created\n");
	    continue;
	}
	if (client_sockfd < -1)
	{
	  printf("Client socket failed\n");
	  continue;
	}
	getpeername (client_sockfd, (struct sockaddr *)&client_address, &client_len);

	printf("client IP = %s \n", inet_ntoa(client_address.sin_addr));
	printf("client port = %d\n", ntohs(client_address.sin_port));

	thread_ret = pthread_create(&thread_id, &attr, &sock_operation, (void *)&client_sockfd);
    }
    //    close(server_sockfd);
    return 0;
}



void *sock_operation(void *sockfd_void)
{
  int recv_bytes, send_bytes;
  int sockfd = *(int *)sockfd_void;
  char buff_r[SIZE], buff_s[SIZE];
  char *words[2];
  int file_fd, rd_bytes;
  DIR *dir;
  struct dirent *ds;
  char *server_root="/workspace/Socket/";
  char file_path[SIZE];

  printf("=============\n");

  while(sockfd != 0)
    {
      recv_bytes = recv(sockfd, buff_r, SIZE, 0);
      if ((recv_bytes <= 0))
        {
          printf("Error received");
          pthread_exit(NULL);
        }
      if(strlen(buff_r) < 1) continue;
//      fflush(stdout);
      char *str_r=malloc(sizeof(buff_r));
      int i = 0, j =0;
      strcpy(str_r, buff_r);

      for (words[i]=strtok(str_r, " "); words[i] != NULL; words[i] = strtok(NULL, " "))
	{
	  i++;
	}

      memset(buff_s, 0, SIZE);
      if (strncmp(words[0], "GET", 3) == 0)
      {
	strcpy(file_path, server_root);
	strcat(file_path, words[1]);
	file_fd = open(file_path, O_RDONLY);
	if(file_fd == -1)
	  {
	    send(sockfd, "Cannot open file for read\n", 50, 0);
	    continue;
	  }
	while(1)
	  {
	    rd_bytes = read(file_fd, buff_s, SIZE);
	    send_bytes = send(sockfd, buff_s, rd_bytes, 0);
            memset(buff_s, 0, SIZE);
	    if(rd_bytes < SIZE) break;
	  }
      }
      else 
      {
	  send(sockfd, "ERROR: Unexpected user input.\n", 50, 0);
	  memset(buff_r, 0, SIZE);
	  continue;
      }
    }
}
