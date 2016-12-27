#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void Add(char *str);
void Addport(char *str);
void Unflodport(char *str);

int main()
{
	FILE *fpi,*fpo;
	fpi = fopen("scanlist","r");
	fpo = fopen("report","w+");
	char str[80];
	while(fgets(str,80,fpi) != NULL){
//remaining 判空，去空格
		Add(str);
	}

	fclose(fpi);fclose(fpo);
	return 0;
}

void Add(char *str){
	char ip[16],str_port[80];
	int len_ip=strcspn(str,":");	
	strncpy(ip,str,len_ip);
	ip[len_ip]='\0';
	strcpy(str_port,strchr(str,':')+1);	
	puts(ip);
	puts(str_port);
	Addport(str_port);
	return;
}

void Addport(char *str){
	char tmp[80];
	char port[12];
	if(strchr(str,',') != NULL){
		strcpy(tmp,strchr(str,',')+1);
		int len_port=strcspn(str,",");
		strncpy(port,str,len_port);
		port[len_port]='\0';
		Unflodport(port);
		Addport(tmp);
	}
	else{
		Unflodport(str);
		return;
	}
}

void Unflodport(char *str){
	char s_port[6],e_port[6];
	if(strchr(str,'-') != NULL){
		int len=strcspn(str,"-");
		strncpy(s_port,str,len);
		s_port[len]='\0';
		strcpy(e_port,strchr(str,'-')+1);
		for(int i=atoi(s_port);i<=atoi(e_port);i++)
			printf("%d\n",i);
	}
	else{
		puts(str);
		return;
	}
}
