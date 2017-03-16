/*******************************************************************************************
Author: Chinmoy Mohapatra (MT2016505)

Topic: 

Software engineers are designing an application that runs on a Linux
machine.The Core of the app is  mathematical engine which processes 
complex mathematical equations based on Linear Algebra.
Multiple users would like to run this this application  through a client.
Design a client and server model to implement this app using message queues

The following program is only the server end program. It is always on

compile: gcc assignment_mq_server.c -o assignment_mq_server
execute: ./assignment_mq_server

******************************************************************************************/

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define max_msg_size 3

char rcv_msg[max_msg_size];
int msg_q_id;

void handler(int signo){
	if (signo==SIGINT){
		//close message Q
		int ret = msgctl(msg_q_id,IPC_RMID,NULL);
		if (ret==-1)
		{
			printf("Message Queue close unsuccessful\n");
			perror("Error:");
			// exit(0);
		}
		printf("Message Queue closed.\n");
		exit(0);
	}
}

int main()
{
	int i;
	struct msqid_ds tmp_buf;
	FILE *fp;


	// signal handler to shut down the server/ exit the code
	if (signal(SIGINT,handler)==SIG_ERR)
	{
		printf("Can't catch SIGINT\n");
	}

	//create unique identifier, ftok creates based on the path and the integer in
	//the arguments, can use getuid() or a user value for key also
	key_t key;
	key = ftok("assignment_mq_server.c",0x25);//put server path for generating unique id
	if(key==-1)//check for succes of ftok
	{
		perror("Error ftok:");
		exit(0);
	}

	//create message queue to recieve information from client
	msg_q_id = msgget(key,0666|IPC_CREAT);
	if (msg_q_id==-1)//check for successful creation of message queue
	{
		perror("Error msgget:");
		exit(0);
	}


	printf("Press ^C to exit server\n");
	while (1)//server is always on, up amd running
	{
		//initialize message buffer to all zero
		bzero(rcv_msg,max_msg_size);
		//recieve status of queue, specifically number of messages= msq_qnum
		int stat = msgctl(msg_q_id,IPC_STAT,&tmp_buf);
		if (stat==-1)//check if status retrival is success
		{
			perror("Error msgctl:");
			exit(0);
		}

		//recieve message
		if (tmp_buf.msg_qnum > 0)//check for any pending message, if yes address them
		{
			//get the pid for the last message
			int client_id  = tmp_buf.msg_lrpid;
			//recieve information from the client
			int ret = msgrcv(msg_q_id,rcv_msg,max_msg_size,0,IPC_NOWAIT);
			if (ret==-1)//check for successful recieve
			{
				perror("Error msgrcv:");
				exit(0);
			}		

			int num1 = rcv_msg[0] - '0',num2 = rcv_msg[1] - '0';
			char optr = rcv_msg[2];


			// perform mathematical operation
			switch(optr)
			{
				case '+':
				{
					fp = fopen("output.txt","a");
					fprintf(fp,"%d %d %c %d = %d\n",client_id,num1,optr,num2,num1+num2);
					fclose(fp);
					printf("Output written to file\n");
					break;
				}
				case '-':
				{
					fp = fopen("output.txt","a");
					fprintf(fp,"%d %d %c %d = %d\n",client_id,num1,optr,num2,num1-num2);
					fclose(fp);
					printf("Output written to file\n");
					break;
				}
				case '*':
				{
					fp = fopen("output.txt","a");
					fprintf(fp,"%d %d %c %d = %d\n",client_id,num1,optr,num2,num1*num2);
					fclose(fp);
					printf("Output written to file\n");
					break;
				}
				case '/':
				{
					if (num2==0)
					{
						fp = fopen("output.txt","a");
						fprintf(fp,"%d %d %c %d = INF",client_id,num1,optr,num2);
						fprintf(fp,"Division by zero\n");
						fclose(fp);
						printf("Output written to file\n");
						break;
					}
					else
					{
						fp = fopen("output.txt","a");
						fprintf(fp,"%d %d %c %d = %f\n",client_id,num1,optr,num2,(float)num1/(float)num2);
						fclose(fp);
						printf("Output written to file\n");
						break;
					}
				}
				default:
				{
					fp = fopen("output.txt","a");
					fprintf(fp,"%d %d %c %d = ???",client_id,num1,optr,num2);
					fprintf(fp,"Invalid operator\n");
					fclose(fp);
					printf("Output written to file\n");
					break;
				}
			}
		}
	}

	return 0;
}
