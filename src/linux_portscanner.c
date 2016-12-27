#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/select.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>

#define nsec 100;

int port_connect(char *ip,int port);
void print_res(int *list,int n);

int remain(int argc,const char * argv[])
{
	if (argc < 4) {
		printf("ip sport eport [thread]\n");
		return 0;
	}
	printf("start scaning...\n");
	
	char *ip = argv[1];
	int s_port = atoi(argv[2]);
	int e_port = atoi(argv[3]);
	int open[65535];
	int count = 0;

//单线程
	for (int i = s_port; i<= e_port; i++) {
		printf("scaning port:%d \n",i);
		int r = -1;
		if ((r=port_connect(ip, i)) == 0) {
			open[count] = i;
			count++;
		}
	}

	print_res(open,count);
	
	return 0;
}

int main(int argc,const char * argv[])
{
	char *ip = argv[1];
	int port = atoi(argv[2]);
	int r=-1;
	if ((r=port_connect(ip,port)) == 0) {
		printf("port: %d  open\n",port);
	}
	return 0;
}

void print_res(int *list,int n)
{
	printf("Results:\n----------------\n");
	for (int i = 0; i < n; i++) {
		printf("port: %d  open\n",list[i]);
	}

	printf("-----------------\n");
}

struct node{
	char *ip;
	int port;
	struct node *next;
};

int port_connect(char *ip,int port)
{
	int flag, n, error;
	fd_set rset,wset;
	struct timeval tval;

	FD_ZERO(&wset);
	FD_ZERO(&rset);
	tval.tv_sec = 1;
	tval.tv_usec = 0;
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	flag = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	error = 0;
	if((n=connect(sockfd,(struct sockaddr *)&address, sizeof(address)))<0) {
		if(errno != EINPROGRESS) {
			printf("connecting 1 error !\n");
			return -1;
		}
		else if (n == 0) {
	//This case may happen on localhost
			printf("connecting 1 success !\n");
			return 0;
		}
	}
	FD_SET(sockfd, &rset);
	wset=rset;
	
	//Do whatever we want while the connect is taking place
	int rst=select(sockfd+1, &rset, &wset, NULL, &tval);

	switch (rst) {
		case -1:
			perror("Select error"); exit(-1);
		case 0:
			close(sockfd);
			printf("Time out !\n");break;
		default:
			if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
				int error;
				socklen_t len = sizeof(error);
				if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
					printf("getsockopt fail,connected fail\n");
					return -1;
				}
				if (error == 0){
					printf("port %d is open\n",port);
					return 0;
				}
			}
			close(sockfd);
	}
	return -1;
}
