/*******************************************************************************************************************
Author: Chinmoy Mohapatra (MT2016505)

Topic
Implement a client and server program
Client takes input message/text from user and send it to server and server displays the message/text to other user
Similarly on other side, the client takes message/text as input to send it to server on the other side
They all use socket API
Note
1 Client and server are two separate executables and both are present with both users for now. 
In the next assignment we will modify to become a single executable
2. You need to provide the IP address and port no of the server  to which client need to connect as 
command line arguments ( char **argv[]   int argc)
3. Simillarly you need to give the port number to which server has to bind as command line argument to it
4. Implement a signal handler which will do  a graceful exit of server by closing all the handles
5. The client has to run in a infinite loop asking for messages from users [ You can use scanf() to input messages]
6. Implement a signal handler in client so that client will "exit Y/N" when you press CTRL-C. 
When you "Y" the signal handler should close all handles and do a graceful exit of the client.


compile:gcc assignment_socket_client.c -o assignment_socket_client
execute: ./assignment_socket_client [IP] [PORT]
********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>


#define MAX_MSG_SIZE 1000

int socket_id,ret;

struct sockaddr_in server_address;


void handler(int signo){

	if (signo==SIGINT){
		printf("Do you want to shutdown[Y/N]:");
		char ip;
		scanf("%c",&ip);//ask user to shutdown or not
		if (ip == 'Y' || ip =='y')
		{
			printf("Client shutting down.\n");
			ret=close(socket_id);
			sleep(1);;
			if(ret==-1)
			{
				perror("Error closing scoket: ");
			}
			exit(0);	
		}
	}
}

void Create_Socket(char * IP,int PORT)
{
	//create socket
	socket_id = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (socket_id==-1)//check for success
	{
		perror("Error creating socket:");
		exit(0);
	}
	//assign port number and ip address to the scoket
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr(IP);
	bzero(server_address.sin_zero,sizeof(server_address.sin_zero));
}

int main(int argc, char * argv[])
{
//create_socket();
//connect_socket();
//write_2_socket();
//read_4m_socket();

//close_socket();

	if (signal(SIGINT,handler)==SIG_ERR)
	{
		printf("Can't catch SIGINT\n");
	}

	if (argc<3)//chcek if sufficient parameter provided by user
	{
		printf("IP address/port no of server not mentioned\n");
		exit(0);
	}

	char * IP = argv[1]; 
	int PORT = atoi(argv[2]);

	if (PORT < 1024)//check for port number to be reserved
	{
		printf("Port is a reserved port\n");
		exit(0);
	}
	Create_Socket(IP,PORT);

	//connect to socket
	int con = connect(socket_id,(struct sockaddr *) &server_address,sizeof(server_address));
	if (con==-1)//check for connection success
	{
		perror("Error connecting socket:");
		exit(0);
	}

	//ask user for input
	char write_buffer[MAX_MSG_SIZE],read_buffer[MAX_MSG_SIZE];
	while(1)
	{
		//ask user for inputs
		printf("Me:>");
		bzero(write_buffer,MAX_MSG_SIZE);
		bzero(read_buffer,MAX_MSG_SIZE);
		scanf("%s",write_buffer);
		//send message to server
		int snd = send(socket_id,write_buffer,strlen(write_buffer)+1,0);
		if (snd==-1)//check for transmission success
		{
			perror("Error sending to server:");
			exit(0);
		}

		//read server response
		int rcv = recv(socket_id,read_buffer,MAX_MSG_SIZE,0);
		if (rcv==-1)
		{
			perror("Error in recieve:");
			exit(0);
		}
		// printf("\t\t\t\t\t\t\tServer says:%s\n",read_buffer);
	}

	close(socket_id);

}