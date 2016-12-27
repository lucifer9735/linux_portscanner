#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
	fpo = fopen("report","w+");

	char str[20];
	
	while(fgets(str,20,fpi) != NULL){
		Build(str,Target);

//		fputs(str,fpo);
	}

	fclose(fpi);
	fclose(fpo);
	return 0;
}

void Build(List L,char *str){
		
}

struct Node{
	char *element;
	int status;
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
	while(P != NULL && P->element != x)
		P = P->Next;
	return P;
}

Position FindPrevious(char *x,List L){
	Position P;
	P=L;
	while(P->Next != NULL && P->Next->element != x)
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
	TmpCell->element=x;
	TmpCell->Next=P->Next;
	P->Next=TmpCell;
}
