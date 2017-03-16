
/*********************************************************************************************************
Author: Chinmoy Mohapatra (MT2016505)

Topic: 
write sample program to demonstrate shared memory between child and parent process created out of fork().
implement semaphore to avoid accidental over write while reading and vice versa.

In this example parent write into the memory and child read from it.

compile: gcc assignment_sem_prot_shdm_using_fork.c -o assignment_sem_prot_shdm_using_fork -lrt
execute: ./assignment_sem_prot_shdm_using_fork [MEM SIZE]

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
#include <semaphore.h>
#include <time.h>

sem_t mutex;

int main(int argc,char * argv[]){

	int shdm_pter,pid,ret;

	void *child_ptr,*parent_ptr;

	if (argc<2)//check whether the data provided by user is sufficient
	{
		printf("No data to send\n");
		printf("Format:: ./assignment_sem_prot_shdm_using_fork [MEM SIZE]\n"); 
		exit(0);
	}

	//input the size of sahred memory from user
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

	//initialize semaphore
	ret = sem_init(&mutex,1,1);
	if (ret==-1)
	{
		perror("Error sem_init:");
		exit(0);
	}

	//create child and parent process
	pid = fork();
	if (pid ==-1)//check whther child process created or not
	{
		perror("Error fork:");
		exit(0);
	}

	if (pid==0) //when child process
	{
		printf("\t\t\tChild: trying to access shared memory\n");
		//added intentional delay so that child access the shared memory little late
		for(int i=0;i<100000;i++);

		//wait semaphore, mutex -1
		ret = sem_wait(&mutex);
		if(ret==-1)//check for success 
		{
			perror("\t\t\tError child sem_wait:");
			exit(0);
		}


		//critical section
		//map child to shared memory
		child_ptr = mmap(0,SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,shdm_pter,0);
		if (child_ptr==MAP_FAILED)//check for mapping failure
		{
			perror("Error child mmap:");
			exit(0);
		}

		//child reads data from shared memory
		printf("\t\t\tChild read=> %s\n",(char *)child_ptr);

		char str[11] = "I am child\0";
		printf("\t\t\tChild write=> %s\n",str);
		strncpy(child_ptr,str,SIZE);
		//critical section ends

		//post or signal semaphore mutex +1;
		ret=sem_post(&mutex);
		if(ret==-1)
		{
			perror("\t\t\tError child sem_post:");
			exit(0);
		}

	}

	else //when parent process
	{
		printf("Parent: trying to access shared memory\n");
		//wait semaphore, mutex -1
		sem_wait(&mutex);
		if(ret==-1)
		{
			perror("Error parent sem_wait:");
			exit(0);
		}

		//critical section
		//map parent to shared memory
		parent_ptr = mmap(0,SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,shdm_pter,0);
		if (parent_ptr==MAP_FAILED)//check  for mapping failure
		{
			perror("Error parent mmap:");
			exit(0);
		}

		//parent writes into shared memory
		printf("Parent read=> %s\n",(char *)parent_ptr);

		char str[12] = "I am parent\0";

		//delay is created to make the child wait
		for(int i=0;i<100000;i++);
		
		printf("Parent write=> %s\n",str);
		strncpy(parent_ptr,str,SIZE);
		//critical section ends

		//post or signal semaphore mutex +1;
		ret=sem_post(&mutex);
		if(ret==-1)
		{
			perror("Error parent sem_post:");
			exit(0);
		}

	}


	// for(int i=0;i<10000000;i++);
	//close or unlink the shared memory
	// sem_destroy(&mutex);
	ret=close(shdm_pter);
	if(ret==-1)
	{
		perror("Error Shared memory close:");
		exit(0);
	}



	return 0;
}