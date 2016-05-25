/********************MyShell********************
 * This is a program to emulate a shell
***********************************************/

/******************Directives******************/
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<ctype.h>
#include<fcntl.h>
#define PROFILE1 ".myprofile"

/******************Globals********************/
char buff[255],cmdpath[300],PS1[30],PS2[5],path[50][45],home[45],cmd[15];
char ipf[20],opf[20],options[100];
int pathnum,ipflag,opflag;
int backprocess=0;

/*******************Functions*****************/
void setenviron();
void parse();
void execute();
void flush();

/********************Execute********************/
void execute()
{
	int i,j,pid,ifd,ofd,dupfd1,dupfd2,tmp;
	i=0;
//	printf("%d\n",pathnum);
	while(i<pathnum)
	{
//		printf("in execute while, i=%d\n",i);
		strcpy(cmdpath,path[i]);
//		printf("%s\n",cmdpath);
//		printf("%s\n",cmd);
		strcat(cmdpath,cmd);
//		printf("%s\n",cmdpath);
		j=fork();
		if(j==0)
		{
			if(opflag == 1)
			{
				printf("ofd=%d\n",ofd);
				dupfd1=dup(1);
				close(1);
				ofd=open(opf,O_WRONLY|O_CREAT,0777);
				printf("file descriptor=%d\n",ofd);
			}	
			if(opflag == 2)
			{
				printf("ofd=%d(output 1)\n",ofd);
				dupfd1=dup(1);
				close(1);
				ofd=open(opf,O_RDWR|O_CREAT,0777);
				printf("file descriptor=%d\n",ofd);
			}
			if(ipflag == 1)
			{
				printf("ifd=%d\n",ifd);
				dupfd2=dup(0);
				close(0);
				tmp=dup(ifd);
				printf("file descriptor=%d\n",tmp);
			}
			pid=execl(cmdpath,cmd,NULL);
//			printf("pid=%d\n",pid);
			if(pid==-1)
			{
				i++;
//				getchar();
				if(opflag)
				{
					close(1);
					dup(dupfd1);
				}
				if(ipflag)
				{
					close(0);
					dup(dupfd2);
				}
				continue;
			}
		}
		else
		{
			if(backprocess==0)
			{
				wait();
			}
			pid=1;
			break;
		}
//		getchar();
	}
	if(pid==-1)
	{
		printf("Error  : %s\n",strerror(errno));
	}
//	strcpy(cmd,"");
}

/********************Setenv********************/
void setenviron()
{
	FILE *fp;
	char buff[255];
	int i,j,k;
	fp=fopen(PROFILE1,"r");
	if(!fp)
	{
		printf("No profile file found\n");
		exit(1);
	}
	else
	{
		while(1)
		{
			if(fgets(buff,255,fp)==NULL)
				break;
			switch(buff[1])
			{
				case 'P' : switch(buff[2])
					   {
						case 'S' : switch(buff[3])
							   {
								case '1' : i=0;
									   while(1)
									   {
										PS1[i]=buff[i+5];
										i++;
										if(buff[i+5]=='\n' || buff[i+5]=='\0')
											break;
									   }
									   PS1[i]='\0'; 
									break;
								case '2' : i=0;
									   while(1)
									   {
										PS2[i]=buff[i+5];
										i++;
										if(buff[i+5]=='\n' || buff[i+5]=='\0')
											break;
									   }
									   PS2[i]='\0'; 
									break;
							   }
							break;
						case 'A' : i=0;j=6;k=0;
							   while(1)
							   {
								path[i][k]=buff[j];
								j++; k++;
								if(buff[j]=='\n' || buff[j]=='\0')
									break;
								else if(buff[j]==':')
								{
									path[i][k++]='/';
									path[i][k]='\0';
									i++; j++; k=0;
								}	
							    }
							    pathnum=i;
							break;
					   } 
					break;
				case 'H' : i=0;
					   while(1)
					   {
						home[i]=buff[i+6];
						i++;
						if(buff[i+6]=='\n' || buff[i+6]=='\0')
							break;
					   }
					   home[i]='\0';
					break;
			}
		}
	}
}
/********************parse********************/
void parse()
{
	int i=1;
	int j=0;
	int state=0;
	printf("in parse\n");
	printf("hello world\n");
	printf("%s\n",&buff[2]);
	printf("%c",buff[i]);
	while(buff[i]!='\0')
	{
		printf("i=%d",i);
		while(buff[i]!=' ' && buff[i]!='\t' && buff[i]!='\n')
		{
			cmd[j]=buff[i];
			i++;
			j++;
		}
		cmd[j]='\0';
		printf("command is : %s",cmd);
		i++;
		j=0;
		if(buff[i]=='-')
		{
			state=1;
			while(buff[i]!=' ' && buff[i]!='\t' && buff[i]!='\n')
			{
				options[j]=buff[i];
				i++;
				j++;
			}
			options[j]='\0';
		}
		i++;
		if(buff[i]=='>')
		{
			state=2;
			i++;
			if(buff[i]=='>')
			{
				opflag=2;i++;j=0;
				while(buff[i]!=' ' && buff[i]!='\t' && buff[i]!='\n')
				{
					opf[j]=buff[i];
					j++;i++;
				}
				i++;
			}
			else
			{
				opflag=1;j=0;
				while(buff[i]!=' ' && buff[i]!='\t' && buff[i]!='\n')
				{
					opf[j]=buff[i];
					j++;i++;
				}
				i++;
			}
		}
		if(buff[i]=='<')
		{
			state=3;
			i++;
			ipflag=1;
			j=0;
			while(buff[i]!=' ' && buff[i]!='\t' && buff[i]!='\n')
			{
				ipf[j]=buff[i];
				j++;i++;
			}
			i++;
		}
		if(buff[i]=='&')
		{
			backprocess=1;
		}
	}
}
/********************flush********************/
void flush()
{
	int i;
	ipf[0]='\0';
	opf[0]='\0';
	cmdpath[0]='\0';
	cmd[0]='\0';
	ipflag=0;
	opflag=0;
	for(i=0;i<255;buff[i++]='\0');
}

/********************Main********************/
int main()
{
	setenviron();
	while(1)
	{
		printf("%s",PS1);
		scanf("%[^\n]s",buff);
		printf("%s\n",buff);
		parse();
		execute();
//		flush();
		getchar();
	}
	return 0;
}
