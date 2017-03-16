/*********************************************************************************************************
Author: Chinmoy Mohapatra (MT2016505)

Topic: 
write sample program to demonstrate shared memory between child and parent process created out of fork().

In this example parent write into the memory and child read from it.

compile: gcc assignment_shdm_using_fork.c -o assignment_shdm_using_fork -lrt
execute: ./assignment_shdm_using_fork [MEM SIZE] <data> <data> <data> ....

*********************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>


int main(int argc,char * argv[]){

	int shdm_pter,pid;

	void *child_ptr,*parent_ptr;

	if (argc<3)//check whether the data provided by user is sufficient
	{
		printf("No data to send\n");
		printf("Format:: ./assignment_shdm_using_fork [MEM SIZE] <data> <data> <data> ....\n"); 
		exit(0);
	}

	//input the size of shared memory from user
	int SIZE = atoi(argv[1]);


	//create a shared memory
	shdm_pter = shm_open("Shared_mem",O_CREAT|O_RDWR,0666);
	if (shdm_pter==-1)//check for error is creation of shared memory
	{
		perror("Error shm_open:");
		exit(0);
	}

	//configure the size of shared memory
	ftruncate(shdm_pter,SIZE);

	//create child process using fork
	pid = fork();
	if (pid ==-1)//check whther child process created or not
	{
		perror("Error fork:");
		exit(0);
	}

	if (pid==0) //when child process
	{
		//map child to shared memory
		child_ptr = mmap(0,SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,shdm_pter,0);
		if (child_ptr==MAP_FAILED)//check for mapping failure
		{
			perror("Error child mmap:");
			exit(0);
		}

		//child reads data from shared memory
		printf("Child read=> %s\n",(char *)child_ptr);
	}

	else //when parent process
	{
		//map parent to shared memory
		parent_ptr = mmap(0,SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,shdm_pter,0);
		if (parent_ptr==MAP_FAILED)//check  for mapping failure
		{
			perror("Error parent mmap:");
			exit(0);
		}

		//parent writes into shared memory
		int i=2;
		while (i<argc)
		{
			printf("Parent write=> %s\n",argv[i]);
			strncpy(parent_ptr,argv[i],SIZE);
			int len =strlen(argv[i]);
			parent_ptr += len;
			i++;
			// sleep(100);
		}
	}

	//close or unlink the shared memory
	close(shdm_pter);


	return 0;
}