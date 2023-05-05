#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BUFFER 1024

//dufinicja plików
#define SYSFS_FILE_WE1 "/sys/kernel/sykt/raba1"
#define SYSFS_FILE_WE2 "/sys/kernel/sykt/raba2"
#define SYSFS_FILE_RES "/sys/kernel/sykt/rabw"
#define SYSFS_FILE_STATUS "/sys/kernel/sykt/rabb"
#define SYSFS_FILE_ONES "/sys/kernel/sykt/rabl"



unsigned int read_from_file(char *);   // oby nie było błędów z kompilacją i z kolejnoscią, definiuje metody na początku
void write_to_file(char *, unsigned int);
unsigned int multiply(unsigned int, unsigned int);
int test_module();


//możliwe trzeba to dać na końcu
/*if(test > 0){
printf("TEST FAILED at %d values\n",test);
}
else{
printf("====== TEST PASSED =====\n");
}
return 0;
}*/

int main(void){
int test = test_module();


// licze próbki poprostu
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
exit(1);
}
int n=read(file, buffer, MAX_BUFFER);
if(n>0){    // wynika z dokumentacji
        buffer[n]='\0';
        printf("%s", buffer); 
    }else{
        printf("Open %s - error %d\n", filePath, errno); 
    }

close(file);
return strtoul(buffer, NULL, 16);  // 16 znaczy HEX
}




void write_to_file(char *filePath, unsigned int input){
	char buffer[MAX_BUFFER];
	int fd_in=open(filePath, O_RDWR); 
	if(fd_in < 0){   // wynika z dokumentacji, wz z instrukcji więc powinno być git
		 printf("Open %s - error number %d\n", filePath, errno);
		 exit(2);
	}
	snprintf(buffer, MAX_BUFFER, "%x", input);
	int n=write(fd_in, buffer, strlen(buffer));
    if(n!=strlen(buffer)){
        printf("Open %s - error number %d\n", filePath, errno);
        close(fd_in);
        exit(3);
    }
	close(fd_in);
}





unsigned int multiply(unsigned int arg1, unsigned int arg2){
write_to_file(SYSFS_FILE_WE1,arg1);
write_to_file(SYSFS_FILE_WE2,arg2);
write_to_file(SYSFS_FILE_STATUS,11);
unsigned int read = 0;
unsigned int readw = 0;
unsigned int readl = 0;
unsigned int readb = 0;
do{
read = read_from_file(SYSFS_FILE_STATUS);
}
while(read != 11);
readw = read_from_file(SYSFS_FILE_RES);
readl = read_from_file(SYSFS_FILE_ONES);
readb = read_from_file(SYSFS_FILE_STATUS);

printf("A1=0x%x, A2=0x%x, W=0x%x, L=0x%x, B =0x%x", arg1, arg2, readw, readl,readb);
return read;
}




int test_module(){
unsigned int args1[3] = { 3, 0xC, 8};
unsigned int args2[3] = { 4, 3, 3};
unsigned int results[3] = { 0xC,24,18 };
unsigned int ones[3] ={3,2,2};
for(int i=0; i<3; i++){
if( multiply(args1[i],args2[i]) != results[i] && multiply(args1[i],args2[i]) != ones[i])
return i+1;

}
return 0;
}


//for commit
