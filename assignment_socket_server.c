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


compile:gcc assignment_socket_server.c -o assignment_socket_server
execute: ./assignment_socket_server [IP] [PORT]
********************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>


#define MAX_MSG_SIZE 1000

int server_socket_id,client_socket_id,ret;
	
struct sockaddr_in server_address;

void handler(int signo){

	if (signo==SIGINT){
		printf("Do you want to shutdown[Y/N]:");
		char ip;
		scanf("%c",&ip);//ask user to shutdown or not
		if (ip == 'Y' || ip =='y')
		{
			printf("Server shutting down.\n");
			ret=close(server_socket_id);
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
	//create_socket(); create a socket
	//IPv4 + stream + TCP protocol
	server_socket_id = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (server_socket_id==-1)//check for socket creation
	{
		perror("Error creating socket:");
		exit(0);
	}
	//initialize the socket with ip,port,etc
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;//ipv4 network
	server_address.sin_port = htons(PORT);//port number
	server_address.sin_addr.s_addr = inet_addr(IP);//ip address
	bzero(server_address.sin_zero,sizeof(server_address.sin_zero));
}

int main(int argc, char * argv[])
{

//create_socket();
//bind_socket();
//listen_at_socket();
// accept_connection();
//read_4m_socket();
//write_2_socket();
//close_socket();

	
	if (argc<3)//check whthwe sufficient parameter is provided
	{
		printf("IP address/port not mentioned\n");
		exit(0);
	}

	char * IP = argv[1];
	int PORT = atoi(argv[2]);

	printf("Server is up and running.\nPress ctrl + c to exit\n");

	//create and initialize socket
	Create_Socket(IP,PORT);

	//signal handler
	if (signal(SIGINT,handler)==SIG_ERR)
	{
		printf("Can't catch SIGINT\n");
	}

	//bind host with the socket
	int bnd = bind(server_socket_id,(struct sockaddr *) &server_address,sizeof(server_address));
	if (bnd==-1)// check for binding success
	{
		perror("Error binding:");
		exit(0);
	}

	//listen_at_socket();
	int lst = listen(server_socket_id,1);
	if (lst==-1)//check whether listen failed
	{
		perror("Error listening:");
		exit(0);
	}


	//accept connection from client
	client_socket_id = accept(server_socket_id,(struct sockaddr *) NULL,NULL);
	if (client_socket_id==-1)//check for connection
	{
		perror("Error connecting:");
		exit(0);
	}
	//start communicating
	char read_buffer[MAX_MSG_SIZE],write_buffer[MAX_MSG_SIZE] = ":)";
	bzero(read_buffer,MAX_MSG_SIZE);

	while(1)
	{

		//read from the socket
		//read() can also be used instead of recv	
		int rx = recv(client_socket_id,read_buffer,MAX_MSG_SIZE,0);
		//int rx = read(client_socket_id,read_buffer,strlen(read_buffer)+1);
		if (rx==-1)//check whether the data read was succesful
		{
			perror("Error reading:");
			// continue;
			exit(0);
		}
		printf("Client says:%s\n",read_buffer);
		

		//write to the socket
		int tx = send(client_socket_id,write_buffer,strlen(write_buffer)+1,0);
		// int tx =  write(client_socket_id,write_buffer,strlen(write_buffer)+1);
		if (tx==-1)
		{
			perror("Error sending:");
			exit(0);
		}
	}
}