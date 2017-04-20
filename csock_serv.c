/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   csock_serv.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qle-guen <qle-guen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/04/19 14:14:08 by qle-guen          #+#    #+#             */
/*   Updated: 2017/04/19 16:26:04 by qle-guen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct cli_list
{
	pthread_t client;
	int fd;
	struct cli_list *next;
} cli_list;

int sockfd;
int yes = 1;
int port;
struct sockaddr_in self_addr;
int backlog = 10;
pthread_t thread;

/*
void *client_routine(void *cli)
{
	cli_list *self = cli;
	(void)self;
	return (NULL);
}

void new_client(cli_list **clients, int fd)
{
	cli_list *new;

	new = malloc(sizeof *new);
	new->fd = fd;
	pthread_create(&new->client, NULL, &client_routine, new);
	new->next = *clients;
	*clients = new;
}
*/

void *accept_routine(void *arg)
{
	(void)arg;
	int maxfd;
	int newfd;
	int nready;
	int nbytes;
	int i;
	fd_set fdset;
	fd_set readset;
	char buffer[1024];

	fcntl(sockfd, F_SETFD, O_NONBLOCK);
	FD_ZERO(&fdset);
	maxfd = sockfd;
	FD_SET(sockfd, &fdset);
	while (42)
	{
		FD_COPY(&fdset, &readset);
		nready = select(maxfd + 1, &readset, NULL, NULL, NULL);
		i = 0;
		while (i <= maxfd && nready > 0)
		{
			if (!FD_ISSET(i, &readset))
				continue ;
			nready--;
			if (i == sockfd)
			{
				printf("new client\n");
				newfd = accept(sockfd, NULL, NULL);
				fcntl(newfd, F_SETFD, O_NONBLOCK);
				if (newfd > maxfd)
					maxfd = newfd;
				FD_SET(newfd, &fdset);
			}
			else
			{
				nbytes = recv(i, buffer, sizeof buffer, 0);
				write(1, buffer, nbytes);
			}
			i++;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		perror("usage: ./csock_serv <port>");
		return (0);
	}
	port = atoi(argv[1]);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		return (1);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
	{
		perror("setsockopt");
		return (1);
	}
	self_addr.sin_family = AF_INET;
	self_addr.sin_port = htons(port);
	self_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *)&self_addr, sizeof self_addr) == -1)
	{
		perror("bind");
		return (1);
	}
	if (listen(sockfd, backlog) == -1)
	{
		perror("listen");
		return (1);
	}
	pthread_create(&thread, NULL, &accept_routine, NULL);
	while (42)
		;
}
