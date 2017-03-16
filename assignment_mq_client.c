/****************************************************************************************
Author: Chinmoy Mohapatra (MT2016505)

Topic: 

Software engineers are designing an application that runs on a Linux
machine.The Core of the app is  mathematical engine which processes 
complex mathematical equations based on Linear Algebra.
Multiple users would like to run this this application  through a client.
Design a client and server model to implement this app using message queues


The following program is the client end program. provide input to the snd_msg 
as command line arguments.

compile: gcc assignment_mq_client.c -o assignment_mq_client
execute:./assignment_mq_client num1 num2 optr
****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#define max_msg_size 3

char snd_msg[max_msg_size];
int msg_q_id;

int main(int argc,char *argv[])
{
	int i;

	//initialize message buffer to all zero
	bzero(snd_msg,max_msg_size);

	//create unique identifier, ftok creates based on the path and the integer in
	//the arguments, can use getuid() or a user value for key also
	key_t key;
	key = ftok("assignment_mq_server.c",0x25);//put server path for generating unique id
	if(key==-1)//check for success of ftok
	{
		perror("Error ftok:");
		exit(0);
	}


	if (argc<2)//check for input numbers
	{
		printf("Error: No numbers to send\n");
		printf("Format::::./assignment_mq_client_1 num1(0-9) num2(0-9) operator(+,-,*,/)\n");
		exit(0);
	}

	//get the msg_id of the msg_q, this should be same as that of server,if the queue does
	//not exist then create one
	msg_q_id = msgget(key,0666|IPC_CREAT);
	if (msg_q_id==-1)//check for sucessful create/fetch of message Q id  
	{
		perror("Error msgget:");
		exit(0);
	}

	snd_msg[0] = *argv[1];
	snd_msg[1] = *argv[2];
	snd_msg[2] = *argv[3];

	//send the msg
	int ret = msgsnd(msg_q_id,snd_msg,max_msg_size,0);
	if(ret==-1)// check for successful sent
	{
		perror("Error msgsnd:");
		exit(0);
	}

	return 0;
}
