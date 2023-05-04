#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 1024
#define SYSFS_FILE_WE1 "/sys/kernel/sykt/gpf2we1"
#define SYSFS_FILE_WE2 "/sys/kernel/sykt/gpf2we2"
#define SYSFS_FILE_WY "/sys/kernel/sykt/gpf2wy"
#define SYSFS_FILE_STATUS "/sys/kernel/sykt/gpf2status"

unsigned int read_from_file(char *);

int write_to_file(char *, unsigned int);

unsigned int calculate_gcd(unsigned int, unsigned int);

int test_module();

int main(void){
	int test = test_module();
	if(test > 0){
		printf("TEST FAILED at %d values\n",test);
	}
	else{
		printf("====== TEST PASSED =====\n");
	}
	return 0;
}

unsigned int read_from_file(char *filePath){
	char buffer[MAX_BUFFER];
	int file=open(filePath, O_RDONLY);
	if(file<0){
		 printf("Open %s - error number %d\n", filePath, errno);
		 exit(5);
	}
	int n=read(file, buffer, MAX_BUFFER);
	close(file);
	return strtoul(buffer, NULL, 16);
	 
}

int write_to_file(char *filePath, unsigned int input){
	char buffer[MAX_BUFFER];
	FILE *file=fopen(filePath, "w");
	if(file == NULL){
		 printf("Open %s - error number %d\n", filePath, errno);
		 exit(6);
	}
	snprintf(buffer, MAX_BUFFER, "%x",input);
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);
	return 0;
}

unsigned int calculate_gcd(unsigned int arg1, unsigned int arg2){
	write_to_file(SYSFS_FILE_WE1,arg1);
	write_to_file(SYSFS_FILE_WE2,arg2);
	write_to_file(SYSFS_FILE_STATUS,1);
	unsigned int read;
	do{
		read = read_from_file(SYSFS_FILE_STATUS);
	 }
	while(read != 1);
	read = read_from_file(SYSFS_FILE_WY);
	printf("Arg1=0x%x, Arg2=0x%x, GCD=0x%x(hex) GCD=%u(dec)\n", arg1, arg2, read, read);
	return read;
}

int test_module(){
	unsigned int args1[7] = {  16,	47194, 290456829, 1, 0, 10, 241318};
	unsigned int args2[7] = {   8,	 3151,	   53667, 0, 1,  1, 327404};
	unsigned int results[7] = { 8,	    1,	       9, 1, 1,  1,    154};
	
	for(int i=0; i<7; i++){
		if( calculate_gcd(args1[i],args2[i]) != results[i])
			return i+1;
	}
	return 0;
	
}
