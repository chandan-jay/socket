/*
 * Client Code. client.c
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define SIZE 1024
extern int h_errno;

int main (int argc, char **argv)
{
    int sockfd;
    char mesg[1024];
    char buff_r[1024], buff_s[1024];
    int recv_bytes, send_bytes, w_bytes;
    struct sockaddr_in address;
    printf("number of argument = %d\n",argc);
    for(int i = 0; i < argc; i++)
      {
	printf("%d --> %s\n",i, argv[i]);
      }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    char *server_ip = argv[1];
    int server_port;
    int ret = sscanf(argv[2], "%i", &server_port);
    printf("\nret --> %d\n", ret);
    if (ret != 1)
      {
	printf("error...!! \n");
      }          

    // Name of socket as agreed with Server.
    address.sin_family = AF_INET;
    /* address.sin_addr.s_addr = inet_addr(server_ip); */
    /* address.sin_port = htons(server_port); */
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9374);
   
    int length, result;
    length = sizeof(address);
      
    result = connect(sockfd, (struct sockaddr *)&address, length);
    
    getpeername(sockfd, (struct sockaddr *)&address, &length);
    printf("Server IP = %s \n", inet_ntoa(address.sin_addr));
    printf("Connecting to Port = %d \n", ntohs(address.sin_port));    


    if (result == -1)
    {
        printf("Error....\n");
        exit(1);
    }
    
    while(sockfd)
      {
	printf("$> ");
	gets(mesg);
	send_bytes = send(sockfd, mesg, strlen(mesg), 0);
	//	printf("====>send bytes %d\n", send_bytes);
	while(send_bytes != 0)
	  {
	    recv_bytes=recv(sockfd, buff_r, SIZE, 0);
	    w_bytes=write(1, buff_r, strlen(buff_r));
	    //	    printf("-----> recv_bytes = %d\n w_bytes = %d\n", recv_bytes, w_bytes);
	    if(w_bytes < SIZE) break;
	  }
	continue;
      }
    close(sockfd);
    exit(0);
}

