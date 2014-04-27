/*
 * =====================================================================================
 *
 *       Filename:  udp_client.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2014 10:14:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void *back_ground(void *arg)
{
	int sock;
	socklen_t addr_len;
	char recv_data[1024];
	struct sockaddr_in server_addr , client_addr;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5001);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);


	if (bind(sock,(struct sockaddr *)&server_addr,
		 sizeof(struct sockaddr)) == -1) {
		perror("Bind:");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);
  
	fflush(stdout);

	while (1) {
		recvfrom(sock,recv_data,1024,0,
			  (struct sockaddr *)&client_addr, &addr_len);

		printf("(%s , %d) said : ",inet_ntoa(client_addr.sin_addr),
			ntohs(client_addr.sin_port));
		printf("%s", recv_data);
		fflush(stdout);
        }

}

void help_msg()
{
	printf("q or Q: quit the program\n");
	printf("help:   print this message\n");
}

int main()
{
	int sock;
	struct sockaddr_in server_addr;
	struct hostent *host;
	char send_data[1024];
	pthread_t pid;

	pthread_create(&pid, NULL, back_ground, NULL);

	host= (struct hostent *) gethostbyname((char *)"127.0.0.1");


	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5000);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8);

	while (1) {
		fgets(send_data, sizeof(send_data), stdin);
	
		if ((strcasecmp(send_data , "q\n") == 0)) 
	    		break;
		else if ((strcasecmp(send_data , "help\n") == 0)) {
			help_msg();
			break;
		} else
			sendto(sock, send_data, strlen(send_data) + 1, 0,
			       (struct sockaddr *)&server_addr,
			       sizeof(struct sockaddr));
	 
	}

	return 0;
}
