#include<stdio.h>
#include<stdlib.h>

struct Node;
typedef struct Node *PtrtoNode;
typedef PtrtoNode List;
typedef PtrtoNode Position;

int IsEmpty(List L);
int IsLast(Position P,List L);
Position Find(char *x,List L);
Position FindPrevious(char *x,List L);
void Insert(char *x,List L,Position P);

int main()
{
	FILE *fpi,*fpo;
	fpi = fopen("scanlist","r");

	char *ip,*str;
	int port;
	
	while(fgets(str,20,fpi) != NULL){
		puts(str);
	}

	fclose(fpi);
	fpo = fopen("report","a");
	fputs(str,fpo);
	fclose(fpo);
	return 0;
}

struct Node{
	char *ip;
	int port;
	Position Next;
};

int IsEmpty(List L){
	return L->Next == NULL;
}

int IsLast(Position P,List L){
	return P->Next == NULL;
}

Position Find(char *x,List L){
	Position P;
	P = L->Next;
	while(P != NULL && P->ip != x)
		P = P->Next;
	return P;
}

Position FindPrevious(char *x,List L){
	Position P;
	P=L;
	while(P->Next != NULL && P->Next->ip != x)
		P=P->Next;
	return P;
}

void Insert(char *x,List L,Position P){
	Position TmpCell;
	TmpCell=malloc(sizeof(struct Node));
	if(TmpCell == NULL){
		printf("Out of space !\n");
		return;
	}
	TmpCell->ip=x;
	TmpCell->Next=P->Next;
	P->Next=TmpCell;
}
