/****************************************************************************
Author: Chinmoy Mohapatra (MT2016505)

Topic: 

Run ls -1 through a C- program and get the output and simillarly run wc 
utility.  “pipe” the output of ls -1 to wc utility through your C program 
itself or search for a particular file name using ls - l and grep.

compile: gcc assignment_pipe.c -o assignment_pipe
execute: ./assignment_pipe

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
	//ask user what operation to do
	
	int option;
	char str[100];

	printf("Enter your option:\n1. ls -l | wc\n2. ls -l | grep assignment_pipe\n0. exit\n>");
	scanf("%d",&option);
	
	//create pipe
	int fd[2];
	pipe(fd);

	//create child process
	pid_t ls_pid = fork();
	if(ls_pid==-1)//check whether the child process created
	{
		printf("FORK ERROR CHILD NOT CREATED\n");
		exit(0);
	}

	while (1)
	{
		switch(option)//switch based on option entered by user
		{
			case 1://ls -l | wc
			{
				//execute ls -l from child process and attach the output
				//to stdout via pipe out fd[1] 
				if(ls_pid==0)
				{
					close(fd[0]);
					dup2(fd[1],STDOUT_FILENO);//attach to stdout
					execl("/bin/ls","ls","-l",NULL);
				}
				//read from stdin as input to pipe fd[0] and feed to wc
				else
				{
					close(fd[1]);
					dup2(fd[0],STDIN_FILENO);//attach to stdin 
					execl("/usr/bin/wc","wc",NULL);
				}
				// continue;
				break;
			}
			case 2: //ls -l | grep assignment_pipe
			{
				//execute ls -l from child process and attach the output
				//to stdout via pipe out fd[1] 
				if(ls_pid==0)
				{
					close(fd[0]);
					dup2(fd[1],STDOUT_FILENO);//attach to stdout
					execl("/bin/ls","ls","-l",NULL);
				}
				//read from stdin as input to pipe fd[0] and feed to grep
				else
				{
					close(fd[1]);
					dup2(fd[0],STDIN_FILENO);//attach to stdin 
					execl("/bin/grep","grep","assignment_pipe",NULL);
				}
				// continue;
				break;
			}
			case 0: 
			{
				exit(0);
				break;
			}
			default:
			{
				printf("Invalid option\n");
				exit(0);
				break;
			}
		} 
	}
	return 0;
}
