#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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

int main()
{
	FILE *fpi,*fpo;
	fpi = fopen("scanlist","r");
	fpo = fopen("report","w+");

	Position Target;
	Target = malloc(sizeof(PtrtoNode));
	Target->Next = NULL;

	char str[80];
	while(fgets(str,80,fpi) != NULL){
//remaining 判空，去空格
		Add(str,Target);
	}

	Target = Target->Next;
	PrintList(Target);

	fclose(fpi);fclose(fpo);
	return 0;
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

Position FindPrevious(char *ip,int port,List L){
	Position P;
	P = L;
	while(P->Next != NULL && P->Next->ip <= ip && P->Next->port < port)
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
	while(L != NULL){
		printf("IP:%s\tport:%d\n",L->ip,L->port);
		L = L->Next;
	}
}
