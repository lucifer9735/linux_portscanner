#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/select.h>
#include<netdb.h>
#include<netinet/in.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<arpa/inet.h>
#include<pthread.h>

#define nsec 100;

#include"thread.c"

struct Node{
	char ip[20];
	int port;
	int status;
	struct Node *Next;
};
typedef struct Node *PtrtoNode;
typedef PtrtoNode List;
typedef PtrtoNode Position;

Position FindPrevious(char *ip,int port,List L);
void Insert(char *ip,int port,Position P);
void PrintList(List L);
void Add(char *str,List L);
void Addport(char *str,char *ip,List L);
void Unflodport(char *str,char *ip,List L);
void Scan(List L);
void port_connect(Position P);
int ipcmp(char *ipa,char *ipb);

int main()
{
	pthread_mutex_init(&target_list_lock,NULL);

	FILE *fpi,*fpo;
	fpi = fopen("scanlist","r");
	fpo = fopen("report","w+");

	Position Target,Tail;
	Target = malloc(sizeof(PtrtoNode));
	Tail = malloc(sizeof(PtrtoNode));
	Tail = NULL;
	Target->Next = Tail;

	char str[80];
	while(fgets(str,80,fpi) != NULL){
//remaining 判空，去空格
		Add(str,Target);
	}

	Target = Target->Next;
	Position P = (Position)malloc(sizeof(Position));
	P = Target;
	pool_init(50);
	while(P != NULL){
		add_task(port_connect,P);
		P=P->Next;
	}
	pool_start();
	pool_join();
	PrintList(Target);

	fclose(fpi);fclose(fpo);
	return 0;
}

void Scan(List L){
	Position P;
	P=L;
	while(P != NULL){
		printf("scaning: IP:%s\tport:%d\n",P->ip,P->port);
		port_connect(P);
		printf("status:%d\n",P->status);
		P = P->Next;
	}
	return;
}

void port_connect(Position P)
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
	inet_pton(AF_INET, P->ip, &address.sin_addr);
	address.sin_port = htons(P->port);

	error = 0;
	if((n=connect(sockfd,(struct sockaddr *)&address, sizeof(address)))<0) {
		if(errno != EINPROGRESS) {
//			printf("connecting 1 error !\n");
			P->status = -1;
			return;
		}
		else if (n == 0) {
	//This case may happen on localhost
//			printf("connecting 1 success !\n");
			P->status = 0;
			return;
		}
	}
	FD_SET(sockfd, &rset);
	wset=rset;
	
	//Do whatever we want while the connect is taking place
	int rst=select(sockfd+1, &rset, &wset, NULL, &tval);

	switch (rst) {
		case -1:
//			perror("Select error");
		       	exit(-1);
		case 0:
			close(sockfd);
//			printf("Time out !\n");
			break;
		default:
			if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
				int error;
				socklen_t len = sizeof(error);
				if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
//					printf("getsockopt fail,connected fail\n");
					P->status = -1;
					return ;
				}
				if (error == 0){
//					printf("port %d is open\n",port);
					P->status = 0;
					return;
				}
			}
			close(sockfd);
	}
	P->status = -1;
	return;
}

void Error(char *str){
	puts(str);
	return;
}

void Add(char *str,List L){
	char ip[16],str_port[80];
	int len_ip=strcspn(str,":");	
	strncpy(ip,str,len_ip);
	ip[len_ip]='\0';
	strcpy(str_port,strchr(str,':')+1);	
//	puts(ip);
//	puts(str_port);
	Addport(str_port,ip,L);
	return;
}

void Addport(char *str,char *ip,List L){
	char tmp[80];
	char port[12];
	if(strchr(str,',') != NULL){
		strcpy(tmp,strchr(str,',')+1);
		int len_port=strcspn(str,",");
		strncpy(port,str,len_port);
		port[len_port]='\0';
		Unflodport(port,ip,L);
		Addport(tmp,ip,L);
	}
	else{
		Unflodport(str,ip,L);
		return;
	}
}

void Unflodport(char *str,char *ip,List L){
	char s_port[6],e_port[6];
	Position tmp;
	if(strchr(str,'-') != NULL){
		int len=strcspn(str,"-");
		strncpy(s_port,str,len);
		s_port[len]='\0';
		strcpy(e_port,strchr(str,'-')+1);
		for(int i=atoi(s_port);i<=atoi(e_port);i++){
			tmp = FindPrevious(ip,i,L);	
			Insert(ip,i,tmp);
//			printf("%d\n",i);
		}
	}
	else{
//		puts(str);
		int i=atoi(str);
		tmp = FindPrevious(ip,i,L);	
		Insert(ip,i,tmp);
		return;
	}
}

int ipcmp(char *ipa,char *ipb){
	if(ipa == ipb) return 0;
	else if(strchr(ipa,'.') != NULL){
		int lena=strcspn(ipa,".");
		int lenb=strcspn(ipb,".");
		char tmpa[4],tmpb[4];
		strncpy(tmpa,ipa,lena);
		strncpy(tmpb,ipb,lenb);
		tmpa[lena]='\0';
		tmpb[lenb]='\0';
		if(tmpa == tmpb){
			char rea[12],reb[12];
			strcpy(rea,strchr(ipa,'.')+1);
			strcpy(reb,strchr(ipb,'.')+1);
			ipcmp(rea,reb);
		}
		return atoi(tmpa)-atoi(tmpb);
	}else return atoi(ipa)-atoi(ipb);
}

Position FindPrevious(char *ip,int port,List L){
	Position P;
	P = L;
	while(P->Next != NULL && ipcmp(P->Next->ip,ip) < 0)
		P = P->Next;
	while(P->Next != NULL && ipcmp(P->Next->ip,ip) == 0 && P->Next->port <= port)
		P = P->Next;
	return P;
}

void Insert(char *ip,int port,Position P){
	Position TmpCell;
	TmpCell = malloc(sizeof(struct Node));
	if(TmpCell == NULL){
		Error("Out of Space !");
		return;
	}
	strcpy(TmpCell->ip,ip);
	TmpCell->port = port;
	TmpCell->Next = P->Next;
	P->Next = TmpCell;
}

void PrintList(List L){
	unsigned int i = 0;
	while(L != NULL){
		if(L->status == 0){
			i++;
			printf("No.%d\tIP:%s\tport:%d\tstatus:open\n",i,L->ip,L->port);
		}
		L = L->Next;
	}
}
